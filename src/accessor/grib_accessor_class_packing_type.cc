
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
#include "grib_accessor_class_packing_type.h"

grib_accessor_class_packing_type_t _grib_accessor_class_packing_type{"packing_type"};
grib_accessor_class* grib_accessor_class_packing_type = &_grib_accessor_class_packing_type;


void grib_accessor_class_packing_type_t::init(grib_accessor* a, const long l, grib_arguments* args){
    int n                              = 0;
    grib_accessor_packing_type_t* self = (grib_accessor_packing_type_t*)a;
    self->values                       = grib_arguments_get_name(grib_handle_of_accessor(a), args, n++);
    self->packing_type                 = grib_arguments_get_name(grib_handle_of_accessor(a), args, n++);
    a->flags |= GRIB_ACCESSOR_FLAG_FUNCTION;
    a->length = 0;
}

size_t grib_accessor_class_packing_type_t::string_length(grib_accessor* a){
    return 1024;
}

int grib_accessor_class_packing_type_t::get_native_type(grib_accessor* a){
    return GRIB_TYPE_STRING;
}

int grib_accessor_class_packing_type_t::pack_string(grib_accessor*a, const char* sval, size_t* len){
    grib_accessor_packing_type_t* self = (grib_accessor_packing_type_t*)a;
    grib_handle* h = grib_handle_of_accessor(a);
    double* values = NULL;
    grib_context* c = a->context;
    size_t size = 0;
    int err = 0;

    if ((err = grib_get_size(h, self->values, &size)) != GRIB_SUCCESS)
        return err;

    values = (double*)grib_context_malloc(c, size * sizeof(double));
    if (!values) return GRIB_OUT_OF_MEMORY;

    if ((err = grib_get_double_array_internal(h, self->values, values, &size)) != GRIB_SUCCESS) {
        grib_context_free(c, values);
        return err;
    }

    if ((err = grib_set_string_internal(h, self->packing_type, sval, len)) != GRIB_SUCCESS) {
        grib_context_free(c, values);
        return err;
    }

    if ((err = grib_set_double_array_internal(h, self->values, values, size)) != GRIB_SUCCESS) {
        grib_context_free(c, values);
        return err;
    }

    grib_context_free(c, values);
    return GRIB_SUCCESS;
}

int grib_accessor_class_packing_type_t::unpack_string(grib_accessor* a, char* val, size_t* len){
    grib_accessor_packing_type_t* self = (grib_accessor_packing_type_t*)a;
    grib_handle* h                   = grib_handle_of_accessor(a);

    return grib_get_string(h, self->packing_type, val, len);
}
