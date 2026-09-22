/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "HeadersOnly.h"

eccodes::AccessorBuilder<eccodes::accessor::HeadersOnly> _grib_accessor_headers_only_builder{};

namespace eccodes::accessor
{

void HeadersOnly::init(const long l, grib_arguments* c)
{
    Gen::init(l, c);
    flags_ |= GRIB_ACCESSOR_FLAG_READ_ONLY;
    flags_ |= GRIB_ACCESSOR_FLAG_HIDDEN;
    length_ = 0;
}

int HeadersOnly::unpack_long(long* val, size_t* len)
{
    *val = this->get_enclosing_handle()->partial;
    *len = 1;
    return 0;
}

long HeadersOnly::get_native_type()
{
    return GRIB_TYPE_LONG;
}

}  // namespace eccodes::accessor
