
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
#include "grib_accessor_class_g1bitmap.h"
#include "shared_functions.h"

grib_accessor_class_g1bitmap_t _grib_accessor_class_g1bitmap{"g1bitmap"};
grib_accessor_class* grib_accessor_class_g1bitmap = &_grib_accessor_class_g1bitmap;


void grib_accessor_class_g1bitmap_t::init(grib_accessor* a, const long len, grib_arguments* arg){
    grib_accessor_class_bitmap_t::init(a, len, arg);
    grib_accessor_g1bitmap_t* self = (grib_accessor_g1bitmap_t*)a;
    self->unusedBits             = grib_arguments_get_name(grib_handle_of_accessor(a), arg, 4);
}

int grib_accessor_class_g1bitmap_t::pack_double(grib_accessor* a, const double* val, size_t* len){
    grib_accessor_g1bitmap_t* self = (grib_accessor_g1bitmap_t*)a;
    size_t tlen;

    unsigned char* buf = NULL;
    size_t i;
    int err               = 0;
    long pos              = 0;
    //long bmaplen          = 0;
    const int bit_padding = 16;
    double miss_values    = 0;
    tlen                  = ((*len + bit_padding - 1) / bit_padding * bit_padding) / 8;

    if ((err = grib_get_double_internal(grib_handle_of_accessor(a), self->missing_value, &miss_values)) != GRIB_SUCCESS)
        return err;

    buf = (unsigned char*)grib_context_malloc_clear(a->context, tlen);
    if (!buf)
        return GRIB_OUT_OF_MEMORY;
    pos = 0;
    for (i = 0; i < *len; i++) {
        if (val[i] == miss_values)
            pos++;
        else {
            //bmaplen++;
            grib_set_bit_on(buf, &pos);
        }
    }

    if ((err = grib_set_long_internal(grib_handle_of_accessor(a), self->unusedBits, tlen * 8 - *len)) != GRIB_SUCCESS)
        return err;

    err = grib_buffer_replace(a, buf, tlen, 1, 1);
    if (err) return err;

    grib_context_free(a->context, buf);

    return GRIB_SUCCESS;
}

int grib_accessor_class_g1bitmap_t::value_count(grib_accessor* a, long* count){
    grib_accessor_g1bitmap_t* self = (grib_accessor_g1bitmap_t*)a;
    long tlen;
    int err;

    if ((err = grib_get_long_internal(grib_handle_of_accessor(a), self->unusedBits, &tlen)) != GRIB_SUCCESS)
        grib_context_log(a->context, GRIB_LOG_ERROR, "grib_accessor_class_bitmap.value_count : cannot get %s err=%d", self->unusedBits, err);

    *count = (a->length * 8) - tlen;
    return err;
}

int grib_accessor_class_g1bitmap_t::unpack_bytes(grib_accessor* a, unsigned char* val, size_t* len){
    unsigned char* buf           = grib_handle_of_accessor(a)->buffer->data;
    grib_accessor_g1bitmap_t* self = (grib_accessor_g1bitmap_t*)a;
    long tlen;
    int err;
    long length = a->byte_count();    long offset = a->byte_offset();
    if (*len < (size_t)length) {
        grib_context_log(a->context, GRIB_LOG_ERROR, "Wrong size for %s it is %ld bytes long\n", a->name, length);
        *len = length;
        return GRIB_ARRAY_TOO_SMALL;
    }

    if ((err = grib_get_long_internal(grib_handle_of_accessor(a), self->unusedBits, &tlen)) != GRIB_SUCCESS)
        grib_context_log(a->context, GRIB_LOG_ERROR,
                         "grib_accessor_class_bitmap.unpack_bytes : cannot get %s err=%d", self->unusedBits, err);

    length -= tlen / 8;
    memcpy(val, buf + offset, length);
    *len = length;

    return GRIB_SUCCESS;
}
