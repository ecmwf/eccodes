/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "G1MonthlyDate.h"

eccodes::accessor::G1MonthlyDate _grib_accessor_g1monthlydate;
eccodes::Accessor* grib_accessor_g1monthlydate = &_grib_accessor_g1monthlydate;

namespace eccodes::accessor
{

void G1MonthlyDate::init(const long l, grib_arguments* c)
{
    Long::init(l, c);
    int n = 0;

    date_ = c->get_name(get_enclosing_handle(), n++);
    flags_ |= GRIB_ACCESSOR_FLAG_READ_ONLY;
}

int G1MonthlyDate::unpack_long(long* val, size_t* len)
{
    long date = 0;

    grib_get_long_internal(get_enclosing_handle(), date_, &date);

    date /= 100;
    date *= 100;
    date += 1;

    *val = date;

    return GRIB_SUCCESS;
}

}  // namespace eccodes::accessor
