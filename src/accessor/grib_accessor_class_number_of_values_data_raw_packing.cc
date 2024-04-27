
/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_accessor_class_number_of_values_data_raw_packing.h"

grib_accessor_class_number_of_values_data_raw_packing_t _grib_accessor_class_number_of_values_data_raw_packing{"number_of_values_data_raw_packing"};
grib_accessor_class* grib_accessor_class_number_of_values_data_raw_packing = &_grib_accessor_class_number_of_values_data_raw_packing;


void grib_accessor_class_number_of_values_data_raw_packing_t::init(grib_accessor* a, const long v, grib_arguments* args){
    grib_accessor_class_gen_t::init(a, v, args);
    int n                                                 = 0;
    grib_accessor_number_of_values_data_raw_packing_t* self = (grib_accessor_number_of_values_data_raw_packing_t*)a;

    self->values    = grib_arguments_get_name(grib_handle_of_accessor(a), args, n++);
    self->precision = grib_arguments_get_name(grib_handle_of_accessor(a), args, n++);
    a->flags |= GRIB_ACCESSOR_FLAG_READ_ONLY;
    a->length = 0;
}

int grib_accessor_class_number_of_values_data_raw_packing_t::unpack_long(grib_accessor* a, long* val, size_t* len){
    int err                                               = 0;
    grib_accessor_number_of_values_data_raw_packing_t* self = (grib_accessor_number_of_values_data_raw_packing_t*)a;
    grib_accessor* adata                                  = NULL;
    long precision                                        = 0;
    int bytes                                             = 0;
    long byte_count                                       = 0;

    adata = grib_find_accessor(grib_handle_of_accessor(a), self->values);
    Assert(adata != NULL);
    byte_count = adata->byte_count();
    if ((err = grib_get_long_internal(grib_handle_of_accessor(a), self->precision, &precision)) != GRIB_SUCCESS)
        return err;

    switch (precision) {
        case 1:
            bytes = 4;
            break;

        case 2:
            bytes = 8;
            break;

        default:
            return GRIB_NOT_IMPLEMENTED;
    }

    *val = byte_count / bytes;

    return err;
}

int grib_accessor_class_number_of_values_data_raw_packing_t::get_native_type(grib_accessor* a){
    return GRIB_TYPE_LONG;
}
