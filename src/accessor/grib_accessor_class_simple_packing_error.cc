
/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_accessor_class_simple_packing_error.h"
#include "grib_scaling.h"

grib_accessor_class_simple_packing_error_t _grib_accessor_class_simple_packing_error{"simple_packing_error"};
grib_accessor_class* grib_accessor_class_simple_packing_error = &_grib_accessor_class_simple_packing_error;


void grib_accessor_class_simple_packing_error_t::init(grib_accessor* a, const long l, grib_arguments* c){
    grib_accessor_class_double_t::init(a, l, c);
    grib_accessor_simple_packing_error_t* self = (grib_accessor_simple_packing_error_t*)a;
    int n = 0;
    grib_handle* h = grib_handle_of_accessor(a);

    self->bitsPerValue       = grib_arguments_get_name(h, c, n++);
    self->binaryScaleFactor  = grib_arguments_get_name(h, c, n++);
    self->decimalScaleFactor = grib_arguments_get_name(h, c, n++);
    self->referenceValue     = grib_arguments_get_name(h, c, n++);
    self->floatType          = grib_arguments_get_name(h, c, n++);

    a->flags |= GRIB_ACCESSOR_FLAG_READ_ONLY;
    a->length = 0;
}

int grib_accessor_class_simple_packing_error_t::unpack_double(grib_accessor* a, double* val, size_t* len){
    grib_accessor_simple_packing_error_t* self = (grib_accessor_simple_packing_error_t*)a;

    int ret                 = 0;
    long binaryScaleFactor  = 0;
    long bitsPerValue       = 0;
    long decimalScaleFactor = 0;
    double referenceValue   = 0;
    grib_handle* h = grib_handle_of_accessor(a);

    if ((ret = grib_get_long_internal(h, self->binaryScaleFactor, &binaryScaleFactor)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_long_internal(h, self->bitsPerValue, &bitsPerValue)) != GRIB_SUCCESS)
        return ret;

    if ((ret = grib_get_long_internal(h, self->decimalScaleFactor, &decimalScaleFactor)) != GRIB_SUCCESS)
        return ret;

    if ((ret = grib_get_double_internal(h, self->referenceValue, &referenceValue)) != GRIB_SUCCESS)
        return ret;

    if (!strcmp(self->floatType, "ibm"))
        *val = grib_ibmfloat_error(referenceValue);
    else if (!strcmp(self->floatType, "ieee"))
        *val = grib_ieeefloat_error(referenceValue);
    else
        Assert(1 == 0);

    if (bitsPerValue != 0)
        *val = (*val + codes_power<double>(binaryScaleFactor, 2)) * codes_power<double>(-decimalScaleFactor, 10) * 0.5;

    if (ret == GRIB_SUCCESS)
        *len = 1;

    return ret;
}
