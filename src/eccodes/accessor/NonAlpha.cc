/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "NonAlpha.h"

eccodes::accessor::NonAlpha _grib_accessor_non_alpha;
eccodes::Accessor* grib_accessor_non_alpha = &_grib_accessor_non_alpha;

namespace eccodes::accessor
{

void NonAlpha::init(const long len, grib_arguments* arg)
{
    Gen::init(len, arg);
    const grib_buffer* buffer = this->get_enclosing_handle()->buffer;
    unsigned char* v = buffer->data + offset_;
    size_t i            = 0;
    while ((*v < 33 || *v > 126) && i <= buffer->ulength) {
        v++;
        i++;
    }
    length_ = i;

    flags_ |= GRIB_ACCESSOR_FLAG_READ_ONLY;
}

int NonAlpha::value_count(long* count)
{
    *count = 1;
    return 0;
}

size_t NonAlpha::string_length()
{
    return length_;
}

void NonAlpha::dump(eccodes::Dumper* dumper)
{
    dumper->dump_string(this, NULL);
}

long NonAlpha::get_native_type()
{
    return GRIB_TYPE_STRING;
}

int NonAlpha::unpack_string(char* val, size_t* len)
{
    grib_handle* hand = get_enclosing_handle();
    size_t u_length = length_;

    if (*len < (u_length + 1)) {
        grib_context_log(context_, GRIB_LOG_ERROR, "unpack_string: Wrong size (%zu) for %s, it contains %ld values",
                         *len, name_, length_ + 1);
        *len = length_ + 1;
        return GRIB_BUFFER_TOO_SMALL;
    }

    long i = 0;
    for (i = 0; i < length_; i++) {
        val[i] = hand->buffer->data[offset_ + i];
    }
    val[i] = 0;
    *len   = i;
    return GRIB_SUCCESS;
}

int NonAlpha::unpack_long(long* v, size_t* len)
{
    char val[1024] = {0,};
    size_t l   = sizeof(val);
    size_t i   = 0;
    char* last = NULL;
    int err    = unpack_string(val, &l);
    if (err)
        return err;

    i = 0;
    while (i < l - 1 && val[i] == ' ')
        i++;

    if (val[i] == 0) {
        *v = 0;
        return 0;
    }
    if (val[i + 1] == ' ' && i < l - 2)
        val[i + 1] = 0;

    *v = strtol(val, &last, 10);

    return GRIB_SUCCESS;
}

int NonAlpha::unpack_double(double* v, size_t* len)
{
    char val[1024] = {0,};
    size_t l = sizeof(val);
    char* last = NULL;
    unpack_string(val, &l);
    *v = strtod(val, &last);

    if (*last == 0) {
        return GRIB_SUCCESS;
    }

    return GRIB_NOT_IMPLEMENTED;
}

long NonAlpha::next_offset()
{
    return offset_ + length_;
}

}  // namespace eccodes::accessor
