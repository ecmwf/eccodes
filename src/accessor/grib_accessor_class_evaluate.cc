
/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_accessor_class_evaluate.h"

grib_accessor_class_evaluate_t _grib_accessor_class_evaluate{"evaluate"};
grib_accessor_class* grib_accessor_class_evaluate = &_grib_accessor_class_evaluate;


void grib_accessor_class_evaluate_t::init(grib_accessor* a, const long l, grib_arguments* c){
    grib_accessor_class_long_t::init(a, l, c);
    grib_accessor_evaluate_t* self = (grib_accessor_evaluate_t*)a;
    self->arg                    = c;
    a->flags |= GRIB_ACCESSOR_FLAG_READ_ONLY;
}

int grib_accessor_class_evaluate_t::unpack_long(grib_accessor* a, long* val, size_t* len){
    grib_accessor_evaluate_t* self = (grib_accessor_evaluate_t*)a;
    grib_expression* e = grib_arguments_get_expression(grib_handle_of_accessor(a), self->arg, 0);

    int ret  = grib_expression_evaluate_long(grib_handle_of_accessor(a), e, val);
    *len = 1;

    return ret;
}
