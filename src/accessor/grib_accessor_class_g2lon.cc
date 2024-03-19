
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
#include "grib_accessor_class_g2lon.h"

grib_accessor_class_g2lon_t _grib_accessor_class_g2lon{"g2lon"};
grib_accessor_class* grib_accessor_class_g2lon = &_grib_accessor_class_g2lon;


void grib_accessor_class_g2lon_t::init(grib_accessor* a, const long l, grib_arguments* c){
    grib_accessor_class_double_t::init(a, l, c);
    grib_accessor_g2lon_t* self = (grib_accessor_g2lon_t*)a;
    int n                     = 0;

    self->longitude = grib_arguments_get_name(grib_handle_of_accessor(a), c, n++);
}

int grib_accessor_class_g2lon_t::unpack_double(grib_accessor* a, double* val, size_t* len){
    grib_accessor_g2lon_t* self = (grib_accessor_g2lon_t*)a;
    int ret                   = 0;
    long longitude;

    if ((ret = grib_get_long(grib_handle_of_accessor(a), self->longitude, &longitude)) != GRIB_SUCCESS)
        return ret;

    if (longitude == GRIB_MISSING_LONG) {
        *val = GRIB_MISSING_DOUBLE;
        return GRIB_SUCCESS;
    }

    *val = ((double)longitude) / 1000000.0;

    return GRIB_SUCCESS;
}

int grib_accessor_class_g2lon_t::pack_double(grib_accessor* a, const double* val, size_t* len){
    grib_accessor_g2lon_t* self = (grib_accessor_g2lon_t*)a;
    long longitude;
    double value = *val;

    if (value == GRIB_MISSING_DOUBLE) {
        longitude = GRIB_MISSING_LONG;
    }
    else {
        if (value < 0)
            value += 360;
        longitude = (long)(value * 1000000);
    }
    return grib_set_long(grib_handle_of_accessor(a), self->longitude, longitude);
}
