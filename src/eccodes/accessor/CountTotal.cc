/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "CountTotal.h"

eccodes::accessor::CountTotal _grib_accessor_count_total;
eccodes::Accessor* grib_accessor_count_total = &_grib_accessor_count_total;

namespace eccodes::accessor
{

void CountTotal::init(const long l, grib_arguments* c)
{
    Long::init(l, c);
    flags_ |= GRIB_ACCESSOR_FLAG_READ_ONLY;
    length_ = 0;
}

int CountTotal::unpack_long(long* val, size_t* len)
{
    *val = grib_context_get_handle_total_count(context_);
    *len = 1;
    return 0;
}

}  // namespace eccodes::accessor
