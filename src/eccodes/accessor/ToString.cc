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
   IMPLEMENTS = unpack_string
   IMPLEMENTS = unpack_long
   IMPLEMENTS = unpack_double
   IMPLEMENTS = init;dump;string_length
   IMPLEMENTS = value_count
   IMPLEMENTS = next_offset
   IMPLEMENTS = get_native_type
   MEMBERS = const char* key
   MEMBERS = long start
   MEMBERS = size_t length
   END_CLASS_DEF

 */

/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "accessor.class" and rerun ./make_class.pl

*/

static int get_native_type(grib_accessor*);
static int unpack_double(grib_accessor*, double* val, size_t* len);
static int unpack_long(grib_accessor*, long* val, size_t* len);
static int unpack_string(grib_accessor*, char*, size_t* len);
static size_t string_length(grib_accessor*);
static long next_offset(grib_accessor*);
static int value_count(grib_accessor*, long*);
static void dump(grib_accessor*, grib_dumper*);
static void init(grib_accessor*, const long, grib_arguments*);

typedef struct grib_accessor_to_string
{
    grib_accessor att;
    /* Members defined in gen */
    /* Members defined in to_string */
    const char* key;
    long start;
    size_t length;
} grib_accessor_to_string;

extern grib_accessor_class* grib_accessor_class_gen;

static grib_accessor_class _grib_accessor_class_to_string = {
    &grib_accessor_class_gen,                      /* super */
    "to_string",                      /* name */
    sizeof(grib_accessor_to_string),  /* size */
    0,                           /* inited */
    0,                           /* init_class */
    &init,                       /* init */
    0,                  /* post_init */
    0,                    /* destroy */
    &dump,                       /* dump */
    &next_offset,                /* next_offset */
    &string_length,              /* get length of string */
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
    &unpack_double,              /* unpack_double */
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


grib_accessor_class* grib_accessor_class_to_string = &_grib_accessor_class_to_string;

/* END_CLASS_IMP */

static void init(grib_accessor* a, const long len, grib_arguments* arg)
{
    grib_accessor_to_string* self = (grib_accessor_to_string*)a;

    self->key    = grib_arguments_get_name(grib_handle_of_accessor(a), arg, 0);
    self->start  = grib_arguments_get_long(grib_handle_of_accessor(a), arg, 1);
    self->length = grib_arguments_get_long(grib_handle_of_accessor(a), arg, 2);

    a->flags |= GRIB_ACCESSOR_FLAG_READ_ONLY;
    a->length = 0;
}

static int value_count(grib_accessor* a, long* count)
{
    grib_accessor_to_string* self = (grib_accessor_to_string*)a;
    size_t size                   = 0;

    int err = grib_get_size(grib_handle_of_accessor(a), self->key, &size);
    *count  = size;

    return err;
}

static size_t string_length(grib_accessor* a)
{
    grib_accessor_to_string* self = (grib_accessor_to_string*)a;

    if (self->length)
        return self->length;

    size_t size = 0;
    grib_get_string_length(grib_handle_of_accessor(a), self->key, &size);
    return size;
}

static void dump(grib_accessor* a, grib_dumper* dumper)
{
    grib_dump_string(dumper, a, NULL);
}

static int get_native_type(grib_accessor* a)
{
    return GRIB_TYPE_STRING;
}

static int unpack_string(grib_accessor* a, char* val, size_t* len)
{
    grib_accessor_to_string* self = (grib_accessor_to_string*)a;

    int err = 0;
    char buff[512] = {0,};

    size_t length = string_length(a);

    if (*len < length + 1) {
        const char* cclass_name = a->cclass->name;
        grib_context_log(a->context, GRIB_LOG_ERROR,
                         "%s: Buffer too small for %s. It is %zu bytes long (len=%zu)",
                         cclass_name, a->name, length+1, *len);
        *len = length + 1;
        return GRIB_BUFFER_TOO_SMALL;
    }

    size_t size = sizeof(buff);
    err = grib_get_string(grib_handle_of_accessor(a), self->key, buff, &size);
    if (err)
        return err;
    if (length > size) {
        /*err=GRIB_STRING_TOO_SMALL;*/
        length = size;
    }

    memcpy(val, buff + self->start, length);

    val[length] = 0;
    *len = length;
    return GRIB_SUCCESS;
}

static int unpack_long(grib_accessor* a, long* v, size_t* len)
{
    char val[1024] = {0,};
    size_t l   = sizeof(val);
    char* last = NULL;
    int err    = unpack_string(a, val, &l);

    if (err)
        return err;

    *v = strtol(val, &last, 10);
    if (*last) {
        err = GRIB_WRONG_CONVERSION;
    }

    return err;
}

static int unpack_double(grib_accessor* a, double* v, size_t* len)
{
    size_t l = 1;
    long val = 0;
    int err  = unpack_long(a, &val, &l);

    *v = (double)val;
    return err;
}

static long next_offset(grib_accessor* a)
{
    return a->offset + a->length;
}
