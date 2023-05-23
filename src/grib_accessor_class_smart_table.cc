/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

/*****************************************
 *  Enrico Fucile
 ****************************************/

#include "grib_api_internal.h"
#include <ctype.h>

/*
   This is used by make_class.pl

   START_CLASS_DEF
   CLASS      = accessor
   SUPER      = grib_accessor_class_unsigned
   IMPLEMENTS = init;dump;unpack_string;unpack_long
   IMPLEMENTS = value_count; destroy; get_native_type;
   MEMBERS    =  const char* values
   MEMBERS    =  const char* tablename
   MEMBERS    =  const char* masterDir
   MEMBERS    =  const char* localDir
   MEMBERS    =  const char* extraDir
   MEMBERS    =  const char* extraTable
   MEMBERS    =  int widthOfCode
   MEMBERS    =  long* tableCodes
   MEMBERS    =  size_t tableCodesSize
   MEMBERS    =  grib_smart_table* table
   MEMBERS    =  int dirty
   END_CLASS_DEF

 */

/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "accessor.class" and rerun ./make_class.pl

*/

static int get_native_type(grib_accessor*);
static int unpack_long(grib_accessor*, long* val, size_t* len);
static int unpack_string(grib_accessor*, char*, size_t* len);
static int value_count(grib_accessor*, long*);
static void destroy(grib_context*, grib_accessor*);
static void dump(grib_accessor*, grib_dumper*);
static void init(grib_accessor*, const long, grib_arguments*);
//static void init_class(grib_accessor_class*);

typedef struct grib_accessor_smart_table
{
    grib_accessor att;
    /* Members defined in gen */
    /* Members defined in long */
    /* Members defined in unsigned */
    long nbytes;
    grib_arguments* arg;
    /* Members defined in smart_table */
    const char* values;
    const char* tablename;
    const char* masterDir;
    const char* localDir;
    const char* extraDir;
    const char* extraTable;
    int widthOfCode;
    long* tableCodes;
    size_t tableCodesSize;
    grib_smart_table* table;
    int dirty;
} grib_accessor_smart_table;

extern grib_accessor_class* grib_accessor_class_unsigned;

static grib_accessor_class _grib_accessor_class_smart_table = {
    &grib_accessor_class_unsigned,                      /* super */
    "smart_table",                      /* name */
    sizeof(grib_accessor_smart_table),  /* size */
    0,                           /* inited */
    0,                           /* init_class */
    &init,                       /* init */
    0,                  /* post_init */
    &destroy,                    /* destroy */
    &dump,                       /* dump */
    0,                /* next_offset */
    0,              /* get length of string */
    &value_count,                /* get number of values */
    0,                 /* get number of bytes */
    0,                /* get offset to bytes */
    &get_native_type,            /* get native type */
    0,                /* get sub_section */
    0,               /* pack_missing */
    0,                 /* is_missing */
    0,                  /* pack_long */
    &unpack_long,                /* unpack_long */
    0,                /* pack_double */
    0,                 /* pack_float */
    0,              /* unpack_double */
    0,               /* unpack_float */
    0,                /* pack_string */
    &unpack_string,              /* unpack_string */
    0,          /* pack_string_array */
    0,        /* unpack_string_array */
    0,                 /* pack_bytes */
    0,               /* unpack_bytes */
    0,            /* pack_expression */
    0,              /* notify_change */
    0,                /* update_size */
    0,             /* preferred_size */
    0,                     /* resize */
    0,      /* nearest_smaller_value */
    0,                       /* next accessor */
    0,                    /* compare vs. another accessor */
    0,      /* unpack only ith value (double) */
    0,       /* unpack only ith value (float) */
    0,  /* unpack a given set of elements (double) */
    0,   /* unpack a given set of elements (float) */
    0,     /* unpack a subarray */
    0,                      /* clear */
    0,                 /* clone accessor */
};


grib_accessor_class* grib_accessor_class_smart_table = &_grib_accessor_class_smart_table;


//static void init_class(grib_accessor_class* c)
//{
// INIT
//}

/* END_CLASS_IMP */

#if GRIB_PTHREADS
static pthread_once_t once   = PTHREAD_ONCE_INIT;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

static void thread_init()
{
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&mutex, &attr);
    pthread_mutexattr_destroy(&attr);
}
#elif GRIB_OMP_THREADS
static int once = 0;
static omp_nest_lock_t mutex;

static void thread_init()
{
    GRIB_OMP_CRITICAL(lock_grib_accessor_class_smart_table_c)
    {
        if (once == 0) {
            omp_init_nest_lock(&mutex);
            once = 1;
        }
    }
}
#endif

static int grib_load_smart_table(grib_context* c, const char* filename,
                                 const char* recomposed_name, size_t size, grib_smart_table* t);

static void init(grib_accessor* a, const long len, grib_arguments* params)
{
    int n                           = 0;
    grib_accessor_smart_table* self = (grib_accessor_smart_table*)a;
    grib_handle* hand               = grib_handle_of_accessor(a);

    self->values      = grib_arguments_get_name(hand, params, n++);
    self->tablename   = grib_arguments_get_string(hand, params, n++);
    self->masterDir   = grib_arguments_get_name(hand, params, n++);
    self->localDir    = grib_arguments_get_name(hand, params, n++);
    self->widthOfCode = grib_arguments_get_long(hand, params, n++);
    self->extraDir    = grib_arguments_get_name(hand, params, n++);
    self->extraTable  = grib_arguments_get_string(hand, params, n++);

    a->length = 0;
    a->flags |= GRIB_ACCESSOR_FLAG_READ_ONLY;
    self->dirty          = 1;
    self->tableCodesSize = 0;
    self->tableCodes     = 0;
}

static grib_smart_table* load_table(grib_accessor_smart_table* self)
{
    size_t size            = 0;
    grib_handle* h         = ((grib_accessor*)self)->parent->h;
    grib_context* c        = h->context;
    grib_smart_table* t    = NULL;
    grib_smart_table* next = NULL;
    char* filename         = 0;
    char recomposed[1024] = {0,};
    char localRecomposed[1024] = {0,};
    char* localFilename        = 0;
    char extraRecomposed[1024] = {0,};
    char* extraFilename  = 0;
    char masterDir[1024] = {0,};
    char localDir[1024] = {0,};
    char extraDir[1024] = {0,};
    size_t len = 1024;

    if (self->masterDir != NULL) {
        grib_get_string(h, self->masterDir, masterDir, &len);
    }

    len = 1024;
    if (self->localDir != NULL) {
        grib_get_string(h, self->localDir, localDir, &len);
    }

    len = 1024;
    if (self->extraDir != NULL && self->extraTable != NULL) {
        grib_get_string(h, self->extraDir, extraDir, &len);
    }

    if (*masterDir != 0) {
        char name[2048] = {0,};
        snprintf(name, sizeof(name), "%s/%s", masterDir, self->tablename);
        grib_recompose_name(h, NULL, name, recomposed, 0);
        filename = grib_context_full_defs_path(c, recomposed);
    }
    else {
        grib_recompose_name(h, NULL, self->tablename, recomposed, 0);
        filename = grib_context_full_defs_path(c, recomposed);
    }

    if (*localDir != 0) {
        char localName[2048] = {0,};
        snprintf(localName, sizeof(localName), "%s/%s", localDir, self->tablename);
        grib_recompose_name(h, NULL, localName, localRecomposed, 0);
        localFilename = grib_context_full_defs_path(c, localRecomposed);
    }

    if (*extraDir != 0) {
        char extraTable[2048] = {0,};
        snprintf(extraTable, sizeof(extraTable), "%s/%s", extraDir, self->extraTable);
        grib_recompose_name(h, NULL, extraTable, extraRecomposed, 0);
        extraFilename = grib_context_full_defs_path(c, extraRecomposed);
    }

    next = c->smart_table;
    while (next) {
        if ((filename && next->filename[0] && strcmp(filename, next->filename[0]) == 0) &&
            ((localFilename == 0 && next->filename[1] == NULL) ||
             ((localFilename != 0 && next->filename[1] != NULL) && strcmp(localFilename, next->filename[1]) == 0)) &&
            ((extraFilename == 0 && next->filename[2] == NULL) ||
             ((extraFilename != 0 && next->filename[2] != NULL) && strcmp(extraFilename, next->filename[2]) == 0)))
            return next;
        next = next->next;
    }

    /* Note: self->widthOfCode is chosen so that 2^width is bigger than the maximum descriptor code,
     * which for BUFR4 is the Table C operator 243255
     */
    size = (1ULL << self->widthOfCode); /* = 2^self->widthOfCode (as a 64 bit number) */

    t                  = (grib_smart_table*)grib_context_malloc_clear_persistent(c, sizeof(grib_smart_table));
    t->entries         = (grib_smart_table_entry*)grib_context_malloc_clear_persistent(c, size * sizeof(grib_smart_table_entry));
    t->numberOfEntries = size;

    if (filename != 0)
        grib_load_smart_table(c, filename, recomposed, size, t);

    if (localFilename != 0)
        grib_load_smart_table(c, localFilename, localRecomposed, size, t);

    if (extraFilename != 0)
        grib_load_smart_table(c, extraFilename, extraRecomposed, size, t);

    if (t->filename[0] == NULL && t->filename[1] == NULL) {
        grib_context_free_persistent(c, t);
        return NULL;
    }

    return t;
}

static int grib_load_smart_table(grib_context* c, const char* filename,
                                 const char* recomposed_name, size_t size, grib_smart_table* t)
{
    char line[1024] = {0,};
    FILE* f = NULL;
    int lineNumber, numberOfColumns, code;

    grib_context_log(c, GRIB_LOG_DEBUG, "Loading code table from %s", filename);

    f = codes_fopen(filename, "r");
    if (!f)
        return GRIB_IO_PROBLEM;

    Assert(t != NULL);

    if (t->filename[0] == NULL) {
        t->filename[0]        = grib_context_strdup_persistent(c, filename);
        t->recomposed_name[0] = grib_context_strdup_persistent(c, recomposed_name);
        t->next               = c->smart_table;
        t->numberOfEntries    = size;
        GRIB_MUTEX_INIT_ONCE(&once, &thread_init)
        GRIB_MUTEX_LOCK(&mutex)
        c->smart_table = t;
        GRIB_MUTEX_UNLOCK(&mutex)
    }
    else if (t->filename[1] == NULL) {
        t->filename[1]        = grib_context_strdup_persistent(c, filename);
        t->recomposed_name[1] = grib_context_strdup_persistent(c, recomposed_name);
    }
    else {
        t->filename[2]        = grib_context_strdup_persistent(c, filename);
        t->recomposed_name[2] = grib_context_strdup_persistent(c, recomposed_name);
    }

    lineNumber = 0;
    while (fgets(line, sizeof(line) - 1, f)) {
        char* s = line;
        char* p;

        line[strlen(line) - 1] = 0;

        ++lineNumber;
        while (*s != '\0' && isspace(*s))
            s++;

        if (*s == '#')
            continue;

        p = s;
        while (*p != '\0' && *p != '|')
            p++;

        *p = 0;

        code = atoi(s);

        p++;
        s = p;
        while (*p != '\0' && *p != '|')
            p++;

        *p = 0;

        numberOfColumns = 0;
        /* The highest possible descriptor code must fit into t->numberOfEntries */
        DebugAssert(code < t->numberOfEntries);
        while (*s) {
            char* tcol = t->entries[code].column[numberOfColumns];
            if ( tcol ) grib_context_free_persistent(c, tcol);
            t->entries[code].column[numberOfColumns] = grib_context_strdup_persistent(c, s);
            numberOfColumns++;
            DebugAssert(numberOfColumns < MAX_SMART_TABLE_COLUMNS);

            p++;
            s = p;
            while (*p != '\0' && *p != '|')
                p++;
            *p = 0;
        }
    }

    fclose(f);
    return 0;
}

void grib_smart_table_delete(grib_context* c)
{
    grib_smart_table* t = c->smart_table;
    while (t) {
        grib_smart_table* s = t->next;
        int i;
        int k;

        for (i = 0; i < t->numberOfEntries; i++) {
            if (t->entries[i].abbreviation)
                grib_context_free_persistent(c, t->entries[i].abbreviation);
            for (k = 0; k < MAX_SMART_TABLE_COLUMNS; k++) {
                if (t->entries[i].column[k])
                    grib_context_free_persistent(c, t->entries[i].column[k]);
            }
        }
        grib_context_free_persistent(c, t->entries);
        grib_context_free_persistent(c, t->filename[0]);
        if (t->filename[1])
            grib_context_free_persistent(c, t->filename[1]);
        if (t->filename[2])
            grib_context_free_persistent(c, t->filename[2]);
        grib_context_free_persistent(c, t->recomposed_name[0]);
        if (t->recomposed_name[1])
            grib_context_free_persistent(c, t->recomposed_name[1]);
        if (t->recomposed_name[2])
            grib_context_free_persistent(c, t->recomposed_name[2]);
        grib_context_free_persistent(c, t);
        t = s;
    }
}

static void dump(grib_accessor* a, grib_dumper* dumper)
{
    grib_dump_long(dumper, a, NULL);
}

static int unpack_string(grib_accessor* a, char* buffer, size_t* len)
{
    grib_accessor_smart_table* self = (grib_accessor_smart_table*)a;
    grib_smart_table* table         = NULL;

    size_t size = 1;
    long value;
    int err = GRIB_SUCCESS;
    char tmp[1024];
    size_t l = 0;

    if ((err = grib_unpack_long(a, &value, &size)) != GRIB_SUCCESS)
        return err;

    if (!self->table)
        self->table = load_table(self);
    table = self->table;

    if (table && (value >= 0) && (value < table->numberOfEntries) && table->entries[value].abbreviation) {
        strcpy(tmp, table->entries[value].abbreviation);
    }
    else {
        snprintf(tmp, sizeof(tmp), "%d", (int)value);
    }

    l = strlen(tmp) + 1;

    if (*len < l) {
        *len = l;
        return GRIB_BUFFER_TOO_SMALL;
    }

    strcpy(buffer, tmp);
    *len        = l;
    self->dirty = 0;

    return GRIB_SUCCESS;
}

static int get_table_codes(grib_accessor* a)
{
    grib_accessor_smart_table* self = (grib_accessor_smart_table*)a;
    size_t size                     = 0;
    long* v                         = 0;
    int err                         = 0;
    int count, j;
    size_t i;

    int table_size;

    if (!self->dirty)
        return 0;

    table_size = (1 << self->widthOfCode); /* 2 ^ self->widthOfCode */

    if (!self->table)
        self->table = load_table(self);

    err = grib_get_size(grib_handle_of_accessor(a), self->values, &size);
    if (err) {
        grib_context_log(a->context, GRIB_LOG_ERROR,
                         "unable to get size of %s", a->name);
        return err;
    }

    v = (long*)grib_context_malloc_clear(a->context, size * sizeof(long));

    grib_get_long_array(grib_handle_of_accessor(a), self->values, v, &size);

    count = 0;
    for (i = 0; i < size; i++) {
        if (v[i] < table_size)
            count++;
    }
    if (self->tableCodes)
        grib_context_free(a->context, self->tableCodes);
    self->tableCodes = (long*)grib_context_malloc_clear(a->context, count * sizeof(long));
    j                = 0;
    for (i = 0; i < size; i++) {
        if (v[i] < table_size)
            self->tableCodes[j++] = v[i];
    }

    grib_context_free(a->context, v);

    self->tableCodesSize = count;
    self->dirty          = 0;

    return 0;
}

static int value_count(grib_accessor* a, long* count)
{
    int err                         = 0;
    grib_accessor_smart_table* self = (grib_accessor_smart_table*)a;
    *count                          = 0;

    if (!self->values)
        return 0;
    err = get_table_codes(a);
    if (err)
        return err;

    *count = self->tableCodesSize;
    return GRIB_SUCCESS;
}

static void destroy(grib_context* context, grib_accessor* a)
{
    grib_accessor_smart_table* self = (grib_accessor_smart_table*)a;
    if (a->vvalue != NULL) {
        grib_context_free(context, a->vvalue);
        a->vvalue = NULL;
    }
    if (self->tableCodes)
        grib_context_free(a->context, self->tableCodes);
}

static int get_native_type(grib_accessor* a)
{
    int type = GRIB_TYPE_LONG;
    /*printf("---------- %s flags=%ld GRIB_ACCESSOR_FLAG_STRING_TYPE=%d\n",
         a->name,a->flags,GRIB_ACCESSOR_FLAG_STRING_TYPE);*/
    if (a->flags & GRIB_ACCESSOR_FLAG_STRING_TYPE)
        type = GRIB_TYPE_STRING;
    return type;
}

static int unpack_long(grib_accessor* a, long* val, size_t* len)
{
    int err                         = 0;
    grib_accessor_smart_table* self = (grib_accessor_smart_table*)a;
    size_t i;

    if (!self->values)
        return 0;

    err = get_table_codes(a);
    if (err)
        return 0;

    if (*len < self->tableCodesSize) {
        grib_context_log(a->context, GRIB_LOG_ERROR,
                         "Wrong size (%lu) for %s it contains %lu values", *len, a->name, self->tableCodesSize);
        *len = 0;
        return GRIB_ARRAY_TOO_SMALL;
    }

    for (i = 0; i < self->tableCodesSize; i++)
        val[i] = self->tableCodes[i];

    return err;
}
