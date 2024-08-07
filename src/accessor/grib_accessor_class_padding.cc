/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_accessor_class_padding.h"

grib_accessor_class_padding_t _grib_accessor_class_padding{ "padding" };
grib_accessor_class* grib_accessor_class_padding = &_grib_accessor_class_padding;


void grib_accessor_class_padding_t::init(grib_accessor* a, const long len, grib_arguments* arg)
{
    grib_accessor_class_bytes_t::init(a, len, arg);
    a->flags |= GRIB_ACCESSOR_FLAG_EDITION_SPECIFIC;
    a->flags |= GRIB_ACCESSOR_FLAG_READ_ONLY;
}

int grib_accessor_class_padding_t::compare(grib_accessor* a, grib_accessor* b)
{
    if (a->length != b->length)
        return GRIB_COUNT_MISMATCH;
    return GRIB_SUCCESS;
}

void grib_accessor_class_padding_t::update_size(grib_accessor* a, size_t new_size)
{
    a->length = new_size;
}

void grib_accessor_class_padding_t::resize(grib_accessor* a, size_t new_size)
{
    void* zero = grib_context_malloc_clear(a->context, new_size);

    grib_buffer_replace(a, (const unsigned char*)zero, new_size,
                        /*update_lengths=*/1, /*update_paddings=*/0);
    grib_context_free(a->context, zero);

    grib_context_log(a->context, GRIB_LOG_DEBUG,
                     "grib_accessor_class_padding::resize new_size=%zu a->length=%ld %s %s",
                     new_size, a->length, a->cclass->name, a->name);
    Assert(new_size == a->length);
}

int grib_accessor_class_padding_t::value_count(grib_accessor* a, long* c)
{
    *c = a->length;
    return 0;
}

long grib_accessor_class_padding_t::byte_count(grib_accessor* a)
{
    return a->length;
}

size_t grib_accessor_class_padding_t::string_length(grib_accessor* a)
{
    return (size_t)a->length;
}
