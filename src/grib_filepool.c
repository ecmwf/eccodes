/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

/*
 *
 * Description: file pool
 *
 */

#include "grib_api_internal.h"
#define GRIB_MAX_OPENED_FILES 200

#if GRIB_PTHREADS
static pthread_once_t once    = PTHREAD_ONCE_INIT;
static pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;

static void init()
{
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&mutex1, &attr);
    pthread_mutexattr_destroy(&attr);
}
#elif GRIB_OMP_THREADS
static int once = 0;
static omp_nest_lock_t mutex1;

static void init()
{
    GRIB_OMP_CRITICAL(lock_grib_filepool_c)
    {
        if (once == 0) {
            omp_init_nest_lock(&mutex1);
            once = 1;
        }
    }
}
#endif

static short next_id = 0;

/* Note: A fast cut-down version of strcmp which does NOT return -1 */
/* 0 means input strings are equal and 1 means not equal */
GRIB_INLINE static int grib_inline_strcmp(const char* a, const char* b)
{
    if (*a != *b)
        return 1;
    while ((*a != 0 && *b != 0) && *(a) == *(b)) {
        a++;
        b++;
    }
    return (*a == 0 && *b == 0) ? 0 : 1;
}

static grib_file_pool file_pool = {
    0,                    /* grib_context* context;*/
    0,                    /* grib_file* first;*/
    0,                    /* grib_file* current; */
    0,                    /* int number_of_opened_files;*/
    GRIB_MAX_OPENED_FILES /* int max_opened_files; */
};


static void grib_file_delete(grib_file* file);

static void grib_file_pool_change_id()
{
    grib_file* file;

    if (!file_pool.first)
        return;

    file = file_pool.first;
    while (file) {
        file->id += 1000;
        file = file->pool_next;
    }
}

static grib_file* grib_read_file(grib_context* c, FILE* fh, int* err)
{
    short marker = 0;
    short id     = 0;
    grib_file* file;
    *err = grib_read_short(fh, &marker);
    if (!marker)
        return NULL;

    file         = (grib_file*)grib_context_malloc_clear(c, sizeof(grib_file));
    file->buffer = 0;
    file->name   = grib_read_string(c, fh, err);
    if (*err)
        return NULL;

    *err     = grib_read_short(fh, &id);
    file->id = id;
    if (*err)
        return NULL;

    file->index_next = grib_read_file(c, fh, err);
    if (*err)
        return NULL;

    return file;
}

static int grib_write_file(FILE* fh, grib_file* file)
{
    int err = 0;

    if (!file)
        return grib_write_null_marker(fh);

    err = grib_write_not_null_marker(fh);
    if (err)
        return err;

    err = grib_write_string(fh, file->name);
    if (err)
        return err;

    err = grib_write_short(fh, (short)file->id);
    if (err)
        return err;

    return grib_write_file(fh, file->index_next);
}

grib_file* grib_file_pool_get_files()
{
    return file_pool.first;
}

int grib_file_pool_read(grib_context* c, FILE* fh)
{
    int err      = 0;
    short marker = 0;
    grib_file* file;

    if (!c)
        c = grib_context_get_default();

    err = grib_read_short(fh, &marker);
    if (!marker) {
        grib_context_log(c, GRIB_LOG_ERROR,
                         "Unable to find file information in index file\n");
        return GRIB_INVALID_FILE;
    }

    grib_file_pool_change_id();
    file = file_pool.first;

    while (file->index_next)
        file = file->index_next;

    file->index_next = grib_read_file(c, fh, &err);
    if (err)
        return err;

    return GRIB_SUCCESS;
}

int grib_file_pool_write(FILE* fh)
{
    int err = 0;
    if (!file_pool.first)
        return grib_write_null_marker(fh);

    err = grib_write_not_null_marker(fh);
    if (err)
        return err;

    return grib_write_file(fh, file_pool.first);
}


static int poolcache_matches(const char *filename)
{
    if (!file_pool.current)
        return 0;
    DebugAssert(file_pool.current->name);
    return !grib_inline_strcmp(filename, file_pool.current->name);
}


grib_file* grib_file_open(const char* filename, const char* mode, int* err)
{
    grib_file *file = 0, *prev = 0;
    int same_mode = 0;
    int is_new    = 0;
    GRIB_MUTEX_INIT_ONCE(&once, &init);

    if (!file_pool.context)
        file_pool.context = grib_context_get_default();

    if (poolcache_matches(filename)) {
        file = file_pool.current;
    }
    else {
        GRIB_MUTEX_LOCK(&mutex1);
        file = file_pool.first;
        while (file) {
            if (!grib_inline_strcmp(filename, file->name))
                break;
            prev = file;
            file = file->pool_next;
        }
        if (!file) {
            is_new = 1;
            file   = grib_file_new(file_pool.context, filename, err);
            if (prev)
                prev->pool_next = file;
            else
                file_pool.first = file;
            file_pool.current = file;
        }
        GRIB_MUTEX_UNLOCK(&mutex1);
    }
    ++file->refcount;

    /* Sorry can't change file-open-mode of dependent 'grib_file's. */
    Assert(!file->is_dependant);
    /* Sorry can't change file-open-mode after an dependant is created. */
    Assert(!file->dependants);

    if (file->mode)
        same_mode = grib_inline_strcmp(mode, file->mode) ? 0 : 1;
    if (file->handle && same_mode) { /* file->handle is NULL if is_new==1 */
        *err = 0;
        return file;
    }

    GRIB_MUTEX_LOCK(&mutex1);
    if (!same_mode && file->handle) {
        fclose(file->handle);
        file->handle = NULL; /* file->handle will stay stale without this. */
        --file_pool.number_of_opened_files;
    }

    if (!file->handle) {
        if (!is_new && *mode == 'w') {
            file->handle = fopen(file->name, "a");
        }
        else {
            file->handle = fopen(file->name, mode);
        }

        if (!file->handle) {
            grib_context_log(file->context, GRIB_LOG_PERROR, "grib_file_open: cannot open file %s", file->name);
            *err = GRIB_IO_PROBLEM;
            GRIB_MUTEX_UNLOCK(&mutex1);
            return NULL;
        }
        if (file->mode) free(file->mode);
        file->mode = strdup(mode);
        if (file_pool.context->io_buffer_size) {
#ifdef POSIX_MEMALIGN
            if (posix_memalign((void**)&(file->buffer), sysconf(_SC_PAGESIZE), file_pool.context->io_buffer_size)) {
                grib_context_log(file->context, GRIB_LOG_FATAL, "posix_memalign unable to allocate io_buffer\n");
            }
#else
            file->buffer = (void*)malloc(file_pool.context->io_buffer_size);
            if (!file->buffer) {
                grib_context_log(file->context, GRIB_LOG_FATAL, "Unable to allocate io_buffer\n");
            }
#endif
            setvbuf(file->handle, file->buffer, _IOFBF, file_pool.context->io_buffer_size);
        }

        file_pool.number_of_opened_files++;
    }

    GRIB_MUTEX_UNLOCK(&mutex1);
    return file;
}


/* Find matching entry from file_pool. Returns NULL if search files. */
static grib_file * grib_file_pool_remove_file(const char * filename)
{
    grib_file **file_pp = &file_pool.first;
    grib_file * file    =  file_pool.first;
    while (file) {
        if(!grib_inline_strcmp(filename, file->name))
            break;
        file_pp = &file->pool_next;
        file    =  file->pool_next;
    }
    if (file) {
        *file_pp = file->pool_next;      /* remove from the pool         */
        if (file == file_pool.current)   /* update 'current' if required */
            file_pool.current = file->pool_next;
    }
    return file;
}


/* Must be called with MUTEX lock being held because we work on static var. */
static void grib_file_pool_delete_file(grib_file* file, int keep_open,
                                       int * err)
{
    Assert(0<file->refcount);
    if (0 < --file->refcount)
        return;

    grib_file * root = grib_file_pool_remove_file(file->name);
    DebugAssert(!root);
    DebugAssert(!root->is_dependant);
    *err = GRIB_SUCCESS; // be lazy for the time being. FixMe!
    if (root->dependants==NULL) {
        /* Only one 'grib_file' represents the given filename. */
        Assert(root == file);
        if( !keep_open ) {
            fclose(root->handle);
            grib_file_delete(root);
            --file_pool.number_of_opened_files;

            /* Early return without putting back the removed. */
            return;
        }
    } else {
        /* More than one 'grib_file' represent the given filename. */
        /* remove '*file' from the dependants chain.               */
        grib_file **pp = &root;
        grib_file *p   =  root;
        while(p) {
            DebugAssert(p->hanlde == file->handle);
            if(p == file)
                break;
            pp = &p->dependants;
            p  =  p->dependants;
        }
        *pp = p->dependants;
        root->is_dependant = 0; /* mark the head as non-dependent */
        grib_file_delete(p);
    }
    /* put back to the head of the pool */
    root->pool_next = file_pool.first;
    file_pool.current = file_pool.first = root;
}


void grib_file_close(grib_file * file, int force, int* err)
{
    grib_context* context = grib_context_get_default();

    /* Performance: keep the files open to avoid opening and closing files when writing the output. */
    /* So only call fclose() when too many files are open. */
    /* Also see ECC-411 */

    int do_close = force ||
        (file_pool.number_of_opened_files > context->file_pool_max_opened_files);
    /*printf("+++++++++++++ closing file %s (n=%d)\n",filename, file_pool.number_of_opened_files);*/

    GRIB_MUTEX_INIT_ONCE(&once, &init);
    GRIB_MUTEX_LOCK(&mutex1);
    grib_file_pool_delete_file(file, !do_close, err);

    GRIB_MUTEX_UNLOCK(&mutex1);
}

void grib_file_pool_clean(int *err)
{
    grib_file * file;
    if (!file_pool.first)
        return;

    GRIB_MUTEX_INIT_ONCE(&once, &init);
    GRIB_MUTEX_LOCK(&mutex1);

    file = file_pool.first;
    while (file) {
        grib_file * next = file->pool_next;
        grib_file * dep = file->dependants;
        Assert(!dep);
#if 0
        while(dep) {
            grib_file * next_dep = dep->dependants;
            DebugAssert(file->handle == dep->handle);
            grib_file_delete(dep);
            dep = next_dep;
        }
#endif
        if (file->handle) {
            if (fclose(file->handle) != 0) {
                *err = GRIB_IO_PROBLEM;
            }
            file->handle = NULL;
        }
        grib_file_delete(file);
        file =next;
    }
    file_pool.first = file_pool.current = NULL;

    GRIB_MUTEX_UNLOCK(&mutex1);
}

grib_file* grib_get_or_create_file(const char* filename, int *created,
                                   int* err)
{
    grib_file* file = NULL;
    if (poolcache_matches(filename)) {
        return file_pool.current;
    }
    file = file_pool.first;
    while (file) {
        if (!grib_inline_strcmp(filename, file->name)) {
            *created = 0;
            return file;
        }
        file = file->pool_next;
    }
    *created = 1;
    return grib_file_new(0, filename, err);
}


grib_file* grib_find_file(short id)
{
    grib_file* file = NULL;

    if (file_pool.current->name && id == file_pool.current->id) {
        return file_pool.current;
    }

    file = file_pool.first;
    while (file) {
        if (id == file->id)
            break;
        file = file->pool_next;
    }

    return file;
}

grib_file* grib_file_new(grib_context* c, const char* name, int* err)
{
    grib_file* file;

    if (!c)
        c = grib_context_get_default();

    file = (grib_file*)grib_context_malloc_clear(c, sizeof(grib_file));

    if (!file) {
        grib_context_log(c, GRIB_LOG_ERROR, "grib_file_new: unable to allocate memory");
        *err = GRIB_OUT_OF_MEMORY;
        return NULL;
    }
    GRIB_MUTEX_INIT_ONCE(&once, &init);

    file->name = strdup(name);
    file->id   = next_id;

    GRIB_MUTEX_LOCK(&mutex1);
    next_id++;
    GRIB_MUTEX_UNLOCK(&mutex1);

    file->mode     = 0;
    file->handle   = 0;
    file->refcount = 0;
    file->context  = c;
    file->pool_next  = NULL;
    file->index_next = NULL;
    file->buffer     = 0;
    file->dependants = NULL;
    file->is_dependant = 0;
    return file;
}

static void grib_file_delete(grib_file* file)
{
    {
        if (!file)
            return;
    }
    GRIB_MUTEX_INIT_ONCE(&once, &init);
    GRIB_MUTEX_LOCK(&mutex1);

    if (file->name)
        free(file->name);
    if (file->mode)
        free(file->mode);

    if (file->buffer) {
        free(file->buffer);
    }
    grib_context_free(file->context, file);
    file = NULL;
    GRIB_MUTEX_UNLOCK(&mutex1);
}
