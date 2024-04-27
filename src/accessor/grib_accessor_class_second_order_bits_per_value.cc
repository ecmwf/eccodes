
/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_accessor_class_second_order_bits_per_value.h"
#include "grib_scaling.h"

grib_accessor_class_second_order_bits_per_value_t _grib_accessor_class_second_order_bits_per_value{"second_order_bits_per_value"};
grib_accessor_class* grib_accessor_class_second_order_bits_per_value = &_grib_accessor_class_second_order_bits_per_value;


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

int number_of_bits(size_t x, long* result){
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

void grib_accessor_class_second_order_bits_per_value_t::init(grib_accessor* a, const long l, grib_arguments* c){
    grib_accessor_class_long_t::init(a, l, c);
    int n                                           = 0;
    grib_accessor_second_order_bits_per_value_t* self = (grib_accessor_second_order_bits_per_value_t*)a;
    self->values                                    = grib_arguments_get_name(grib_handle_of_accessor(a), c, n++);
    self->binaryScaleFactor                         = grib_arguments_get_name(grib_handle_of_accessor(a), c, n++);
    self->decimalScaleFactor                        = grib_arguments_get_name(grib_handle_of_accessor(a), c, n++);
    self->bitsPerValue                              = 0;

    a->length = 0;
}

int grib_accessor_class_second_order_bits_per_value_t::pack_long(grib_accessor* a, const long* val, size_t* len){
    grib_accessor_second_order_bits_per_value_t* self = (grib_accessor_second_order_bits_per_value_t*)a;
    self->bitsPerValue                              = (long)*val;
    *len                                            = 1;

    return 0;
}

int grib_accessor_class_second_order_bits_per_value_t::unpack_long(grib_accessor* a, long* val, size_t* len){
    int ret     = GRIB_SUCCESS;
    size_t size = 0;
    size_t i;
    double max, min, d, b;
    double* values = 0;
    long binaryScaleFactor, decimalScaleFactor;

    grib_accessor_second_order_bits_per_value_t* self = (grib_accessor_second_order_bits_per_value_t*)a;
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
