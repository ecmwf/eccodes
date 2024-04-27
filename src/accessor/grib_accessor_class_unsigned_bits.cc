
/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_accessor_class_unsigned_bits.h"

grib_accessor_class_unsigned_bits_t _grib_accessor_class_unsigned_bits{"unsigned_bits"};
grib_accessor_class* grib_accessor_class_unsigned_bits = &_grib_accessor_class_unsigned_bits;



static long compute_byte_count(grib_accessor* a){
    grib_accessor_unsigned_bits_t* self = (grib_accessor_unsigned_bits_t*)a;
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


void grib_accessor_class_unsigned_bits_t::init(grib_accessor* a, const long len, grib_arguments* args){
    grib_accessor_class_long_t::init(a, len, args);
    grib_accessor_unsigned_bits_t* self = (grib_accessor_unsigned_bits_t*)a;
    int n                             = 0;
    self->numberOfBits                = grib_arguments_get_name(grib_handle_of_accessor(a), args, n++);
    self->numberOfElements            = grib_arguments_get_name(grib_handle_of_accessor(a), args, n++);
    a->length                         = compute_byte_count(a);
}

int grib_accessor_class_unsigned_bits_t::unpack_long(grib_accessor* a, long* val, size_t* len){
    grib_accessor_unsigned_bits_t* self = (grib_accessor_unsigned_bits_t*)a;
    int ret                           = 0;
    long pos                          = a->offset * 8;
    long rlen                         = 0;
    long numberOfBits                 = 0;

    ret = value_count(a, &rlen);
    if (ret)
        return ret;

    if (*len < rlen) {
        grib_context_log(a->context, GRIB_LOG_ERROR,
                         "Wrong size (%ld) for %s, it contains %ld values", *len, a->name, rlen);
        *len = rlen;
        return GRIB_ARRAY_TOO_SMALL;
    }

    ret = grib_get_long(grib_handle_of_accessor(a), self->numberOfBits, &numberOfBits);
    if (ret)
        return ret;
    if (numberOfBits == 0) {
        int i;
        for (i = 0; i < rlen; i++)
            val[i] = 0;
        return GRIB_SUCCESS;
    }

    grib_decode_long_array(grib_handle_of_accessor(a)->buffer->data, &pos, numberOfBits, rlen, val);

    *len = rlen;

    return GRIB_SUCCESS;
}

int grib_accessor_class_unsigned_bits_t::pack_long(grib_accessor* a, const long* val, size_t* len){
    grib_accessor_unsigned_bits_t* self = (grib_accessor_unsigned_bits_t*)a;
    int ret                           = 0;
    long off                          = 0;
    long numberOfBits                 = 0;
    size_t buflen                     = 0;
    unsigned char* buf                = NULL;
    unsigned long i                   = 0;
    long rlen                         = 0;
    ret                               = value_count(a, &rlen);
    if (ret) return ret;

    /*
    if(*len < rlen)
    {
        grib_context_log(a->context, GRIB_LOG_ERROR,
            "Wrong size for %s it contains %d values ", a->name , rlen );
        return GRIB_ARRAY_TOO_SMALL;
    }
     */
    if (*len != rlen)
        ret = grib_set_long(grib_handle_of_accessor(a), self->numberOfElements, *len);
    if (ret) return ret;

    ret = grib_get_long(grib_handle_of_accessor(a), self->numberOfBits, &numberOfBits);
    if (ret) return ret;
    if (numberOfBits == 0) {
        grib_buffer_replace(a, NULL, 0, 1, 1);
        return GRIB_SUCCESS;
    }

    buflen = compute_byte_count(a);
    buf    = (unsigned char*)grib_context_malloc_clear(a->context, buflen + sizeof(long));

    for (i = 0; i < *len; i++)
        grib_encode_unsigned_longb(buf, val[i], &off, numberOfBits);

    grib_buffer_replace(a, buf, buflen, 1, 1);

    grib_context_free(a->context, buf);

    return ret;
}

long grib_accessor_class_unsigned_bits_t::byte_count(grib_accessor* a){
    return a->length;
}

int grib_accessor_class_unsigned_bits_t::value_count(grib_accessor* a, long* numberOfElements){
    grib_accessor_unsigned_bits_t* self = (grib_accessor_unsigned_bits_t*)a;
    int ret;
    *numberOfElements = 0;

    ret = grib_get_long(grib_handle_of_accessor(a), self->numberOfElements, numberOfElements);
    if (ret) {
        grib_context_log(a->context, GRIB_LOG_ERROR,
                         "%s unable to get %s to compute size", a->name, self->numberOfElements);
    }

    return ret;
}

long grib_accessor_class_unsigned_bits_t::byte_offset(grib_accessor* a){
    return a->offset;
}

void grib_accessor_class_unsigned_bits_t::update_size(grib_accessor* a, size_t s){
    a->length = s;
}

long grib_accessor_class_unsigned_bits_t::next_offset(grib_accessor* a){
    return a->byte_offset() + a->length;
}
