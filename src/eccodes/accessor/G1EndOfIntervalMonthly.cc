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
   SUPER      = grib_accessor_class_bitmap
   IMPLEMENTS = pack_double; unpack_bytes
   IMPLEMENTS = value_count
   IMPLEMENTS = init;
   MEMBERS=const char* unusedBits
   END_CLASS_DEF

 */

/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "accessor.class" and rerun ./make_class.pl

*/

static int pack_double(grib_accessor*, const double* val, size_t* len);
static int unpack_bytes(grib_accessor*, unsigned char*, size_t* len);
static int value_count(grib_accessor*, long*);
static void init(grib_accessor*, const long, grib_arguments*);

typedef struct grib_accessor_g1bitmap
{
    grib_accessor att;
    /* Members defined in gen */
    /* Members defined in bytes */
    /* Members defined in bitmap */
    const char* tableReference;
    const char* missing_value;
    const char* offsetbsec;
    const char* sLength;
    /* Members defined in g1bitmap */
    const char* unusedBits;
} grib_accessor_g1bitmap;

extern grib_accessor_class* grib_accessor_class_bitmap;

static grib_accessor_class _grib_accessor_class_g1bitmap = {
    &grib_accessor_class_bitmap,                      /* super */
    "g1bitmap",                      /* name */
    sizeof(grib_accessor_g1bitmap),  /* size */
    0,                           /* inited */
    0,                           /* init_class */
    &init,                       /* init */
    0,                  /* post_init */
    0,                    /* destroy */
    0,                       /* dump */
    0,                /* next_offset */
    0,              /* get length of string */
    &value_count,                /* get number of values */
    0,                 /* get number of bytes */
    0,                /* get offset to bytes */
    0,            /* get native type */
    0,                /* get sub_section */
    0,               /* pack_missing */
    0,                 /* is_missing */
    0,                  /* pack_long */
    0,                /* unpack_long */
    &pack_double,                /* pack_double */
    0,                 /* pack_float */
    0,              /* unpack_double */
    0,               /* unpack_float */
    0,                /* pack_string */
    0,              /* unpack_string */
    0,          /* pack_string_array */
    0,        /* unpack_string_array */
    0,                 /* pack_bytes */
    &unpack_bytes,               /* unpack_bytes */
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


grib_accessor_class* grib_accessor_class_g1bitmap = &_grib_accessor_class_g1bitmap;

/* END_CLASS_IMP */


static void grib_set_bit_on(unsigned char* p, long* bitp);

static void init(grib_accessor* a, const long len, grib_arguments* arg)
{
    grib_accessor_g1bitmap* self = (grib_accessor_g1bitmap*)a;
    self->unusedBits             = grib_arguments_get_name(grib_handle_of_accessor(a), arg, 4);
}

static int pack_double(grib_accessor* a, const double* val, size_t* len)
{
    grib_accessor_g1bitmap* self = (grib_accessor_g1bitmap*)a;
    size_t tlen;

    unsigned char* buf = NULL;
    size_t i;
    int err               = 0;
    long pos              = 0;
    //long bmaplen          = 0;
    const int bit_padding = 16;
    double miss_values    = 0;
    tlen                  = ((*len + bit_padding - 1) / bit_padding * bit_padding) / 8;

    if ((err = grib_get_double_internal(grib_handle_of_accessor(a), self->missing_value, &miss_values)) != GRIB_SUCCESS)
        return err;

    buf = (unsigned char*)grib_context_malloc_clear(a->context, tlen);
    if (!buf)
        return GRIB_OUT_OF_MEMORY;
    pos = 0;
    for (i = 0; i < *len; i++) {
        if (val[i] == miss_values)
            pos++;
        else {
            //bmaplen++;
            grib_set_bit_on(buf, &pos);
        }
    }

    if ((err = grib_set_long_internal(grib_handle_of_accessor(a), self->unusedBits, tlen * 8 - *len)) != GRIB_SUCCESS)
        return err;

    err = grib_buffer_replace(a, buf, tlen, 1, 1);
    if (err) return err;

    grib_context_free(a->context, buf);

    return GRIB_SUCCESS;
}

static int value_count(grib_accessor* a, long* count)
{
    grib_accessor_g1bitmap* self = (grib_accessor_g1bitmap*)a;
    long tlen;
    int err;

    if ((err = grib_get_long_internal(grib_handle_of_accessor(a), self->unusedBits, &tlen)) != GRIB_SUCCESS)
        grib_context_log(a->context, GRIB_LOG_ERROR, "grib_accessor_class_bitmap.value_count : cannot get %s err=%d", self->unusedBits, err);

    *count = (a->length * 8) - tlen;
    return err;
}

static int unpack_bytes(grib_accessor* a, unsigned char* val, size_t* len)
{
    unsigned char* buf           = grib_handle_of_accessor(a)->buffer->data;
    grib_accessor_g1bitmap* self = (grib_accessor_g1bitmap*)a;
    long tlen;
    int err;
    long length = grib_byte_count(a);
    long offset = grib_byte_offset(a);

    if (*len < (size_t)length) {
        grib_context_log(a->context, GRIB_LOG_ERROR, "Wrong size for %s it is %ld bytes long\n", a->name, length);
        *len = length;
        return GRIB_ARRAY_TOO_SMALL;
    }

    if ((err = grib_get_long_internal(grib_handle_of_accessor(a), self->unusedBits, &tlen)) != GRIB_SUCCESS)
        grib_context_log(a->context, GRIB_LOG_ERROR,
                         "grib_accessor_class_bitmap.unpack_bytes : cannot get %s err=%d", self->unusedBits, err);

    length -= tlen / 8;
    memcpy(val, buf + offset, length);
    *len = length;

    return GRIB_SUCCESS;
}

static void grib_set_bit_on(unsigned char* p, long* bitp)
{
    unsigned char o = 1;
    p += (*bitp >> 3);
    o <<= 7 - ((*bitp) % 8);
    *p |= o;
    (*bitp) += 1;
}
