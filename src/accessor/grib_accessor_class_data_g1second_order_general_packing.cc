
/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_accessor_class_data_g1second_order_general_packing.h"

grib_accessor_class_data_g1second_order_general_packing_t _grib_accessor_class_data_g1second_order_general_packing{"data_g1second_order_general_packing"};
grib_accessor_class* grib_accessor_class_data_g1second_order_general_packing = &_grib_accessor_class_data_g1second_order_general_packing;


void grib_accessor_class_data_g1second_order_general_packing_t::init(grib_accessor* a, const long v, grib_arguments* args){
    grib_accessor_class_data_simple_packing_t::init(a, v, args);
    grib_accessor_data_g1second_order_general_packing_t* self = (grib_accessor_data_g1second_order_general_packing_t*)a;
    grib_handle* hand                                       = grib_handle_of_accessor(a);

    self->half_byte                       = grib_arguments_get_name(hand, args, self->carg++);
    self->packingType                     = grib_arguments_get_name(hand, args, self->carg++);
    self->ieee_packing                    = grib_arguments_get_name(hand, args, self->carg++);
    self->precision                       = grib_arguments_get_name(hand, args, self->carg++);
    self->widthOfFirstOrderValues         = grib_arguments_get_name(hand, args, self->carg++);
    self->N1                              = grib_arguments_get_name(hand, args, self->carg++);
    self->N2                              = grib_arguments_get_name(hand, args, self->carg++);
    self->numberOfGroups                  = grib_arguments_get_name(hand, args, self->carg++);
    self->numberOfSecondOrderPackedValues = grib_arguments_get_name(hand, args, self->carg++);
    self->extraValues                     = grib_arguments_get_name(hand, args, self->carg++);
    self->Ni                              = grib_arguments_get_name(hand, args, self->carg++);
    self->Nj                              = grib_arguments_get_name(hand, args, self->carg++);
    self->pl                              = grib_arguments_get_name(hand, args, self->carg++);
    self->jPointsAreConsecutive           = grib_arguments_get_name(hand, args, self->carg++);
    self->bitmap                          = grib_arguments_get_name(hand, args, self->carg++);
    self->groupWidths                     = grib_arguments_get_name(hand, args, self->carg++);
    self->edition                         = 1;
    a->flags |= GRIB_ACCESSOR_FLAG_DATA;
}

int grib_accessor_class_data_g1second_order_general_packing_t::value_count(grib_accessor* a, long* numberOfSecondOrderPackedValues){
    int err                                                 = 0;
    grib_accessor_data_g1second_order_general_packing_t* self = (grib_accessor_data_g1second_order_general_packing_t*)a;
    *numberOfSecondOrderPackedValues                        = 0;

    err = grib_get_long_internal(grib_handle_of_accessor(a), self->numberOfSecondOrderPackedValues, numberOfSecondOrderPackedValues);

    return err;
}

int grib_accessor_class_data_g1second_order_general_packing_t::unpack_float(grib_accessor* a, float* values, size_t* len){
    return unpack<float>(a, values, len);
}

int grib_accessor_class_data_g1second_order_general_packing_t::unpack_double(grib_accessor* a, double* values, size_t* len){
    return unpack<double>(a, values, len);
}

int grib_accessor_class_data_g1second_order_general_packing_t::pack_double(grib_accessor* a, const double* cval, size_t* len){
    /* return GRIB_NOT_IMPLEMENTED; */
    int err           = 0;
    char type[]       = "grid_second_order";
    size_t size       = strlen(type);
    grib_handle* hand = grib_handle_of_accessor(a);

    err = grib_set_string(hand, "packingType", type, &size);
    if (err)
        return err;

    return grib_set_double_array(hand, "values", cval, *len);
}
