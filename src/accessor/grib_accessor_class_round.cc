
/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_accessor_class_round.h"

grib_accessor_class_round_t _grib_accessor_class_round{"round"};
grib_accessor_class* grib_accessor_class_round = &_grib_accessor_class_round;


int grib_accessor_class_round_t::unpack_double(grib_accessor* a, double* val, size_t* len){
    grib_accessor_round_t* self = (grib_accessor_round_t*)a;

    int ret = GRIB_SUCCESS;

    size_t replen             = 0;
    double rounding_precision = 0;
    double rounded            = 0;
    double toround            = 0;

    const char* oval = NULL;
    oval             = grib_arguments_get_name(grib_handle_of_accessor(a), self->arg, 0);

    if ((ret = grib_get_double_internal(grib_handle_of_accessor(a), oval, &toround)) != 0)
        return ret;

    rounding_precision = grib_arguments_get_long(grib_handle_of_accessor(a), self->arg, 1);

    rounded = floor(rounding_precision * toround + 0.5) / rounding_precision;

    *len = replen;

    *val = rounded;

    return ret;
}

int grib_accessor_class_round_t::unpack_string(grib_accessor* a, char* val, size_t* len){
    char result[1024];
    int ret       = GRIB_SUCCESS;
    size_t replen = 1;

    double value = 0;

    ret = unpack_double(a, &value, &replen);

    snprintf(result, sizeof(result), "%.3f", value);

    replen = strlen(result) + 1;

    if (*len < replen) {
        *len = replen;
        return GRIB_ARRAY_TOO_SMALL;
    }

    *len = replen;

    snprintf(val, 1024, "%s", result);

    return ret;
}
