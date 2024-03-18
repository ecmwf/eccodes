
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
#include "grib_accessor_class_blob.h"

grib_accessor_class_blob_t _grib_accessor_class_blob{"blob"};
grib_accessor_class* grib_accessor_class_blob = &_grib_accessor_class_blob;


void grib_accessor_class_blob_t::init(grib_accessor* a, const long len, grib_arguments* arg){
    grib_get_long_internal(grib_handle_of_accessor(a), grib_arguments_get_name(a->parent->h, arg, 0), &a->length);
    Assert(a->length >= 0);
}

int grib_accessor_class_blob_t::get_native_type(grib_accessor* a){
    return GRIB_TYPE_BYTES;
}

int grib_accessor_class_blob_t::unpack_bytes(grib_accessor* a, unsigned char* buffer, size_t* len){
    if (*len < (size_t)a->length) {
        *len = a->length;
        return GRIB_ARRAY_TOO_SMALL;
    }
    *len = a->length;

    memcpy(buffer, grib_handle_of_accessor(a)->buffer->data + a->offset, *len);

    return GRIB_SUCCESS;
}

void grib_accessor_class_blob_t::dump(grib_accessor* a, grib_dumper* dumper){
    grib_dump_bytes(dumper, a, NULL);
}
