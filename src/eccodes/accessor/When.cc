/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "When.h"

eccodes::accessor::When _grib_accessor_when;
eccodes::Accessor* grib_accessor_when = &_grib_accessor_when;

namespace eccodes::accessor
{

void When::init(const long len, grib_arguments* arg)
{
    Gen::init(len, arg);
    length_ = 0;
    flags_ |= GRIB_ACCESSOR_FLAG_HIDDEN;
    flags_ |= GRIB_ACCESSOR_FLAG_READ_ONLY;
}

void When::dump(eccodes::Dumper* dumper)
{
    /* dumper->dump_when(a,NULL); */
}

int When::notify_change(grib_accessor* changed)
{
    return creator_->notify_change(this, changed);
}

long When::get_native_type()
{
    return GRIB_TYPE_UNDEFINED;
}

}  // namespace eccodes::accessor
