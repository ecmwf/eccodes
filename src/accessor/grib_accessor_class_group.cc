
/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_api_internal.h"
#include "grib_accessor_class_group.h"

grib_accessor_class_group_t _grib_accessor_class_group{"group"};
grib_accessor_class* grib_accessor_class_group = &_grib_accessor_class_group;


void grib_accessor_class_group_t::init(grib_accessor* a, const long len, grib_arguments* arg){
    grib_accessor_class_gen_t::init(a, len, arg);
    grib_buffer* buffer       = grib_handle_of_accessor(a)->buffer;
    grib_accessor_group_t* self = (grib_accessor_group_t*)a;

    size_t i = 0;
    unsigned char* v;
    const char* s = grib_arguments_get_string(grib_handle_of_accessor(a), arg, 0);

    if (s && strlen(s) > 1) {
        grib_context_log(a->context, GRIB_LOG_WARNING,
                         "Using only first character as group end of %s not the string %s", a->name, s);
    }

    self->endCharacter = s ? s[0] : 0;

    v = buffer->data + a->offset;
    i = 0;
    if (s) {
        while (*v != self->endCharacter && i <= buffer->ulength) {
            if (*v > 126)
                *v = 32;
            v++;
            i++;
        }
    }
    else {
        while (*v > 32 && *v != 61 && *v < 127 && i <= buffer->ulength) {
            v++;
            i++;
        }
    }
    a->length = i;

    a->flags |= GRIB_ACCESSOR_FLAG_READ_ONLY;
}

int grib_accessor_class_group_t::value_count(grib_accessor* a, long* count){
    *count = 1;
    return 0;
}

size_t grib_accessor_class_group_t::string_length(grib_accessor* a){
    return a->length;
}

void grib_accessor_class_group_t::dump(grib_accessor* a, grib_dumper* dumper){
    grib_dump_string(dumper, a, NULL);
}

int grib_accessor_class_group_t::get_native_type(grib_accessor* a){
    return GRIB_TYPE_STRING;
}

int grib_accessor_class_group_t::unpack_string(grib_accessor* a, char* val, size_t* len){
    long i = 0;
    size_t l = a->length + 1;
    grib_handle* h = grib_handle_of_accessor(a);

    if (*len < l) {
        grib_context_log(a->context, GRIB_LOG_ERROR,
                         "%s: Buffer too small for %s. It is %zu bytes long (len=%zu)",
                         a->cclass->name, a->name, l, *len);
        *len = l;
        return GRIB_ARRAY_TOO_SMALL;
    }

    for (i = 0; i < a->length; i++)
        val[i] = h->buffer->data[a->offset + i];
    val[i] = 0;
    *len = i;
    return GRIB_SUCCESS;
}

int grib_accessor_class_group_t::unpack_long(grib_accessor* a, long* v, size_t* len){
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

int grib_accessor_class_group_t::unpack_double(grib_accessor* a, double* v, size_t* len){
    char val[1024];
    size_t l   = sizeof(val);
    char* last = NULL;
    a->unpack_string(val, &l);
    *v = strtod(val, &last);

    if (*last == 0) {
        grib_context_log(a->context, GRIB_LOG_DEBUG, " Casting string %s to long", a->name);
        return GRIB_SUCCESS;
    }

    return GRIB_NOT_IMPLEMENTED;
}

int grib_accessor_class_group_t::compare(grib_accessor* a, grib_accessor* b){
    grib_context_log(a->context, GRIB_LOG_ERROR, "%s:%s not implemented", __func__, a->name);
    return GRIB_NOT_IMPLEMENTED;

    // int retval = 0;
    // char* aval = 0;
    // char* bval = 0;
    // int err    = 0;
    // size_t alen = 0;
    // size_t blen = 0;
    // long count  = 0;
    // err = a->value_count(&count);    // if (err) return err;
    // alen = count;
    // err = b->value_count(&count);    // if (err) return err;
    // blen = count;
    // if (alen != blen) return GRIB_COUNT_MISMATCH;
    // aval = (char*)grib_context_malloc(a->context, alen * sizeof(char));
    // bval = (char*)grib_context_malloc(b->context, blen * sizeof(char));
    // a->unpack_string(aval, &alen);    // b->unpack_string(bval, &blen);    // retval = GRIB_SUCCESS;
    // if (strcmp(aval, bval)) retval = GRIB_STRING_VALUE_MISMATCH;
    // grib_context_free(a->context, aval);
    // grib_context_free(b->context, bval);
    // return retval;
}

long grib_accessor_class_group_t::next_offset(grib_accessor* a){
    return a->offset + a->length;
}
