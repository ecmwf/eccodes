
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
#include "grib_accessor_class_data_dummy_field.h"

grib_accessor_class_data_dummy_field_t _grib_accessor_class_data_dummy_field{"data_dummy_field"};
grib_accessor_class* grib_accessor_class_data_dummy_field = &_grib_accessor_class_data_dummy_field;


void grib_accessor_class_data_dummy_field_t::init(grib_accessor* a, const long v, grib_arguments* args){
    grib_accessor_data_dummy_field_t* self = (grib_accessor_data_dummy_field_t*)a;
    self->missing_value  = grib_arguments_get_name(grib_handle_of_accessor(a), args, self->carg++);
    self->numberOfPoints = grib_arguments_get_name(grib_handle_of_accessor(a), args, self->carg++);
    self->bitmap         = grib_arguments_get_name(grib_handle_of_accessor(a), args, self->carg++);
}

int grib_accessor_class_data_dummy_field_t::unpack_double(grib_accessor* a, double* val, size_t* len){
    grib_accessor_data_dummy_field_t* self = (grib_accessor_data_dummy_field_t*)a;
    size_t i = 0, n_vals = 0;
    long numberOfPoints;
    double missing_value = 0;
    int err = 0;

    if ((err = grib_get_long_internal(grib_handle_of_accessor(a), self->numberOfPoints, &numberOfPoints)) != GRIB_SUCCESS)
        return err;
    n_vals = numberOfPoints;

    if ((err = grib_get_double_internal(grib_handle_of_accessor(a), self->missing_value, &missing_value)) != GRIB_SUCCESS)
        return err;

    if (*len < n_vals) {
        *len = n_vals;
        return GRIB_ARRAY_TOO_SMALL;
    }

    for (i = 0; i < n_vals; i++)
        val[i] = missing_value;

    if (grib_find_accessor(grib_handle_of_accessor(a), self->bitmap)) {
        if ((err = grib_set_double_array_internal(grib_handle_of_accessor(a), self->bitmap, val, n_vals)) != GRIB_SUCCESS)
            return err;
    }

    *len = (long)n_vals;
    return err;
}

int grib_accessor_class_data_dummy_field_t::pack_double(grib_accessor* a, const double* val, size_t* len){
    grib_accessor_data_dummy_field_t* self = (grib_accessor_data_dummy_field_t*)a;

    size_t n_vals = *len;
    int err       = 0;
    long bits_per_value = 0;
    long half_byte = 0;
    size_t buflen  = 0;
    unsigned char* buf = NULL;

    if (*len == 0)
        return GRIB_NO_VALUES;

    if ((err = grib_get_long_internal(grib_handle_of_accessor(a), self->bits_per_value, &bits_per_value)) != GRIB_SUCCESS)
        return err;

    buflen = (1 + ((bits_per_value * n_vals) / 8)) * sizeof(unsigned char);

    buf = (unsigned char*)grib_context_malloc_clear(a->context, buflen);
    if (!buf)
        return GRIB_OUT_OF_MEMORY;

    half_byte = (buflen * 8) - ((*len) * bits_per_value);

    if ((err = grib_set_long_internal(grib_handle_of_accessor(a), self->half_byte, half_byte)) != GRIB_SUCCESS) {
        grib_context_free(a->context, buf);
        return err;
    }
    grib_buffer_replace(a, buf, buflen, 1, 1);

    grib_context_free(a->context, buf);

    return GRIB_SUCCESS;
}

int grib_accessor_class_data_dummy_field_t::value_count(grib_accessor* a, long* numberOfPoints){
    grib_accessor_data_dummy_field_t* self = (grib_accessor_data_dummy_field_t*)a;
    int err = 0;
    *numberOfPoints = 0;

    if ((err = grib_get_long_internal(grib_handle_of_accessor(a), self->numberOfPoints, numberOfPoints)) != GRIB_SUCCESS) {
        grib_context_log(a->context, GRIB_LOG_ERROR, "Unable to get count of %s (%s)", a->name, grib_get_error_message(err));
    }

    return err;
}
