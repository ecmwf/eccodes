
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
#include "grib_accessor_class_raw.h"

grib_accessor_class_raw_t _grib_accessor_class_raw{"raw"};
grib_accessor_class* grib_accessor_class_raw = &_grib_accessor_class_raw;


void grib_accessor_class_raw_t::init(grib_accessor* a, const long len, grib_arguments* arg){
    int n   = 0;
    int err = 0;
    long sectionLength;
    grib_accessor_raw_t* self = (grib_accessor_raw_t*)a;
    grib_expression* e;
    grib_handle* hand = grib_handle_of_accessor(a);

    a->length           = 0;
    self->totalLength   = grib_arguments_get_name(hand, arg, n++);
    self->sectionLength = grib_arguments_get_name(hand, arg, n++);

    e   = grib_arguments_get_expression(hand, arg, n++);
    err = grib_expression_evaluate_long(hand, e, &(self->relativeOffset));
    if (err)
        grib_context_log(hand->context, GRIB_LOG_ERROR, "Unable to evaluate relativeOffset");

    grib_get_long(hand, self->sectionLength, &sectionLength);

    a->length = sectionLength - self->relativeOffset;
    if (a->length < 0)
        a->length = 0;

    /* Assert(a->length>=0); */
}

int grib_accessor_class_raw_t::get_native_type(grib_accessor* a){
    return GRIB_TYPE_BYTES;
}


int grib_accessor_class_raw_t::compare(grib_accessor* a, grib_accessor* b){
    int retval = GRIB_SUCCESS;

    size_t alen = (size_t)a->byte_count();    size_t blen = (size_t)b->byte_count();
    if (alen != blen)
        return GRIB_COUNT_MISMATCH;

    return retval;
}

long grib_accessor_class_raw_t::byte_count(grib_accessor* a){
    return a->length;
}

int grib_accessor_class_raw_t::value_count(grib_accessor* a, long* len){
    *len = a->length;
    return 0;
}

int grib_accessor_class_raw_t::unpack_bytes(grib_accessor* a, unsigned char* buffer, size_t* len){
    if (*len < a->length) {
        *len = a->length;
        return GRIB_ARRAY_TOO_SMALL;
    }
    *len = a->length;

    memcpy(buffer, grib_handle_of_accessor(a)->buffer->data + a->offset, *len);

    return GRIB_SUCCESS;
}

void grib_accessor_class_raw_t::update_size(grib_accessor* a, size_t s){
    grib_context_log(a->context, GRIB_LOG_DEBUG, "updating size of %s old %ld new %ld", a->name, a->length, s);
    a->length = s;
    Assert(a->length >= 0);
}

void accessor_raw_set_length(grib_accessor* a, size_t len)
{
    a->length = len;
}

long accessor_raw_get_offset(grib_accessor* a)
{
    return a->offset;
}

int grib_accessor_class_raw_t::pack_bytes(grib_accessor* a, const unsigned char* val, size_t* len){
    size_t length = *len;
    long totalLength;
    long sectionLength;
    grib_handle* h          = grib_handle_of_accessor(a);
    grib_accessor_raw_t* self = (grib_accessor_raw_t*)a;
    long dlen               = length - a->length;

    grib_get_long(h, self->totalLength, &totalLength);
    totalLength += dlen;
    grib_get_long(h, self->sectionLength, &sectionLength);
    sectionLength += dlen;

    grib_buffer_replace(a, val, length, 1, 1);

    grib_set_long(h, self->totalLength, totalLength);
    grib_set_long(h, self->sectionLength, sectionLength);
    a->length = length;

    return GRIB_SUCCESS;
}
