/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "DataG2ShSimplePacking.h"

namespace eccodes::accessor
{

void DataG2ShSimplePacking::init(const long v, grib_arguments* args)
{
    DataShSimplePacking::init(v, args);

    numberOfValues_ = args->get_name(2);
    numberOfDataPoints_ = args->get_name(3);
    flags_ |= GRIB_ACCESSOR_FLAG_DATA;
}

int DataG2ShSimplePacking::value_count(long* len)
{
    *len = 0;
    return grib_get_long(get_enclosing_handle(), numberOfValues_, len);
}

// After packing: numberOfValues and numberOfDataPoints include the real part
int DataG2ShSimplePacking::set_number_of_values(int err, size_t n)
{
    if (err == GRIB_SUCCESS)
        err = grib_set_long_internal(get_enclosing_handle(), numberOfValues_, (long)n);
    if (err == GRIB_SUCCESS)
        err = grib_set_long_internal(get_enclosing_handle(), numberOfDataPoints_, (long)n);
    return err;
}

int DataG2ShSimplePacking::pack_double(const double* val, size_t* len)
{
    return set_number_of_values(DataShSimplePacking::pack_double(val, len), *len);
}

int DataG2ShSimplePacking::pack_float(const float* val, size_t* len)
{
    return set_number_of_values(DataShSimplePacking::pack_float(val, len), *len);
}

}  // namespace eccodes::accessor
