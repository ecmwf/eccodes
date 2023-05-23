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

/*
   This is used by make_class.pl

   START_CLASS_DEF
   CLASS      = accessor
   SUPER      = grib_accessor_class_gen
   IMPLEMENTS = init;dump;unpack_string_array;unpack_long
   IMPLEMENTS = value_count; destroy; get_native_type;
   MEMBERS    =  const char* smartTable
   MEMBERS    =  int index
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
static int unpack_string_array(grib_accessor*, char**, size_t* len);
static int value_count(grib_accessor*, long*);
static void destroy(grib_context*, grib_accessor*);
static void dump(grib_accessor*, grib_dumper*);
static void init(grib_accessor*, const long, grib_arguments*);
//static void init_class(grib_accessor_class*);

typedef struct grib_accessor_smart_table_column
{
    grib_accessor att;
    /* Members defined in gen */
    /* Members defined in smart_table_column */
    const char* smartTable;
    int index;
} grib_accessor_smart_table_column;

extern grib_accessor_class* grib_accessor_class_gen;

static grib_accessor_class _grib_accessor_class_smart_table_column = {
    &grib_accessor_class_gen,                      /* super */
    "smart_table_column",                      /* name */
    sizeof(grib_accessor_smart_table_column),  /* size */
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
    0,              /* unpack_string */
    0,          /* pack_string_array */
    &unpack_string_array,        /* unpack_string_array */
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


grib_accessor_class* grib_accessor_class_smart_table_column = &_grib_accessor_class_smart_table_column;


//static void init_class(grib_accessor_class* c)
//{
// INIT
//}

/* END_CLASS_IMP */

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

static void init(grib_accessor* a, const long len, grib_arguments* params)
{
    int n                                  = 0;
    grib_accessor_smart_table_column* self = (grib_accessor_smart_table_column*)a;

    self->smartTable = grib_arguments_get_name(grib_handle_of_accessor(a), params, n++);
    self->index      = grib_arguments_get_long(grib_handle_of_accessor(a), params, n++);

    a->length = 0;
    a->flags |= GRIB_ACCESSOR_FLAG_READ_ONLY;
}

static void dump(grib_accessor* a, grib_dumper* dumper)
{
    int type = get_native_type(a);

    switch (type) {
        case GRIB_TYPE_LONG:
            grib_dump_long(dumper, a, NULL);
            break;
        case GRIB_TYPE_STRING:
            grib_dump_string_array(dumper, a, NULL);
            break;
    }
}

static int unpack_string_array(grib_accessor* a, char** buffer, size_t* len)
{
    grib_accessor_smart_table_column* self   = (grib_accessor_smart_table_column*)a;
    grib_accessor_smart_table* tableAccessor = NULL;
    grib_smart_table* table                  = NULL;

    size_t size = 1;
    long* code;
    int err        = GRIB_SUCCESS;
    char tmp[1024] = {0,};
    int i = 0;

    tableAccessor = (grib_accessor_smart_table*)grib_find_accessor(grib_handle_of_accessor(a), self->smartTable);
    if (!tableAccessor) {
        grib_context_log(a->context, GRIB_LOG_ERROR,
                         "unable to find accessor %s", self->smartTable);
        return GRIB_NOT_FOUND;
    }

    err = ecc__grib_get_size(grib_handle_of_accessor(a), (grib_accessor*)tableAccessor, &size);
    if (err)
        return err;
    if (*len < size) {
        return GRIB_BUFFER_TOO_SMALL;
    }

    code = (long*)grib_context_malloc_clear(a->context, sizeof(long) * size);
    if (!code) {
        grib_context_log(a->context, GRIB_LOG_FATAL, "%s: Memory allocation error: %zu bytes", a->name, size);
        return GRIB_OUT_OF_MEMORY;
    }

    if ((err = grib_unpack_long((grib_accessor*)tableAccessor, code, &size)) != GRIB_SUCCESS)
        return err;

    table = tableAccessor->table;

    for (i = 0; i < size; i++) {
        if (table && (code[i] >= 0) &&
            (code[i] < table->numberOfEntries) &&
            table->entries[code[i]].column[self->index]) {
            strcpy(tmp, table->entries[code[i]].column[self->index]);
        }
        else {
            snprintf(tmp, sizeof(tmp), "%d", (int)code[i]);
        }

        buffer[i] = grib_context_strdup(a->context, tmp);
    }
    *len = size;
    grib_context_free(a->context, code);

    return GRIB_SUCCESS;
}

static int unpack_long(grib_accessor* a, long* val, size_t* len)
{
    grib_accessor_smart_table_column* self   = (grib_accessor_smart_table_column*)a;
    grib_accessor_smart_table* tableAccessor = NULL;
    grib_smart_table* table                  = NULL;

    size_t size = 1;
    long* code;
    int err = GRIB_SUCCESS;
    int i   = 0;

    for (i = 0; i < *len; i++)
        val[i] = GRIB_MISSING_LONG;

    tableAccessor = (grib_accessor_smart_table*)grib_find_accessor(grib_handle_of_accessor(a), self->smartTable);
    if (!tableAccessor) {
        grib_context_log(a->context, GRIB_LOG_ERROR,
                         "unable to find accessor %s", self->smartTable);
        return GRIB_NOT_FOUND;
    }

    err = ecc__grib_get_size(grib_handle_of_accessor(a), (grib_accessor*)tableAccessor, &size);
    if (err)
        return err;
    if (*len < size) {
        return GRIB_BUFFER_TOO_SMALL;
    }

    code = (long*)grib_context_malloc_clear(a->context, sizeof(long) * size);
    if (!code) return GRIB_OUT_OF_MEMORY;

    if ((err = grib_unpack_long((grib_accessor*)tableAccessor, code, &size)) != GRIB_SUCCESS) {
        grib_context_free(a->context, code);
        return err;
    }

    table = tableAccessor->table;

    for (i = 0; i < size; i++) {
        if (table && (code[i] >= 0) &&
            (code[i] < table->numberOfEntries) &&
            table->entries[code[i]].column[self->index]) {
            val[i] = atol(table->entries[code[i]].column[self->index]);
        }
    }
    *len = size;
    grib_context_free(a->context, code);

    return GRIB_SUCCESS;
}

static int value_count(grib_accessor* a, long* count)
{
    grib_accessor_smart_table_column* self = (grib_accessor_smart_table_column*)a;
    size_t size                            = 0;
    int err                                = 0;
    *count                                 = 0;

    if (!self->smartTable)
        return 0;

    err    = grib_get_size(grib_handle_of_accessor(a), self->smartTable, &size);
    *count = size;
    return err;
}

static void destroy(grib_context* context, grib_accessor* a)
{
    if (a->vvalue != NULL) {
        grib_context_free(context, a->vvalue);
        a->vvalue = NULL;
    }
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
