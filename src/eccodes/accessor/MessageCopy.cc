/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "MessageCopy.h"

eccodes::AccessorBuilder<eccodes::accessor::MessageCopy> _grib_accessor_message_copy_builder{};

namespace eccodes::accessor
{

void MessageCopy::init(const long length, grib_arguments* args)
{
    Gen::init(length, args);
    flags_ |= GRIB_ACCESSOR_FLAG_READ_ONLY;
    length_ = 0;
}

void MessageCopy::dump(eccodes::Dumper* dumper)
{
    dumper->dump_string(this, NULL);
}

long MessageCopy::get_native_type()
{
    return GRIB_TYPE_STRING;
}

int MessageCopy::unpack_string(char* val, size_t* len)
{
    size_t slen = grib_handle_of_accessor(this)->buffer->ulength;
    size_t i;
    unsigned char* v = 0;

    if (*len < slen) {
        return GRIB_ARRAY_TOO_SMALL;
    }
    v = grib_handle_of_accessor(this)->buffer->data;
    /* replace unprintable characters with space */
    for (i = 0; i < slen; i++)
        if (v[i] > 126)
            v[i] = 32;
    memcpy(val, grib_handle_of_accessor(this)->buffer->data, slen);
    val[i] = 0;

    *len = slen;

    return GRIB_SUCCESS;
}

size_t MessageCopy::string_length()
{
    return grib_handle_of_accessor(this)->buffer->ulength;
}

long MessageCopy::byte_count()
{
    return length_;
}

}  // namespace eccodes::accessor
