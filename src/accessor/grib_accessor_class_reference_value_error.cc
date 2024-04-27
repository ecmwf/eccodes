
/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_accessor_class_reference_value_error.h"

grib_accessor_class_reference_value_error_t _grib_accessor_class_reference_value_error{"reference_value_error"};
grib_accessor_class* grib_accessor_class_reference_value_error = &_grib_accessor_class_reference_value_error;


void grib_accessor_class_reference_value_error_t::init(grib_accessor* a, const long l, grib_arguments* c){
    grib_accessor_class_double_t::init(a, l, c);
    grib_accessor_reference_value_error_t* self = (grib_accessor_reference_value_error_t*)a;
    int n                                     = 0;

    self->referenceValue = grib_arguments_get_name(grib_handle_of_accessor(a), c, n++);
    self->floatType      = grib_arguments_get_name(grib_handle_of_accessor(a), c, n++);

    a->flags |= GRIB_ACCESSOR_FLAG_READ_ONLY;
    a->length = 0;
}

int grib_accessor_class_reference_value_error_t::unpack_double(grib_accessor* a, double* val, size_t* len){
    grib_accessor_reference_value_error_t* self = (grib_accessor_reference_value_error_t*)a;
    int ret                                   = 0;
    double referenceValue                     = 0;

    if ((ret = grib_get_double_internal(grib_handle_of_accessor(a),
                                        self->referenceValue, &referenceValue)) != GRIB_SUCCESS)
        return ret;

    if (!strcmp(self->floatType, "ibm"))
        *val = grib_ibmfloat_error(referenceValue);
    else if (!strcmp(self->floatType, "ieee"))
        *val = grib_ieeefloat_error(referenceValue);
    else
        Assert(1 == 0);

    if (ret == GRIB_SUCCESS)
        *len = 1;

    return ret;
}
