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
   IMPLEMENTS = init;unpack_string; get_native_type
   MEMBERS    = const char* codetable
   END_CLASS_DEF

 */

/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "accessor.class" and rerun ./make_class.pl

*/

static int get_native_type(grib_accessor*);
static int unpack_string(grib_accessor*, char*, size_t* len);
static void init(grib_accessor*, const long, grib_arguments*);

typedef struct grib_accessor_codetable_units
{
    grib_accessor att;
    /* Members defined in gen */
    /* Members defined in codetable_units */
    const char* codetable;
} grib_accessor_codetable_units;

extern grib_accessor_class* grib_accessor_class_gen;

static grib_accessor_class _grib_accessor_class_codetable_units = {
    &grib_accessor_class_gen,                      /* super */
    "codetable_units",                      /* name */
    sizeof(grib_accessor_codetable_units),  /* size */
    0,                           /* inited */
    0,                           /* init_class */
    &init,                       /* init */
    0,                  /* post_init */
    0,                    /* destroy */
    0,                       /* dump */
    0,                /* next_offset */
    0,              /* get length of string */
    0,                /* get number of values */
    0,                 /* get number of bytes */
    0,                /* get offset to bytes */
    &get_native_type,            /* get native type */
    0,                /* get sub_section */
    0,               /* pack_missing */
    0,                 /* is_missing */
    0,                  /* pack_long */
    0,                /* unpack_long */
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


grib_accessor_class* grib_accessor_class_codetable_units = &_grib_accessor_class_codetable_units;

/* END_CLASS_IMP */

typedef struct grib_accessor_codetable
{
    grib_accessor att;
    /* Members defined in gen */
    /* Members defined in long */
    /* Members defined in unsigned */
    long nbytes;
    grib_arguments* arg;
    /* Members defined in codetable */
    const char* tablename;
    const char* masterDir;
    const char* localDir;
    grib_codetable* table;
} grib_accessor_codetable;

static void init(grib_accessor* a, const long len, grib_arguments* params)
{
    grib_accessor_codetable_units* self = (grib_accessor_codetable_units*)a;

    int n           = 0;
    self->codetable = grib_arguments_get_name(grib_handle_of_accessor(a), params, n++);
    a->length       = 0;
    a->flags |= GRIB_ACCESSOR_FLAG_READ_ONLY;
}

static int get_native_type(grib_accessor* a)
{
    return GRIB_TYPE_STRING;
}

static int unpack_string(grib_accessor* a, char* buffer, size_t* len)
{
    grib_accessor_codetable_units* self = (grib_accessor_codetable_units*)a;
    grib_codetable* table = NULL;

    size_t size = 1;
    long value;
    int err = GRIB_SUCCESS;
    char tmp[1024];
    size_t l = sizeof(tmp);
    grib_accessor_codetable* ca = (grib_accessor_codetable*)grib_find_accessor(grib_handle_of_accessor(a), self->codetable);

    if ((err = grib_unpack_long((grib_accessor*)ca, &value, &size)) != GRIB_SUCCESS)
        return err;

    table = ca->table;

    if (table && (value >= 0) && (value < table->size) && table->entries[value].units) {
        strcpy(tmp, table->entries[value].units);
    }
    else {
        snprintf(tmp, sizeof(tmp), "%d", (int)value);
    }

    l = strlen(tmp) + 1;

    if (*len < l) {
        const char* cclass_name = a->cclass->name;
        grib_context_log(a->context, GRIB_LOG_ERROR,
                         "%s: Buffer too small for %s. It is %zu bytes long (len=%zu)",
                         cclass_name, a->name, l, *len);
        *len = l;
        return GRIB_BUFFER_TOO_SMALL;
    }

    strcpy(buffer, tmp);
    *len = l;

    return GRIB_SUCCESS;
}
