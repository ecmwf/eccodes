/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_accessor_class_when.h"

AccessorBuilder<grib_accessor_when_t> _grib_accessor_when_builder{};

void grib_accessor_when_t::init(const long len, grib_arguments* arg)
{
    grib_accessor_gen_t::init(len, arg);
    length_ = 0;
    flags_ |= GRIB_ACCESSOR_FLAG_HIDDEN;
    flags_ |= GRIB_ACCESSOR_FLAG_READ_ONLY;
}

void grib_accessor_when_t::dump(grib_dumper* dumper)
{
    /* grib_dump_when(dumper,a,NULL); */
}

int grib_accessor_when_t::notify_change(grib_accessor* changed)
{
    return grib_action_notify_change(creator_, this, changed);
}

long grib_accessor_when_t::get_native_type()
{
    return GRIB_TYPE_UNDEFINED;
}
