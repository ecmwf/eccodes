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
   IMPLEMENTS = unpack_long;pack_long
   IMPLEMENTS = init;dump
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
static int pack_long(grib_accessor*, const long* val, size_t* len);
static int unpack_long(grib_accessor*, long* val, size_t* len);
static void dump(grib_accessor*, grib_dumper*);
static void init(grib_accessor*, const long, grib_arguments*);
static int compare(grib_accessor*, grib_accessor*);

typedef struct grib_accessor_g1_half_byte_codeflag
{
    grib_accessor att;
    /* Members defined in gen */
    /* Members defined in g1_half_byte_codeflag */
} grib_accessor_g1_half_byte_codeflag;

extern grib_accessor_class* grib_accessor_class_gen;

static grib_accessor_class _grib_accessor_class_g1_half_byte_codeflag = {
    &grib_accessor_class_gen,                      /* super */
    "g1_half_byte_codeflag",                      /* name */
    sizeof(grib_accessor_g1_half_byte_codeflag),  /* size */
    0,                           /* inited */
    0,                           /* init_class */
    &init,                       /* init */
    0,                  /* post_init */
    0,                    /* destroy */
    &dump,                       /* dump */
    0,                /* next_offset */
    0,              /* get length of string */
    0,                /* get number of values */
    0,                 /* get number of bytes */
    0,                /* get offset to bytes */
    &get_native_type,            /* get native type */
    0,                /* get sub_section */
    0,               /* pack_missing */
    0,                 /* is_missing */
    &pack_long,                  /* pack_long */
    &unpack_long,                /* unpack_long */
    0,                /* pack_double */
    0,                 /* pack_float */
    0,              /* unpack_double */
    0,               /* unpack_float */
    0,                /* pack_string */
    0,              /* unpack_string */
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


grib_accessor_class* grib_accessor_class_g1_half_byte_codeflag = &_grib_accessor_class_g1_half_byte_codeflag;

/* END_CLASS_IMP */

static void init(grib_accessor* a, const long len, grib_arguments* arg)
{
    a->length = 0;
    a->flags |= GRIB_ACCESSOR_FLAG_READ_ONLY;
    a->flags |= GRIB_ACCESSOR_FLAG_EDITION_SPECIFIC;
}

static void dump(grib_accessor* a, grib_dumper* dumper)
{
    grib_dump_long(dumper, a, NULL);
}

static int unpack_long(grib_accessor* a, long* val, size_t* len)
{
    unsigned char dat = 0;
    if (*len < 1) {
        grib_context_log(a->context, GRIB_LOG_ERROR, "Wrong size for %s it contains %d values ", a->name, 1);
        *len = 0;
        return GRIB_ARRAY_TOO_SMALL;
    }
    dat = grib_handle_of_accessor(a)->buffer->data[a->offset] & 0x0f;

    *val = dat;
    *len = 1;
    return GRIB_SUCCESS;
}

static int pack_long(grib_accessor* a, const long* val, size_t* len)
{
    int ret = 0;
    if (*len < 1) {
        grib_context_log(a->context, GRIB_LOG_ERROR, "Wrong size for %s it contains %d values ", a->name, 1);
        *len = 0;
        return GRIB_ARRAY_TOO_SMALL;
    }
    /*  printf("HALF BYTE pack long %ld %02x\n",*val,grib_handle_of_accessor(a)->buffer->data[a->offset]);*/
    grib_handle_of_accessor(a)->buffer->data[a->offset] = (a->parent->h->buffer->data[a->offset] & 0xf0) | (*val & 0x0f);
    /*  printf("HALF BYTE pack long %ld %02x\n",*val,grib_handle_of_accessor(a)->buffer->data[a->offset]);*/

    *len = 1;
    return ret;
}

static int get_native_type(grib_accessor* a)
{
    return GRIB_TYPE_LONG;
}

static int compare(grib_accessor* a, grib_accessor* b)
{
    long aval = 0;
    long bval = 0;

    long count  = 0;
    size_t alen = 0;
    size_t blen = 0;
    int err     = 0;

    err = grib_value_count(a, &count);
    if (err)
        return err;
    alen = count;

    err = grib_value_count(b, &count);
    if (err)
        return err;
    blen = count;

    if (alen != 1 || blen != 1)
        return GRIB_COUNT_MISMATCH;

    grib_unpack_long(a, &aval, &alen);
    grib_unpack_long(b, &bval, &blen);

    if (bval != aval)
        return GRIB_VALUE_MISMATCH;
    return GRIB_SUCCESS;
}
