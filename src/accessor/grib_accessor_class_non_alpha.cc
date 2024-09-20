/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_accessor_class_non_alpha.h"

grib_accessor_class_non_alpha_t _grib_accessor_class_non_alpha{ "non_alpha" };
grib_accessor_class* grib_accessor_class_non_alpha = &_grib_accessor_class_non_alpha;


void grib_accessor_class_non_alpha_t::init(grib_accessor* a, const long len, grib_arguments* arg)
{
    grib_accessor_class_gen_t::init(a, len, arg);

    const grib_buffer* buffer = grib_handle_of_accessor(a)->buffer;
    unsigned char* v = buffer->data + a->offset;
    size_t i = 0;
    while ((*v < 33 || *v > 126) && i <= buffer->ulength) {
        v++;
        i++;
    }
    a->length = i;

    a->flags |= GRIB_ACCESSOR_FLAG_READ_ONLY;
}

int grib_accessor_class_non_alpha_t::value_count(grib_accessor* a, long* count)
{
    *count = 1;
    return 0;
}

size_t grib_accessor_class_non_alpha_t::string_length(grib_accessor* a)
{
    return a->length;
}

void grib_accessor_class_non_alpha_t::dump(grib_accessor* a, grib_dumper* dumper)
{
    grib_dump_string(dumper, a, NULL);
}

int grib_accessor_class_non_alpha_t::get_native_type(grib_accessor* a)
{
    return GRIB_TYPE_STRING;
}

int grib_accessor_class_non_alpha_t::unpack_string(grib_accessor* a, char* val, size_t* len)
{
    grib_handle* hand = grib_handle_of_accessor(a);
    long i            = 0;

    if (*len < (a->length + 1)) {
        grib_context_log(a->context, GRIB_LOG_ERROR, "unpack_string: Wrong size (%lu) for %s, it contains %ld values",
                         *len, a->name, a->length + 1);
        *len = a->length + 1;
        return GRIB_BUFFER_TOO_SMALL;
    }

    for (i = 0; i < a->length; i++) {
        val[i] = hand->buffer->data[a->offset + i];
    }
    val[i] = 0;
    *len   = i;
    return GRIB_SUCCESS;
}

int grib_accessor_class_non_alpha_t::unpack_long(grib_accessor* a, long* v, size_t* len)
{
    char val[1024] = {0,};
    size_t l   = sizeof(val);
    size_t i   = 0;
    char* last = NULL;
    int err    = a->unpack_string(val, &l);
    if (err)
        return err;

    i = 0;
    while (i < l - 1 && val[i] == ' ')
        i++;

    if (val[i] == 0) {
        *v = 0;
        return 0;
    }
    if (val[i + 1] == ' ' && i < l - 2)
        val[i + 1] = 0;

    *v = strtol(val, &last, 10);

    return GRIB_SUCCESS;
}

int grib_accessor_class_non_alpha_t::unpack_double(grib_accessor* a, double* v, size_t* len)
{
    char val[1024];
    size_t l   = sizeof(val);
    char* last = NULL;
    a->unpack_string(val, &l);
    *v = strtod(val, &last);

    if (*last == 0) {
        return GRIB_SUCCESS;
    }

    return GRIB_NOT_IMPLEMENTED;
}

long grib_accessor_class_non_alpha_t::next_offset(grib_accessor* a)
{
    return a->offset + a->length;
}
