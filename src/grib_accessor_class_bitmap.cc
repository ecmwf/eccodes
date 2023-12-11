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
#include <type_traits>

/*
   This is used by make_class.pl

   START_CLASS_DEF
   CLASS      = accessor
   SUPER      = grib_accessor_class_bytes

   IMPLEMENTS = next_offset
   IMPLEMENTS = unpack_double;unpack_double_element;unpack_double_element_set
   IMPLEMENTS = unpack_float
   IMPLEMENTS = unpack_long
   IMPLEMENTS = unpack_string
   IMPLEMENTS = init;dump;update_size
   MEMBERS=const char* tableReference
   MEMBERS=const char* missing_value
   MEMBERS=const char* offsetbsec
   MEMBERS=const char* sLength
   END_CLASS_DEF

 */

/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "accessor.class" and rerun ./make_class.pl

*/

static int unpack_double(grib_accessor*, double* val, size_t* len);
static int unpack_float(grib_accessor*, float* val, size_t* len);
static int unpack_long(grib_accessor*, long* val, size_t* len);
static int unpack_string(grib_accessor*, char*, size_t* len);
static long next_offset(grib_accessor*);
static void dump(grib_accessor*, grib_dumper*);
static void init(grib_accessor*, const long, grib_arguments*);
static void update_size(grib_accessor*, size_t);
static int unpack_double_element(grib_accessor*, size_t i, double* val);
static int unpack_double_element_set(grib_accessor*, const size_t* index_array, size_t len, double* val_array);

typedef struct grib_accessor_bitmap
{
    grib_accessor att;
    /* Members defined in gen */
    /* Members defined in bytes */
    /* Members defined in bitmap */
    const char* tableReference;
    const char* missing_value;
    const char* offsetbsec;
    const char* sLength;
} grib_accessor_bitmap;

extern grib_accessor_class* grib_accessor_class_bytes;

static grib_accessor_class _grib_accessor_class_bitmap = {
    &grib_accessor_class_bytes,                      /* super */
    "bitmap",                      /* name */
    sizeof(grib_accessor_bitmap),  /* size */
    0,                           /* inited */
    0,                           /* init_class */
    &init,                       /* init */
    0,                  /* post_init */
    0,                    /* destroy */
    &dump,                       /* dump */
    &next_offset,                /* next_offset */
    0,              /* get length of string */
    0,                /* get number of values */
    0,                 /* get number of bytes */
    0,                /* get offset to bytes */
    0,            /* get native type */
    0,                /* get sub_section */
    0,               /* pack_missing */
    0,                 /* is_missing */
    0,                  /* pack_long */
    &unpack_long,                /* unpack_long */
    0,                /* pack_double */
    0,                 /* pack_float */
    &unpack_double,              /* unpack_double */
    &unpack_float,               /* unpack_float */
    0,                /* pack_string */
    &unpack_string,              /* unpack_string */
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
    &unpack_double_element,      /* unpack only ith value (double) */
    0,       /* unpack only ith value (float) */
    &unpack_double_element_set,  /* unpack a given set of elements (double) */
    0,   /* unpack a given set of elements (float) */
    0,     /* unpack a subarray */
    0,                      /* clear */
    0,                 /* clone accessor */
};


grib_accessor_class* grib_accessor_class_bitmap = &_grib_accessor_class_bitmap;

/* END_CLASS_IMP */


static void compute_size(grib_accessor* a)
{
    long slen         = 0;
    long off          = 0;
    grib_handle* hand = grib_handle_of_accessor(a);

    grib_accessor_bitmap* self = (grib_accessor_bitmap*)a;
    grib_get_long_internal(hand, self->offsetbsec, &off);
    grib_get_long_internal(hand, self->sLength, &slen);

    if (slen == 0) {
        grib_accessor* seclen;
        size_t size;
        /* Assume reparsing */
        Assert(hand->loader != 0);
        if (hand->loader != 0) {
            seclen = grib_find_accessor(hand, self->sLength);
            Assert(seclen);
            grib_get_block_length(seclen->parent, &size);
            slen = size;
        }
    }

    // printf("compute_size off=%ld slen=%ld a->offset=%ld\n", (long)off,(long)slen,(long)a->offset);

    a->length = off + (slen - a->offset);

    if (a->length < 0) {
        /* Assume reparsing */
        /*Assert(hand->loader != 0);*/
        a->length = 0;
    }

    Assert(a->length >= 0);
}

static void init(grib_accessor* a, const long len, grib_arguments* arg)
{
    grib_accessor_bitmap* self = (grib_accessor_bitmap*)a;
    grib_handle* hand          = grib_handle_of_accessor(a);
    int n                      = 0;

    self->tableReference = grib_arguments_get_name(hand, arg, n++);
    self->missing_value  = grib_arguments_get_name(hand, arg, n++);
    self->offsetbsec     = grib_arguments_get_name(hand, arg, n++);
    self->sLength        = grib_arguments_get_name(hand, arg, n++);

    compute_size(a);
}

static long next_offset(grib_accessor* a)
{
    return grib_byte_offset(a) + grib_byte_count(a);
}

static void dump(grib_accessor* a, grib_dumper* dumper)
{
    long len = 0;
    char label[1024];

    grib_value_count(a, &len);

    snprintf(label, 1024, "Bitmap of %ld values", len);
    grib_dump_bytes(dumper, a, label);
}

static int unpack_long(grib_accessor* a, long* val, size_t* len)
{
    long pos          = a->offset * 8;
    long tlen         = 0;
    long i            = 0;
    int err           = 0;
    grib_handle* hand = grib_handle_of_accessor(a);

    err = grib_value_count(a, &tlen);
    if (err)
        return err;

    if (*len < tlen) {
        grib_context_log(a->context, GRIB_LOG_ERROR, "Wrong size for %s, it contains %ld values", a->name, tlen);
        *len = 0;
        return GRIB_ARRAY_TOO_SMALL;
    }

    for (i = 0; i < tlen; i++) {
        val[i] = (long)grib_decode_unsigned_long(hand->buffer->data, &pos, 1);
    }
    *len = tlen;
    return GRIB_SUCCESS;
}

template <typename T>
static int unpack(grib_accessor* a, T* val, size_t* len)
{
    static_assert(std::is_floating_point<T>::value, "Requires floating points numbers");
    long pos = a->offset * 8;
    long tlen;
    long i;
    int err           = 0;
    grib_handle* hand = grib_handle_of_accessor(a);

    err = grib_value_count(a, &tlen);
    if (err)
        return err;

    if (*len < tlen) {
        grib_context_log(a->context, GRIB_LOG_ERROR, "Wrong size for %s, it contains %ld values", a->name, tlen);
        *len = 0;
        return GRIB_ARRAY_TOO_SMALL;
    }

    for (i = 0; i < tlen; i++) {
        val[i] = (T)grib_decode_unsigned_long(hand->buffer->data, &pos, 1);
    }
    *len = tlen;
    return GRIB_SUCCESS;
}

static int unpack_double(grib_accessor* a, double* val, size_t* len)
{
    return unpack<double>(a, val, len);
}

static int unpack_float(grib_accessor* a, float* val, size_t* len)
{
    return unpack<float>(a, val, len);
}

static int unpack_double_element(grib_accessor* a, size_t idx, double* val)
{
    long pos = a->offset * 8;

    pos += idx;
    *val = (double)grib_decode_unsigned_long(grib_handle_of_accessor(a)->buffer->data, &pos, 1);

    return GRIB_SUCCESS;
}
static int unpack_double_element_set(grib_accessor* a, const size_t* index_array, size_t len, double* val_array)
{
    size_t i = 0;
    for (i=0; i<len; ++i) {
        unpack_double_element(a, index_array[i], val_array + i);
    }
    return GRIB_SUCCESS;
}

static void update_size(grib_accessor* a, size_t s)
{
    a->length = s;
}

static int unpack_string(grib_accessor* a, char* val, size_t* len)
{
    int i             = 0;
    grib_handle* hand = grib_handle_of_accessor(a);

    if (len[0] < (a->length)) {
        grib_context_log(a->context, GRIB_LOG_ERROR, "unpack_string: Wrong size (%lu) for %s, it contains %ld values",
                len[0], a->name, a->length);
        len[0] = 0;
        return GRIB_ARRAY_TOO_SMALL;
    }

    for (i = 0; i < a->length; i++)
        val[i] = hand->buffer->data[a->offset + i];

    len[0] = a->length;

    return GRIB_SUCCESS;
}
