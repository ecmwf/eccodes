/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "Message.h"

eccodes::AccessorBuilder<eccodes::accessor::Message> _grib_accessor_message_builder{};

namespace eccodes::accessor
{

void Message::init(const long len, grib_arguments* arg)
{
    Bytes::init(len, arg);
    flags_ |= GRIB_ACCESSOR_FLAG_EDITION_SPECIFIC;
    flags_ |= GRIB_ACCESSOR_FLAG_READ_ONLY;
    length_ = this->get_enclosing_handle()->buffer->ulength - len - offset_;
}

// static int compare(grib_accessor* a, grib_accessor* b)
// {
//     if (length_ != b->length)
//         return GRIB_COUNT_MISMATCH;
//     return GRIB_SUCCESS;
// }

void Message::update_size(size_t new_size)
{
    length_ = new_size;
}

void Message::resize(size_t new_size)
{
    grib_context_log(context_, GRIB_LOG_FATAL, "%s %s: Not supported", accessor_type().get().c_str(), __func__);

    // void* zero = grib_context_malloc_clear(context_ , new_size);
    // grib_buffer_replace(a, (const unsigned char*)zero, new_size, 1, 0);
    // grib_context_free(context_ , zero);
    // grib_context_log(context_ , GRIB_LOG_DEBUG, "resize: grib_accessor_message %ld %ld %s %s",
    //                 (long)new_size, (long)a->length, a->cclass->name, name_ );
    // ECCODES_ASSERT(new_size == length_ );
}

int Message::value_count(long* count)
{
    *count = 1;
    return 0;
}

int Message::unpack_string(char* val, size_t* len)
{
    long i         = 0;
    size_t l       = string_length() + 1;
    grib_handle* h = get_enclosing_handle();

    if (*len < l) {
        grib_context_log(context_, GRIB_LOG_ERROR,
                         "%s: Buffer too small for %s. It is %zu bytes long (len=%zu)",
                         accessor_type().get().c_str(), name_, l, *len);
        *len = l;
        return GRIB_BUFFER_TOO_SMALL;
    }

    for (i = 0; i < length_; i++)
        val[i] = h->buffer->data[offset_ + i];
    val[i] = 0;
    *len   = i;
    return GRIB_SUCCESS;
}

size_t Message::string_length()
{
    return length_;
}

}  // namespace eccodes::accessor
