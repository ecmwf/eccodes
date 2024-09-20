/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_accessor_class_offset_values.h"

grib_accessor_class_offset_values_t _grib_accessor_class_offset_values{ "offset_values" };
grib_accessor_class* grib_accessor_class_offset_values = &_grib_accessor_class_offset_values;


void grib_accessor_class_offset_values_t::init(grib_accessor* a, const long l, grib_arguments* args)
{
    grib_accessor_class_double_t::init(a, l, args);
    int n = 0;
    grib_accessor_offset_values_t* self = (grib_accessor_offset_values_t*)a;
    self->values                        = grib_arguments_get_name(grib_handle_of_accessor(a), args, n++);
    self->missingValue                  = grib_arguments_get_name(grib_handle_of_accessor(a), args, n++);
    a->flags |= GRIB_ACCESSOR_FLAG_FUNCTION;
    a->length = 0;
}

int grib_accessor_class_offset_values_t::unpack_double(grib_accessor* a, double* val, size_t* len)
{
    int ret = 0;
    *val    = 0;
    *len    = 1;
    return ret;
}

int grib_accessor_class_offset_values_t::pack_double(grib_accessor* a, const double* val, size_t* len)
{
    double* values            = NULL;
    size_t size               = 0;
    double missingValue       = 0;
    long missingValuesPresent = 0;
    int ret = 0, i = 0;
    grib_accessor_offset_values_t* self = (grib_accessor_offset_values_t*)a;
    grib_context* c                     = a->context;
    grib_handle* h                      = grib_handle_of_accessor(a);

    if (*val == 0)
        return GRIB_SUCCESS;

    if ((ret = grib_get_double_internal(h, self->missingValue, &missingValue)) != GRIB_SUCCESS) {
        return ret;
    }
    if ((ret = grib_get_long_internal(h, "missingValuesPresent", &missingValuesPresent)) != GRIB_SUCCESS) {
        return ret;
    }

    if ((ret = grib_get_size(h, self->values, &size)) != GRIB_SUCCESS)
        return ret;

    values = (double*)grib_context_malloc(c, size * sizeof(double));
    if (!values)
        return GRIB_OUT_OF_MEMORY;

    if ((ret = grib_get_double_array_internal(h, self->values, values, &size)) != GRIB_SUCCESS) {
        grib_context_free(c, values);
        return ret;
    }

    for (i = 0; i < size; i++) {
        if (missingValuesPresent) {
            if (values[i] != missingValue)
                values[i] += *val;
        }
        else {
            values[i] += *val;
        }
    }

    if ((ret = grib_set_double_array_internal(h, self->values, values, size)) != GRIB_SUCCESS)
        return ret;

    grib_context_free(c, values);

    return GRIB_SUCCESS;
}
