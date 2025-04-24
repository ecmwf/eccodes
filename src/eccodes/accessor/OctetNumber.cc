/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "OctetNumber.h"

eccodes::accessor::OctetNumber _grib_accessor_octet_number;
eccodes::Accessor* grib_accessor_octet_number = &_grib_accessor_octet_number;

namespace eccodes::accessor
{

void OctetNumber::init(const long l, grib_arguments* c)
{
    Long::init(l, c);

    int n  = 0;
    left_  = c->get_name(get_enclosing_handle(), n++);
    right_ = c->get_long(get_enclosing_handle(), n++);

    length_ = 0;
}

int OctetNumber::unpack_long(long* val, size_t* len)
{
    int ret = GRIB_SUCCESS;
    long offset;

    offset = offset_ + right_;

    if ((ret = grib_set_long_internal(get_enclosing_handle(), left_, offset)) != GRIB_SUCCESS)
        return ret;

    *val = offset;
    *len = 1;

    return ret;
}

int OctetNumber::pack_long(const long* val, size_t* len)
{
    return GRIB_SUCCESS;
}

}  // namespace eccodes::accessor
