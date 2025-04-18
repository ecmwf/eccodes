/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "MultDouble.h"

eccodes::accessor::MultDouble _grib_accessor_multdouble;
eccodes::Accessor* grib_accessor_multdouble = &_grib_accessor_multdouble;

namespace eccodes::accessor
{

void MultDouble::init(const long l, grib_arguments* c)
{
    Double::init(l, c);
    int n = 0;

    val_        = c->get_name(get_enclosing_handle(), n++);
    multiplier_ = c->get_double(get_enclosing_handle(), n++);
}

int MultDouble::unpack_double(double* val, size_t* len)
{
    int ret      = GRIB_SUCCESS;
    double value = 0;

    ret = grib_get_double_internal(get_enclosing_handle(), val_, &value);
    if (ret != GRIB_SUCCESS)
        return ret;

    *val = value * multiplier_;

    *len = 1;
    return GRIB_SUCCESS;
}

}  // namespace eccodes::accessor
