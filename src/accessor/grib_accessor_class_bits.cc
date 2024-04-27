
/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_accessor_class_bits.h"

grib_accessor_class_bits_t _grib_accessor_class_bits{"bits"};
grib_accessor_class* grib_accessor_class_bits = &_grib_accessor_class_bits;


void grib_accessor_class_bits_t::init(grib_accessor* a, const long l, grib_arguments* c){
    grib_accessor_class_gen_t::init(a, l, c);
    grib_accessor_bits_t* self = (grib_accessor_bits_t*)a;
    grib_handle* hand        = grib_handle_of_accessor(a);
    grib_expression* e       = NULL;
    int n                    = 0;

    self->argument = grib_arguments_get_name(hand, c, n++);
    self->start    = grib_arguments_get_long(hand, c, n++);
    self->len      = grib_arguments_get_long(hand, c, n++);
    e              = grib_arguments_get_expression(hand, c, n++);
    if (e) {
        grib_expression_evaluate_double(hand, e, &(self->referenceValue));
        self->referenceValuePresent = 1;
    }
    else {
        self->referenceValuePresent = 0;
    }
    self->scale = 1;
    if (self->referenceValuePresent) {
        self->scale = grib_arguments_get_double(hand, c, n++);
    }

    Assert(self->len <= sizeof(long) * 8);

    a->length = 0;
}

int grib_accessor_class_bits_t::unpack_long(grib_accessor* a, long* val, size_t* len){
    grib_accessor_bits_t* self = (grib_accessor_bits_t*)a;
    grib_accessor* x         = NULL;
    unsigned char* p         = NULL;
    grib_handle* h           = grib_handle_of_accessor(a);
    long start, length;
    int ret = 0;

    if (*len < 1)
        return GRIB_WRONG_ARRAY_SIZE;

    start  = self->start;
    length = self->len;

    x = grib_find_accessor(grib_handle_of_accessor(a), self->argument);
    if (!x)
        return GRIB_NOT_FOUND;

    p    = h->buffer->data + x->byte_offset();    *val = grib_decode_unsigned_long(p, &start, length);

    *len = 1;

    return ret;
}

int grib_accessor_class_bits_t::unpack_double(grib_accessor* a, double* val, size_t* len){
    grib_accessor_bits_t* self = (grib_accessor_bits_t*)a;
    grib_accessor* x         = NULL;
    unsigned char* p         = NULL;
    grib_handle* h           = grib_handle_of_accessor(a);
    long start, length;
    int ret = 0;

    if (*len < 1)
        return GRIB_WRONG_ARRAY_SIZE;

    start  = self->start;
    length = self->len;

    x = grib_find_accessor(grib_handle_of_accessor(a), self->argument);
    if (!x)
        return GRIB_NOT_FOUND;

    p    = h->buffer->data + x->byte_offset();    *val = grib_decode_unsigned_long(p, &start, length);

    *val = ((long)*val + self->referenceValue) / self->scale;

    *len = 1;

    return ret;
}

int grib_accessor_class_bits_t::pack_double(grib_accessor* a, const double* val, size_t* len){
    grib_accessor_bits_t* self = (grib_accessor_bits_t*)a;
    grib_accessor* x         = NULL;
    grib_handle* h           = grib_handle_of_accessor(a);
    unsigned char* p         = NULL;
    long start, length, lval;

    if (*len != 1)
        return GRIB_WRONG_ARRAY_SIZE;

    start  = self->start;
    length = self->len;

    x = grib_find_accessor(grib_handle_of_accessor(a), self->argument);
    if (!x)
        return GRIB_NOT_FOUND;

    p = h->buffer->data + x->byte_offset();
    lval = round(*val * self->scale) - self->referenceValue;
    return grib_encode_unsigned_longb(p, lval, &start, length);
}

int grib_accessor_class_bits_t::pack_long(grib_accessor* a, const long* val, size_t* len){
    grib_accessor_bits_t* self = (grib_accessor_bits_t*)a;
    grib_accessor* x         = NULL;
    grib_handle* h           = grib_handle_of_accessor(a);
    unsigned char* p         = NULL;
    long start, length, maxval;

    if (*len != 1)
        return GRIB_WRONG_ARRAY_SIZE;

    if (get_native_type(a) == GRIB_TYPE_DOUBLE) {
        /* ECC-402 */
        const double dVal = (double)(*val);
        return pack_double(a, &dVal, len);
    }

    start  = self->start;
    length = self->len;

    x = grib_find_accessor(grib_handle_of_accessor(a), self->argument);
    if (!x)
        return GRIB_NOT_FOUND;

    /* Check the input value */
    if (*val < 0) {
        grib_context_log(h->context, GRIB_LOG_ERROR, "key=%s: value cannot be negative", a->name);
        return GRIB_ENCODING_ERROR;
    }

#ifdef DEBUG
    {
        const long numbits = (x->length)*8;
        if (start + length > numbits) {
            grib_context_log(h->context, GRIB_LOG_ERROR,
                    "grib_accessor_class_bits::pack_long: key=%s (x=%s): "
                    "Invalid start/length. x->length=%ld, start=%ld, length=%ld",
                    a->name, x->name, numbits, start, length);
            return GRIB_ENCODING_ERROR;
        }
    }
#endif

    maxval = (1 << length) - 1;
    if (*val > maxval) {
        grib_context_log(h->context, GRIB_LOG_ERROR,
                         "key=%s: Trying to encode value of %ld but the maximum allowable value is %ld (number of bits=%ld)",
                         a->name, *val, maxval, length);
        return GRIB_ENCODING_ERROR;
    }

    p = h->buffer->data + x->byte_offset();    return grib_encode_unsigned_longb(p, *val, &start, length);
}

int grib_accessor_class_bits_t::get_native_type(grib_accessor* a){
    int type                 = GRIB_TYPE_BYTES;
    grib_accessor_bits_t* self = (grib_accessor_bits_t*)a;

    if (a->flags & GRIB_ACCESSOR_FLAG_STRING_TYPE)
        type = GRIB_TYPE_STRING;

    if (a->flags & GRIB_ACCESSOR_FLAG_LONG_TYPE)
        type = GRIB_TYPE_LONG;

    if (self->referenceValuePresent)
        type = GRIB_TYPE_DOUBLE;

    return type;
}

int grib_accessor_class_bits_t::unpack_string(grib_accessor* a, char* v, size_t* len){
    int ret                    = 0;
    double dval                = 0;
    long lval                  = 0;
    size_t llen                = 1;

    switch (get_native_type(a)) {
        case GRIB_TYPE_LONG:
            ret = unpack_long(a, &lval, &llen);
            snprintf(v, 64, "%ld", lval);
            *len = strlen(v);
            break;

        case GRIB_TYPE_DOUBLE:
            ret = unpack_double(a, &dval, &llen);
            snprintf(v, 64, "%g", dval);
            *len = strlen(v);
            break;

        default:
            ret = grib_accessor_class_gen_t::unpack_string(a, v, len);
    }
    return ret;
}

long grib_accessor_class_bits_t::byte_count(grib_accessor* a){
    grib_context_log(a->context, GRIB_LOG_DEBUG, "byte_count of %s = %ld", a->name, a->length);
    return a->length;
}

int grib_accessor_class_bits_t::unpack_bytes(grib_accessor* a, unsigned char* buffer, size_t* len){
    if (*len < a->length) {
        *len = a->length;
        return GRIB_ARRAY_TOO_SMALL;
    }
    *len = a->length;

    memcpy(buffer, grib_handle_of_accessor(a)->buffer->data + a->offset, *len);

    return GRIB_SUCCESS;
}
