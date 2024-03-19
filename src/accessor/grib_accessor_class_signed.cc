
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
#include "grib_accessor_class_signed.h"

grib_accessor_class_signed_t _grib_accessor_class_signed{"signed"};
grib_accessor_class* grib_accessor_class_signed = &_grib_accessor_class_signed;


void grib_accessor_class_signed_t::init(grib_accessor* a, const long len, grib_arguments* arg){
    grib_accessor_class_long_t::init(a, len, arg);
    grib_accessor_signed_t* self = (grib_accessor_signed_t*)a;
    long count                 = 0;

    self->arg = arg;
    a->value_count(&count);    a->length    = len * count;
    self->nbytes = len;
    Assert(a->length >= 0);
}

void grib_accessor_class_signed_t::dump(grib_accessor* a, grib_dumper* dumper){
    long rlen = 0;
    a->value_count(&rlen);    if (rlen == 1)
        grib_dump_long(dumper, a, NULL);
    else
        grib_dump_values(dumper, a);
}

static const long ones[] = {
    0,
    -0x7f,
    -0x7fff,
    -0x7fffff,
    -0x7fffffff,
};

int grib_accessor_class_signed_t::unpack_long(grib_accessor* a, long* val, size_t* len){
    grib_accessor_signed_t* self = (grib_accessor_signed_t*)a;
    unsigned long rlen         = 0;
    int err                    = 0;
    long count                 = 0;
    unsigned long i            = 0;
    grib_handle* hand          = grib_handle_of_accessor(a);
    long pos                   = a->offset;
    long missing               = 0;

    err = a->value_count(&count);    if (err)
        return err;
    rlen = count;

    if (*len < rlen) {
        grib_context_log(a->context, GRIB_LOG_ERROR, "Wrong size for %s, it contains %lu values", a->name, rlen);
        *len = 0;
        return GRIB_ARRAY_TOO_SMALL;
    }

    if (a->flags & GRIB_ACCESSOR_FLAG_CAN_BE_MISSING) {
        Assert(self->nbytes <= 4);
        missing = ones[self->nbytes];
    }

    for (i = 0; i < rlen; i++) {
        val[i] = (long)grib_decode_signed_long(hand->buffer->data, pos, self->nbytes);
        if (missing)
            if (val[i] == missing)
                val[i] = GRIB_MISSING_LONG;
        pos += self->nbytes;
    }

    *len = rlen;
    return GRIB_SUCCESS;
}

int grib_accessor_class_signed_t::pack_long(grib_accessor* a, const long* val, size_t* len){
    grib_accessor_signed_t* self = (grib_accessor_signed_t*)a;
    int ret                    = 0;
    long off                   = 0;
    unsigned long rlen         = 0;
    int err                    = 0;
    long count                 = 0;
    size_t buflen              = 0;
    unsigned char* buf         = NULL;
    unsigned long i            = 0;
    long missing               = 0;

    err = a->value_count(&count);    if (err)
        return err;
    rlen = count;

    if (*len < 1) {
        grib_context_log(a->context, GRIB_LOG_ERROR, "Wrong size for %s, it contains %d values", a->name, 1);
        len[0] = 0;
        return GRIB_ARRAY_TOO_SMALL;
    }

    if (a->flags & GRIB_ACCESSOR_FLAG_CAN_BE_MISSING) {
        Assert(self->nbytes <= 4);
        missing = ones[self->nbytes];
    }

    if (rlen == 1) {
        long v = val[0];
        if (missing) {
            if (v == GRIB_MISSING_LONG)
                v = missing;
        } else {
            // ECC-1605: Check overflow/underflow
            const int nbits = self->nbytes * 8;
            const long minval = -(1L << (nbits-1)) + 1;
            const long maxval = (1L << (nbits-1)) - 1;
            //printf("  key=%s: v=%ld  (minval=%ld  maxval=%ld)\n", a->name, v, minval, maxval);
            if (v > maxval || v < minval) {
                grib_context_log(a->context, GRIB_LOG_ERROR,
                     "Key \"%s\": Trying to encode value of %ld but the allowable range is %ld to %ld (number of bits=%d)",
                     a->name, v, minval, maxval, nbits);
                return GRIB_ENCODING_ERROR;
            }
        }

        off = a->offset;
        ret = grib_encode_signed_long(grib_handle_of_accessor(a)->buffer->data, v, off, a->length);
        if (ret == GRIB_SUCCESS)
            len[0] = 1;
        if (*len > 1)
            grib_context_log(a->context, GRIB_LOG_WARNING, "grib_accessor_signed_t : Trying to pack %d values in a scalar %s, packing first value", *len, a->name);
        len[0] = 1;
        return ret;
    }

    /* TODO: We assume that there are no missing values if there are more that 1 value */

    buflen = *len * a->length;

    buf = (unsigned char*)grib_context_malloc(a->context, buflen);

    for (i = 0; i < *len; i++) {
        grib_encode_signed_long(buf, val[i], off, a->length);
        off += a->length;
    }
    ret = grib_set_long_internal(grib_handle_of_accessor(a), grib_arguments_get_name(a->parent->h, self->arg, 0), *len);

    if (ret == GRIB_SUCCESS)
        grib_buffer_replace(a, buf, buflen, 1, 1);
    else
        *len = 0;

    grib_context_free(a->context, buf);
    return ret;
}

long grib_accessor_class_signed_t::byte_count(grib_accessor* a){
    return a->length;
}

int grib_accessor_class_signed_t::value_count(grib_accessor* a, long* len){
    grib_accessor_signed_t* self = (grib_accessor_signed_t*)a;
    *len                       = 0;
    if (!self->arg) {
        *len = 1;
        return 0;
    }
    return grib_get_long_internal(grib_handle_of_accessor(a), grib_arguments_get_name(a->parent->h, self->arg, 0), len);
}

long grib_accessor_class_signed_t::byte_offset(grib_accessor* a){
    return a->offset;
}

void grib_accessor_class_signed_t::update_size(grib_accessor* a, size_t s){
    a->length = s;
    Assert(a->length >= 0);
}

long grib_accessor_class_signed_t::next_offset(grib_accessor* a){
    return a->byte_offset() + a->byte_count();}

int grib_accessor_class_signed_t::is_missing(grib_accessor* a){
    int i                = 0;
    unsigned char ff     = 0xff;
    unsigned long offset = a->offset;
    grib_handle* hand    = grib_handle_of_accessor(a);

    if (a->length == 0) {
        Assert(a->vvalue != NULL);
        return a->vvalue->missing;
    }

    for (i = 0; i < a->length; i++) {
        if (hand->buffer->data[offset] != ff)
            return 0;
        offset++;
    }

    return 1;
}
