
/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_accessor_class_bits_per_value.h"

grib_accessor_class_bits_per_value_t _grib_accessor_class_bits_per_value{ "bits_per_value" };
grib_accessor_class* grib_accessor_class_bits_per_value = &_grib_accessor_class_bits_per_value;


void grib_accessor_class_bits_per_value_t::init(grib_accessor* a, const long l, grib_arguments* args)
{
    grib_accessor_class_long_t::init(a, l, args);
    int n = 0;
    grib_accessor_bits_per_value_t* self = (grib_accessor_bits_per_value_t*)a;
    self->values         = grib_arguments_get_name(grib_handle_of_accessor(a), args, n++);
    self->bits_per_value = grib_arguments_get_name(grib_handle_of_accessor(a), args, n++);
    a->flags |= GRIB_ACCESSOR_FLAG_FUNCTION;
    a->length = 0;
}

int grib_accessor_class_bits_per_value_t::unpack_long(grib_accessor* a, long* val, size_t* len)
{
    grib_accessor_bits_per_value_t* self = (grib_accessor_bits_per_value_t*)a;

    int ret = 0;
    grib_handle* h = grib_handle_of_accessor(a);

    if ((ret = grib_get_long_internal(h, self->bits_per_value, val)) != GRIB_SUCCESS)
        return ret;

    *len = 1;
    return ret;
}

int grib_accessor_class_bits_per_value_t::pack_long(grib_accessor* a, const long* val, size_t* len)
{
    grib_accessor_bits_per_value_t* self = (grib_accessor_bits_per_value_t*)a;

    double* values  = NULL;
    size_t size     = 0;
    int ret         = 0;
    grib_context* c = a->context;
    grib_handle* h  = grib_handle_of_accessor(a);

    if ((ret = grib_get_size(h, self->values, &size)) != GRIB_SUCCESS)
        return ret;

    values = (double*)grib_context_malloc(c, size * sizeof(double));
    if (!values)
        return GRIB_OUT_OF_MEMORY;

    if ((ret = grib_get_double_array_internal(h, self->values, values, &size)) != GRIB_SUCCESS) {
        grib_context_free(c, values);
        return ret;
    }

    if ((ret = grib_set_long_internal(h, self->bits_per_value, *val)) != GRIB_SUCCESS) {
        grib_context_free(c, values);
        return ret;
    }

    if ((ret = grib_set_double_array_internal(h, self->values, values, size)) != GRIB_SUCCESS) {
        grib_context_free(c, values);
        return ret;
    }

    grib_context_free(c, values);

    return GRIB_SUCCESS;
}
