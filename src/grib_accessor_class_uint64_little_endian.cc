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
   IMPLEMENTS = unpack_long
   IMPLEMENTS = pack_long
   IMPLEMENTS = get_native_type
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

typedef struct grib_accessor_uint64_little_endian
{
    grib_accessor att;
    /* Members defined in gen */
    /* Members defined in uint64_little_endian */
} grib_accessor_uint64_little_endian;

extern grib_accessor_class* grib_accessor_class_gen;

static grib_accessor_class _grib_accessor_class_uint64_little_endian = {
    &grib_accessor_class_gen,                      /* super */
    "uint64_little_endian",                      /* name */
    sizeof(grib_accessor_uint64_little_endian),  /* size */
    0,                           /* inited */
    0,                           /* init_class */
    0,                       /* init */
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
    0,                    /* compare vs. another accessor */
    0,      /* unpack only ith value (double) */
    0,       /* unpack only ith value (float) */
    0,  /* unpack a given set of elements (double) */
    0,   /* unpack a given set of elements (float) */
    0,     /* unpack a subarray */
    0,                      /* clear */
    0,                 /* clone accessor */
};


grib_accessor_class* grib_accessor_class_uint64_little_endian = &_grib_accessor_class_uint64_little_endian;

/* END_CLASS_IMP */

static int unpack_long(grib_accessor* a, long* val, size_t* len)
{
    long value                = 0;
    long pos                  = a->offset;
    unsigned char* data       = grib_handle_of_accessor(a)->buffer->data;
    unsigned long long result = 0, tmp;
    int i;

    if (*len < 1) {
        grib_context_log(a->context, GRIB_LOG_ERROR, "Wrong size for %s it contains %d values ", a->name, 1);
        *len = 0;
        return GRIB_ARRAY_TOO_SMALL;
    }

    for (i = 7; i >= 0; i--) {
        result <<= 8;
        result |= data[pos + i];
    }

    value = result;
    tmp   = value;

    /* Result does not fit in long */
    if (tmp != result) {
        grib_context_log(a->context, GRIB_LOG_ERROR, "Value for %s cannot be decoded as a 'long' (%llu)", a->name, result);
        return GRIB_DECODING_ERROR;
    }

    *val = value;
    *len = 1;
    return GRIB_SUCCESS;
}


static int pack_long(grib_accessor* a, const long* val, size_t* len)
{
    return GRIB_NOT_IMPLEMENTED;
}

static int get_native_type(grib_accessor* a)
{
    return GRIB_TYPE_LONG;
}
