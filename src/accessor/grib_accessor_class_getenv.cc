/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_accessor_class_getenv.h"

grib_accessor_class_getenv_t _grib_accessor_class_getenv{ "getenv" };
grib_accessor_class* grib_accessor_class_getenv = &_grib_accessor_class_getenv;


void grib_accessor_class_getenv_t::init(grib_accessor* a, const long l, grib_arguments* args)
{
    grib_accessor_class_ascii_t::init(a, l, args);
    grib_accessor_getenv_t* self = (grib_accessor_getenv_t*)a;
    static char undefined[] = "undefined";
    grib_handle* hand = grib_handle_of_accessor(a);

    self->envvar        = grib_arguments_get_string(hand, args, 0);
    self->default_value = grib_arguments_get_string(hand, args, 1);
    if (!self->default_value)
        self->default_value = undefined;
    self->value = 0;
}

int grib_accessor_class_getenv_t::pack_string(grib_accessor* a, const char* val, size_t* len)
{
    return GRIB_NOT_IMPLEMENTED;
}

int grib_accessor_class_getenv_t::unpack_string(grib_accessor* a, char* val, size_t* len)
{
    grib_accessor_getenv_t* self = (grib_accessor_getenv_t*)a;
    char* v = 0;
    size_t l = 0;

    if (!self->value) {
        v = getenv(self->envvar);
        if (!v)
            v = (char*)self->default_value;
        self->value = v;
    }

    l = strlen(self->value);
    if (*len < l)
        return GRIB_BUFFER_TOO_SMALL;
    snprintf(val, 1024, "%s", self->value);
    *len = strlen(self->value);

    return GRIB_SUCCESS;
}

int grib_accessor_class_getenv_t::value_count(grib_accessor* a, long* count)
{
    *count = 1;
    return 0;
}

size_t grib_accessor_class_getenv_t::string_length(grib_accessor* a)
{
    return 1024;
}
