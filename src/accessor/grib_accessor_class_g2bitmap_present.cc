
/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_api_internal.h"
#include "grib_accessor_class_g2bitmap_present.h"

grib_accessor_class_g2bitmap_present_t _grib_accessor_class_g2bitmap_present{"g2bitmap_present"};
grib_accessor_class* grib_accessor_class_g2bitmap_present = &_grib_accessor_class_g2bitmap_present;


void grib_accessor_class_g2bitmap_present_t::init(grib_accessor* a, const long l, grib_arguments* c){
    grib_accessor_class_long_t::init(a, l, c);
    int n                                = 0;
    grib_accessor_g2bitmap_present_t* self = (grib_accessor_g2bitmap_present_t*)a;
    self->bitmapIndicator                = grib_arguments_get_name(grib_handle_of_accessor(a), c, n++);

    a->length = 0;
}

int grib_accessor_class_g2bitmap_present_t::unpack_long(grib_accessor* a, long* val, size_t* len){
    int ret              = GRIB_SUCCESS;
    long bitmapIndicator = 0;

    grib_accessor_g2bitmap_present_t* self = (grib_accessor_g2bitmap_present_t*)a;

    ret = grib_get_long_internal(grib_handle_of_accessor(a), self->bitmapIndicator, &bitmapIndicator);
    if (ret) {
        if (ret == GRIB_NOT_FOUND) {
            *val = 0;
            return 0;
        }
        else
            return ret;
    }

    *val = 1;
    if (bitmapIndicator == 255)
        *val = 0;

    *len = 1;
    return ret;
}

int grib_accessor_class_g2bitmap_present_t::pack_long(grib_accessor* a, const long* val, size_t* len){
    grib_accessor_g2bitmap_present_t* self = (grib_accessor_g2bitmap_present_t*)a;
    long bitmapIndicator                 = 0;

    if (*val == 0)
        bitmapIndicator = 255;

    return grib_set_long(grib_handle_of_accessor(a), self->bitmapIndicator, bitmapIndicator);
}
