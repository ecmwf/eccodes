
/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_accessor_class_position.h"

grib_accessor_class_position_t _grib_accessor_class_position{ "position" };
grib_accessor_class* grib_accessor_class_position = &_grib_accessor_class_position;


void grib_accessor_class_position_t::init(grib_accessor* a, const long len, grib_arguments* arg)
{
    grib_accessor_class_gen_t::init(a, len, arg);
    a->length = 0;
    a->flags |= GRIB_ACCESSOR_FLAG_READ_ONLY;
    a->flags |= GRIB_ACCESSOR_FLAG_HIDDEN;
    a->flags |= GRIB_ACCESSOR_FLAG_EDITION_SPECIFIC;
}

int grib_accessor_class_position_t::get_native_type(grib_accessor* a)
{
    return GRIB_TYPE_LONG;
}

void grib_accessor_class_position_t::dump(grib_accessor* a, grib_dumper* dumper)
{
    grib_dump_long(dumper, a, NULL);
}

int grib_accessor_class_position_t::unpack_long(grib_accessor* a, long* val, size_t* len)
{
    if (*len < 1) {
        grib_context_log(a->context, GRIB_LOG_ERROR, "Wrong size for %s, it contains %d values ", a->name, 1);
        *len = 0;
        return GRIB_ARRAY_TOO_SMALL;
    }
    *val = a->offset;
    *len = 1;
    return GRIB_SUCCESS;
}

// static int compare(grib_accessor* a, grib_accessor* b)
// {
//     if (a->offset != b->offset)
//         return GRIB_OFFSET_MISMATCH;
//     return GRIB_SUCCESS;
// }
