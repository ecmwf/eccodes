
/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_accessor_class_multdouble.h"

grib_accessor_class_multdouble_t _grib_accessor_class_multdouble{"multdouble"};
grib_accessor_class* grib_accessor_class_multdouble = &_grib_accessor_class_multdouble;


void grib_accessor_class_multdouble_t::init(grib_accessor* a, const long l, grib_arguments* c){
    grib_accessor_class_double_t::init(a, l, c);
    grib_accessor_multdouble_t* self = (grib_accessor_multdouble_t*)a;
    int n = 0;

    self->val = grib_arguments_get_name(grib_handle_of_accessor(a), c, n++);
    self->multiplier = grib_arguments_get_double(grib_handle_of_accessor(a), c, n++);
}

int grib_accessor_class_multdouble_t::unpack_double(grib_accessor* a, double* val, size_t* len){
    const grib_accessor_multdouble_t* self = (grib_accessor_multdouble_t*)a;
    int ret = GRIB_SUCCESS;
    double value = 0;

    ret = grib_get_double_internal(grib_handle_of_accessor(a), self->val, &value);
    if (ret != GRIB_SUCCESS)
        return ret;

    *val = value * self->multiplier;

    *len = 1;
    return GRIB_SUCCESS;
}
