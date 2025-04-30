/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "Getenv.h"

eccodes::AccessorBuilder<eccodes::accessor::Getenv> _grib_accessor_getenv_builder{};

namespace eccodes::accessor
{

void Getenv::init(const long l, grib_arguments* args)
{
    Ascii::init(l, args);
    static char undefined[] = "undefined";

    envvar_          = args->get_string(get_enclosing_handle(), 0);
    default_value_ = args->get_string(get_enclosing_handle(), 1);
    if (!default_value_)
        default_value_ = undefined;
    value_ = 0;
}

int Getenv::pack_string(const char* val, size_t* len)
{
    return GRIB_NOT_IMPLEMENTED;
}

int Getenv::unpack_string(char* val, size_t* len)
{
    char* v  = 0;
    size_t l = 0;

    if (!value_) {
        v = getenv(envvar_);
        if (!v)
            v = (char*)default_value_;
        value_ = v;
    }

    l = strlen(value_);
    if (*len < l)
        return GRIB_BUFFER_TOO_SMALL;
    snprintf(val, 1024, "%s", value_);
    *len = strlen(value_);

    return GRIB_SUCCESS;
}

int Getenv::value_count(long* count)
{
    *count = 1;
    return 0;
}

size_t Getenv::string_length()
{
    return 1024;
}

}  // namespace eccodes::accessor
