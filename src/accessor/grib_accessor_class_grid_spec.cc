/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_accessor_class_grid_spec.h"

grib_accessor_grid_spec_t _grib_accessor_grid_spec{};
grib_accessor* grib_accessor_grid_spec = &_grib_accessor_grid_spec;

void grib_accessor_grid_spec_t::init(const long len, grib_arguments* arg)
{
    grib_accessor_gen_t::init(len, arg);
    // grib_handle* h = grib_handle_of_accessor(this);
    // grid_type_ = arg->get_name(h, 0);
    // endpoint_  = arg->get_long(h, 1);
    length_ = 0;

    // For now read only but later we can make it settable
    // and provide a pack_string method
    flags_ |= GRIB_ACCESSOR_FLAG_READ_ONLY;
    flags_ |= GRIB_ACCESSOR_FLAG_EDITION_SPECIFIC;
}

long grib_accessor_grid_spec_t::get_native_type()
{
    return GRIB_TYPE_STRING;
}

int grib_accessor_grid_spec_t::unpack_string(char* v, size_t* len)
{
    // TODO(mapm): Please fill in
    grib_handle* h = grib_handle_of_accessor(this);

    char gridName[128] = {0,};
    size_t size = sizeof(gridName);
    int err = grib_get_string(h, "gridName", gridName, &size);
    if (!err) {
        const size_t dsize = string_length() - 1; // max size for destination string "v"
        snprintf(v, dsize, "{\"grid\": \"%s\"}", gridName);
        *len = strlen(v) + 1;
        return GRIB_SUCCESS;
    }

    return GRIB_NOT_IMPLEMENTED;
}
