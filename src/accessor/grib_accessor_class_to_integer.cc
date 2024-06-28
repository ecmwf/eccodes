
/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_accessor_class_to_integer.h"

grib_accessor_class_to_integer_t _grib_accessor_class_to_integer{ "to_integer" };
grib_accessor_class* grib_accessor_class_to_integer = &_grib_accessor_class_to_integer;


void grib_accessor_class_to_integer_t::init(grib_accessor* a, const long len, grib_arguments* arg)
{
    grib_accessor_class_gen_t::init(a, len, arg);
    grib_accessor_to_integer_t* self = (grib_accessor_to_integer_t*)a;

    self->key    = grib_arguments_get_name(grib_handle_of_accessor(a), arg, 0);
    self->start  = grib_arguments_get_long(grib_handle_of_accessor(a), arg, 1);
    self->length = grib_arguments_get_long(grib_handle_of_accessor(a), arg, 2);

    a->flags |= GRIB_ACCESSOR_FLAG_READ_ONLY;
    a->length = 0;
}

int grib_accessor_class_to_integer_t::value_count(grib_accessor* a, long* count)
{
    grib_accessor_to_integer_t* self = (grib_accessor_to_integer_t*)a;
    size_t size = 0;

    int err = grib_get_size(grib_handle_of_accessor(a), self->key, &size);
    *count  = size;

    return err;
}

size_t grib_accessor_class_to_integer_t::string_length(grib_accessor* a)
{
    grib_accessor_to_integer_t* self = (grib_accessor_to_integer_t*)a;
    size_t size = 0;

    if (self->length)
        return self->length;

    grib_get_string_length(grib_handle_of_accessor(a), self->key, &size);
    return size;
}

void grib_accessor_class_to_integer_t::dump(grib_accessor* a, grib_dumper* dumper)
{
    grib_dump_long(dumper, a, NULL);
}

int grib_accessor_class_to_integer_t::get_native_type(grib_accessor* a)
{
    return GRIB_TYPE_LONG;
}

int grib_accessor_class_to_integer_t::unpack_string(grib_accessor* a, char* val, size_t* len)
{
    grib_accessor_to_integer_t* self = (grib_accessor_to_integer_t*)a;

    int err = 0;
    char buff[512] = {0,};
    size_t size = 512;

    size_t length = string_length(a);

    if (*len < length + 1) {
        const char* cclass_name = a->cclass->name;
        grib_context_log(a->context, GRIB_LOG_ERROR,
                         "%s: Buffer too small for %s. It is %zu bytes long (len=%zu)",
                         cclass_name, a->name, length + 1, *len);
        *len = length + 1;
        return GRIB_BUFFER_TOO_SMALL;
    }

    err = grib_get_string(grib_handle_of_accessor(a), self->key, buff, &size);
    if (err)
        return err;
    if (length > size) {
        /*err=GRIB_STRING_TOO_SMALL;*/
        length = size;
    }

    memcpy(val, buff + self->start, length);

    val[length] = 0;
    *len = length;
    return GRIB_SUCCESS;
}

int grib_accessor_class_to_integer_t::pack_string(grib_accessor* a, const char* val, size_t* len)
{
    return GRIB_NOT_IMPLEMENTED;
}

int grib_accessor_class_to_integer_t::pack_long(grib_accessor* a, const long* v, size_t* len)
{
    grib_context_log(a->context, GRIB_LOG_ERROR, "Should not pack %s as an integer", a->name);
    return GRIB_NOT_IMPLEMENTED;
}

int grib_accessor_class_to_integer_t::pack_double(grib_accessor* a, const double* v, size_t* len)
{
    grib_context_log(a->context, GRIB_LOG_ERROR, "Should not pack %s as a double", a->name);
    return GRIB_NOT_IMPLEMENTED;
}

int grib_accessor_class_to_integer_t::unpack_long(grib_accessor* a, long* v, size_t* len)
{
    char val[1024] = {0,};
    size_t l   = sizeof(val);
    char* last = NULL;
    int err    = unpack_string(a, val, &l);

    if (err)
        return err;

    *v = strtol(val, &last, 10);
    /* if (*last) {err=GRIB_WRONG_CONVERSION;} */

    return GRIB_SUCCESS;
}

int grib_accessor_class_to_integer_t::unpack_double(grib_accessor* a, double* v, size_t* len)
{
    size_t l = 1;
    long val = 0;
    int err  = unpack_long(a, &val, &l);

    *v = (double)val;
    return err;
}

long grib_accessor_class_to_integer_t::next_offset(grib_accessor* a)
{
    return a->offset + a->length;
}
