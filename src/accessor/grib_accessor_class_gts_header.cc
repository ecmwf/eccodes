
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
#include "grib_accessor_class_gts_header.h"

grib_accessor_class_gts_header_t _grib_accessor_class_gts_header{"gts_header"};
grib_accessor_class* grib_accessor_class_gts_header = &_grib_accessor_class_gts_header;


void grib_accessor_class_gts_header_t::init(grib_accessor* a, const long l, grib_arguments* c){
    grib_accessor_gts_header_t* self = (grib_accessor_gts_header_t*)a;
    self->gts_offset = -1;
    self->gts_length = -1;
    self->gts_offset = grib_arguments_get_long(grib_handle_of_accessor(a), c, 0);
    self->gts_length = grib_arguments_get_long(grib_handle_of_accessor(a), c, 1);
    a->flags |= GRIB_ACCESSOR_FLAG_READ_ONLY;
}

int grib_accessor_class_gts_header_t::unpack_string(grib_accessor* a, char* val, size_t* len){
    grib_accessor_gts_header_t* self = (grib_accessor_gts_header_t*)a;
    grib_handle* h                 = grib_handle_of_accessor(a);
    int offset                     = 0;
    size_t length                  = 0;

    if (h->gts_header == NULL || h->gts_header_len < 8) {
        if (*len < 8)
            return GRIB_BUFFER_TOO_SMALL;
        snprintf(val, 1024, "missing");
        return GRIB_SUCCESS;
    }
    if (*len < h->gts_header_len)
        return GRIB_BUFFER_TOO_SMALL;

    offset = self->gts_offset > 0 ? self->gts_offset : 0;
    length = self->gts_length > 0 ? self->gts_length : h->gts_header_len;

    memcpy(val, h->gts_header + offset, length);

    *len = length;

    return GRIB_SUCCESS;
}

int grib_accessor_class_gts_header_t::value_count(grib_accessor* a, long* count){
    *count = 1;
    return 0;
}

size_t grib_accessor_class_gts_header_t::string_length(grib_accessor* a){
    grib_handle* h = grib_handle_of_accessor(a);
    return h->gts_header_len;
}
