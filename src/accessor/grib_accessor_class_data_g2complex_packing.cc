
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
#include "grib_accessor_class_data_g2complex_packing.h"

grib_accessor_class_data_g2complex_packing_t _grib_accessor_class_data_g2complex_packing{"data_g2complex_packing"};
grib_accessor_class* grib_accessor_class_data_g2complex_packing = &_grib_accessor_class_data_g2complex_packing;


void grib_accessor_class_data_g2complex_packing_t::init(grib_accessor* a, const long v, grib_arguments* args){
    grib_accessor_data_g2complex_packing_t* self = (grib_accessor_data_g2complex_packing_t*)a;
    self->numberOfValues                       = grib_arguments_get_name(grib_handle_of_accessor(a), args, self->carg++);
    self->edition                              = 2;

    a->flags |= GRIB_ACCESSOR_FLAG_DATA;
}

int grib_accessor_class_data_g2complex_packing_t::value_count(grib_accessor* a, long* numberOfValues){
    grib_accessor_data_g2complex_packing_t* self = (grib_accessor_data_g2complex_packing_t*)a;
    *numberOfValues                            = 0;

    return grib_get_long(grib_handle_of_accessor(a), self->numberOfValues, numberOfValues);
}

int grib_accessor_class_data_g2complex_packing_t::pack_double(grib_accessor* a, const double* val, size_t* len){
    grib_accessor_data_g2complex_packing_t* self = (grib_accessor_data_g2complex_packing_t*)a;
    int ret                                    = GRIB_SUCCESS;

    if (*len == 0)
        return GRIB_NO_VALUES;

    ret = grib_accessor_class_data_complex_packing_t::pack_double(a, val, len);

    if (ret == GRIB_SUCCESS)
        ret = grib_set_long_internal(grib_handle_of_accessor(a), self->numberOfValues, *len);

    return ret;
}
