
/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_accessor_class_uint16.h"

grib_accessor_class_uint16_t _grib_accessor_class_uint16{"uint16"};
grib_accessor_class* grib_accessor_class_uint16 = &_grib_accessor_class_uint16;


int grib_accessor_class_uint16_t::unpack_long(grib_accessor* a, long* val, size_t* len){
    long value = 0;
    /*long pos = a->offset;*/
    /*unsigned char* data = grib_handle_of_accessor(a)->buffer->data;*/

    if (*len < 1) {
        return GRIB_ARRAY_TOO_SMALL;
    }

    *val = value;
    *len = 1;
    return GRIB_NOT_IMPLEMENTED;
}

int grib_accessor_class_uint16_t::get_native_type(grib_accessor* a){
    return GRIB_TYPE_LONG;
}
