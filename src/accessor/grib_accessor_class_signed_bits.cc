
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
#include "grib_accessor_class_signed_bits.h"

grib_accessor_class_signed_bits_t _grib_accessor_class_signed_bits{"signed_bits"};
grib_accessor_class* grib_accessor_class_signed_bits = &_grib_accessor_class_signed_bits;


long grib_accessor_class_signed_bits_t::byte_count(grib_accessor* a){
    return a->length;
}

static long compute_byte_count(grib_accessor* a){
    grib_accessor_signed_bits_t* self = (grib_accessor_signed_bits_t*)a;
    long numberOfBits;
    long numberOfElements;
    int ret = 0;

    ret = grib_get_long(grib_handle_of_accessor(a), self->numberOfBits, &numberOfBits);
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

    return (numberOfBits * numberOfElements + 7) / 8;
}

void grib_accessor_class_signed_bits_t::init(grib_accessor* a, const long len, grib_arguments* args){
    grib_accessor_class_long_t::init(a, len, args);
    grib_accessor_signed_bits_t* self = (grib_accessor_signed_bits_t*)a;
    int n                           = 0;
    self->numberOfBits              = grib_arguments_get_name(grib_handle_of_accessor(a), args, n++);
    self->numberOfElements          = grib_arguments_get_name(grib_handle_of_accessor(a), args, n++);
    a->length                       = compute_byte_count(a);
}

int grib_accessor_class_signed_bits_t::unpack_long(grib_accessor* a, long* val, size_t* len){
    return GRIB_NOT_IMPLEMENTED;
#if 0
    grib_accessor_signed_bits_t* self = (grib_accessor_signed_bits_t*)a;
    int i;
    int ret           = 0;
    long pos          = a->offset * 8;
    long rlen         = 0;
    long numberOfBits = 0;

    ret = value_count(a, &rlen);
    if (ret)
        return ret;

    if (*len < rlen) {
        grib_context_log(a->context, GRIB_LOG_ERROR,
                         "Wrong size (%ld) for %s it contains %ld values", *len, a->name, rlen);
        *len = 0;
        return GRIB_ARRAY_TOO_SMALL;
    }

    ret = grib_get_long(grib_handle_of_accessor(a), self->numberOfBits, &numberOfBits);
    if (ret)
        return ret;

    if (numberOfBits == 0) {
        for (i = 0; i < rlen; i++)
            val[i] = 0;
        return GRIB_SUCCESS;
    }

    for (i = 0; i < rlen; i++)
        val[i] = grib_decode_signed_longb(grib_handle_of_accessor(a)->buffer->data, &pos, numberOfBits);

    *len = rlen;

    return GRIB_SUCCESS;
#endif
}

int grib_accessor_class_signed_bits_t::pack_long(grib_accessor* a, const long* val, size_t* len){
    return GRIB_NOT_IMPLEMENTED;
#if 0
    grib_accessor_signed_bits_t* self = (grib_accessor_signed_bits_t*)a;
    int ret                         = 0;
    long off                        = 0;
    long numberOfBits               = 0;
    size_t buflen                   = 0;
    unsigned char* buf              = NULL;
    unsigned long i                 = 0;
    unsigned long rlen              = 0;
    long count                      = 0;

    ret = value_count(a, &count);
    if (ret)
        return ret;
    rlen = count;
    if (*len != rlen) {
        ret = grib_set_long(grib_handle_of_accessor(a), self->numberOfElements, rlen);
        if (ret)
            return ret;
    }

    ret = grib_get_long(grib_handle_of_accessor(a), self->numberOfBits, &numberOfBits);
    if (ret)
        return ret;

    buflen = compute_byte_count(a);
    buf    = (unsigned char*)grib_context_malloc_clear(a->context, buflen + sizeof(long));

    for (i = 0; i < rlen; i++)
        grib_encode_signed_longb(buf, val[i], &off, numberOfBits);

    grib_buffer_replace(a, buf, buflen, 1, 1);

    grib_context_free(a->context, buf);

    return ret;
#endif
}

int grib_accessor_class_signed_bits_t::value_count(grib_accessor* a, long* numberOfElements){
    grib_accessor_signed_bits_t* self = (grib_accessor_signed_bits_t*)a;
    *numberOfElements               = 0;

    return grib_get_long(grib_handle_of_accessor(a), self->numberOfElements, numberOfElements);
}

long grib_accessor_class_signed_bits_t::byte_offset(grib_accessor* a){
    return a->offset;
}

void grib_accessor_class_signed_bits_t::update_size(grib_accessor* a, size_t s){
    a->length = s;
}

long grib_accessor_class_signed_bits_t::next_offset(grib_accessor* a){
    return a->byte_offset() + a->byte_count();
}
