
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
#include "grib_accessor_class_pack_bufr_values.h"

grib_accessor_class_pack_bufr_values_t _grib_accessor_class_pack_bufr_values{"pack_bufr_values"};
grib_accessor_class* grib_accessor_class_pack_bufr_values = &_grib_accessor_class_pack_bufr_values;


void grib_accessor_class_pack_bufr_values_t::init(grib_accessor* a, const long len, grib_arguments* params){
    grib_accessor_class_gen_t::init(a, len, params);
    char* key;
    grib_accessor_pack_bufr_values_t* self = (grib_accessor_pack_bufr_values_t*)a;
    key                                  = (char*)grib_arguments_get_name(grib_handle_of_accessor(a), params, 0);
    self->data_accessor                  = grib_find_accessor(grib_handle_of_accessor(a), key);

    a->length = 0;
}

void grib_accessor_class_pack_bufr_values_t::dump(grib_accessor* a, grib_dumper* dumper){
}

int grib_accessor_class_pack_bufr_values_t::unpack_string_array(grib_accessor* a, char** buffer, size_t* len){
    return GRIB_NOT_IMPLEMENTED;
}

int grib_accessor_class_pack_bufr_values_t::unpack_string(grib_accessor* a, char* buffer, size_t* len){
    return GRIB_NOT_IMPLEMENTED;
}

int grib_accessor_class_pack_bufr_values_t::unpack_long(grib_accessor* a, long* val, size_t* len){
    return GRIB_NOT_IMPLEMENTED;
}

int grib_accessor_class_pack_bufr_values_t::unpack_double(grib_accessor* a, double* val, size_t* len){
    return GRIB_NOT_IMPLEMENTED;
}

int grib_accessor_class_pack_bufr_values_t::value_count(grib_accessor* a, long* count){
    *count = 1;
    return 0;
}

void grib_accessor_class_pack_bufr_values_t::destroy(grib_context* context, grib_accessor* a){
}

int grib_accessor_class_pack_bufr_values_t::get_native_type(grib_accessor* a){
    return GRIB_TYPE_LONG;
}

int grib_accessor_class_pack_bufr_values_t::pack_long(grib_accessor* a, const long* val, size_t* len){
    grib_accessor_pack_bufr_values_t* self = (grib_accessor_pack_bufr_values_t*)a;
    grib_accessor* data                  = (grib_accessor*)self->data_accessor;

    return data->pack_double(0, 0);}

int grib_accessor_class_pack_bufr_values_t::pack_double(grib_accessor* a, const double* val, size_t* len){
    grib_accessor_pack_bufr_values_t* self = (grib_accessor_pack_bufr_values_t*)a;
    grib_accessor* data                  = (grib_accessor*)self->data_accessor;

    return data->pack_double(0, 0);}
