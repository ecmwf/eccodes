/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "BufrGroup.h"

eccodes::accessor::BufrGroup _grib_accessor_bufr_group;
eccodes::Accessor* grib_accessor_bufr_group = &_grib_accessor_bufr_group;

namespace eccodes::accessor
{

void BufrGroup::dump(eccodes::Dumper* dumper)
{
    dumper->dump_section(this, sub_section_->block);
}

grib_accessor* BufrGroup::next(grib_accessor* a, int explore)
{
    grib_accessor* next = NULL;
    if (explore) {
        next = a->sub_section_->block->first;
        if (!next)
            next = a->next_;
    }
    else {
        next = a->next_;
    }
    if (!next) {
        if (a->parent_->owner)
            next = a->parent_->owner->next(a->parent_->owner, 0);
    }
    return next;
}

}  // namespace eccodes::accessor
