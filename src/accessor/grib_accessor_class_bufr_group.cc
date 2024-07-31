/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_accessor_class_bufr_group.h"

grib_accessor_class_bufr_group_t _grib_accessor_class_bufr_group{ "bufr_group" };
grib_accessor_class* grib_accessor_class_bufr_group = &_grib_accessor_class_bufr_group;


void grib_accessor_class_bufr_group_t::dump(grib_accessor* a, grib_dumper* dumper)
{
    grib_dump_section(dumper, a, a->sub_section->block);
}

grib_accessor* grib_accessor_class_bufr_group_t::next(grib_accessor* a, int explore)
{
    grib_accessor* next = NULL;
    if (explore) {
        next = a->sub_section->block->first;
        if (!next)
            next = a->next_;
    }
    else {
        next = a->next_;
    }
    if (!next) {
        if (a->parent->owner)
            next = a->parent->owner->cclass->next(a->parent->owner, 0);
    }
    return next;
}
