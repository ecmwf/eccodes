/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "DataShSimplePacking.h"

namespace eccodes::accessor
{

void DataShSimplePacking::init(const long v, grib_arguments* args)
{
    Gen::init(v, args);

    coded_values_ = args->get_name(0);
    real_part_ = args->get_name(1);
    flags_ |= GRIB_ACCESSOR_FLAG_DATA;

    length_ = 0;
}

void DataShSimplePacking::dump(eccodes::Dumper* dumper)
{
    dumper->dump_values(this);
}

template <typename T>
int DataShSimplePacking::pack_values(const T* val, size_t* len)
{
    int err = GRIB_SUCCESS;

    if (*len == 0)
        return GRIB_NO_VALUES;

    size_t coded_n_vals = *len - 1;
    size_t n_vals = *len;

    dirty_ = 1;

    // The real part of the (0,0) coefficient is stored separately as a float
    if ((err = grib_set_double_internal(get_enclosing_handle(), real_part_, (double)*val)) != GRIB_SUCCESS)
        return err;

    val++;

    if ((err = grib_set_array_internal(get_enclosing_handle(), coded_values_, val, coded_n_vals)) != GRIB_SUCCESS)
        return err;

    *len = n_vals;

    return err;
}

template <typename T>
int DataShSimplePacking::unpack_values(T* val, size_t* len)
{
    int err = GRIB_SUCCESS;
    size_t coded_n_vals = 0;
    double real_part = 0;
    grib_handle* h = get_enclosing_handle();

    if ((err = grib_get_size(h, coded_values_, &coded_n_vals)) != GRIB_SUCCESS)
        return err;

    const size_t n_vals = coded_n_vals + 1;
    if (*len < n_vals) {
        *len = n_vals;
        return GRIB_ARRAY_TOO_SMALL;
    }

    dirty_ = 0;

    if ((err = grib_get_double_internal(h, real_part_, &real_part)) != GRIB_SUCCESS)
        return err;
    val[0] = (T)real_part;

    if ((err = grib_get_array_internal(h, coded_values_, val + 1, &coded_n_vals)) != GRIB_SUCCESS)
        return err;

    *len = coded_n_vals + 1;
    return err;
}

int DataShSimplePacking::pack_double(const double* val, size_t* len)
{
    return pack_values(val, len);
}

int DataShSimplePacking::pack_float(const float* val, size_t* len)
{
    return pack_values(val, len);
}

int DataShSimplePacking::unpack_double(double* val, size_t* len)
{
    return unpack_values(val, len);
}

int DataShSimplePacking::unpack_float(float* val, size_t* len)
{
    return unpack_values(val, len);
}

long DataShSimplePacking::get_native_type()
{
    return GRIB_TYPE_DOUBLE;
}

}  // namespace eccodes::accessor
