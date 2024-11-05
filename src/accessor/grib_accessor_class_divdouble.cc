/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_accessor_class_divdouble.h"

grib_accessor_divdouble_t _grib_accessor_divdouble{};
grib_accessor* grib_accessor_divdouble = &_grib_accessor_divdouble;

void grib_accessor_divdouble_t::init(const long l, grib_arguments* c)
{
    grib_accessor_double_t::init(l, c);
    int n = 0;

    val_     = grib_arguments_get_name(grib_handle_of_accessor(this), c, n++);
    divisor_ = grib_arguments_get_double(grib_handle_of_accessor(this), c, n++);
}

int grib_accessor_divdouble_t::unpack_double(double* val, size_t* len)
{
    int ret      = GRIB_SUCCESS;
    double value = 0;

    ret = grib_get_double_internal(grib_handle_of_accessor(this), val_, &value);
    if (ret != GRIB_SUCCESS)
        return ret;

    if (divisor_ == 0) {
        return GRIB_INVALID_ARGUMENT;
    }
    *val = value / divisor_;

    *len = 1;
    return GRIB_SUCCESS;
}
