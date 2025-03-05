/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "Size.h"

eccodes::accessor::Size _grib_accessor_size;
eccodes::Accessor* grib_accessor_size = &_grib_accessor_size;

namespace eccodes::accessor
{

void Size::init(const long l, grib_arguments* c)
{
    Long::init(l, c);
    accessor_ = c->get_name(grib_handle_of_accessor(this), 0);
    flags_ |= GRIB_ACCESSOR_FLAG_READ_ONLY;
    flags_ |= GRIB_ACCESSOR_FLAG_FUNCTION;
    length_ = 0;
}

int Size::unpack_long(long* val, size_t* len)
{
    size_t size = 0;
    int ret     = grib_get_size(grib_handle_of_accessor(this), accessor_, &size);
    *val        = (long)size;
    *len        = 1;
    return ret;
}

}  // namespace eccodes::accessor
