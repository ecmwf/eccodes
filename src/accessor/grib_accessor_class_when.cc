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

grib_accessor_class_when_t _grib_accessor_class_when{ "when" };
grib_accessor_class* grib_accessor_class_when = &_grib_accessor_class_when;


void grib_accessor_class_when_t::init(grib_accessor* a, const long len, grib_arguments* arg)
{
    grib_accessor_class_gen_t::init(a, len, arg);
    a->length = 0;
    a->flags |= GRIB_ACCESSOR_FLAG_HIDDEN;
    a->flags |= GRIB_ACCESSOR_FLAG_READ_ONLY;
}

void grib_accessor_class_when_t::dump(grib_accessor* a, grib_dumper* dumper)
{
    /* grib_dump_when(dumper,a,NULL); */
}

int grib_accessor_class_when_t::notify_change(grib_accessor* a, grib_accessor* changed)
{
    return grib_action_notify_change(a->creator, a, changed);
}

int grib_accessor_class_when_t::get_native_type(grib_accessor* a)
{
    return GRIB_TYPE_UNDEFINED;
}
