/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

/**********************************
 *  Enrico Fucile
 **********************************/

#include "grib_scaling.h"
#include "grib_api_internal.h"
/*
   This is used by make_class.pl

   START_CLASS_DEF
   CLASS      = accessor
   SUPER      = grib_accessor_class_long
   IMPLEMENTS = unpack_long
   IMPLEMENTS = pack_long
   IMPLEMENTS = init
   MEMBERS = const char* values
   MEMBERS = const char* binaryScaleFactor
   MEMBERS = const char* decimalScaleFactor
   MEMBERS = long bitsPerValue
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
static void init(grib_accessor*, const long, grib_arguments*);
//static void init_class(grib_accessor_class*);

typedef struct grib_accessor_second_order_bits_per_value
{
    grib_accessor att;
    /* Members defined in gen */
    /* Members defined in long */
    /* Members defined in second_order_bits_per_value */
    const char* values;
    const char* binaryScaleFactor;
    const char* decimalScaleFactor;
    long bitsPerValue;
} grib_accessor_second_order_bits_per_value;

extern grib_accessor_class* grib_accessor_class_long;

static grib_accessor_class _grib_accessor_class_second_order_bits_per_value = {
    &grib_accessor_class_long,                      /* super */
    "second_order_bits_per_value",                      /* name */
    sizeof(grib_accessor_second_order_bits_per_value),  /* size */
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


grib_accessor_class* grib_accessor_class_second_order_bits_per_value = &_grib_accessor_class_second_order_bits_per_value;

/* END_CLASS_IMP */

static const size_t nbits[64] = {
    0x1, 0x2, 0x4, 0x8,
    0x10, 0x20, 0x40, 0x80,
    0x100, 0x200, 0x400, 0x800,
    0x1000, 0x2000, 0x4000, 0x8000,
    0x10000, 0x20000, 0x40000, 0x80000,
    0x100000, 0x200000, 0x400000, 0x800000,
    0x1000000, 0x2000000, 0x4000000, 0x8000000,
    0x10000000, 0x20000000, 0x40000000, 0x80000000,
    0x100000000, 0x200000000, 0x400000000, 0x800000000,
    0x1000000000, 0x2000000000, 0x4000000000, 0x8000000000,
    0x10000000000, 0x20000000000, 0x40000000000, 0x80000000000,
    0x100000000000, 0x200000000000, 0x400000000000, 0x800000000000,
    0x1000000000000, 0x2000000000000, 0x4000000000000, 0x8000000000000,
    0x10000000000000, 0x20000000000000, 0x40000000000000, 0x80000000000000,
    0x100000000000000, 0x200000000000000, 0x400000000000000, 0x800000000000000,
    0x1000000000000000, 0x2000000000000000, 0x4000000000000000, 0x8000000000000000
};

static int number_of_bits(size_t x, long* result)
{
    const size_t* n = nbits;
    const int count = sizeof(nbits) / sizeof(nbits[0]);
    *result         = 0;
    while (x >= *n) {
        n++;
        (*result)++;
        if (*result >= count) {
            return GRIB_ENCODING_ERROR;
        }
    }
    return GRIB_SUCCESS;
}

static void init(grib_accessor* a, const long l, grib_arguments* c)
{
    int n                                           = 0;
    grib_accessor_second_order_bits_per_value* self = (grib_accessor_second_order_bits_per_value*)a;
    self->values                                    = grib_arguments_get_name(grib_handle_of_accessor(a), c, n++);
    self->binaryScaleFactor                         = grib_arguments_get_name(grib_handle_of_accessor(a), c, n++);
    self->decimalScaleFactor                        = grib_arguments_get_name(grib_handle_of_accessor(a), c, n++);
    self->bitsPerValue                              = 0;

    a->length = 0;
}

static int pack_long(grib_accessor* a, const long* val, size_t* len)
{
    grib_accessor_second_order_bits_per_value* self = (grib_accessor_second_order_bits_per_value*)a;
    self->bitsPerValue                              = (long)*val;
    *len                                            = 1;

    return 0;
}

static int unpack_long(grib_accessor* a, long* val, size_t* len)
{
    int ret     = GRIB_SUCCESS;
    size_t size = 0;
    size_t i;
    double max, min, d, b;
    double* values = 0;
    long binaryScaleFactor, decimalScaleFactor;

    grib_accessor_second_order_bits_per_value* self = (grib_accessor_second_order_bits_per_value*)a;
    if (self->bitsPerValue) {
        *val = self->bitsPerValue;
        return GRIB_SUCCESS;
    }

    if ((ret = grib_get_size(grib_handle_of_accessor(a), self->values, &size)) != GRIB_SUCCESS) {
        *val = self->bitsPerValue;
        return GRIB_SUCCESS;
    }

    if ((ret = grib_get_long(grib_handle_of_accessor(a), self->binaryScaleFactor, &binaryScaleFactor)) != GRIB_SUCCESS)
        return ret;

    if ((ret = grib_get_long_internal(grib_handle_of_accessor(a), self->decimalScaleFactor, &decimalScaleFactor)) != GRIB_SUCCESS)
        return ret;

    values = (double*)grib_context_malloc_clear(a->context, sizeof(double) * size);
    if (!values) {
        grib_context_log(a->context, GRIB_LOG_ERROR, "%s: Memory allocation error: %zu bytes", a->name, size);
        return GRIB_OUT_OF_MEMORY;
    }
    if ((ret = grib_get_double_array_internal(grib_handle_of_accessor(a), self->values, values, &size)) != GRIB_SUCCESS)
        return ret;

    max = values[0];
    min = max;
    for (i = 1; i < size; i++) {
        if (max < values[i])
            max = values[i];
        else if (min > values[i])
            min = values[i];
    }

    d = codes_power<double>(decimalScaleFactor, 10);
    b = codes_power<double>(-binaryScaleFactor, 2);

    /* self->bitsPerValue=(long)ceil(log((double)((max-min)*d+1))/log(2.0))-binaryScaleFactor; */
    /* See GRIB-540 for why we use ceil */
    ret = number_of_bits((size_t)ceil((fabs(max - min) * b * d)), &(self->bitsPerValue));
    if (ret != GRIB_SUCCESS)
        return ret;
    *val = self->bitsPerValue;

    grib_context_free(a->context, values);

    return ret;
}
