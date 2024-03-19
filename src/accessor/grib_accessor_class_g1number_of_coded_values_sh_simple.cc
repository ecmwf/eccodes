
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
#include "grib_accessor_class_g1number_of_coded_values_sh_simple.h"

grib_accessor_class_g1number_of_coded_values_sh_simple_t _grib_accessor_class_g1number_of_coded_values_sh_simple{"g1number_of_coded_values_sh_simple"};
grib_accessor_class* grib_accessor_class_g1number_of_coded_values_sh_simple = &_grib_accessor_class_g1number_of_coded_values_sh_simple;


void grib_accessor_class_g1number_of_coded_values_sh_simple_t::init(grib_accessor* a, const long l, grib_arguments* c){
    grib_accessor_class_long_t::init(a, l, c);
    int n                                                  = 0;
    grib_accessor_g1number_of_coded_values_sh_simple_t* self = (grib_accessor_g1number_of_coded_values_sh_simple_t*)a;
    self->bitsPerValue                                     = grib_arguments_get_name(grib_handle_of_accessor(a), c, n++);
    self->offsetBeforeData                                 = grib_arguments_get_name(grib_handle_of_accessor(a), c, n++);
    self->offsetAfterData                                  = grib_arguments_get_name(grib_handle_of_accessor(a), c, n++);
    self->unusedBits                                       = grib_arguments_get_name(grib_handle_of_accessor(a), c, n++);
    self->numberOfValues                                   = grib_arguments_get_name(grib_handle_of_accessor(a), c, n++);
    a->flags |= GRIB_ACCESSOR_FLAG_READ_ONLY;
    a->flags |= GRIB_ACCESSOR_FLAG_FUNCTION;
    a->length = 0;
}

int grib_accessor_class_g1number_of_coded_values_sh_simple_t::unpack_long(grib_accessor* a, long* val, size_t* len){
    int ret               = GRIB_SUCCESS;
    long bpv              = 0;
    long offsetBeforeData = 0, offsetAfterData = 0, unusedBits = 0, numberOfValues;

    grib_accessor_g1number_of_coded_values_sh_simple_t* self = (grib_accessor_g1number_of_coded_values_sh_simple_t*)a;

    if ((ret = grib_get_long_internal(grib_handle_of_accessor(a), self->bitsPerValue, &bpv)) != GRIB_SUCCESS)
        return ret;

    if ((ret = grib_get_long_internal(grib_handle_of_accessor(a), self->offsetBeforeData, &offsetBeforeData)) != GRIB_SUCCESS)
        return ret;

    if ((ret = grib_get_long_internal(grib_handle_of_accessor(a), self->offsetAfterData, &offsetAfterData)) != GRIB_SUCCESS)
        return ret;

    if ((ret = grib_get_long_internal(grib_handle_of_accessor(a), self->unusedBits, &unusedBits)) != GRIB_SUCCESS)
        return ret;

    if (bpv != 0) {
        DEBUG_ASSERT(offsetAfterData > offsetBeforeData);
        *val = ((offsetAfterData - offsetBeforeData) * 8 - unusedBits) / bpv;
    }
    else {
        if ((ret = grib_get_long_internal(grib_handle_of_accessor(a), self->numberOfValues, &numberOfValues)) != GRIB_SUCCESS)
            return ret;

        *val = numberOfValues;
    }
    return ret;
}
