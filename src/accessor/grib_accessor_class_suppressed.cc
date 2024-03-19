
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
#include "grib_accessor_class_suppressed.h"

grib_accessor_class_suppressed_t _grib_accessor_class_suppressed{"suppressed"};
grib_accessor_class* grib_accessor_class_suppressed = &_grib_accessor_class_suppressed;


void grib_accessor_class_suppressed_t::init(grib_accessor* a, const long l, grib_arguments* c){
    grib_accessor_class_long_t::init(a, l, c);
    grib_accessor_suppressed_t* self = (grib_accessor_suppressed_t*)a;
    self->args                     = c;
    a->flags |= GRIB_ACCESSOR_FLAG_READ_ONLY;
    a->flags |= GRIB_ACCESSOR_FLAG_FUNCTION;
    a->length = 0;
}

void log_message(grib_accessor* a){
    grib_accessor_suppressed_t* self = (grib_accessor_suppressed_t*)a;
    int i                          = 0;
    grib_handle* hand              = grib_handle_of_accessor(a);

    grib_context_log(a->context, GRIB_LOG_ERROR,
                     "key '%s' is unavailable in this version.", a->name);
    grib_context_log(a->context, GRIB_LOG_ERROR,
                     "Please use the following key(s):");
    while (grib_arguments_get_name(hand, self->args, i)) {
        grib_context_log(a->context, GRIB_LOG_ERROR, "\t- %s",
                         grib_arguments_get_name(hand, self->args, i));
        i++;
    }
}

int grib_accessor_class_suppressed_t::unpack_string(grib_accessor* a, char* val, size_t* len){
    log_message(a);
    return GRIB_NOT_FOUND;
}

int grib_accessor_class_suppressed_t::unpack_long(grib_accessor* a, long* val, size_t* len){
    log_message(a);
    return GRIB_NOT_FOUND;
}

int grib_accessor_class_suppressed_t::unpack_double(grib_accessor* a, double* val, size_t* len){
    log_message(a);
    return GRIB_NOT_FOUND;
}

int grib_accessor_class_suppressed_t::value_count(grib_accessor* a, long* count){
    *count = 1;
    return 0;
}

int grib_accessor_class_suppressed_t::get_native_type(grib_accessor* a){
    return GRIB_TYPE_STRING;
}
