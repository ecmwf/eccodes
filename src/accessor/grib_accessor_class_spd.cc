
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
#include "grib_accessor_class_spd.h"

grib_accessor_class_spd_t _grib_accessor_class_spd{"spd"};
grib_accessor_class* grib_accessor_class_spd = &_grib_accessor_class_spd;


long grib_accessor_class_spd_t::byte_count(grib_accessor* a){
    return a->length;
}

static long compute_byte_count(grib_accessor* a){
    grib_accessor_spd_t* self = (grib_accessor_spd_t*)a;
    long numberOfBits = 0;
    long numberOfElements = 0;

    int ret = grib_get_long(grib_handle_of_accessor(a), self->numberOfBits, &numberOfBits);
    if (ret) {
        grib_context_log(a->context, GRIB_LOG_ERROR,
                         "%s unable to get %s to compute size", a->name, self->numberOfBits);
        return 0;
    }

    ret = grib_get_long(grib_handle_of_accessor(a), self->numberOfElements, &numberOfElements);
    if (ret) {
        grib_context_log(a->context, GRIB_LOG_ERROR,
                         "%s unable to get %s to compute size", a->name, self->numberOfElements);
        return 0;
    }
    numberOfElements++;

    return (numberOfBits * numberOfElements + 7) / 8;
}

void grib_accessor_class_spd_t::init(grib_accessor* a, const long len, grib_arguments* args){
    grib_accessor_class_long_t::init(a, len, args);
    grib_accessor_spd_t* self = (grib_accessor_spd_t*)a;
    int n                   = 0;
    self->numberOfBits      = grib_arguments_get_name(grib_handle_of_accessor(a), args, n++);
    self->numberOfElements  = grib_arguments_get_name(grib_handle_of_accessor(a), args, n++);
    a->length               = compute_byte_count(a);
}

int grib_accessor_class_spd_t::unpack_long(grib_accessor* a, long* val, size_t* len){
    grib_accessor_spd_t* self = (grib_accessor_spd_t*)a;
    long pos          = a->offset * 8;
    long rlen         = 0;
    long numberOfBits = 0;

    int ret = value_count(a, &rlen);
    if (ret)
        return ret;

    if (*len < rlen) {
        grib_context_log(a->context, GRIB_LOG_ERROR,
                         "Wrong size (%zu) for %s, it contains %ld values", *len, a->name, rlen);
        *len = rlen;
        return GRIB_ARRAY_TOO_SMALL;
    }

    ret = grib_get_long(grib_handle_of_accessor(a), self->numberOfBits, &numberOfBits);
    if (ret)
        return ret;
    if (numberOfBits > 64) {
        grib_context_log(a->context, GRIB_LOG_ERROR,"Invalid number of bits: %ld",numberOfBits);
        return GRIB_DECODING_ERROR;
    }

    for (long i = 0; i < rlen - 1; i++)
        val[i] = grib_decode_unsigned_long(grib_handle_of_accessor(a)->buffer->data, &pos, numberOfBits);

    val[rlen - 1] = grib_decode_signed_longb(grib_handle_of_accessor(a)->buffer->data, &pos, numberOfBits);

    *len = rlen;

    return GRIB_SUCCESS;
}

int grib_accessor_class_spd_t::pack_long(grib_accessor* a, const long* val, size_t* len){
    grib_accessor_spd_t* self = (grib_accessor_spd_t*)a;
    int ret                 = 0;
    long off                = 0;
    long numberOfBits       = 0;
    size_t buflen           = 0;
    unsigned char* buf      = NULL;
    unsigned long i         = 0;
    long rlen               = 0;

    ret = value_count(a, &rlen);
    if (ret)
        return ret;

    if (*len != rlen) {
        ret = grib_set_long(grib_handle_of_accessor(a), self->numberOfElements, (*len) - 1);
        if (ret) return ret;
    }

    ret = grib_get_long(grib_handle_of_accessor(a), self->numberOfBits, &numberOfBits);
    if (ret)
        return ret;

    buflen = compute_byte_count(a);
    buf    = (unsigned char*)grib_context_malloc_clear(a->context, buflen);

    for (i = 0; i < rlen - 1; i++) {
        grib_encode_unsigned_longb(buf, val[i], &off, numberOfBits);
    }

    grib_encode_signed_longb(buf, val[rlen - 1], &off, numberOfBits);

    grib_buffer_replace(a, buf, buflen, 1, 1);

    grib_context_free(a->context, buf);

    *len = rlen;
    return ret;
}

int grib_accessor_class_spd_t::value_count(grib_accessor* a, long* numberOfElements){
    grib_accessor_spd_t* self = (grib_accessor_spd_t*)a;
    int ret;
    *numberOfElements = 0;

    ret = grib_get_long(grib_handle_of_accessor(a), self->numberOfElements, numberOfElements);
    if (ret) {
        grib_context_log(a->context, GRIB_LOG_ERROR,
                         "%s unable to get %s to compute size", a->name, self->numberOfElements);
        return ret;
    }
    (*numberOfElements)++;

    return ret;
}

long grib_accessor_class_spd_t::byte_offset(grib_accessor* a){
    return a->offset;
}

void grib_accessor_class_spd_t::update_size(grib_accessor* a, size_t s){
    a->length = s;
}

long grib_accessor_class_spd_t::next_offset(grib_accessor* a){
    return a->byte_offset() + a->length;
}
