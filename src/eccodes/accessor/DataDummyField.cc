/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "DataDummyField.h"

eccodes::accessor::DataDummyField _grib_accessor_data_dummy_field;
eccodes::Accessor* grib_accessor_data_dummy_field = &_grib_accessor_data_dummy_field;

namespace eccodes::accessor
{

void DataDummyField::init(const long v, grib_arguments* args)
{
    DataG1SimplePacking::init(v, args);
    missing_value_  = args->get_name(get_enclosing_handle(), carg_++);
    numberOfPoints_ = args->get_name(get_enclosing_handle(), carg_++);
    bitmap_         = args->get_name(get_enclosing_handle(), carg_++);
}

int DataDummyField::unpack_double(double* val, size_t* len)
{
    size_t i = 0, n_vals = 0;
    long numberOfPoints;
    double missing_value = 0;
    int err              = 0;

    if ((err = grib_get_long_internal(get_enclosing_handle(), numberOfPoints_, &numberOfPoints)) != GRIB_SUCCESS)
        return err;
    n_vals = numberOfPoints;

    if ((err = grib_get_double_internal(get_enclosing_handle(), missing_value_, &missing_value)) != GRIB_SUCCESS)
        return err;

    if (*len < n_vals) {
        *len = n_vals;
        return GRIB_ARRAY_TOO_SMALL;
    }

    for (i = 0; i < n_vals; i++)
        val[i] = missing_value;

    if (grib_find_accessor(get_enclosing_handle(), bitmap_)) {
        if ((err = grib_set_double_array_internal(get_enclosing_handle(), bitmap_, val, n_vals)) != GRIB_SUCCESS)
            return err;
    }

    *len = (long)n_vals;
    return err;
}

int DataDummyField::pack_double(const double* val, size_t* len)
{
    size_t n_vals       = *len;
    int err             = 0;
    long bits_per_value = 0;
    long half_byte      = 0;
    size_t buflen       = 0;
    unsigned char* buf  = NULL;

    if (*len == 0)
        return GRIB_NO_VALUES;

    if ((err = grib_get_long_internal(get_enclosing_handle(), bits_per_value_, &bits_per_value)) != GRIB_SUCCESS)
        return err;

    buflen = (1 + ((bits_per_value * n_vals) / 8)) * sizeof(unsigned char);

    buf = (unsigned char*)grib_context_malloc_clear(context_, buflen);
    if (!buf)
        return GRIB_OUT_OF_MEMORY;

    half_byte = (buflen * 8) - ((*len) * bits_per_value);

    if ((err = grib_set_long_internal(get_enclosing_handle(), half_byte_, half_byte)) != GRIB_SUCCESS) {
        grib_context_free(context_, buf);
        return err;
    }
    grib_buffer_replace(this, buf, buflen, 1, 1);

    grib_context_free(context_, buf);

    return GRIB_SUCCESS;
}

int DataDummyField::value_count(long* numberOfPoints)
{
    int err         = 0;
    *numberOfPoints = 0;

    if ((err = grib_get_long_internal(get_enclosing_handle(), numberOfPoints_, numberOfPoints)) != GRIB_SUCCESS) {
        grib_context_log(context_, GRIB_LOG_ERROR, "Unable to get count of %s (%s)", name_, grib_get_error_message(err));
    }

    return err;
}

}  // namespace eccodes::accessor
