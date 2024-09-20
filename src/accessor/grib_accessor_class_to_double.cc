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

grib_accessor_class_to_double_t _grib_accessor_class_to_double{ "to_double" };
grib_accessor_class* grib_accessor_class_to_double = &_grib_accessor_class_to_double;


void grib_accessor_class_to_double_t::init(grib_accessor* a, const long len, grib_arguments* arg)
{
    grib_accessor_class_gen_t::init(a, len, arg);
    grib_accessor_to_double_t* self = (grib_accessor_to_double_t*)a;

    self->key    = grib_arguments_get_name(grib_handle_of_accessor(a), arg, 0);
    self->start  = grib_arguments_get_long(grib_handle_of_accessor(a), arg, 1);
    self->length = grib_arguments_get_long(grib_handle_of_accessor(a), arg, 2);
    self->scale  = grib_arguments_get_long(grib_handle_of_accessor(a), arg, 3);
    if (!self->scale)
        self->scale = 1;

    a->flags |= GRIB_ACCESSOR_FLAG_READ_ONLY;
    a->length = 0;
}

int grib_accessor_class_to_double_t::value_count(grib_accessor* a, long* count)
{
    grib_accessor_to_double_t* self = (grib_accessor_to_double_t*)a;
    size_t size = 0;

    int err = grib_get_size(grib_handle_of_accessor(a), self->key, &size);
    *count  = size;

    return err;
}

size_t grib_accessor_class_to_double_t::string_length(grib_accessor* a)
{
    grib_accessor_to_double_t* self = (grib_accessor_to_double_t*)a;
    size_t size = 0;

    if (self->length)
        return self->length;

    grib_get_string_length_acc(a, &size);
    return size;
}

void grib_accessor_class_to_double_t::dump(grib_accessor* a, grib_dumper* dumper)
{
    grib_dump_string(dumper, a, NULL);
}

int grib_accessor_class_to_double_t::get_native_type(grib_accessor* a)
{
    return GRIB_TYPE_LONG;
}

int grib_accessor_class_to_double_t::unpack_string(grib_accessor* a, char* val, size_t* len)
{
    grib_accessor_to_double_t* self = (grib_accessor_to_double_t*)a;

    int err = 0;
    char buff[512] = {0,};
    size_t size   = 512;
    size_t length = string_length(a);

    if (*len < length + 1) {
        grib_context_log(a->context, GRIB_LOG_ERROR, "unpack_string: Wrong size (%lu) for %s, it contains %ld values",
                         *len, a->name, a->length + 1);
        *len = length + 1;
        return GRIB_ARRAY_TOO_SMALL;
    }

    err = grib_get_string(grib_handle_of_accessor(a), self->key, buff, &size);
    if (err)
        return err;
    if (length > size) {
        err    = GRIB_STRING_TOO_SMALL;
        length = size;
    }

    memcpy(val, buff + self->start, length);

    val[length] = 0;
    *len = length;
    return err;
}

int grib_accessor_class_to_double_t::unpack_long(grib_accessor* a, long* v, size_t* len)
{
    grib_accessor_to_double_t* self = (grib_accessor_to_double_t*)a;
    char val[1024] = {0,};
    size_t l   = sizeof(val);
    char* last = NULL;
    int err    = a->unpack_string(val, &l);
    if (err)
        return err;

    *v = strtol(val, &last, 10);
    if (*last) {
        err = GRIB_WRONG_CONVERSION;
    }
    *v /= self->scale;

    return err;
}

int grib_accessor_class_to_double_t::unpack_double(grib_accessor* a, double* v, size_t* len)
{
    grib_accessor_to_double_t* self = (grib_accessor_to_double_t*)a;
    char val[1024] = {0,};
    size_t l   = sizeof(val);
    char* last = NULL;
    int err    = a->unpack_string(val, &l);
    if (err)
        return err;

    *v = strtod(val, &last);
    if (*last) {
        err = GRIB_WRONG_CONVERSION;
    }
    *v /= self->scale;

    return err;
}

long grib_accessor_class_to_double_t::next_offset(grib_accessor* a)
{
    return a->offset + a->length;
}
