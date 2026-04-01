/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "ToInteger.h"

eccodes::AccessorBuilder<eccodes::accessor::ToInteger> _grib_accessor_to_integer_builder{};

namespace eccodes::accessor
{

void ToInteger::init(const long len, grib_arguments* arg)
{
    Gen::init(len, arg);
    grib_handle* hand = get_enclosing_handle();

    key_        = arg->get_name(hand, 0);
    start_      = arg->get_long(hand, 1);
    str_length_ = arg->get_long(hand, 2);

    flags_ |= GRIB_ACCESSOR_FLAG_READ_ONLY;
    grib_accessor::length_ = 0;
}

int ToInteger::value_count(long* count)
{
    size_t size = 0;

    int err = grib_get_size(get_enclosing_handle(), key_, &size);
    *count  = size;

    return err;
}

size_t ToInteger::string_length()
{
    size_t size = 0;

    if (str_length_)
        return str_length_;

    grib_get_string_length(get_enclosing_handle(), key_, &size);
    return size;
}

void ToInteger::dump(eccodes::Dumper* dumper)
{
    dumper->dump_long(this, NULL);
}

long ToInteger::get_native_type()
{
    return GRIB_TYPE_LONG;
}

int ToInteger::unpack_string(char* val, size_t* len)
{
    int err        = 0;
    char buff[512] = {0,};
    size_t size = sizeof(buff);
    size_t length = string_length();

    if (*len < length + 1) {
        grib_context_log(context_, GRIB_LOG_ERROR,
                         "%s: Buffer too small for %s. It is %zu bytes long (len=%zu)",
                         accessor_type().get().c_str(), name_, length + 1, *len);
        *len = length + 1;
        return GRIB_BUFFER_TOO_SMALL;
    }

    err = grib_get_string(get_enclosing_handle(), key_, buff, &size);
    if (err)
        return err;
    if (length > size) {
        /*err=GRIB_STRING_TOO_SMALL;*/
        length = size;
    }

    memcpy(val, buff + start_, length);

    val[length] = 0;
    *len        = length;
    return GRIB_SUCCESS;
}

int ToInteger::pack_string(const char* val, size_t* len)
{
    return GRIB_NOT_IMPLEMENTED;
}

int ToInteger::pack_long(const long* v, size_t* len)
{
    grib_context_log(context_, GRIB_LOG_ERROR, "Should not pack %s as an integer", name_);
    return GRIB_NOT_IMPLEMENTED;
}

int ToInteger::pack_double(const double* v, size_t* len)
{
    grib_context_log(context_, GRIB_LOG_ERROR, "Should not pack %s as a double", name_);
    return GRIB_NOT_IMPLEMENTED;
}

int ToInteger::unpack_long(long* v, size_t* len)
{
    char val[1024] = {0,};
    size_t l   = sizeof(val);
    char* last = NULL;
    int err    = unpack_string(val, &l);

    if (err)
        return err;

    *v = strtol(val, &last, 10);
    /* if (*last) {err=GRIB_WRONG_CONVERSION;} */

    return GRIB_SUCCESS;
}

int ToInteger::unpack_double(double* v, size_t* len)
{
    size_t l = 1;
    long val = 0;
    int err  = unpack_long(&val, &l);

    *v = (double)val;
    return err;
}

long ToInteger::next_offset()
{
    return offset_ + grib_accessor::length_;
}

}  // namespace eccodes::accessor
