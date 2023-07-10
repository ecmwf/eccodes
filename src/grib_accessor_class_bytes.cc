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
   IMPLEMENTS = get_native_type;init
   IMPLEMENTS = compare;unpack_string;pack_string
   END_CLASS_DEF

 */

/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "accessor.class" and rerun ./make_class.pl

*/

static int get_native_type(grib_accessor*);
static int pack_string(grib_accessor*, const char*, size_t* len);
static int unpack_string(grib_accessor*, char*, size_t* len);
static void init(grib_accessor*, const long, grib_arguments*);
static int compare(grib_accessor*, grib_accessor*);

typedef struct grib_accessor_bytes
{
    grib_accessor att;
    /* Members defined in gen */
    /* Members defined in bytes */
} grib_accessor_bytes;

extern grib_accessor_class* grib_accessor_class_gen;

static grib_accessor_class _grib_accessor_class_bytes = {
    &grib_accessor_class_gen,                      /* super */
    "bytes",                      /* name */
    sizeof(grib_accessor_bytes),  /* size */
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


grib_accessor_class* grib_accessor_class_bytes = &_grib_accessor_class_bytes;

/* END_CLASS_IMP */

static void init(grib_accessor* a, const long len, grib_arguments* arg)
{
    /*grib_accessor_signed* self = (grib_accessor_signed*)a;  */
    a->length = len;
    Assert(a->length >= 0);
}

static int get_native_type(grib_accessor* a)
{
    return GRIB_TYPE_BYTES;
}

static int compare(grib_accessor* a, grib_accessor* b)
{
    int retval = GRIB_SUCCESS;

    size_t alen = (size_t)grib_byte_count(a);
    size_t blen = (size_t)grib_byte_count(b);

    if (alen != blen)
        return GRIB_COUNT_MISMATCH;

    return retval;
}

static int unpack_string(grib_accessor* a, char* v, size_t* len)
{
    unsigned char* p = NULL;
    char* s          = v;
    long i = 0;
    const long length = grib_byte_count(a);
    const long slength = 2 * length;

    if (*len < (size_t)slength) {
        *len = slength;
        return GRIB_ARRAY_TOO_SMALL;
    }

    p = grib_handle_of_accessor(a)->buffer->data + grib_byte_offset(a);

    for (i = 0; i < length; i++) {
        snprintf(s, INT_MAX, "%02x", *(p++));
        s += 2;
    }

    *len = slength;

    return GRIB_SUCCESS;
}

static int pack_string(grib_accessor* a, const char* val, size_t* len)
{
    /* The string representation (val) of the byte array will have two chars
     * per byte e.g. 4C5B means the 2 bytes 0114 and 0133 in octal
     * so has to be twice the length of the byte array
     */
    int err = 0;
    grib_accessor_class* super = *(a->cclass->super);
    grib_context* c = a->context;
    size_t nbytes = a->length;
    const size_t expected_blen = nbytes;
    const size_t expected_slen = 2 * expected_blen;
    unsigned char* bytearray = NULL;
    size_t i = 0, slen = strlen(val);

    if (slen != expected_slen || *len != expected_slen) {
        grib_context_log(c, GRIB_LOG_ERROR,"pack_string: key %s is %lu bytes. Expected a string with %lu characters",
                         a->name, expected_blen, expected_slen);
        return GRIB_WRONG_ARRAY_SIZE;
    }

    bytearray = (unsigned char*)grib_context_malloc(c, nbytes * (sizeof(unsigned char)));
    if (!bytearray) return GRIB_OUT_OF_MEMORY;

    for (i = 0; i < (slen/2); i++) {
        unsigned int byteVal = 0;
        if (sscanf(val + 2*i, "%02x", &byteVal) != 1) {
            grib_context_log(c, GRIB_LOG_ERROR,"pack_string: Invalid hex byte specfication '%.2s'",val + 2*i);
            grib_context_free(c, bytearray);
            return GRIB_INVALID_KEY_VALUE;
        }
        Assert(byteVal < 256);
        bytearray[i] = (int)byteVal;
    }

    /* Forward to base class to pack the byte array */
    err = super->pack_bytes(a, bytearray, &nbytes);
    grib_context_free(c, bytearray);
    return err;
}
