
/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_accessor_class_message.h"

grib_accessor_class_message_t _grib_accessor_class_message{"message"};
grib_accessor_class* grib_accessor_class_message = &_grib_accessor_class_message;



void grib_accessor_class_message_t::init(grib_accessor* a, const long len, grib_arguments* arg){
    grib_accessor_class_bytes_t::init(a, len, arg);
    a->flags |= GRIB_ACCESSOR_FLAG_EDITION_SPECIFIC;
    a->flags |= GRIB_ACCESSOR_FLAG_READ_ONLY;
    a->length = grib_handle_of_accessor(a)->buffer->ulength - len - a->offset;
}

// static int compare(grib_accessor* a, grib_accessor* b)
// {
//     if (a->length != b->length)
//         return GRIB_COUNT_MISMATCH;
//     return GRIB_SUCCESS;
// }

void grib_accessor_class_message_t::update_size(grib_accessor* a, size_t new_size){
    a->length = new_size;
}

void grib_accessor_class_message_t::resize(grib_accessor* a, size_t new_size){
    grib_context_log(a->context, GRIB_LOG_FATAL, "%s %s: Not supported", a->cclass->name, __func__);

    // void* zero = grib_context_malloc_clear(a->context, new_size);
    // grib_buffer_replace(a, (const unsigned char*)zero, new_size, 1, 0);
    // grib_context_free(a->context, zero);
    // grib_context_log(a->context, GRIB_LOG_DEBUG, "resize: grib_accessor_class_message %ld %ld %s %s",
    //                 (long)new_size, (long)a->length, a->cclass->name, a->name);
    // Assert(new_size == a->length);
}

int grib_accessor_class_message_t::value_count(grib_accessor* a, long* count){
    *count = 1;
    return 0;
}

int grib_accessor_class_message_t::unpack_string(grib_accessor* a, char* val, size_t* len){
    long i = 0;
    size_t l = string_length(a) + 1;
    grib_handle* h = grib_handle_of_accessor(a);

    if (*len < l) {
        grib_context_log(a->context, GRIB_LOG_ERROR,
                         "%s: Buffer too small for %s. It is %zu bytes long (len=%zu)",
                         a->cclass->name, a->name, l, *len);
        *len = l;
        return GRIB_BUFFER_TOO_SMALL;
    }

    for (i = 0; i < a->length; i++)
        val[i] = h->buffer->data[a->offset + i];
    val[i] = 0;
    *len = i;
    return GRIB_SUCCESS;
}

size_t grib_accessor_class_message_t::string_length(grib_accessor* a){
    return a->length;
}
