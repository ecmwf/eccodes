
/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_accessor_class_ascii.h"

grib_accessor_class_ascii_t _grib_accessor_class_ascii{ "ascii" };
grib_accessor_class* grib_accessor_class_ascii = &_grib_accessor_class_ascii;


void grib_accessor_class_ascii_t::init(grib_accessor* a, const long len, grib_arguments* arg)
{
    grib_accessor_class_gen_t::init(a, len, arg);
    a->length = len;
    Assert(a->length >= 0);
}

int grib_accessor_class_ascii_t::value_count(grib_accessor* a, long* count)
{
    *count = 1;
    return 0;
}

size_t grib_accessor_class_ascii_t::string_length(grib_accessor* a)
{
    return a->length;
}

void grib_accessor_class_ascii_t::dump(grib_accessor* a, grib_dumper* dumper)
{
    grib_dump_string(dumper, a, NULL);
}

int grib_accessor_class_ascii_t::get_native_type(grib_accessor* a)
{
    return GRIB_TYPE_STRING;
}

int grib_accessor_class_ascii_t::unpack_string(grib_accessor* a, char* val, size_t* len)
{
    grib_handle* hand = grib_handle_of_accessor(a);
    const size_t alen = a->length;

    if (*len < (alen + 1)) {
        const char* cclass_name = a->cclass->name;
        grib_context_log(a->context, GRIB_LOG_ERROR,
                         "%s: Buffer too small for %s. It is %zu bytes long (len=%zu)",
                         cclass_name, a->name, alen + 1, *len);
        *len = alen + 1;
        return GRIB_BUFFER_TOO_SMALL;
    }

    size_t i = 0;
    for (i = 0; i < alen; i++)
        val[i] = hand->buffer->data[a->offset + i];
    val[i] = 0;
    *len   = i;
    return GRIB_SUCCESS;
}

int grib_accessor_class_ascii_t::pack_string(grib_accessor* a, const char* val, size_t* len)
{
    grib_handle* hand = grib_handle_of_accessor(a);
    const size_t alen = a->length;
    if (len[0] > (alen + 1)) {
        grib_context_log(a->context, GRIB_LOG_ERROR,
                         "pack_string: Wrong size (%zu) for %s, it contains %ld values",
                         len[0], a->name, a->length + 1);
        len[0] = 0;
        return GRIB_BUFFER_TOO_SMALL;
    }

    for (size_t i = 0; i < alen; i++) {
        if (i < len[0])
            hand->buffer->data[a->offset + i] = val[i];
        else
            hand->buffer->data[a->offset + i] = 0;
    }

    return GRIB_SUCCESS;
}

int grib_accessor_class_ascii_t::pack_long(grib_accessor* a, const long* v, size_t* len)
{
    grib_context_log(a->context, GRIB_LOG_ERROR, "Should not pack %s as long (It's a string)", a->name);
    return GRIB_NOT_IMPLEMENTED;
}

int grib_accessor_class_ascii_t::pack_double(grib_accessor* a, const double* v, size_t* len)
{
    grib_context_log(a->context, GRIB_LOG_ERROR, "Should not pack %s as double (It's a string)", a->name);
    return GRIB_NOT_IMPLEMENTED;
}

int grib_accessor_class_ascii_t::unpack_long(grib_accessor* a, long* v, size_t* len)
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

    grib_context_log(a->context, GRIB_LOG_DEBUG, " Casting string %s to long", a->name);
    return GRIB_SUCCESS;
}

int grib_accessor_class_ascii_t::unpack_double(grib_accessor* a, double* v, size_t* len)
{
    char val[1024];
    size_t l   = sizeof(val);
    char* last = NULL;

    int err = a->unpack_string(val, &l);
    if (err) return err;

    *v = strtod(val, &last);

    if (*last == 0) {
        grib_context_log(a->context, GRIB_LOG_DEBUG, " Casting string %s to long", a->name);
        return GRIB_SUCCESS;
    }

    grib_context_log(a->context, GRIB_LOG_WARNING, "Cannot unpack %s as double. Hint: Try unpacking as string", a->name);

    return GRIB_NOT_IMPLEMENTED;
}

int grib_accessor_class_ascii_t::compare(grib_accessor* a, grib_accessor* b)
{
    int retval = 0;
    char* aval = 0;
    char* bval = 0;
    int err    = 0;

    size_t alen = a->length + 1;
    size_t blen = b->length + 1;

    if (alen != blen)
        return GRIB_COUNT_MISMATCH;

    aval = (char*)grib_context_malloc(a->context, alen * sizeof(char));
    bval = (char*)grib_context_malloc(b->context, blen * sizeof(char));

    err = a->unpack_string(aval, &alen);
    if (err) return err;
    err = b->unpack_string(bval, &blen);
    if (err) return err;

    retval = GRIB_SUCCESS;
    if (!STR_EQUAL(aval, bval))
        retval = GRIB_STRING_VALUE_MISMATCH;

    grib_context_free(a->context, aval);
    grib_context_free(b->context, bval);

    return retval;
}
