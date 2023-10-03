/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */
/***********************************************************
 *  Enrico Fucile
 ***********************************************************/

#include "grib_api_internal.h"
/*
   This is used by make_class.pl

   START_CLASS_DEF
   CLASS      = accessor
   SUPER      = grib_accessor_class_gen
   IMPLEMENTS = unpack_string;pack_string
   IMPLEMENTS = unpack_long;pack_long
   IMPLEMENTS = unpack_double;pack_double
   IMPLEMENTS = init;dump;string_length
   IMPLEMENTS = value_count
   IMPLEMENTS = get_native_type
   IMPLEMENTS = compare
   END_CLASS_DEF

 */

/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "accessor.class" and rerun ./make_class.pl

*/

static int get_native_type(grib_accessor*);
static int pack_double(grib_accessor*, const double* val, size_t* len);
static int pack_long(grib_accessor*, const long* val, size_t* len);
static int pack_string(grib_accessor*, const char*, size_t* len);
static int unpack_double(grib_accessor*, double* val, size_t* len);
static int unpack_long(grib_accessor*, long* val, size_t* len);
static int unpack_string(grib_accessor*, char*, size_t* len);
static size_t string_length(grib_accessor*);
static int value_count(grib_accessor*, long*);
static void dump(grib_accessor*, grib_dumper*);
static void init(grib_accessor*, const long, grib_arguments*);
static int compare(grib_accessor*, grib_accessor*);

typedef struct grib_accessor_ascii
{
    grib_accessor att;
    /* Members defined in gen */
    /* Members defined in ascii */
} grib_accessor_ascii;

extern grib_accessor_class* grib_accessor_class_gen;

static grib_accessor_class _grib_accessor_class_ascii = {
    &grib_accessor_class_gen,                      /* super */
    "ascii",                      /* name */
    sizeof(grib_accessor_ascii),  /* size */
    0,                           /* inited */
    0,                           /* init_class */
    &init,                       /* init */
    0,                  /* post_init */
    0,                    /* destroy */
    &dump,                       /* dump */
    0,                /* next_offset */
    &string_length,              /* get length of string */
    &value_count,                /* get number of values */
    0,                 /* get number of bytes */
    0,                /* get offset to bytes */
    &get_native_type,            /* get native type */
    0,                /* get sub_section */
    0,               /* pack_missing */
    0,                 /* is_missing */
    &pack_long,                  /* pack_long */
    &unpack_long,                /* unpack_long */
    &pack_double,                /* pack_double */
    0,                 /* pack_float */
    &unpack_double,              /* unpack_double */
    0,               /* unpack_float */
    &pack_string,                /* pack_string */
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
    &compare,                    /* compare vs. another accessor */
    0,      /* unpack only ith value (double) */
    0,       /* unpack only ith value (float) */
    0,  /* unpack a given set of elements (double) */
    0,   /* unpack a given set of elements (float) */
    0,     /* unpack a subarray */
    0,                      /* clear */
    0,                 /* clone accessor */
};


grib_accessor_class* grib_accessor_class_ascii = &_grib_accessor_class_ascii;

/* END_CLASS_IMP */

static void init(grib_accessor* a, const long len, grib_arguments* arg)
{
    a->length = len;
    Assert(a->length >= 0);
}

static int value_count(grib_accessor* a, long* count)
{
    *count = 1;
    return 0;
}

static size_t string_length(grib_accessor* a)
{
    return a->length;
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
    size_t i = 0;
    grib_handle* hand = grib_handle_of_accessor(a);
    const size_t alen = a->length;

    if (len[0] < (alen + 1)) {
        grib_context_log(a->context, GRIB_LOG_ERROR, "unpack_string: Wrong size (%zu) for %s, it contains %ld values",
                len[0], a->name, a->length + 1);
        len[0] = 0;
        return GRIB_ARRAY_TOO_SMALL;
    }

    for (i = 0; i < alen; i++)
        val[i] = hand->buffer->data[a->offset + i];
    val[i] = 0;
    len[0] = i;
    return GRIB_SUCCESS;
}

static int pack_string(grib_accessor* a, const char* val, size_t* len)
{
    size_t i = 0;
    grib_handle* hand = grib_handle_of_accessor(a);
    const size_t alen = a->length;
    if (len[0] > (alen + 1)) {
        grib_context_log(a->context, GRIB_LOG_ERROR,
                "pack_string: Wrong size (%zu) for %s, it contains %ld values", len[0], a->name, a->length + 1);
        len[0] = 0;
        return GRIB_BUFFER_TOO_SMALL;
    }

    for (i = 0; i < alen; i++) {
        if (i < len[0])
            hand->buffer->data[a->offset + i] = val[i];
        else
            hand->buffer->data[a->offset + i] = 0;
    }

    return GRIB_SUCCESS;
}

static int pack_long(grib_accessor* a, const long* v, size_t* len)
{
    grib_context_log(a->context, GRIB_LOG_ERROR, "Should not pack %s as long (It's a string)", a->name);
    return GRIB_NOT_IMPLEMENTED;
}

static int pack_double(grib_accessor* a, const double* v, size_t* len)
{
    grib_context_log(a->context, GRIB_LOG_ERROR, "Should not pack %s as double (It's a string)", a->name);
    return GRIB_NOT_IMPLEMENTED;
}

static int unpack_long(grib_accessor* a, long* v, size_t* len)
{
    char val[1024] = {0,};
    size_t l   = sizeof(val);
    size_t i   = 0;
    char* last = NULL;
    int err    = grib_unpack_string(a, val, &l);

    if (err)
        return err;

    i = 0;
    while (i < l - 1 && val[i] == ' ')
        i++;

    if (val[i] == 0) {
        *v = 0;
        return 0;
    }
    if (val[i + 1] == ' ' && i < l - 2)
        val[i + 1] = 0;

    *v = strtol(val, &last, 10);

    grib_context_log(a->context, GRIB_LOG_DEBUG, " Casting string %s to long", a->name);
    return GRIB_SUCCESS;
}

static int unpack_double(grib_accessor* a, double* v, size_t* len)
{
    char val[1024];
    size_t l   = sizeof(val);
    char* last = NULL;

    int err = grib_unpack_string(a, val, &l);
    if (err) return err;

    *v = strtod(val, &last);

    if (*last == 0) {
        grib_context_log(a->context, GRIB_LOG_DEBUG, " Casting string %s to long", a->name);
        return GRIB_SUCCESS;
    }

    grib_context_log(a->context, GRIB_LOG_WARNING, "Cannot unpack %s as double. Hint: Try unpacking as string", a->name);

    return GRIB_NOT_IMPLEMENTED;
}

static int compare(grib_accessor* a, grib_accessor* b)
{
    int retval = 0;
    char* aval = 0;
    char* bval = 0;
    int err    = 0;

    size_t alen = a->length+1;
    size_t blen = b->length+1;

    if (alen != blen)
        return GRIB_COUNT_MISMATCH;

    aval = (char*)grib_context_malloc(a->context, alen * sizeof(char));
    bval = (char*)grib_context_malloc(b->context, blen * sizeof(char));

    err = grib_unpack_string(a, aval, &alen);
    if (err) return err;
    err = grib_unpack_string(b, bval, &blen);
    if (err) return err;

    retval = GRIB_SUCCESS;
    if (!STR_EQUAL(aval, bval))
        retval = GRIB_STRING_VALUE_MISMATCH;

    grib_context_free(a->context, aval);
    grib_context_free(b->context, bval);

    return retval;
}
