/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_accessor_class_to_double.h"

grib_accessor_to_double_t _grib_accessor_to_double{};
grib_accessor* grib_accessor_to_double = &_grib_accessor_to_double;

void grib_accessor_to_double_t::init(const long len, grib_arguments* arg)
{
    grib_accessor_gen_t::init(len, arg);

    key_    = grib_arguments_get_name(grib_handle_of_accessor(this), arg, 0);
    start_  = grib_arguments_get_long(grib_handle_of_accessor(this), arg, 1);
    length_ = grib_arguments_get_long(grib_handle_of_accessor(this), arg, 2);
    scale_  = grib_arguments_get_long(grib_handle_of_accessor(this), arg, 3);
    if (!scale_)
        scale_ = 1;

    grib_accessor::flags_ |= GRIB_ACCESSOR_FLAG_READ_ONLY;
    grib_accessor::length_ = 0;
}

int grib_accessor_to_double_t::value_count(long* count)
{
    size_t size = 0;

    int err = grib_get_size(grib_handle_of_accessor(this), key_, &size);
    *count  = size;

    return err;
}

size_t grib_accessor_to_double_t::string_length()
{
    size_t size = 0;

    if (length_)
        return length_;

    grib_get_string_length_acc(this, &size);
    return size;
}

void grib_accessor_to_double_t::dump(grib_dumper* dumper)
{
    grib_dump_string(dumper, this, NULL);
}

long grib_accessor_to_double_t::get_native_type()
{
    return GRIB_TYPE_LONG;
}

int grib_accessor_to_double_t::unpack_string(char* val, size_t* len)
{
    int err        = 0;
    char buff[512] = {
        0,
    };
    size_t size   = 512;
    size_t length = string_length();

    if (*len < length + 1) {
        grib_context_log(context_, GRIB_LOG_ERROR, "unpack_string: Wrong size (%lu) for %s, it contains %ld values",
                         *len, name_, grib_accessor::length_ + 1);
        *len = length + 1;
        return GRIB_ARRAY_TOO_SMALL;
    }

    err = grib_get_string(grib_handle_of_accessor(this), key_, buff, &size);
    if (err)
        return err;
    if (length > size) {
        err    = GRIB_STRING_TOO_SMALL;
        length = size;
    }

    memcpy(val, buff + start_, length);

    val[length] = 0;
    *len        = length;
    return err;
}

int grib_accessor_to_double_t::unpack_long(long* v, size_t* len)
{
    char val[1024] = {
        0,
    };
    size_t l   = sizeof(val);
    char* last = NULL;
    int err    = unpack_string(val, &l);
    if (err)
        return err;

    *v = strtol(val, &last, 10);
    if (*last) {
        err = GRIB_WRONG_CONVERSION;
    }
    *v /= scale_;

    return err;
}

int grib_accessor_to_double_t::unpack_double(double* v, size_t* len)
{
    char val[1024] = {
        0,
    };
    size_t l   = sizeof(val);
    char* last = NULL;
    int err    = unpack_string(val, &l);
    if (err)
        return err;

    *v = strtod(val, &last);
    if (*last) {
        err = GRIB_WRONG_CONVERSION;
    }
    *v /= scale_;

    return err;
}

long grib_accessor_to_double_t::next_offset()
{
    return offset_ + grib_accessor::length_;
}
