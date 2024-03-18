
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
#include "grib_accessor_class_message_copy.h"

grib_accessor_class_message_copy_t _grib_accessor_class_message_copy{"message_copy"};
grib_accessor_class* grib_accessor_class_message_copy = &_grib_accessor_class_message_copy;


void grib_accessor_class_message_copy_t::init(grib_accessor* a, const long length, grib_arguments* args){
    a->flags |= GRIB_ACCESSOR_FLAG_READ_ONLY;
    a->length = 0;
}

void grib_accessor_class_message_copy_t::dump(grib_accessor* a, grib_dumper* dumper){
    grib_dump_string(dumper, a, NULL);
}

int grib_accessor_class_message_copy_t::get_native_type(grib_accessor* a){
    return GRIB_TYPE_STRING;
}

int grib_accessor_class_message_copy_t::unpack_string(grib_accessor* a, char* val, size_t* len){
    size_t slen = grib_handle_of_accessor(a)->buffer->ulength;
    size_t i;
    unsigned char* v = 0;

    if (*len < slen) {
        return GRIB_ARRAY_TOO_SMALL;
    }
    v = grib_handle_of_accessor(a)->buffer->data;
    /* replace unprintable characters with space */
    for (i = 0; i < slen; i++)
        if (v[i] > 126)
            v[i] = 32;
    memcpy(val, grib_handle_of_accessor(a)->buffer->data, slen);
    val[i] = 0;

    *len = slen;

    return GRIB_SUCCESS;
}

size_t grib_accessor_class_message_copy_t::string_length(grib_accessor* a){
    return grib_handle_of_accessor(a)->buffer->ulength;
}

long grib_accessor_class_message_copy_t::byte_count(grib_accessor* a){
    return a->length;
}
