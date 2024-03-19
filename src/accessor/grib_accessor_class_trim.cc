
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
#include "grib_accessor_class_trim.h"

grib_accessor_class_trim_t _grib_accessor_class_trim{"trim"};
grib_accessor_class* grib_accessor_class_trim = &_grib_accessor_class_trim;


void grib_accessor_class_trim_t::init(grib_accessor* a, const long l, grib_arguments* arg){
    grib_accessor_class_ascii_t::init(a, l, arg);
    int n                    = 0;
    grib_accessor_trim_t* self = (grib_accessor_trim_t*)a;
    grib_handle* h           = grib_handle_of_accessor(a);

    self->input     = grib_arguments_get_name(h, arg, n++);
    self->trim_left = grib_arguments_get_long(h, arg, n++);
    self->trim_right= grib_arguments_get_long(h, arg, n++);
    DEBUG_ASSERT(self->trim_left == 0 || self->trim_left == 1);
    DEBUG_ASSERT(self->trim_right == 0 || self->trim_right == 1);
}

int grib_accessor_class_trim_t::unpack_string(grib_accessor* a, char* val, size_t* len){
    grib_accessor_trim_t* self = (grib_accessor_trim_t*)a;

    int err        = 0;
    grib_handle* h = grib_handle_of_accessor(a);
    char input[256] = {0,};
    size_t size    = sizeof(input) / sizeof(*input);
    char* pInput   = input;

    err = grib_get_string(h, self->input, input, &size);
    if (err) return err;

    string_lrtrim(&pInput, self->trim_left, self->trim_right);
    snprintf(val, 1024, "%s", pInput);
    size = strlen(val);
    *len = size + 1;
    return GRIB_SUCCESS;
}

int grib_accessor_class_trim_t::pack_string(grib_accessor* a, const char* val, size_t* len){
    char input[256] = {0,};

    size_t inputLen = 256;
    char buf[256]   = {0,};
    char* pBuf = NULL;
    int err;
    grib_handle* h = grib_handle_of_accessor(a);
    grib_accessor_trim_t* self = (grib_accessor_trim_t*)a;
    grib_accessor* inputAccesstor = grib_find_accessor(h, self->input);
    if (!inputAccesstor) {
        grib_context_log(a->context, GRIB_LOG_ERROR, "Accessor for %s not found", self->input);
        return GRIB_NOT_FOUND;
    }

    if ((err = grib_get_string(h, self->input, input, &inputLen)) != GRIB_SUCCESS)
        return err;

    snprintf(buf, sizeof(buf), "%s", val);
    pBuf = buf;
    string_lrtrim(&pBuf, self->trim_left, self->trim_right);

    return inputAccesstor->pack_string(pBuf, len);}

size_t grib_accessor_class_trim_t::string_length(grib_accessor* a){
    return 1024;
}
