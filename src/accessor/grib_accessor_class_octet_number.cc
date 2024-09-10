/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_accessor_class_octet_number.h"

AccessorBuilder<grib_accessor_octet_number_t> _grib_accessor_octet_number_builder{};

void grib_accessor_octet_number_t::init(const long l, grib_arguments* c)
{
    grib_accessor_long_t::init(l, c);

    int n  = 0;
    left_  = grib_arguments_get_name(grib_handle_of_accessor(this), c, n++);
    right_ = grib_arguments_get_long(grib_handle_of_accessor(this), c, n++);

    length_ = 0;
}

int grib_accessor_octet_number_t::unpack_long(long* val, size_t* len)
{
    int ret = GRIB_SUCCESS;
    long offset;

    offset = offset_ + right_;

    if ((ret = grib_set_long_internal(grib_handle_of_accessor(this), left_, offset)) != GRIB_SUCCESS)
        return ret;

    *val = offset;
    *len = 1;

    return ret;
}

int grib_accessor_octet_number_t::pack_long(const long* val, size_t* len)
{
    return GRIB_SUCCESS;
}
