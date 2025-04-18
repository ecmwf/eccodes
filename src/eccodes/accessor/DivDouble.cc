/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "DivDouble.h"

eccodes::accessor::DivDouble _grib_accessor_divdouble;
eccodes::Accessor* grib_accessor_divdouble = &_grib_accessor_divdouble;

namespace eccodes::accessor
{

void DivDouble::init(const long l, grib_arguments* c)
{
    Double::init(l, c);
    int n = 0;

    val_     = c->get_name(get_enclosing_handle(), n++);
    divisor_ = c->get_double(get_enclosing_handle(), n++);
}

int DivDouble::unpack_double(double* val, size_t* len)
{
    int ret      = GRIB_SUCCESS;
    double value = 0;

    ret = grib_get_double_internal(get_enclosing_handle(), val_, &value);
    if (ret != GRIB_SUCCESS)
        return ret;

    if (divisor_ == 0) {
        return GRIB_INVALID_ARGUMENT;
    }
    *val = value / divisor_;

    *len = 1;
    return GRIB_SUCCESS;
}

}  // namespace eccodes::accessor
