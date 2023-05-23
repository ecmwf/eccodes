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
   SUPER      = grib_accessor_class_long
   IMPLEMENTS = unpack_long;pack_long
   IMPLEMENTS = init;dump
   IMPLEMENTS = next_offset
   IMPLEMENTS = byte_count
   IMPLEMENTS = value_count
   IMPLEMENTS = byte_offset
   IMPLEMENTS = update_size
   MEMBERS    = const char* numberOfBits
   MEMBERS    = const char* numberOfElements

   END_CLASS_DEF

 */

/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "accessor.class" and rerun ./make_class.pl

*/

static int pack_long(grib_accessor*, const long* val, size_t* len);
static int unpack_long(grib_accessor*, long* val, size_t* len);
static long byte_count(grib_accessor*);
static long byte_offset(grib_accessor*);
static long next_offset(grib_accessor*);
static int value_count(grib_accessor*, long*);
static void dump(grib_accessor*, grib_dumper*);
static void init(grib_accessor*, const long, grib_arguments*);
//static void init_class(grib_accessor_class*);
static void update_size(grib_accessor*, size_t);

typedef struct grib_accessor_signed_bits
{
    grib_accessor att;
    /* Members defined in gen */
    /* Members defined in long */
    /* Members defined in signed_bits */
    const char* numberOfBits;
    const char* numberOfElements;
} grib_accessor_signed_bits;

extern grib_accessor_class* grib_accessor_class_long;

static grib_accessor_class _grib_accessor_class_signed_bits = {
    &grib_accessor_class_long,                      /* super */
    "signed_bits",                      /* name */
    sizeof(grib_accessor_signed_bits),  /* size */
    0,                           /* inited */
    0,                           /* init_class */
    &init,                       /* init */
    0,                  /* post_init */
    0,                    /* destroy */
    &dump,                       /* dump */
    &next_offset,                /* next_offset */
    0,              /* get length of string */
    &value_count,                /* get number of values */
    &byte_count,                 /* get number of bytes */
    &byte_offset,                /* get offset to bytes */
    0,            /* get native type */
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
    &update_size,                /* update_size */
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


grib_accessor_class* grib_accessor_class_signed_bits = &_grib_accessor_class_signed_bits;


//static void init_class(grib_accessor_class* c)
//{
// INIT
//}

/* END_CLASS_IMP */

static long byte_count(grib_accessor* a)
{
    return a->length;
}

static long compute_byte_count(grib_accessor* a)
{
    grib_accessor_signed_bits* self = (grib_accessor_signed_bits*)a;
    long numberOfBits;
    long numberOfElements;
    int ret = 0;

    ret = grib_get_long(grib_handle_of_accessor(a), self->numberOfBits, &numberOfBits);
    if (ret) {
        grib_context_log(a->context, GRIB_LOG_ERROR,
                         "%s unable to get %s to compute size", a->name, self->numberOfBits);
        return 0;
    }

    ret = grib_get_long(grib_handle_of_accessor(a), self->numberOfElements, &numberOfElements);
    if (ret) {
        grib_context_log(a->context, GRIB_LOG_ERROR,
                         "%s unable to get %s to compute size", a->name, self->numberOfElements);
        return 0;
    }

    return (numberOfBits * numberOfElements + 7) / 8;
}

static void init(grib_accessor* a, const long len, grib_arguments* args)
{
    grib_accessor_signed_bits* self = (grib_accessor_signed_bits*)a;
    int n                           = 0;
    self->numberOfBits              = grib_arguments_get_name(grib_handle_of_accessor(a), args, n++);
    self->numberOfElements          = grib_arguments_get_name(grib_handle_of_accessor(a), args, n++);
    a->length                       = compute_byte_count(a);
}

static void dump(grib_accessor* a, grib_dumper* dumper)
{
    grib_dump_long(dumper, a, NULL);
}

static int unpack_long(grib_accessor* a, long* val, size_t* len)
{
    grib_accessor_signed_bits* self = (grib_accessor_signed_bits*)a;
    int i;
    int ret           = 0;
    long pos          = a->offset * 8;
    long rlen         = 0;
    long numberOfBits = 0;

    ret = value_count(a, &rlen);
    if (ret)
        return ret;

    if (*len < rlen) {
        grib_context_log(a->context, GRIB_LOG_ERROR,
                         "Wrong size (%ld) for %s it contains %ld values", *len, a->name, rlen);
        *len = 0;
        return GRIB_ARRAY_TOO_SMALL;
    }

    ret = grib_get_long(grib_handle_of_accessor(a), self->numberOfBits, &numberOfBits);
    if (ret)
        return ret;

    if (numberOfBits == 0) {
        for (i = 0; i < rlen; i++)
            val[i] = 0;
        return GRIB_SUCCESS;
    }

    for (i = 0; i < rlen; i++)
        val[i] = grib_decode_signed_longb(grib_handle_of_accessor(a)->buffer->data, &pos, numberOfBits);

    *len = rlen;

    return GRIB_SUCCESS;
}

static int pack_long(grib_accessor* a, const long* val, size_t* len)
{
    grib_accessor_signed_bits* self = (grib_accessor_signed_bits*)a;
    int ret                         = 0;
    long off                        = 0;
    long numberOfBits               = 0;
    size_t buflen                   = 0;
    unsigned char* buf              = NULL;
    unsigned long i                 = 0;
    unsigned long rlen              = 0;
    long count                      = 0;

    ret = value_count(a, &count);
    if (ret)
        return ret;
    rlen = count;
    if (*len != rlen) {
        ret = grib_set_long(grib_handle_of_accessor(a), self->numberOfElements, rlen);
        if (ret)
            return ret;
    }

    ret = grib_get_long(grib_handle_of_accessor(a), self->numberOfBits, &numberOfBits);
    if (ret)
        return ret;

    buflen = compute_byte_count(a);
    buf    = (unsigned char*)grib_context_malloc_clear(a->context, buflen + sizeof(long));

    for (i = 0; i < rlen; i++)
        grib_encode_signed_longb(buf, val[i], &off, numberOfBits);

    grib_buffer_replace(a, buf, buflen, 1, 1);

    grib_context_free(a->context, buf);

    return ret;
}

static int value_count(grib_accessor* a, long* numberOfElements)
{
    grib_accessor_signed_bits* self = (grib_accessor_signed_bits*)a;
    *numberOfElements               = 0;

    return grib_get_long(grib_handle_of_accessor(a), self->numberOfElements, numberOfElements);
}

static long byte_offset(grib_accessor* a)
{
    return a->offset;
}

static void update_size(grib_accessor* a, size_t s)
{
    a->length = s;
}

static long next_offset(grib_accessor* a)
{
    return byte_offset(a) + byte_count(a);
}
