/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_accessor_class_suppressed.h"

AccessorBuilder<grib_accessor_suppressed_t> _grib_accessor_suppressed_builder{};

void grib_accessor_suppressed_t::init(const long l, grib_arguments* c)
{
    grib_accessor_long_t::init(l, c);
    args_ = c;
    flags_ |= GRIB_ACCESSOR_FLAG_READ_ONLY;
    flags_ |= GRIB_ACCESSOR_FLAG_FUNCTION;
    length_ = 0;
}

static void log_message(grib_accessor* a)
{
    grib_accessor_suppressed_t* self = (grib_accessor_suppressed_t*)a;
    int i                            = 0;
    grib_handle* hand                = grib_handle_of_accessor(a);

    grib_context_log(a->context_, GRIB_LOG_ERROR,
                     "key '%s' is unavailable in this version.", a->name_);
    grib_context_log(a->context_, GRIB_LOG_ERROR,
                     "Please use the following key(s):");
    while (grib_arguments_get_name(hand, self->args_, i)) {
        grib_context_log(a->context_, GRIB_LOG_ERROR, "\t- %s",
                         grib_arguments_get_name(hand, self->args_, i));
        i++;
    }
}

int grib_accessor_suppressed_t::unpack_string(char* val, size_t* len)
{
    log_message(this);
    return GRIB_NOT_FOUND;
}

int grib_accessor_suppressed_t::unpack_long(long* val, size_t* len)
{
    log_message(this);
    return GRIB_NOT_FOUND;
}

int grib_accessor_suppressed_t::unpack_double(double* val, size_t* len)
{
    log_message(this);
    return GRIB_NOT_FOUND;
}

int grib_accessor_suppressed_t::value_count(long* count)
{
    *count = 1;
    return 0;
}

long grib_accessor_suppressed_t::get_native_type()
{
    return GRIB_TYPE_STRING;
}
