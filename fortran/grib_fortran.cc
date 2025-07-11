/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_api_internal.h"
#include "grib_fortran_prototypes.h"

#if HAVE_SYS_TYPES_H
# include <sys/types.h>
#endif

/*
#if HAVE_SYS_STAT_H
# include <sys/stat.h>
#endif
*/

#ifdef HAVE_FCNTL_H
# include <fcntl.h>
#endif

#include <ctype.h>

/* Have file ids distinct from GRIB/BUFR ids, in order to be
 * protected against user errors where a file id is given
 * instead of a GRIB/BUFR id or vice versa
 */
#define MIN_FILE_ID 50000

#if GRIB_PTHREADS
static pthread_once_t once  = PTHREAD_ONCE_INIT;
static pthread_mutex_t handle_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t index_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t read_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t multi_handle_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t iterator_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t keys_iterator_mutex = PTHREAD_MUTEX_INITIALIZER;

static void init(void) {
    pthread_mutexattr_t attr;

    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr,PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&handle_mutex,&attr);
    pthread_mutex_init(&index_mutex,&attr);
    pthread_mutex_init(&read_mutex,&attr);
    pthread_mutex_init(&multi_handle_mutex,&attr);
    pthread_mutex_init(&iterator_mutex,&attr);
    pthread_mutex_init(&keys_iterator_mutex,&attr);
    pthread_mutexattr_destroy(&attr);
}
#elif GRIB_OMP_THREADS
static int once = 0;
static omp_nest_lock_t handle_mutex;
static omp_nest_lock_t index_mutex;
static omp_nest_lock_t read_mutex;
static omp_nest_lock_t multi_handle_mutex;
static omp_nest_lock_t iterator_mutex;
static omp_nest_lock_t keys_iterator_mutex;

static void init()
{
    GRIB_OMP_CRITICAL(lock_fortran)
    {
        if (once == 0) {
            omp_init_nest_lock(&handle_mutex);
            omp_init_nest_lock(&index_mutex);
            omp_init_nest_lock(&read_mutex);
            omp_init_nest_lock(&multi_handle_mutex);
            omp_init_nest_lock(&iterator_mutex);
            omp_init_nest_lock(&keys_iterator_mutex);
            once = 1;
        }
    }
}
#endif

typedef enum FileMode {
    FILE_MODE_READ,
    FILE_MODE_WRITE,
    FILE_MODE_APPEND
} FileMode;

int GRIB_NULL = -1;
int GRIB_NULL_NEAREST = -1;

typedef struct l_grib_file l_grib_file;

struct l_grib_file {
    FILE* f;
    char* buffer;
    int id;
    FileMode mode;
    l_grib_file* next;
};

typedef struct l_grib_handle l_grib_handle;

struct l_grib_handle {
    int id;
    grib_handle* h;
    l_grib_handle* next;
};

typedef struct l_grib_index l_grib_index;

struct l_grib_index {
    int id;
    grib_index* h;
    l_grib_index* next;
};

typedef struct l_grib_multi_handle l_grib_multi_handle;

struct l_grib_multi_handle {
    int id;
    grib_multi_handle* h;
    l_grib_multi_handle* next;
};

typedef struct l_grib_iterator l_grib_iterator;

struct l_grib_iterator {
    int id;
    grib_iterator* i;
    l_grib_iterator* next;
};

typedef struct l_grib_keys_iterator l_grib_keys_iterator;
struct l_grib_keys_iterator {
    int id;
    grib_keys_iterator* i;
    l_grib_keys_iterator* next;
};

typedef struct l_bufr_keys_iterator l_bufr_keys_iterator;
struct l_bufr_keys_iterator {
    int id;
    bufr_keys_iterator* i;
    l_bufr_keys_iterator* next;
};

typedef struct l_binary_message l_binary_message;
struct l_binary_message {
    size_t size;
    void* data;
};

typedef struct l_message_info l_message_info;
struct l_message_info {
    off_t offset;
    size_t size;
};

static l_grib_handle* handle_set = NULL;
static l_grib_index* index_set = NULL;
static l_grib_multi_handle* multi_handle_set = NULL;
static l_grib_file*   file_set   = NULL;
/*static l_grib_iterator* iterator_set = NULL;*/
static l_grib_keys_iterator* keys_iterator_set = NULL;
static l_bufr_keys_iterator* bufr_keys_iterator_set = NULL;
static grib_oarray* binary_messages = NULL;
static grib_oarray* info_messages = NULL;

/* Convert from Fortran string to C string - chop at first space character */
static char* cast_char(char* buf, char* fortstr, int len)
{
    char *p,*end;
    if (len == 0 || fortstr == NULL) return NULL;
    memcpy(buf,fortstr,len);
    p=buf;
    end=buf+len-1;
    while (isgraph(*p) && p != end) {
        p++;
    }
    if (*p==' ') *p='\0';
    if (p==end) *(p+1)='\0';
    else *p='\0';

    return buf;
}

/* Convert from Fortran string to C string - non chopping version */
static char* cast_char_no_cut(char* buf, char* fortstr, int len)
{
    if (len == 0 || fortstr == NULL) return NULL;
    memcpy(buf,fortstr,len);
    buf[len]='\0';

    return buf;
}

static void czstr_to_fortran(char* str, int len)
{
    char *p,*end;
    p=str; end=str+len-1;
    while (*p != '\0' && p != end) p++;
    while (p !=end) *(p++)=' ';
    *p=' ';
}

/*static void czstr_to_fortran_replace0(char* str,int len)
{
    char *p,*end;
    p=str; end=str+len-1;
    while (p != end) {
      if (*p=='\0') *p=' ';
      p++;
    }
}*/

static void fort_char_clean(char* str,int len)
{
    char *p,*end;
    p=str; end=str+len-1;
    while (p != end) *(p++)=' ';
    *p=' ';
}

/* Note: the open_mode argument will be all lowercase. See grib_f_open_file_ */
static int push_file(FILE* f, const char* open_mode, char* buffer)
{
    l_grib_file* current  = file_set;
    l_grib_file* previous = file_set;
    l_grib_file* the_new      = NULL;
    int myindex = MIN_FILE_ID;
    FileMode fmode = FILE_MODE_READ;

    if (strcmp(open_mode, "w") == 0) fmode = FILE_MODE_WRITE;
    else if (strcmp(open_mode, "a") == 0) fmode = FILE_MODE_APPEND;

    if(!file_set){
        file_set = (l_grib_file*)malloc(sizeof(l_grib_file));
        ECCODES_ASSERT(file_set);
        file_set->id     = myindex;
        file_set->f      = f;
        file_set->mode   = fmode;
        file_set->buffer = buffer;
        file_set->next   = NULL;
        return myindex;
    }

    while(current){
        if(current->id < 0){
            current->id = -(current->id);
            current->f      = f;
            current->mode   = fmode;
            current->buffer = buffer;
            return current->id ;
        } else{
            myindex++;
            previous = current;
            current = current->next;
        }
    }

    the_new = (l_grib_file*)malloc(sizeof(l_grib_file));
    ECCODES_ASSERT(the_new);
    the_new->id     = myindex;
    the_new->f      = f;
    the_new->mode   = fmode;
    the_new->buffer = buffer;
    the_new->next   = current;
    previous->next  = the_new;
    return myindex;
}

static void _push_handle(grib_handle *h,int *gid)
{
    l_grib_handle* current= NULL;
    l_grib_handle* previous= handle_set;
    l_grib_handle* the_new= NULL;
    int myindex= 1;

    /*
  if (*gid > 0 ) {
    while(current) {
      if(current->id == *gid) break;
      current = current->next;
    }
    if (current) {
      grib_handle_delete(current->h);
      current->h=h;
      return;
    }
  }
     */

    if(!handle_set){
        handle_set = (l_grib_handle*)malloc(sizeof(l_grib_handle));
        ECCODES_ASSERT(handle_set);
        handle_set->id   = myindex;
        handle_set->h    = h;
        handle_set->next = NULL;
        *gid=myindex;
        return;
    }

    current= handle_set;

    while(current){
        if(current->id < 0){
            current->id = -(current->id);
            current->h  = h;
            *gid=current->id;
            return;
        }
        else{
            myindex++;
            previous = current;
            current = current->next;
        }
    }

    the_new = (l_grib_handle*)malloc(sizeof(l_grib_handle));
    ECCODES_ASSERT(the_new);
    the_new->id   = myindex;
    the_new->h    = h;
    the_new->next = current;
    previous->next = the_new;

    *gid=myindex;
    return;
}

static void _push_index(grib_index *h,int *gid)
{
    l_grib_index* current= NULL;
    l_grib_index* previous= index_set;
    l_grib_index* the_new= NULL;
    int myindex= 1;

    /*
    if (*gid > 0 ) {
      while(current) {
        if(current->id == *gid) break;
        current = current->next;
      }
      if (current) {
        grib_index_delete(current->h);
        current->h=h;
        return;
      }
    }
    */

    if(!index_set){
        index_set = (l_grib_index*)malloc(sizeof(l_grib_index));
        ECCODES_ASSERT(index_set);
        index_set->id   = myindex;
        index_set->h    = h;
        index_set->next = NULL;
        *gid=myindex;
        return;
    }

    current= index_set;

    while(current){
        if(current->id < 0){
            current->id = -(current->id);
            current->h  = h;
            *gid=current->id;
            return;
        }
        else{
            myindex++;
            previous = current;
            current = current->next;
        }
    }

    the_new = (l_grib_index*)malloc(sizeof(l_grib_index));
    ECCODES_ASSERT(the_new);
    the_new->id   = myindex;
    the_new->h    = h;
    the_new->next = current;
    previous->next = the_new;

    *gid=myindex;
    return;
}

static void _push_multi_handle(grib_multi_handle *h,int *gid)
{
    l_grib_multi_handle* current= NULL;
    l_grib_multi_handle* previous= multi_handle_set;
    l_grib_multi_handle* the_new= NULL;
    int myindex= 1;

    /*
    if (*gid > 0 ) {
        while(current) {
          if(current->id == *gid) break;
          current = current->next;
        }
        if (current) {
          grib_multi_handle_delete(current->h);
          current->h=h;
          return;
        }
    }
    */

    if(!multi_handle_set){
        multi_handle_set = (l_grib_multi_handle*)malloc(sizeof(l_grib_multi_handle));
        ECCODES_ASSERT(multi_handle_set);
        multi_handle_set->id   = myindex;
        multi_handle_set->h    = h;
        multi_handle_set->next = NULL;
        *gid=myindex;
        return;
    }

    current= multi_handle_set;

    while(current){
        if(current->id < 0){
            current->id = -(current->id);
            current->h  = h;
            *gid=current->id;
            return;
        }
        else{
            myindex++;
            previous = current;
            current = current->next;
        }
    }

    the_new = (l_grib_multi_handle*)malloc(sizeof(l_grib_multi_handle));
    ECCODES_ASSERT(the_new);
    the_new->id   = myindex;
    the_new->h    = h;
    the_new->next = current;
    previous->next = the_new;

    *gid=myindex;
    return;
}

static void push_handle(grib_handle *h,int *gid)
{
    GRIB_MUTEX_INIT_ONCE(&once,&init);
    GRIB_MUTEX_LOCK(&handle_mutex);
    _push_handle(h,gid);
    GRIB_MUTEX_UNLOCK(&handle_mutex);
    return;
}

static void push_index(grib_index *h,int *gid)
{
    GRIB_MUTEX_INIT_ONCE(&once,&init);
    GRIB_MUTEX_LOCK(&index_mutex);
    _push_index(h,gid);
    GRIB_MUTEX_UNLOCK(&index_mutex);
    return;
}

static void push_multi_handle(grib_multi_handle *h,int *gid)
{
    GRIB_MUTEX_INIT_ONCE(&once,&init);
    GRIB_MUTEX_LOCK(&multi_handle_mutex);
    _push_multi_handle(h,gid);
    GRIB_MUTEX_UNLOCK(&multi_handle_mutex);
    return;
}

static int _push_keys_iterator(grib_keys_iterator *i)
{
    l_grib_keys_iterator* current  = keys_iterator_set;
    l_grib_keys_iterator* previous = keys_iterator_set;
    l_grib_keys_iterator* the_new      = NULL;
    int myindex = 1;

    if(!keys_iterator_set){
        keys_iterator_set = (l_grib_keys_iterator*)malloc(sizeof(l_grib_keys_iterator));
        ECCODES_ASSERT(keys_iterator_set);
        keys_iterator_set->id   = myindex;
        keys_iterator_set->i    = i;
        keys_iterator_set->next = NULL;
        return myindex;
    }

    while(current){
        if(current->id < 0){
            current->id = -(current->id);
            current->i  = i;
            return current->id;
        }
        else{
            myindex++;
            previous = current;
            current = current->next;
        }
    }
    if(!previous) return -1;

    the_new = (l_grib_keys_iterator*)malloc(sizeof(l_grib_keys_iterator));
    ECCODES_ASSERT(the_new);
    the_new->id   = myindex;
    the_new->i    = i;
    the_new->next = current;
    previous->next = the_new;

    return myindex;
}

static int push_keys_iterator(grib_keys_iterator *i)
{
    int ret=0;
    GRIB_MUTEX_INIT_ONCE(&once,&init);
    GRIB_MUTEX_LOCK(&keys_iterator_mutex);
    ret=_push_keys_iterator(i);
    GRIB_MUTEX_UNLOCK(&keys_iterator_mutex);
    return ret;
}

/* BUFR Keys iterator */
static int _push_bufr_keys_iterator(bufr_keys_iterator *i)
{
    l_bufr_keys_iterator* current  = bufr_keys_iterator_set;
    l_bufr_keys_iterator* previous = bufr_keys_iterator_set;
    l_bufr_keys_iterator* the_new  = NULL;
    int myindex = 1;

    if(!bufr_keys_iterator_set){
        bufr_keys_iterator_set = (l_bufr_keys_iterator*)malloc(sizeof(l_bufr_keys_iterator));
        ECCODES_ASSERT(bufr_keys_iterator_set);
        bufr_keys_iterator_set->id   = myindex;
        bufr_keys_iterator_set->i    = i;
        bufr_keys_iterator_set->next = NULL;
        return myindex;
    }

    while(current){
        if(current->id < 0){
            current->id = -(current->id);
            current->i  = i;
            return current->id;
        }
        else{
            myindex++;
            previous = current;
            current = current->next;
        }
    }
    if(!previous) return -1;

    the_new = (l_bufr_keys_iterator*)malloc(sizeof(l_bufr_keys_iterator));
    ECCODES_ASSERT(the_new);
    the_new->id    = myindex;
    the_new->i     = i;
    the_new->next  = current;
    previous->next = the_new;

    return myindex;
}
static int push_bufr_keys_iterator(bufr_keys_iterator *i)
{
    int ret=0;
    GRIB_MUTEX_INIT_ONCE(&once,&init);
    GRIB_MUTEX_LOCK(&keys_iterator_mutex);
    ret=_push_bufr_keys_iterator(i);
    GRIB_MUTEX_UNLOCK(&keys_iterator_mutex);
    return ret;
}


static grib_handle* _get_handle(int handle_id)
{
    l_grib_handle* current= handle_set;

    while(current){
        if(current->id == handle_id) return current->h;
        current = current->next;
    }
    return NULL;
}

static grib_index* _get_index(int index_id)
{
    l_grib_index* current= index_set;

    while(current){
        if(current->id == index_id) return current->h;
        current = current->next;
    }
    return NULL;
}

static grib_multi_handle* _get_multi_handle(int multi_handle_id)
{
    l_grib_multi_handle* current= multi_handle_set;

    while(current){
        if(current->id == multi_handle_id) return current->h;
        current = current->next;
    }
    return NULL;
}

static grib_handle* get_handle(int handle_id)
{
    grib_handle* h=NULL;
    GRIB_MUTEX_INIT_ONCE(&once,&init);
    GRIB_MUTEX_LOCK(&handle_mutex);
    h=_get_handle(handle_id);
    GRIB_MUTEX_UNLOCK(&handle_mutex);
    return h;
}

extern "C" {
// Needed for fortran2c interoperability
grib_handle* f_handle_id2c_handle(int handle_id){
  return get_handle(handle_id);
}
}

static grib_index* get_index(int index_id)
{
    grib_index* h=NULL;
    GRIB_MUTEX_INIT_ONCE(&once,&init);
    GRIB_MUTEX_LOCK(&index_mutex);
    h=_get_index(index_id);
    GRIB_MUTEX_UNLOCK(&index_mutex);
    return h;
}

static grib_multi_handle* get_multi_handle(int multi_handle_id)
{
    grib_multi_handle* h=NULL;
    GRIB_MUTEX_INIT_ONCE(&once,&init);
    GRIB_MUTEX_LOCK(&multi_handle_mutex);
    h=_get_multi_handle(multi_handle_id);
    GRIB_MUTEX_UNLOCK(&multi_handle_mutex);
    return h;
}

static FILE* get_file(int file_id)
{
    l_grib_file* current  = file_set;

    if ( file_id < MIN_FILE_ID ) return NULL;

    while(current){
        if(current->id == file_id) return current->f;
        current = current->next;
    }
    return NULL;
}

static grib_keys_iterator* _get_keys_iterator(int keys_iterator_id)
{
    l_grib_keys_iterator* current  = keys_iterator_set;

    while(current){
        if(current->id == keys_iterator_id) return current->i;
        current = current->next;
    }
    return NULL;
}
static grib_keys_iterator* get_keys_iterator(int keys_iterator_id)
{
    grib_keys_iterator* i=NULL;
    GRIB_MUTEX_INIT_ONCE(&once,&init);
    GRIB_MUTEX_LOCK(&keys_iterator_mutex);
    i=_get_keys_iterator(keys_iterator_id);
    GRIB_MUTEX_UNLOCK(&keys_iterator_mutex);
    return i;
}

/* BUFR */
static bufr_keys_iterator* _get_bufr_keys_iterator(int keys_iterator_id)
{
    l_bufr_keys_iterator* current  = bufr_keys_iterator_set;

    while(current){
        if(current->id == keys_iterator_id) return current->i;
        current = current->next;
    }
    return NULL;
}
static bufr_keys_iterator* get_bufr_keys_iterator(int keys_iterator_id)
{
    bufr_keys_iterator* i=NULL;
    GRIB_MUTEX_INIT_ONCE(&once,&init);
    GRIB_MUTEX_LOCK(&keys_iterator_mutex);
    i=_get_bufr_keys_iterator(keys_iterator_id);
    GRIB_MUTEX_UNLOCK(&keys_iterator_mutex);
    return i;
}


static int clear_file(int file_id)
{
    int err = 0;
    l_grib_file* current = file_set;
    while(current){
        if(current->id == file_id){
            current->id = -(current->id);
            if (current->f) {
                if (current->mode == FILE_MODE_WRITE || current->mode == FILE_MODE_APPEND)
                    err = codes_flush_sync_close_file(current->f);
                else
                    err = fclose(current->f);
                if (err) {
                    int ioerr = errno;
                    grib_context* c = grib_context_get_default();
                    grib_context_log(c,(GRIB_LOG_ERROR)|(GRIB_LOG_PERROR),"IO ERROR: %s",strerror(ioerr));
                    return GRIB_IO_PROBLEM;
                }
            }
            if (current->buffer) free(current->buffer);
            return GRIB_SUCCESS;
        }
        current = current->next;
    }
    return GRIB_INVALID_FILE;
}

static int _clear_handle(int handle_id)
{
    l_grib_handle* current  = handle_set;
    if (handle_id<0) return 0;
    while(current){
        if(current->id == handle_id){
            current->id = -(current->id);
            if(current->h) return grib_handle_delete(current->h);
        }
        current = current->next;
    }
    return GRIB_SUCCESS;
}

static int _clear_index(int index_id)
{
    l_grib_index* current  = index_set;

    while(current){
        if(current->id == index_id){
            current->id = -(current->id);
            if (current->h) {
                grib_index_delete(current->h);
                return GRIB_SUCCESS;
            }
        }
        current = current->next;
    }
    return GRIB_SUCCESS;
}

#if 0
static int _clear_multi_handle(int multi_handle_id)
{
    l_grib_multi_handle* current  = multi_handle_set;

    while(current){
        if(current->id == multi_handle_id){
            current->id = -(current->id);
            if(current->h) return grib_multi_handle_delete(current->h);
        }
        current = current->next;
    }
    return GRIB_SUCCESS;
}
#endif

static int clear_handle(int handle_id)
{
    int ret=0;
    GRIB_MUTEX_INIT_ONCE(&once,&init);
    GRIB_MUTEX_LOCK(&handle_mutex);
    ret=_clear_handle(handle_id);
    GRIB_MUTEX_UNLOCK(&handle_mutex);
    return ret;
}

static int clear_index(int index_id)
{
    int ret=0;
    GRIB_MUTEX_INIT_ONCE(&once,&init);
    GRIB_MUTEX_LOCK(&index_mutex);
    ret=_clear_index(index_id);
    GRIB_MUTEX_UNLOCK(&index_mutex);
    return ret;
}

#if 0
static int clear_multi_handle(int multi_handle_id)
{
    int ret=0;
    GRIB_MUTEX_INIT_ONCE(&once,&init);
    GRIB_MUTEX_LOCK(&multi_handle_mutex);
    ret=_clear_multi_handle(multi_handle_id);
    GRIB_MUTEX_UNLOCK(&multi_handle_mutex);
    return ret;
}
#endif

static int _clear_keys_iterator(int keys_iterator_id)
{
    l_grib_keys_iterator* current  = keys_iterator_set;

    while(current){
        if(current->id == keys_iterator_id){
            current->id = -(current->id);
            return grib_keys_iterator_delete(current->i);
        }
        current = current->next;
    }
    return GRIB_INVALID_KEYS_ITERATOR;
}
static int clear_keys_iterator(int keys_iterator_id)
{
    int ret=0;
    GRIB_MUTEX_INIT_ONCE(&once,&init);
    GRIB_MUTEX_LOCK(&keys_iterator_mutex);
    ret=_clear_keys_iterator(keys_iterator_id);
    GRIB_MUTEX_UNLOCK(&keys_iterator_mutex);
    return ret;
}

/* BUFR */
static int _clear_bufr_keys_iterator(int keys_iterator_id)
{
    l_bufr_keys_iterator* current  = bufr_keys_iterator_set;

    while(current){
        if(current->id == keys_iterator_id){
            current->id = -(current->id);
            return codes_bufr_keys_iterator_delete(current->i);
        }
        current = current->next;
    }
    return GRIB_INVALID_KEYS_ITERATOR;
}
static int clear_bufr_keys_iterator(int keys_iterator_id)
{
    int ret=0;
    GRIB_MUTEX_INIT_ONCE(&once,&init);
    GRIB_MUTEX_LOCK(&keys_iterator_mutex);
    ret=_clear_bufr_keys_iterator(keys_iterator_id);
    GRIB_MUTEX_UNLOCK(&keys_iterator_mutex);
    return ret;
}

/*****************************************************************************/
#if 0
int grib_f_read_any_headers_only_from_file_(int* fid, char* buffer, size_t* nbytes)
{
    grib_context* c;
    int err=0;
    FILE* f=get_file(*fid);

    if (f) {
        c=grib_context_get_default( );
        err=grib_read_any_headers_only_from_file(c,f,buffer,nbytes);
        return err;
    } else {
        return GRIB_INVALID_FILE;
    }
}
#endif

/*****************************************************************************/
int grib_f_read_any_from_file_(int* fid, void* buffer, size_t* nbytes)
{
    FILE* f = get_file(*fid);

    if (f) {
        grib_context* c = grib_context_get_default( );
        int err=grib_read_any_from_file(c,f,buffer,nbytes);
        return err;
    } else {
        return GRIB_INVALID_FILE;
    }
}

/*****************************************************************************/
int grib_f_write_file_(int* fid, void* buffer, size_t* nbytes)
{
    grib_context* c;
    FILE* f=get_file(*fid);

    if (f) {
        int ioerr;
        c=grib_context_get_default( );
        if( fwrite(buffer, 1, *nbytes, f)  != *nbytes) {
            ioerr=errno;
            grib_context_log(c,(GRIB_LOG_ERROR)|(GRIB_LOG_PERROR),"IO ERROR: %s",strerror(ioerr));
            return GRIB_IO_PROBLEM;
        }
        return GRIB_SUCCESS;
    } else {
        return GRIB_INVALID_FILE;
    }
}
/*****************************************************************************/
int grib_f_get_message_(int* gid, void** mess, size_t* mess_len)
{
    const void *message = NULL;
    int iret = 0;
    grib_handle *h = get_handle(*gid);
    if (!h) return GRIB_INVALID_GRIB;
    iret = grib_get_message(h,&message,mess_len);
    if(iret != 0){
        return iret;
    }
    *mess = (void*) message;
    return GRIB_SUCCESS;
}


/*****************************************************************************/
int grib_f_read_file_(int* fid, void* buffer, size_t* nbytes)
{
    grib_context* c;
    FILE* f=get_file(*fid);

    if (f) {
        int ioerr;
        c=grib_context_get_default( );
        if( fread(buffer, 1, *nbytes, f)  != *nbytes) {
            ioerr=errno;
            grib_context_log(c,(GRIB_LOG_ERROR)|(GRIB_LOG_PERROR),"IO ERROR: %s",strerror(ioerr));
            return GRIB_IO_PROBLEM;
        }
        return GRIB_SUCCESS;
    } else {
        return GRIB_INVALID_FILE;
    }
}

/*****************************************************************************/
int grib_f_open_file_(int* fid, char* name , char* op, int lname, int lop)
{
    FILE* f = NULL;
    int ioerr=0;
    char oper[1024]; /* GRIB-576: open mode */
    char *p;
    char fname[1024];
    int ret=GRIB_SUCCESS;
    char* iobuf=NULL;
    char* trimmed = NULL; /* filename trimmed */
    grib_context* context=grib_context_get_default();

    cast_char(oper,op,lop);

    p=oper;

    while (*p != '\0') { *p=tolower(*p);p++;}

    trimmed = cast_char_no_cut(fname,name,lname); /* ECC-1392 */
    string_rtrim( trimmed );
    f = fopen(trimmed, oper);
    if(!f) {
        ioerr=errno;
        grib_context_log(context,(GRIB_LOG_ERROR)|(GRIB_LOG_PERROR),"IO ERROR: %s: '%s'",strerror(ioerr),trimmed);
        *fid = -1;
        ret=GRIB_IO_PROBLEM;
    } else {
        if (context->io_buffer_size) {
            /* printf("setting vbuf = %d\n",context->io_buffer_size); */
#ifdef POSIX_MEMALIGN
            if (posix_memalign((void **)&iobuf,sysconf(_SC_PAGESIZE),context->io_buffer_size) ) {
                grib_context_log(context,GRIB_LOG_FATAL,"grib_f_open_file_: posix_memalign unable to allocate io_buffer\n");
            }
#else
            iobuf = (void*)malloc(context->io_buffer_size);
            if (!iobuf) {
                grib_context_log(context,GRIB_LOG_FATAL,"grib_f_open_file_: Unable to allocate io_buffer\n");
            }
#endif
            setvbuf(f,iobuf,_IOFBF,context->io_buffer_size);
        }
        *fid = push_file(f, oper, iobuf);
        ret=GRIB_SUCCESS;
    }
    return ret;
}

/*****************************************************************************/
int grib_f_close_file_(int* fid)
{
    return clear_file(*fid);
}

/*****************************************************************************/
static int file_count=0;

void grib_f_write_on_fail(int* gid)
{
    grib_context* c = grib_context_get_default();
    if (c->write_on_fail) {
        char filename[100]={0,};
        grib_handle* h=NULL;
        pid_t pid=getpid();

        GRIB_MUTEX_INIT_ONCE(&once,&init)
        GRIB_MUTEX_LOCK(&handle_mutex)
        file_count++;
        GRIB_MUTEX_UNLOCK(&handle_mutex)

        snprintf(filename, sizeof(filename), "%ld_%d_error.grib",(long)pid,file_count);

        h=get_handle(*gid);
        if (h) grib_write_message(h,filename,"w");
    }
}
void grib_f_write_on_fail_(int* gid)
{
    grib_f_write_on_fail(gid);
}
/*****************************************************************************/
int grib_f_multi_support_on_(void)
{
    grib_multi_support_on(0);
    return GRIB_SUCCESS;
}

int grib_f_multi_support_off_(void)
{
    grib_multi_support_off(0);
    return GRIB_SUCCESS;
}

/*****************************************************************************/
#ifdef FORTRAN_GEOITERATOR_SUPPORT
static int _push_iterator(grib_iterator *i)
{
    l_grib_iterator* current  = iterator_set;
    l_grib_iterator* previous = iterator_set;
    l_grib_iterator* the_new      = NULL;
    int myindex = 1;

    if(!iterator_set){
        iterator_set = (l_grib_iterator*)malloc(sizeof(l_grib_iterator));
        ECCODES_ASSERT(iterator_set);
        iterator_set->id   = myindex;
        iterator_set->i    = i;
        iterator_set->next = NULL;
        return myindex;
    }

    while(current){
        if(current->id < 0){
            current->id = -(current->id);
            current->i  = i;
            return current->id;
        }
        else{
            myindex++;
            previous = current;
            current = current->next;
        }
    }

    the_new = (l_grib_iterator*)malloc(sizeof(l_grib_iterator));
    ECCODES_ASSERT(the_new);
    the_new->id   = myindex;
    the_new->i    = i;
    the_new->next = current;
    previous->next = the_new;

    return myindex;
}
static int push_iterator(grib_iterator *i)
{
    int ret=0;
    GRIB_MUTEX_INIT_ONCE(&once,&init);
    GRIB_MUTEX_LOCK(&iterator_mutex);
    ret=_push_iterator(i);
    GRIB_MUTEX_UNLOCK(&iterator_mutex);
    return ret;
}
static grib_iterator* _get_iterator(int iterator_id)
{
    l_grib_iterator* current  = iterator_set;

    while(current){
        if(current->id == iterator_id) return current->i;
        current = current->next;
    }
    return NULL;
}
static grib_iterator* get_iterator(int iterator_id)
{
    grib_iterator* i=NULL;
    GRIB_MUTEX_INIT_ONCE(&once,&init);
    GRIB_MUTEX_LOCK(&iterator_mutex);
    i=_get_iterator(iterator_id);
    GRIB_MUTEX_UNLOCK(&iterator_mutex);
    return i;
}
static int _clear_iterator(int iterator_id)
{
    l_grib_iterator* current  = iterator_set;

    while(current){
        if(current->id == iterator_id){
            current->id = -(current->id);
            return grib_iterator_delete(current->i);
        }
        current = current->next;
    }
    return GRIB_INVALID_ITERATOR;
}
static int clear_iterator(int iterator_id)
{
    int ret=0;
    GRIB_MUTEX_INIT_ONCE(&once,&init);
    GRIB_MUTEX_LOCK(&iterator_mutex);
    ret=_clear_iterator(iterator_id);
    GRIB_MUTEX_UNLOCK(&iterator_mutex);
    return ret;
}
static int _grib_f_iterator_new_(int* gid,int* iterid,int* mode) {
    int err=0;
    grib_handle* h;
    grib_iterator* iter;

    h=get_handle(*gid);
    if (!h) {
        *iterid=-1;
        return GRIB_NULL_HANDLE;
    }
    iter=grib_iterator_new(h,*mode,&err);
    if (iter)
        *iterid=push_iterator(iter);
    else
        *iterid=-1;
    return err;
}
int grib_f_iterator_new_(int* gid,int* iterid,int* mode) {
    int ret=0;
    GRIB_MUTEX_INIT_ONCE(&once,&init)
    GRIB_MUTEX_LOCK(&iterator_mutex)
    ret=_grib_f_iterator_new_(gid,iterid,mode);
    GRIB_MUTEX_UNLOCK(&iterator_mutex)
    return ret;
}
/*****************************************************************************/
int grib_f_iterator_next_(int* iterid,double* lat,double* lon,double* value) {
    grib_iterator* iter=get_iterator(*iterid);
    if (!iter) return GRIB_INVALID_ITERATOR;
    return grib_iterator_next(iter,lat,lon,value);
}
/*****************************************************************************/
int grib_f_iterator_delete_(int* iterid) {
    return clear_iterator(*iterid);
}
#endif /*FORTRAN_GEOITERATOR_SUPPORT*/

/*****************************************************************************/
static int _grib_f_keys_iterator_new_(int* gid,int* iterid,char* name_space,int len)
{
    char buf[1024];
    grib_handle* h;
    grib_keys_iterator* iter;

    h=get_handle(*gid);
    if (!h) {
        *iterid=-1;
        return GRIB_NULL_HANDLE;
    }
    iter=grib_keys_iterator_new(h,0,cast_char(buf,name_space,len));
    if (iter)
        *iterid=push_keys_iterator(iter);
    else
        *iterid=-1;
    return GRIB_SUCCESS;
}
int grib_f_keys_iterator_new_(int* gid,int* iterid,char* name_space,int len)
{
    int ret=0;
    GRIB_MUTEX_INIT_ONCE(&once,&init)
    GRIB_MUTEX_LOCK(&keys_iterator_mutex)
    ret=_grib_f_keys_iterator_new_(gid,iterid,name_space,len);
    GRIB_MUTEX_UNLOCK(&keys_iterator_mutex)
    return ret;
}

/*****************************************************************************/
int grib_f_keys_iterator_next_(int* iterid)
{
    grib_keys_iterator* iter=get_keys_iterator(*iterid);
    if (!iter) return GRIB_INVALID_KEYS_ITERATOR;

    return grib_keys_iterator_next(iter);
}

/*****************************************************************************/
int grib_f_keys_iterator_delete_(int* iterid)
{
    return clear_keys_iterator(*iterid);
}

/*****************************************************************************/
int grib_f_gribex_mode_on_(void)
{
    grib_gribex_mode_on(0);
    return GRIB_SUCCESS;
}

int grib_f_gribex_mode_off_(void)
{
    grib_gribex_mode_off(0);
    return GRIB_SUCCESS;
}

/*****************************************************************************/
int grib_f_skip_computed_(int* iterid)
{
    grib_keys_iterator* iter=get_keys_iterator(*iterid);
    if (!iter) return GRIB_INVALID_KEYS_ITERATOR;
    return grib_keys_iterator_set_flags(iter,GRIB_KEYS_ITERATOR_SKIP_COMPUTED);
}

int grib_f_skip_coded_(int* iterid)
{
    grib_keys_iterator* iter=get_keys_iterator(*iterid);
    if (!iter) return GRIB_INVALID_KEYS_ITERATOR;
    return grib_keys_iterator_set_flags(iter,GRIB_KEYS_ITERATOR_SKIP_CODED);
}

int grib_f_skip_edition_specific_(int* iterid)
{
    grib_keys_iterator* iter=get_keys_iterator(*iterid);
    if (!iter) return GRIB_INVALID_KEYS_ITERATOR;
    return grib_keys_iterator_set_flags(iter,GRIB_KEYS_ITERATOR_SKIP_EDITION_SPECIFIC);
}

int grib_f_skip_duplicates_(int* iterid)
{
    grib_keys_iterator* iter=get_keys_iterator(*iterid);
    if (!iter) return GRIB_INVALID_KEYS_ITERATOR;
    return grib_keys_iterator_set_flags(iter,GRIB_KEYS_ITERATOR_SKIP_DUPLICATES);
}

int grib_f_skip_read_only_(int* iterid)
{
    grib_keys_iterator* iter=get_keys_iterator(*iterid);
    if (!iter) return GRIB_INVALID_KEYS_ITERATOR;
    return grib_keys_iterator_set_flags(iter,GRIB_KEYS_ITERATOR_SKIP_READ_ONLY);
}

int grib_f_skip_function_(int* iterid)
{
    grib_keys_iterator* iter=get_keys_iterator(*iterid);
    if (!iter) return GRIB_INVALID_KEYS_ITERATOR;
    return grib_keys_iterator_set_flags(iter,GRIB_KEYS_ITERATOR_SKIP_FUNCTION);
}

/*****************************************************************************/
int grib_f_keys_iterator_get_name_(int* iterid,char* name,int len)
{
    size_t input_len = len;
    size_t lsize = len;
    char buf[1024]={0,};

    grib_keys_iterator* kiter=get_keys_iterator(*iterid);

    if (!kiter) return GRIB_INVALID_KEYS_ITERATOR;

    fort_char_clean(name, len);

    snprintf(buf, sizeof(buf), "%s",grib_keys_iterator_get_name(kiter));
    lsize = strlen(buf);
    if (input_len < lsize) return GRIB_ARRAY_TOO_SMALL;

    memcpy(name, buf, lsize);

    czstr_to_fortran(name, len);

    return 0;
}

/*****************************************************************************/
int grib_f_keys_iterator_rewind_(int* kiter)
{
    grib_keys_iterator* i=get_keys_iterator(*kiter);

    if (!i) return GRIB_INVALID_KEYS_ITERATOR;
    return grib_keys_iterator_rewind(i);
}

/* BUFR keys iterator */
/*****************************************************************************/
static int _codes_f_bufr_keys_iterator_new_(int* gid,int* iterid)
{
    grib_handle* h;
    bufr_keys_iterator* iter;

    h=get_handle(*gid);
    if (!h) {
        *iterid=-1;
        return GRIB_NULL_HANDLE;
    }
    ECCODES_ASSERT(h->product_kind==PRODUCT_BUFR);
    iter=codes_bufr_keys_iterator_new(h,0);
    if (iter)
        *iterid=push_bufr_keys_iterator(iter);
    else
        *iterid=-1;
    return GRIB_SUCCESS;
}
int codes_f_bufr_keys_iterator_new_(int* gid,int* iterid)
{
    int ret=0;
    GRIB_MUTEX_INIT_ONCE(&once,&init)
    GRIB_MUTEX_LOCK(&keys_iterator_mutex)
    ret=_codes_f_bufr_keys_iterator_new_(gid,iterid);
    GRIB_MUTEX_UNLOCK(&keys_iterator_mutex)
    return ret;
}
/*****************************************************************************/
int codes_f_bufr_keys_iterator_next_(int* iterid)
{
    bufr_keys_iterator* iter=get_bufr_keys_iterator(*iterid);
    if (!iter) return GRIB_INVALID_KEYS_ITERATOR;

    return codes_bufr_keys_iterator_next(iter);
}

/*****************************************************************************/
int codes_f_bufr_keys_iterator_get_name_(int* iterid, char* name, int len)
{
    size_t input_len = len;
    size_t lsize = len;
    char buf[1024] = {0,};

    bufr_keys_iterator* kiter = get_bufr_keys_iterator(*iterid);

    if (!kiter) return GRIB_INVALID_KEYS_ITERATOR;

    fort_char_clean(name, len);

    snprintf(buf, sizeof(buf), "%s", codes_bufr_keys_iterator_get_name(kiter));
    lsize = strlen(buf);
    if (input_len < lsize) return GRIB_ARRAY_TOO_SMALL;

    memcpy(name, buf, lsize);

    czstr_to_fortran(name, len);

    return 0;
}
/*****************************************************************************/
int codes_f_bufr_keys_iterator_rewind_(int* kiter)
{
    bufr_keys_iterator* i=get_bufr_keys_iterator(*kiter);

    if (!i) return GRIB_INVALID_KEYS_ITERATOR;
    return codes_bufr_keys_iterator_rewind(i);
}

/*****************************************************************************/
int codes_f_bufr_keys_iterator_delete_(int* iterid)
{
    return clear_bufr_keys_iterator(*iterid);
}

/*****************************************************************************/
int grib_f_new_from_message_(int* gid, void* buffer, size_t* bufsize)
{
    grib_handle *h = NULL;
    h = grib_handle_new_from_message_copy(0, buffer, *bufsize);
    if (h){
        push_handle(h,gid);
        return GRIB_SUCCESS;
    }
    *gid = -1;
    return  GRIB_INTERNAL_ERROR;
}
/*****************************************************************************/

/* See SUP-3893: Need to provide an 'int' version */
int grib_f_new_from_message_int_(int* gid, int* buffer , size_t* bufsize)
{
    /* Call the version with void pointer */
    return grib_f_new_from_message_(gid, (void*)buffer, bufsize);
}

/*****************************************************************************/
int grib_f_new_from_message_no_copy_(int* gid, void* buffer, size_t* bufsize)
{
    grib_handle *h = NULL;
    h = grib_handle_new_from_message(0, buffer, *bufsize);
    if (h){
        push_handle(h,gid);
        return GRIB_SUCCESS;
    }
    *gid = -1;
    return  GRIB_INTERNAL_ERROR;
}

/*****************************************************************************/
int grib_f_new_from_message_no_copy_int_(int* gid, int* buffer, size_t* bufsize)
{
    return grib_f_new_from_message_no_copy_(gid, (void*)buffer, bufsize);
}

#if 0
int grib_f_new_from_message_copy_(int* gid, void* buffer, size_t* bufsize)
{
    grib_handle *h = NULL;
    h = grib_handle_new_from_message_copy(0, buffer, *bufsize);
    if(h){
        push_handle(h,gid);
        return GRIB_SUCCESS;
    }
    *gid = -1;
    return  GRIB_INTERNAL_ERROR;
}
#endif
/*****************************************************************************/
int grib_f_new_from_samples_(int* gid, char* name, int lname)
{
    char fname[1024];
    grib_handle *h = grib_handle_new_from_samples(NULL,cast_char(fname,name,lname));
    /*   grib_context_set_debug(h->context,1);*/

    if(h){
        push_handle(h,gid);
        return GRIB_SUCCESS;
    }
    *gid = -1;
    return  GRIB_FILE_NOT_FOUND;
}

/*****************************************************************************/
int codes_bufr_f_new_from_samples_(int* gid, char* name, int lname)
{
    char fname[1024];
    grib_handle* h = codes_bufr_handle_new_from_samples(NULL,cast_char(fname,name,lname));
    /*   grib_context_set_debug(h->context,1);*/

    if(h){
        push_handle(h,gid);
        return GRIB_SUCCESS;
    }
    *gid = -1;
    return  GRIB_FILE_NOT_FOUND;
}

/*****************************************************************************/
int grib_f_clone_(int* gidsrc,int* giddest)
{
    grib_handle *src  = get_handle(*gidsrc);
    grib_handle *dest = NULL;

    if(src){
        dest = grib_handle_clone(src);
        if(dest){
            push_handle(dest,giddest);
            return GRIB_SUCCESS;
        }
    }

    *giddest = -1;
    return GRIB_INVALID_GRIB;
}

/*****************************************************************************/
int grib_f_copy_key_(int* gidsrc, char* key, int* giddest, int len)
{
    grib_handle *src  = get_handle(*gidsrc);
    grib_handle *dest = get_handle(*giddest);

    if(src && dest) {
        char buf[1024]={0,};
        char* ckey = (char*)key;
        const int type = GRIB_TYPE_UNDEFINED; /* will be computed */
        return codes_copy_key(src, dest, cast_char(buf,ckey,len), type);
    }

    return GRIB_INVALID_GRIB;
}

/*****************************************************************************/
int grib_f_util_sections_copy_(int* gidfrom, int* gidto, int* what, int* gidout)
{
    int err = 0;
    grib_handle* hfrom = get_handle(*gidfrom);
    grib_handle* hto   = get_handle(*gidto);
    grib_handle* out   = 0;

    if (hfrom && hto) out = grib_util_sections_copy(hfrom, hto, *what, &err);
    if (out) {
        push_handle(out, gidout);
        return GRIB_SUCCESS;
    }

    return err;
}

/*****************************************************************************/
int grib_f_copy_namespace_(int* gidsrc,char* name,int* giddest,int len)
{
    char buf[1024]={0,};
    grib_handle *src  = get_handle(*gidsrc);
    grib_handle *dest = get_handle(*giddest);

    if(src && dest)
        return grib_copy_namespace(dest,cast_char(buf,name,len),src);

    return GRIB_INVALID_GRIB;
}

/*****************************************************************************/
int any_f_scan_file_(int* fid, int* n)
{
    int err = 0;
    off_t offset=0;
    void *data = NULL;
    size_t olen = 0;
    l_message_info* msg=0;
    FILE* f = get_file(*fid);
    grib_context* c=grib_context_get_default();

    /* this needs a callback to a destructor*/
    /* grib_oarray_delete_content(c, info_messages); */

    grib_oarray_delete(info_messages);
    info_messages=grib_oarray_new(1000, 1000);

    if (f) {
        while (err!=GRIB_END_OF_FILE) {
            data = wmo_read_any_from_file_malloc ( f, 0, &olen, &offset, &err );
            msg=(l_message_info*)grib_context_malloc_clear(c,sizeof(l_message_info));
            msg->offset = offset;
            msg->size = olen;

            if (err == 0 && data) grib_oarray_push(info_messages, msg);
            grib_context_free(c, data);
        }
        if (err == GRIB_END_OF_FILE) err = 0;
    }
    *n = info_messages->n;
    return err;
}

/*****************************************************************************/
int any_f_new_from_scanned_file_(int* fid, int* msgid, int* gid)
{
    grib_handle *h = NULL;
    grib_context* c = grib_context_get_default();
    int err = 0;
    FILE* f = get_file(*fid);
    l_message_info* msg = NULL;

    /* fortran convention of 1-based index */
    const int n = *msgid - 1;

    if (info_messages == NULL) {
        return GRIB_INVALID_ARGUMENT;
    }
    if (*msgid < 1 || (size_t)*msgid > info_messages->n) {
        return GRIB_INVALID_ARGUMENT;
    }

    msg = (l_message_info*)grib_oarray_get(info_messages, n);

    if (msg && f) {
        GRIB_MUTEX_INIT_ONCE(&once, &init);
        GRIB_MUTEX_LOCK(&read_mutex);
        fseeko(f, msg->offset, SEEK_SET);
        h = any_new_from_file (c, f, &err);
        GRIB_MUTEX_UNLOCK(&read_mutex);
    }
    if (err) return err;

    if (h) {
        push_handle(h, gid);
        return GRIB_SUCCESS;
    } else {
        *gid = -1;
        return GRIB_END_OF_FILE;
    }
}

/*****************************************************************************/
int any_f_load_all_from_file_(int* fid, int* n)
{
    int err = 0;
    off_t offset=0;
    void* data = NULL;
    size_t olen = 0;
    l_binary_message* msg=0;
    FILE* f = get_file(*fid);
    grib_context* c = grib_context_get_default();

    /* this needs a callback to a destructor*/
    /* grib_oarray_delete_content(c, binary_messages); */

    grib_oarray_delete(binary_messages);
    binary_messages = grib_oarray_new(1000, 1000);

    if (f) {
      while (err != GRIB_END_OF_FILE) {
        data = wmo_read_any_from_file_malloc (f, 0,&olen, &offset, &err);
        msg = (l_binary_message*)grib_context_malloc_clear(c,sizeof(l_binary_message));
        msg->data = data;
        msg->size = olen;

        if (err == 0 && data) grib_oarray_push(binary_messages, msg);
      }
      if (err == GRIB_END_OF_FILE) err = 0;
    }
    *n = binary_messages->n;
    return err;
}

/*****************************************************************************/
int any_f_new_from_loaded_(int* msgid, int* gid)
{
    grib_handle* h  = NULL;
    grib_context* c = grib_context_get_default();

    /* fortran convention of 1 based index*/
    const int n = *msgid - 1;

    l_binary_message* msg = (l_binary_message*)grib_oarray_get(binary_messages, n);

    if (msg && msg->data)
        h = grib_handle_new_from_message_copy(c, msg->data, msg->size);

    if (h) {
        push_handle(h, gid);
        return GRIB_SUCCESS;
    }
    else {
        *gid = -1;
        return GRIB_END_OF_FILE;
    }
}

/*****************************************************************************/
int codes_f_clear_loaded_from_file_(void)
{
    /* grib_oarray_delete_content(c,binary_messages); */
    grib_oarray_delete(binary_messages);
    return GRIB_SUCCESS;
}

/*****************************************************************************/
int grib_f_count_in_file_(int* fid,int* n)
{
    int err = 0;
    FILE* f = get_file(*fid);
    if (f) err = grib_count_in_file(0, f, n);
    return err;
}

/*****************************************************************************/
int any_f_new_from_file_(int* fid, int* gid)
{
    int err        = 0;
    FILE* f        = get_file(*fid);
    grib_handle* h = NULL;

    if (f) {
        h = codes_handle_new_from_file(0, f, PRODUCT_ANY, &err);
        if (h) {
            push_handle(h, gid);
            return GRIB_SUCCESS;
        }
        else {
            *gid = -1;
            return GRIB_END_OF_FILE;
        }
    }
    *gid = -1;
    return GRIB_INVALID_FILE;
}

/*****************************************************************************/
int bufr_f_new_from_file_(int* fid, int* gid)
{
    int err = 0;
    FILE* f = get_file(*fid);

    grib_handle *h = NULL;

    if(f){
        h = codes_handle_new_from_file(0,f,PRODUCT_BUFR,&err);
        if(h){
            push_handle(h,gid);
            return GRIB_SUCCESS;
        } else {
            *gid=-1;
            return GRIB_END_OF_FILE;
        }
    }

    *gid = -1;
    return GRIB_INVALID_FILE;
}

/*****************************************************************************/
int grib_f_new_from_file_(int* fid, int* gid)
{
    int err = 0;
    FILE* f = get_file(*fid);

    grib_handle *h = NULL;

    if(f){
        h = grib_handle_new_from_file(0,f,&err);
        if(h){
            push_handle(h,gid);
            return GRIB_SUCCESS;
        } else {
            *gid=-1;
            return GRIB_END_OF_FILE;
        }
    }

    *gid=-1;
    return GRIB_INVALID_FILE;
}

/*****************************************************************************/
int grib_f_headers_only_new_from_file_(int* fid, int* gid)
{
    int err = 0;
    FILE* f = get_file(*fid);
    grib_handle *h = NULL;
    const int headers_only = 1;

    if (f){
        h=grib_new_from_file (0, f, headers_only, &err);
        if (h){
            push_handle(h,gid);
            return GRIB_SUCCESS;
        } else {
            *gid = -1;
            return GRIB_END_OF_FILE;
        }
    }

    *gid = -1;
    return GRIB_INVALID_FILE;
}

/*****************************************************************************/
int grib_f_new_from_index_(int* index_id, int* gid)
{
    int err = 0;
    grib_index* i = get_index(*index_id);
    grib_handle *h = NULL;

    if (i) {
        h = grib_handle_new_from_index(i, &err);
        if (h){
            push_handle(h,gid);
            return GRIB_SUCCESS;
        } else {
            *gid = -1;
            return GRIB_END_OF_INDEX;
        }
    }

    *gid = -1;
    return GRIB_INVALID_INDEX;
}

/*****************************************************************************/
int grib_f_index_new_from_file_(char* file, char* keys, int* gid, int lfile, int lkeys)
{
    int err = 0;
    char fname[1024] = {0,};
    char knames[1024] = {0,};
    grib_index *i = NULL;

    if (*file){
        i = grib_index_new_from_file(0,cast_char(fname,file,lfile),
                cast_char(knames,keys,lkeys),&err);
        if (i) {
            push_index(i,gid);
            return GRIB_SUCCESS;
        } else {
            *gid=-1;
            return GRIB_END_OF_FILE;
        }
    }

    *gid = -1;
    return GRIB_INVALID_FILE;
}

/*****************************************************************************/
int grib_f_index_add_file_(int* index_id, char* file, int lfile)
{
    grib_index *i = get_index(*index_id);
    char buf[1024];

    if (!i) {
        return GRIB_INVALID_INDEX;
    } else {
        return grib_index_add_file(i,cast_char(buf,file,lfile));
    }
}

/*****************************************************************************/
int grib_f_index_read_(char* file, int* gid, int lfile)
{
    int err = 0;
    char fname[1024]={0,};

    grib_index *i = NULL;

    if (*file) {
        i = grib_index_read(0,cast_char(fname,file,lfile),&err);
        if (i) {
            push_index(i,gid);
            return GRIB_SUCCESS;
        } else {
            *gid = -1;
            return GRIB_END_OF_FILE;
        }
    }

    *gid=-1;
    return GRIB_INVALID_FILE;
}

/*****************************************************************************/
int grib_f_index_write_(int* gid, char* file, int lfile)
{
    grib_index *i = get_index(*gid);
    char buf[1024];

    if (!i) {
        return GRIB_INVALID_GRIB;
    } else {
        return grib_index_write(i,cast_char(buf,file,lfile));
    }
}

/*****************************************************************************/
int grib_f_index_release_(int* hid)
{
    return clear_index(*hid);
}

/* int grib_f_multi_handle_release_(int* hid){
    return clear_multi_handle(*hid);
} */

int grib_f_release_(int* hid)
{
    return clear_handle(*hid);
}

/*****************************************************************************/
static void do_the_dump(grib_handle* h)
{
    /* Add some debugging info too */
    printf("ecCodes version: ");    grib_print_api_version(stdout);   printf("\n");
    printf("Definitions path: %s\n", grib_definition_path(NULL));
    printf("Samples path:     %s\n", grib_samples_path(NULL));

    if (h->product_kind == PRODUCT_GRIB)
    {
        const int dump_flags = GRIB_DUMP_FLAG_VALUES
                |  GRIB_DUMP_FLAG_READ_ONLY
                |  GRIB_DUMP_FLAG_ALIASES
                |  GRIB_DUMP_FLAG_TYPE;
        grib_dump_content(h,stdout, "debug", dump_flags, NULL);
    }
    else
    {
        const int dump_flags = GRIB_DUMP_FLAG_CODED | GRIB_DUMP_FLAG_OCTET | GRIB_DUMP_FLAG_VALUES | GRIB_DUMP_FLAG_READ_ONLY;
        grib_dump_content(h,stdout, "wmo", dump_flags, NULL);
    }
}
int grib_f_dump_(int* gid)
{
    grib_handle *h = get_handle(*gid);
    if(!h)
        return GRIB_INVALID_GRIB;
    else
        do_the_dump(h);

    return GRIB_SUCCESS;
}

/*****************************************************************************/
#ifdef USE_GRIB_PRINT
int grib_f_print_(int* gid, char* key, int len)
{
    grib_handle *h = get_handle(*gid);
    int err = GRIB_SUCCESS;
    grib_dumper* d = NULL;
    char buf[1024];

    if(!h){
        return GRIB_INVALID_GRIB;
    }else{
        d = grib_dumper_factory("file",h,stdout,0,0);
        err = grib_print(h, cast_char(buf,key,len), d);
        grib_dumper_delete(d);
        return  err;
    }
}
#endif
/*****************************************************************************/
int grib_f_get_error_string_(int* err, char* buf, int len)
{
    const char* err_msg = grib_get_error_message(*err);
    const size_t erlen = strlen(err_msg);
    if ( (size_t)len <  erlen) return GRIB_ARRAY_TOO_SMALL;
    strncpy(buf, err_msg, (size_t)erlen); /* ECC-1488 */
    return GRIB_SUCCESS;
}

/*****************************************************************************/
int grib_f_get_api_version_(int* apiVersion,int len)
{
    *apiVersion = grib_get_api_version();
    return GRIB_SUCCESS;
}

/*****************************************************************************/
int grib_f_get_size_int_(int* gid, char* key, int* val, int len)
{
    grib_handle* h = get_handle(*gid);

    char buf[1024];
    size_t tsize = 0;

    if (!h) return GRIB_INVALID_GRIB;

    int err = grib_get_size(h, cast_char(buf, key, len), &tsize);
    *val    = tsize;
    return err;
}

int grib_f_get_size_long_(int* gid, char* key, long* val, int len)
{
    grib_handle *h = get_handle(*gid);

    char buf[1024];
    size_t tsize = 0;

    if (!h) return GRIB_INVALID_GRIB;

    int err = grib_get_size(h, cast_char(buf,key,len), &tsize);
    *val = tsize;
    return  err;
}

/*****************************************************************************/
int grib_f_index_get_size_int_(int* index_id, char* key, int* val, int len)
{
    grib_index *h = get_index(*index_id);

    char buf[1024];
    size_t tsize = 0;

    if (!h) return GRIB_INVALID_GRIB;

    int err = grib_index_get_size(h, cast_char(buf,key,len), &tsize);
    *val = tsize;
    return  err;
}

int grib_f_index_get_size_long_(int* index_id, char* key, long* val, int len)
{
    grib_index *h = get_index(*index_id);
    char buf[1024];
    size_t tsize = 0;

    if (!h) return GRIB_INVALID_GRIB;

    int err = grib_index_get_size(h, cast_char(buf,key,len), &tsize);
    *val = tsize;
    return err;
}

/*****************************************************************************/
int grib_f_get_int_(int* gid, char* key, int* val, int len)
{
    grib_handle *h = get_handle(*gid);
    long long_val;
    char buf[1024];

    if (!h) return GRIB_INVALID_GRIB;
    int err = grib_get_long(h, cast_char(buf,key,len),&long_val);
    *val = long_val;
    return err;
}

int grib_f_get_long_(int* gid, char* key, long* val, int len)
{
    grib_handle *h = get_handle(*gid);

    char buf[1024];

    if (!h) return GRIB_INVALID_GRIB;
    int err = grib_get_long(h, cast_char(buf,key,len),val);
    return err;
}

/*****************************************************************************/
int grib_f_get_native_type_(int* gid, char* key, int* val, int len)
{
    grib_handle *h = get_handle(*gid);
    int type_val = 0;
    char buf[1024];

    if (!h) return GRIB_INVALID_GRIB;
    int err = grib_get_native_type(h, cast_char(buf,key,len), &type_val);
    *val = type_val;
    return err;
}

/*****************************************************************************/
int grib_f_get_int_array_(int* gid, char* key, int *val, int* size, int len)
{
    grib_handle *h = get_handle(*gid);
    long* long_val = NULL;
    int err = GRIB_SUCCESS;
    char buf[1024];
    size_t lsize = *size;

    if (!h)  return GRIB_INVALID_GRIB;

    if (sizeof(long) == sizeof(int)){
        long_val = (long*)val;
        err = grib_get_long_array(h, cast_char(buf,key,len), long_val, &lsize);
        *size = lsize;
        return  err;
    }
    if (*size)
        long_val = (long*)grib_context_malloc(h->context,(*size)*(sizeof(long)));
    else
        long_val = (long*)grib_context_malloc(h->context,(sizeof(long)));

    if (!long_val) return GRIB_OUT_OF_MEMORY;

    err = grib_get_long_array(h, cast_char(buf,key,len), long_val, &lsize);

    for (size_t i=0; i<lsize; ++i) {
        val[i] = long_val[i];
    }
    *size = lsize;

    grib_context_free(h->context,long_val);
    return  err;
}
/*****************************************************************************/
int grib_f_get_long_array_(int* gid, char* key, long *val, int* size, int len)
{
    grib_handle *h = get_handle(*gid);

    char buf[1024];
    size_t lsize = *size;

    if(!h)  return GRIB_INVALID_GRIB;

    int err = grib_get_long_array(h, cast_char(buf,key,len), val, &lsize);
    *size=lsize;

    return  err;
}

/*****************************************************************************/
int grib_f_get_byte_array_(int* gid, char* key, unsigned char *val, int* size, int len, int lenv)
{
    grib_handle *h = get_handle(*gid);

    char buf[1024];
    size_t lsize = *size;

    if(!h)  return GRIB_INVALID_GRIB;

    int err = grib_get_bytes(h, cast_char(buf,key,len), val, &lsize);
    *size = (int) lsize;

    return  err;
}

/*****************************************************************************/
int grib_f_index_get_string_(int* index_id, char* key, char* val, int *eachsize,int* size, int len)
{
    grib_index *h = get_index(*index_id);

    char buf[1024];
    size_t lsize = *size;
    char* p = val;

    if(!h)  return GRIB_INVALID_GRIB;

    char** bufval=(char**)grib_context_malloc_clear(h->context,sizeof(char*)* *size);

    int err = grib_index_get_string(h, cast_char(buf,key,len), bufval, &lsize);
    *size = lsize;

    if (err) return err;

    for (size_t i=0;i<lsize;i++) {
        int l=strlen(bufval[i]);
        if (*eachsize < l ) {
            fprintf(stderr, "eachsize=%d strlen(bufval[i])=%zu\n",
                    *eachsize, strlen(bufval[i]));
            grib_context_free(h->context,bufval);
            return GRIB_ARRAY_TOO_SMALL;
        }
        memcpy(p,bufval[i],l);
        p+=l;
        for (int j=0;j<*eachsize-l;j++) *(p++)=' ';
    }
    for (size_t i=0;i<lsize;i++) {
        grib_context_free(h->context, bufval[i]);
    }
    grib_context_free(h->context,bufval);

    return err;
}

/*****************************************************************************/
int grib_f_index_get_long_(int* index_id, char* key, long *val, int* size, int len)
{
    grib_index* h = get_index(*index_id);
    char buf[1024];
    size_t lsize = *size;

    if (!h) return GRIB_INVALID_GRIB;
    int err = grib_index_get_long(h, cast_char(buf,key,len), val, &lsize);
    *size = lsize;
    return err;
}

/*****************************************************************************/
int grib_f_index_get_int_(int* index_id, char* key, int *val, int* size, int len)
{
    grib_index *h = get_index(*index_id);

    char buf[1024];
    size_t lsize = *size, i = 0;
    long* lval=0;

    if (!h)  return GRIB_INVALID_GRIB;

    lval=(long*)grib_context_malloc(h->context,sizeof(long)* *size);
    if (!lval) return GRIB_OUT_OF_MEMORY;

    int err = grib_index_get_long(h, cast_char(buf,key,len), lval, &lsize);
    for (i=0;i<lsize;i++) val[i]=lval[i];

    *size = lsize;
    grib_context_free(h->context, lval);
    return  err;
}

/*****************************************************************************/
int grib_f_index_get_real8_(int* index_id, char* key, double *val, int* size, int len)
{
    grib_index* h = get_index(*index_id);
    char buf[1024];
    size_t lsize = *size;

    if (!h) return GRIB_INVALID_GRIB;
    int err = grib_index_get_double(h, cast_char(buf,key,len), val, &lsize);
    *size = lsize;
    return err;
}

/*****************************************************************************/
int grib_f_set_int_array_(int* gid, char* key, int* val, int* size, int len)
{
    grib_handle *h = get_handle(*gid);
    int err = GRIB_SUCCESS;
    char buf[1024];
    long* long_val = NULL;
    size_t lsize = *size;

    if(!h) return GRIB_INVALID_GRIB;

    if(sizeof(long) == sizeof(int)){
        long_val = (long*)val;
        return  grib_set_long_array(h, cast_char(buf,key,len), long_val, lsize);
    }

    if (lsize)
        long_val = (long*)grib_context_malloc(h->context,(lsize)*(sizeof(long)));
    else
        long_val = (long*)grib_context_malloc(h->context,(sizeof(long)));

    if (!long_val) return GRIB_OUT_OF_MEMORY;

    const size_t u_size = *size;
    for (lsize = 0; lsize < u_size; lsize++)
        long_val[lsize] = val[lsize];

    err = grib_set_long_array(h, cast_char(buf,key,len), long_val, lsize);

    grib_context_free(h->context,long_val);
    return err;
}

/*****************************************************************************/
int grib_f_set_long_array_(int* gid, char* key, long* val, int* size, int len)
{
    grib_handle *h = get_handle(*gid);
    char buf[1024];
    size_t lsize = *size;

    if(!h) return GRIB_INVALID_GRIB;

    return grib_set_long_array(h, cast_char(buf,key,len), val, lsize);
}

/*****************************************************************************/
int grib_f_set_byte_array_(int* gid, char* key, unsigned char* val, int* size, int len, int lenv)
{
    grib_handle *h = get_handle(*gid);

    char buf[1024];
    size_t lsize = *size;

    if (!h) return GRIB_INVALID_GRIB;

    int err = grib_set_bytes(h, cast_char(buf,key,len), val, &lsize);
    *size = (int) lsize;
    return err;
}

/*****************************************************************************/
int grib_f_set_int_(int* gid, char* key, int* val, int len)
{
    grib_handle *h = get_handle(*gid);
    char buf[1024];
    long long_val = *val;
    if(!h) return GRIB_INVALID_GRIB;
    return grib_set_long(h, cast_char(buf,key,len), long_val);
}

int grib_f_set_long_(int* gid, char* key, long* val, int len)
{
    grib_handle *h = get_handle(*gid);
    char buf[1024];
    if(!h) return GRIB_INVALID_GRIB;
    return grib_set_long(h, cast_char(buf,key,len), *val);
}

/*****************************************************************************/
int grib_f_set_missing_(int* gid, char* key,int len)
{

    grib_handle *h = get_handle(*gid);
    char buf[1024];
    if (!h) return GRIB_INVALID_GRIB;

    const char* theKey = cast_char(buf, key, len);
    if (!theKey) return GRIB_INVALID_ARGUMENT;

    return grib_set_missing(h, theKey);
}

int grib_f_is_missing_(int* gid, char* key, int* isMissing,int len)
{
    int err=0;
    grib_handle *h = get_handle(*gid);
    char buf[1024];
    if (!h) return GRIB_INVALID_GRIB;

    const char* theKey = cast_char(buf, key, len);
    if (!theKey) return GRIB_INVALID_ARGUMENT;

    *isMissing=grib_is_missing(h, theKey, &err);
    return err;
}

int grib_f_grib_surface_type_requires_value_(int* edition, int* type_of_surface_code, int* requires_value)
{
    int err = 0;
    *requires_value = codes_grib_surface_type_requires_value(*edition, *type_of_surface_code, &err);
    return err;
}

/*****************************************************************************/
int grib_f_is_defined_(int* gid, char* key,int* isDefined,int len)
{
    grib_handle *h = get_handle(*gid);
    char buf[1024];
    if (!h) return GRIB_INVALID_GRIB;
    const char* theKey = cast_char(buf, key, len);
    if (!theKey) return GRIB_INVALID_ARGUMENT;

    *isDefined = grib_is_defined(h, theKey);
    return GRIB_SUCCESS;
}

/*****************************************************************************/
int grib_f_key_is_computed_(int* gid, char* key,int* isComputed,int len)
{
    int err = 0;
    grib_handle *h = get_handle(*gid);
    char buf[1024];
    if (!h) return GRIB_INVALID_GRIB;

    *isComputed = codes_key_is_computed(h, cast_char(buf,key,len), &err);
    return err;
}

/*****************************************************************************/
int grib_f_set_real4_(int* gid, char* key, float* val, int len)
{
    grib_handle *h = get_handle(*gid);
    char buf[1024];
    double val8 = *val;
    if(!h)  return GRIB_INVALID_GRIB;

    return grib_set_double(h, cast_char(buf,key,len), val8);
}

int grib_f_get_real4_element_(int* gid, char* key, int* index,float* val, int len)
{
    grib_handle *h = get_handle(*gid);
    int err = GRIB_SUCCESS;
    char buf[1024];
    double val8 = 0;

    if(!h) return GRIB_INVALID_GRIB;

    err = grib_get_double_element(h, cast_char(buf,key,len), *index,&val8);
    *val = val8;
    return err;
}

int grib_f_get_real4_elements_(int* gid, char* key,int* index, float *val,int* size, int len)
{
    grib_handle *h = get_handle(*gid);
    int err = GRIB_SUCCESS;
    char buf[1024];
    size_t lsize = *size;
    size_t i = 0;
    double* val8 = NULL;

    if(!h) return GRIB_INVALID_GRIB;

    if(*size)
        val8 = (double*)grib_context_malloc(h->context,(*size)*(sizeof(double)));
    else
        val8 = (double*)grib_context_malloc(h->context,sizeof(double));

    if(!val8) return GRIB_OUT_OF_MEMORY;

    err = grib_get_double_elements(h, cast_char(buf,key,len), index,(long)lsize,val8);

    for(i=0;i<lsize;(i)++)
        val[i] = val8[i];

    grib_context_free(h->context,val8);

    return  err;
}

int grib_f_get_real4_(int* gid, char* key, float* val, int len)
{
    grib_handle *h = get_handle(*gid);
    int err = GRIB_SUCCESS;
    char buf[1024];
    double val8 = 0;

    if(!h)  return GRIB_INVALID_GRIB;

    err = grib_get_double(h, cast_char(buf,key,len), &val8);
    *val = val8;
    return err;
}

/*****************************************************************************/
int grib_f_get_real4_array_(int* gid, char* key, float* val, int* size, int len)
{
    /* See ECC-1579:
     * Ideally we should ALWAYS be calling:
     * err = grib_get_float_array(h, cast_char(buf,key,len), val, &lsize);
     */

    grib_handle* h = get_handle(*gid);
    size_t lsize   = *size;
    char buf[1024];
    int err        = GRIB_SUCCESS;
    const int single_precision_mode = (h->context->single_precision != 0);

    if (!h) return GRIB_INVALID_GRIB;

    if (single_precision_mode) {
        err = grib_get_float_array(h, cast_char(buf, key, len), val, &lsize);
    }
    else {
        double* val8 = NULL;
        size_t i;

        if (*size)
            val8 = (double*)grib_context_malloc(h->context, (*size) * (sizeof(double)));
        else
            val8 = (double*)grib_context_malloc(h->context, sizeof(double));

        if (!val8) return GRIB_OUT_OF_MEMORY;

        err = grib_get_double_array(h, cast_char(buf, key, len), val8, &lsize);
        if (err) {
            grib_context_free(h->context, val8);
            return err;
        }

        for (i = 0; i < lsize; i++)
            val[i] = val8[i];

        grib_context_free(h->context, val8);
    }

    return err;
}

/*****************************************************************************/
int grib_f_set_force_real4_array_(int* gid, char* key, float* val, int* size, int len)
{
    grib_handle *h = get_handle(*gid);
    int err = GRIB_SUCCESS;
    char buf[1024];
    size_t lsize = *size;
    double* val8 = NULL;
    size_t numElements = lsize;

    if(!h) return GRIB_INVALID_GRIB;

    if(*size)
        val8 = (double*)grib_context_malloc(h->context,lsize*(sizeof(double)));
    else
        val8 = (double*)grib_context_malloc(h->context,sizeof(double));

    if(!val8) return GRIB_OUT_OF_MEMORY;

    for (lsize = 0; lsize < numElements; lsize++)
        val8[lsize] = val[lsize];

    err = grib_set_force_double_array(h, cast_char(buf,key,len), val8, lsize);
    grib_context_free(h->context,val8);
    return err;
}

/*****************************************************************************/
int grib_f_set_real4_array_(int* gid, char* key, float* val, int* size, int len)
{
    grib_handle* h = get_handle(*gid);
    int err        = GRIB_SUCCESS;
    char buf[1024];
    const size_t lsize = *size;
    const int single_precision_mode = (h->context->single_precision != 0);

    if (!h) return GRIB_INVALID_GRIB;

    if (single_precision_mode) {
        err = grib_set_float_array(h, cast_char(buf, key, len), val, lsize);
    }
    else {
        double* val8 = NULL;
        size_t i     = 0;
        if (*size)
            val8 = (double*)grib_context_malloc(h->context, lsize * (sizeof(double)));
        else
            val8 = (double*)grib_context_malloc(h->context, sizeof(double));

        if (!val8) return GRIB_OUT_OF_MEMORY;

        for (i = 0; i < lsize; i++)
            val8[i] = val[i];

        err = grib_set_double_array(h, cast_char(buf, key, len), val8, lsize);
        grib_context_free(h->context, val8);
    }

    return err;
}

/*****************************************************************************/
int grib_f_index_select_real8_(int* index_id, char* key, double* val, int len)
{
    grib_index *h = get_index(*index_id);
    char buf[1024];

    if(!h) return GRIB_INVALID_GRIB;
    return grib_index_select_double(h, cast_char(buf,key,len), *val);
}

/*****************************************************************************/
int grib_f_index_select_string_(int* index_id, char* key, char* val, int len, int vallen)
{
    grib_index *h = get_index(*index_id);

    char buf[1024];
    char bufval[1024];

    if(!h) return GRIB_INVALID_GRIB;

    /* ECC-1316 */
    cast_char_no_cut(bufval,val,vallen);
    string_rtrim( bufval );

    return grib_index_select_string(h, cast_char(buf,key,len), bufval);
}

/*****************************************************************************/
int grib_f_index_select_int_(int* index_id, char* key, int* val, int len)
{
    grib_index *h = get_index(*index_id);
    long lval = *val;
    char buf[1024] = {0,};

    if (!h) return GRIB_INVALID_GRIB;
    return grib_index_select_long(h, cast_char(buf, key, len), lval);
}

/*****************************************************************************/
int grib_f_index_select_long_(int* index_id, char* key, long* val, int len)
{
    grib_index *h = get_index(*index_id);
    char buf[1024] = {0,};

    if (!h) return GRIB_INVALID_GRIB;
    return grib_index_select_long(h, cast_char(buf, key, len), *val);
}

/*****************************************************************************/
int grib_f_set_real8_(int* gid, char* key, double* val, int len)
{
    grib_handle *h = get_handle(*gid);
    char buf[1024];

    if(!h) return GRIB_INVALID_GRIB;
    return grib_set_double(h, cast_char(buf,key,len), *val);
}

int grib_f_get_real8_(int* gid, char* key, double* val, int len)
{
    grib_handle *h = get_handle(*gid);
    char buf[1024];

    if(!h) return GRIB_INVALID_GRIB;

    return grib_get_double(h, cast_char(buf,key,len), val);
}

/*****************************************************************************/
int grib_f_get_real8_element_(int* gid, char* key,int* index, double* val, int len)
{
    grib_handle *h = get_handle(*gid);
    char buf[1024];

    if(!h) return GRIB_INVALID_GRIB;

    return grib_get_double_element(h, cast_char(buf,key,len), *index,val);
}

/*****************************************************************************/
int grib_f_get_real8_elements_(int* gid, char* key,int* index, double* val, int *size, int len)
{
    grib_handle *h = get_handle(*gid);
    char buf[1024];

    if(!h) return GRIB_INVALID_GRIB;

    return grib_get_double_elements(h, cast_char(buf,key,len), index,*size,val);
}

/*****************************************************************************/
int grib_f_find_nearest_four_single_(int* gid,int* is_lsm,
        double* inlat,double* inlon,
        double* outlats,double* outlons,
        double* values,double* distances,
        int* indexes)
{
    grib_nearest* nearest=NULL;
    int err=0, result=0;
    unsigned long flags=0;
    size_t len=4;
    grib_handle *h = get_handle(*gid);

    if(!h) return GRIB_INVALID_GRIB;

    nearest=grib_nearest_new(h,&err);
    if (err!=GRIB_SUCCESS) return err;

    result = grib_nearest_find(nearest,h,*inlat,*inlon,
            flags,outlats,outlons,values,distances,indexes,&len);

    grib_nearest_delete(nearest);
    return result;
}

/*****************************************************************************/
int grib_f_find_nearest_single_(int* gid,int* is_lsm,
        double* inlats,double* inlons,
        double* outlats,double* outlons,
        double* values,double* distances,
        int* indexes)
{
    grib_handle *h = get_handle(*gid);
    if(!h) return GRIB_INVALID_GRIB;

    return grib_nearest_find_multiple(h,*is_lsm,
            inlats,inlons,1,outlats,outlons,
            values,distances,indexes);
}

/*****************************************************************************/
int grib_f_find_nearest_multiple_(int* gid,int* is_lsm,
        double* inlats,double* inlons,
        double* outlats,double* outlons,
        double* values,double* distances,
        int* indexes, int* npoints)
{
    grib_handle *h = get_handle(*gid);
    if(!h) return GRIB_INVALID_GRIB;

    return grib_nearest_find_multiple(h,*is_lsm,
            inlats,inlons,*npoints,outlats,outlons,
            values,distances,indexes);
}

/*****************************************************************************/
int grib_f_get_real8_array_(int* gid, char* key, double*val, int* size, int len)
{
    grib_handle *h = get_handle(*gid);
    int err = GRIB_SUCCESS;
    char buf[1024];
    size_t lsize = *size;

    if(!h){
        return GRIB_INVALID_GRIB;
    }else{
        err = grib_get_double_array(h, cast_char(buf,key,len), val, &lsize);
        *size = lsize;
        return  err;
    }
}

/*****************************************************************************/
int grib_f_set_force_real8_array_(int* gid, char* key, double*val, int* size, int len)
{
    grib_handle *h = get_handle(*gid);
    char buf[1024];
    size_t lsize = *size;

    if(!h) return GRIB_INVALID_GRIB;

    return grib_set_force_double_array(h, cast_char(buf,key,len), val, lsize);
}

/*****************************************************************************/
int grib_f_set_real8_array_(int* gid, char* key, double*val, int* size, int len)
{
    grib_handle *h = get_handle(*gid);
    char buf[1024];
    size_t lsize = *size;

    if (!h) return GRIB_INVALID_GRIB;

    return grib_set_double_array(h, cast_char(buf,key,len), val, lsize);
}

/*****************************************************************************/
int grib_f_get_string_array_(int* gid, char* key, char* val,int* nvals,int* slen, int len)
{
    grib_handle *h = get_handle(*gid);
    int err = GRIB_SUCCESS;
    size_t i;
    char buf[1024];
    size_t lsize = *nvals;
    char** cval=0;
    char* p=val;

    if(!h) return  GRIB_INVALID_GRIB;

    cval=(char**)grib_context_malloc_clear(h->context,sizeof(char*)*lsize);
    err = grib_get_string_array(h, cast_char(buf,key,len), cval, &lsize);
    if (err) return err;

    const size_t u_slen = *slen;
    if (strlen(cval[0]) > u_slen) err = GRIB_ARRAY_TOO_SMALL;

    for (i=0;i<lsize;i++) {
        strcpy(p,cval[i]);
        czstr_to_fortran(p,*slen);
        grib_context_free(h->context,cval[i]);
        p+= *slen;
    }
    grib_context_free(h->context,cval);

    return  err;
}

/*****************************************************************************/
int codes_f_bufr_copy_data_(int* gid1,int* gid2)
{
    grib_handle *hin = get_handle(*gid1);
    grib_handle *hout = get_handle(*gid2);
    int err = GRIB_SUCCESS;

    if(!hin || !hout ) return  GRIB_INVALID_GRIB;

    err=codes_bufr_copy_data(hin,hout);
    if (err) return err;

    return  err;
}

/*****************************************************************************/
int grib_f_set_string_array_(int* gid, char* key, char* val,int* nvals,int* slen, int len)
{
    grib_handle *h = get_handle(*gid);
    int err = GRIB_SUCCESS;
    size_t i;
    char buf[1024];
    size_t lsize = *nvals;
    char** cval=0;
    char* p=val;
    grib_context* c;

    if(!h) return  GRIB_INVALID_GRIB;
    c=h->context;

    cval=(char**)grib_context_malloc_clear(h->context,sizeof(char*)*lsize);
    for (i=0;i<lsize;i++) {
        cval[i]=(char*)grib_context_malloc_clear(c,sizeof(char)* (*slen+1));
        cast_char_no_cut(cval[i],p,*slen);
        string_rtrim( cval[i] ); /* trim spaces at end of string */
        p+= *slen;
    }
    err = grib_set_string_array(h, cast_char(buf,key,len), (const char **)cval, lsize);
    if (err) return err;

    for (i=0;i<lsize;i++) {
        grib_context_free(c,cval[i]);
    }
    grib_context_free(c,cval);

    return  err;
}

/*****************************************************************************/
int grib_f_get_string_(int* gid, char* key, char* val,int len, int len2)
{
    grib_handle *h = get_handle(*gid);
    int err = GRIB_SUCCESS;
    char buf[1024];
    size_t lsize = len2;

    if(!h) return  GRIB_INVALID_GRIB;

    fort_char_clean(val,len2);

    err = grib_get_string(h, cast_char(buf,key,len), val, &lsize);

    czstr_to_fortran(val,len2);

    return  err;
}

static int is_all_spaces(const char *s)
{
    while (*s != '\0') {
        if (!isspace(*s)) return 0;
        s++;
    }
    return 1;
}

/*****************************************************************************/
int grib_f_set_string_(int* gid, char* key, char* val, int len, int len2)
{
    grib_handle *h = get_handle(*gid);
    char* val_str = NULL;

    char buf[1024]={0,};
    char buf2[1024]={0,};
    size_t lsize = len2;

    if(!h) return GRIB_INVALID_GRIB;
    
    /* For BUFR, the value may contain spaces e.g. stationOrSiteName='CAMPO NOVO' */
    /* So do not use cast_char. cast_char_no_cut does not stop at first space */
    val_str = cast_char_no_cut(buf2,val,len2);
    if (val_str && !is_all_spaces(val_str)) {
        string_rtrim( val_str ); /* trim spaces at end of string */
    }

    return grib_set_string(h, cast_char(buf,key,len), val_str, &lsize);
}

/*****************************************************************************/
int grib_f_get_data_real4_(int* gid,float* lats, float* lons,float* values,size_t* size)
{
    grib_handle *h = get_handle(*gid);
    int err = GRIB_SUCCESS;
    double *lat8=NULL,*lon8=NULL,*val8 = NULL;
    size_t i=0;

    if(!h) return GRIB_INVALID_GRIB;

    val8 = (double*)grib_context_malloc(h->context,(*size)*(sizeof(double)));
    if(!val8) return GRIB_OUT_OF_MEMORY;
    lon8 = (double*)grib_context_malloc(h->context,(*size)*(sizeof(double)));
    if(!lon8) return GRIB_OUT_OF_MEMORY;
    lat8 = (double*)grib_context_malloc(h->context,(*size)*(sizeof(double)));
    if(!lat8) return GRIB_OUT_OF_MEMORY;

    err=grib_get_data(h,lat8,lon8,val8);

    for(i=0;i<*size;i++) {
        values[i] = val8[i];
        lats[i] = lat8[i];
        lons[i] = lon8[i];
    }

    grib_context_free(h->context,val8);
    grib_context_free(h->context,lat8);
    grib_context_free(h->context,lon8);

    return err;
}

int grib_f_get_data_real8_(int* gid,double* lats, double* lons,double* values,size_t* size)
{
    grib_handle *h = get_handle(*gid);
    return grib_get_data(h,lats,lons,values);
}

/*****************************************************************************/
int grib_f_get_message_size_(int* gid, size_t *len)
{
    grib_handle *h = get_handle(*gid);
    if(!h) return GRIB_INVALID_GRIB;
    *len = h->buffer->ulength;
    return GRIB_SUCCESS;
}

/*****************************************************************************/
int grib_f_copy_message_(int* gid, void* mess, size_t* len)
{
    grib_handle *h = get_handle(*gid);
    if(!h)
        return GRIB_INVALID_GRIB;

    if(*len < h->buffer->ulength) {
        grib_context_log(h->context,GRIB_LOG_ERROR,
                "grib_copy_message: buffer=%zu message size=%zu", *len, h->buffer->ulength);
        return GRIB_BUFFER_TOO_SMALL;
    }

    memcpy(mess,h->buffer->data,h->buffer->ulength);
    *len=h->buffer->ulength;
    return GRIB_SUCCESS;
}

/*****************************************************************************/
void grib_f_check_(int* err,char* call,char* str,int lencall,int lenstr)
{
    char bufstr[1024]={0,};
    char bufcall[1024]={0,};
    grib_context* c=grib_context_get_default();
    if ( *err == GRIB_SUCCESS || *err == GRIB_END_OF_FILE ) return;
    cast_char(bufcall,call,lencall);
    /* ECC-1392 */
    cast_char_no_cut(bufstr,str,lenstr);

    grib_context_log(c,GRIB_LOG_ERROR,"%s: %s %s",
            bufcall,bufstr,grib_get_error_message(*err));
    exit(*err);
}

/*****************************************************************************/
int grib_f_write_(int* gid, int* fid)
{
    grib_handle *h = get_handle(*gid);
    FILE* f = get_file(*fid);
    const void* mess = NULL;
    size_t mess_len = 0;

    if(!f) return GRIB_INVALID_FILE;
    if (!h) return GRIB_INVALID_GRIB;

    grib_get_message(h,&mess,&mess_len);
    if(fwrite(mess,1, mess_len,f) != mess_len) {
        perror("write");
        return GRIB_IO_PROBLEM;
    }

    return GRIB_SUCCESS;
}

/*****************************************************************************/
int grib_f_multi_write_(int* gid, int* fid)
{
    grib_multi_handle *h = get_multi_handle(*gid);
    FILE* f = get_file(*fid);

    if(!f) return GRIB_INVALID_FILE;
    if (!h) return GRIB_INVALID_GRIB;

    return grib_multi_handle_write(h,f);
}

int grib_f_multi_append_(int* ingid, int* sec,int* mgid)
{
    grib_handle *h = get_handle(*ingid);
    grib_multi_handle *mh = get_multi_handle(*mgid);

    if (!h) return GRIB_INVALID_GRIB;

    if (!mh) {
        mh=grib_multi_handle_new(h->context);
        push_multi_handle(mh,mgid);
    }

    return grib_multi_handle_append(h,*sec,mh);
}

/*****************************************************************************/
int codes_f_bufr_multi_element_constant_arrays_on_(void)
{
    codes_bufr_multi_element_constant_arrays_on(NULL);
    return GRIB_SUCCESS;
}

int codes_f_bufr_multi_element_constant_arrays_off_(void)
{
    codes_bufr_multi_element_constant_arrays_off(NULL);
    return GRIB_SUCCESS;
}

/*****************************************************************************/
void grib_f_set_debug_(int* dmode)
{
    ECCODES_ASSERT(dmode);
    grib_context* c = grib_context_get_default();
    grib_context_set_debug(c, *dmode);
}
/*****************************************************************************/
void grib_f_set_data_quality_checks_(int* val)
{
    ECCODES_ASSERT(val);
    grib_context* c = grib_context_get_default();
    grib_context_set_data_quality_checks(c, *val);
}

/*****************************************************************************/
int grib_f_set_definitions_path_(char* path, int len)
{
    grib_context* c = grib_context_get_default();
    char buf[1024];
    grib_context_set_definitions_path(c, cast_char(buf,path,len));
    return GRIB_SUCCESS;
}

/*****************************************************************************/
int grib_f_set_samples_path_(char* path, int len)
{
    grib_context* c = grib_context_get_default();
    char buf[1024];
    grib_context_set_samples_path(c, cast_char(buf,path,len));
    return GRIB_SUCCESS;
}

/*****************************************************************************/
int grib_f_julian_to_datetime_(double* jd,long* year,long* month,long* day,long *hour,long *minute,long *second)
{
    return grib_julian_to_datetime(*jd,year,month,day,hour,minute,second);
}

/*****************************************************************************/
int grib_f_datetime_to_julian_(long* year,long* month,long* day, long* hour,long* minute,long* second,double* jd)
{
    return grib_datetime_to_julian(*year,*month,*day,*hour,*minute,*second,jd);
}
