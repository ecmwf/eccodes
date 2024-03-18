
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
#include "grib_accessor_class_g2latlon.h"

grib_accessor_class_g2latlon_t _grib_accessor_class_g2latlon{"g2latlon"};
grib_accessor_class* grib_accessor_class_g2latlon = &_grib_accessor_class_g2latlon;


void grib_accessor_class_g2latlon_t::init(grib_accessor* a, const long l, grib_arguments* c){
    grib_accessor_g2latlon_t* self = (grib_accessor_g2latlon_t*)a;
    int n                        = 0;

    self->grid  = grib_arguments_get_name(grib_handle_of_accessor(a), c, n++);
    self->index = grib_arguments_get_long(grib_handle_of_accessor(a), c, n++);
    self->given = grib_arguments_get_name(grib_handle_of_accessor(a), c, n++);
}

int grib_accessor_class_g2latlon_t::unpack_double(grib_accessor* a, double* val, size_t* len){
    grib_accessor_g2latlon_t* self = (grib_accessor_g2latlon_t*)a;
    int ret                      = 0;

    long given = 1;
    double grid[6];
    size_t size = 6;

    if (*len < 1) {
        ret = GRIB_ARRAY_TOO_SMALL;
        return ret;
    }

    if (self->given)
        if ((ret = grib_get_long_internal(grib_handle_of_accessor(a), self->given, &given)) != GRIB_SUCCESS)
            return ret;

    if (!given) {
        *val = GRIB_MISSING_DOUBLE;
        return GRIB_SUCCESS;
    }

    if ((ret = grib_get_double_array_internal(grib_handle_of_accessor(a), self->grid, grid, &size)) != GRIB_SUCCESS)
        return ret;

    *val = grid[self->index];

    return GRIB_SUCCESS;
}

int grib_accessor_class_g2latlon_t::pack_double(grib_accessor* a, const double* val, size_t* len){
    grib_accessor_g2latlon_t* self = (grib_accessor_g2latlon_t*)a;
    int ret                      = 0;
    double grid[6];
    size_t size    = 6;
    double new_val = *val;
    grib_handle* hand = grib_handle_of_accessor(a);

    if (*len < 1) {
        ret = GRIB_ARRAY_TOO_SMALL;
        return ret;
    }

    if (self->given) {
        long given = *val != GRIB_MISSING_DOUBLE;
        if ((ret = grib_set_long_internal(hand, self->given, given)) != GRIB_SUCCESS)
            return ret;
    }

    if ((ret = grib_get_double_array_internal(hand, self->grid, grid, &size)) != GRIB_SUCCESS)
        return ret;

    /* index 1 is longitudeOfFirstGridPointInDegrees
     * index 3 is longitudeOfLastGridPointInDegrees
     */
    if ((self->index == 1 || self->index == 3)) {
        /* WMO regulation for GRIB edition 2:
         * The longitude values shall be limited to the range 0 to 360 degrees inclusive */
        new_val = normalise_longitude_in_degrees(*val);
        if (hand->context->debug && new_val != *val) {
            fprintf(stderr, "ECCODES DEBUG pack_double g2latlon: normalise longitude %g -> %g\n", *val, new_val);
        }
    }
    grid[self->index] = new_val;

    return grib_set_double_array_internal(hand, self->grid, grid, size);
}

int grib_accessor_class_g2latlon_t::pack_missing(grib_accessor* a){
    grib_accessor_g2latlon_t* self = (grib_accessor_g2latlon_t*)a;
    double missing               = GRIB_MISSING_DOUBLE;
    size_t size                  = 1;

    if (!self->given)
        return GRIB_NOT_IMPLEMENTED;

    return pack_double(a, &missing, &size);
}

int grib_accessor_class_g2latlon_t::is_missing(grib_accessor* a){
    grib_accessor_g2latlon_t* self = (grib_accessor_g2latlon_t*)a;
    long given                   = 1;

    if (self->given)
        grib_get_long_internal(grib_handle_of_accessor(a), self->given, &given);

    return !given;
}
