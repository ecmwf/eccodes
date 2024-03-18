
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
#include "grib_accessor_class_bitmap.h"

grib_accessor_class_bitmap_t _grib_accessor_class_bitmap{"bitmap"};
grib_accessor_class* grib_accessor_class_bitmap = &_grib_accessor_class_bitmap;



void compute_size(grib_accessor* a){
    long slen         = 0;
    long off          = 0;
    grib_handle* hand = grib_handle_of_accessor(a);

    grib_accessor_bitmap_t* self = (grib_accessor_bitmap_t*)a;
    grib_get_long_internal(hand, self->offsetbsec, &off);
    grib_get_long_internal(hand, self->sLength, &slen);

    if (slen == 0) {
        grib_accessor* seclen;
        size_t size;
        /* Assume reparsing */
        Assert(hand->loader != 0);
        if (hand->loader != 0) {
            seclen = grib_find_accessor(hand, self->sLength);
            Assert(seclen);
            grib_get_block_length(seclen->parent, &size);
            slen = size;
        }
    }

    // printf("compute_size off=%ld slen=%ld a->offset=%ld\n", (long)off,(long)slen,(long)a->offset);

    a->length = off + (slen - a->offset);

    if (a->length < 0) {
        /* Assume reparsing */
        /*Assert(hand->loader != 0);*/
        a->length = 0;
    }

    Assert(a->length >= 0);
}

void grib_accessor_class_bitmap_t::init(grib_accessor* a, const long len, grib_arguments* arg){
    grib_accessor_bitmap_t* self = (grib_accessor_bitmap_t*)a;
    grib_handle* hand = grib_handle_of_accessor(a);
    int n = 0;

    self->tableReference = grib_arguments_get_name(hand, arg, n++);
    self->missing_value  = grib_arguments_get_name(hand, arg, n++);
    self->offsetbsec     = grib_arguments_get_name(hand, arg, n++);
    self->sLength        = grib_arguments_get_name(hand, arg, n++);

    compute_size(a);
}

long grib_accessor_class_bitmap_t::next_offset(grib_accessor* a){
    return a->byte_offset() + a->byte_count();}

void grib_accessor_class_bitmap_t::dump(grib_accessor* a, grib_dumper* dumper){
    long len = 0;
    char label[1024];

    a->value_count(&len);
    snprintf(label, 1024, "Bitmap of %ld values", len);
    grib_dump_bytes(dumper, a, label);
}

int grib_accessor_class_bitmap_t::unpack_long(grib_accessor* a, long* val, size_t* len){
    long pos  = a->offset * 8;
    long tlen = 0;
    const grib_handle* hand = grib_handle_of_accessor(a);

    int err = a->value_count(&tlen);    if (err)
        return err;

    if (*len < tlen) {
        grib_context_log(a->context, GRIB_LOG_ERROR, "Wrong size for %s, it contains %ld values", a->name, tlen);
        *len = tlen;
        return GRIB_ARRAY_TOO_SMALL;
    }

    for (long i = 0; i < tlen; i++) {
        val[i] = (long)grib_decode_unsigned_long(hand->buffer->data, &pos, 1);
    }
    *len = tlen;
    return GRIB_SUCCESS;
}

template <typename T>
int unpack(grib_accessor* a, T* val, size_t* len){
    static_assert(std::is_floating_point<T>::value, "Requires floating points numbers");
    long pos = a->offset * 8;
    long tlen;
    grib_handle* hand = grib_handle_of_accessor(a);

    int err = a->value_count(&tlen);    if (err)
        return err;

    if (*len < tlen) {
        grib_context_log(a->context, GRIB_LOG_ERROR, "Wrong size for %s, it contains %ld values", a->name, tlen);
        *len = tlen;
        return GRIB_ARRAY_TOO_SMALL;
    }

    for (long i = 0; i < tlen; i++) {
        val[i] = (T)grib_decode_unsigned_long(hand->buffer->data, &pos, 1);
    }
    *len = tlen;
    return GRIB_SUCCESS;
}

int grib_accessor_class_bitmap_t::unpack_double(grib_accessor* a, double* val, size_t* len){
    return unpack<double>(a, val, len);
}

int grib_accessor_class_bitmap_t::unpack_float(grib_accessor* a, float* val, size_t* len){
    return unpack<float>(a, val, len);
}

int grib_accessor_class_bitmap_t::unpack_double_element(grib_accessor* a, size_t idx, double* val){
    long pos = a->offset * 8;

    pos += idx;
    *val = (double)grib_decode_unsigned_long(grib_handle_of_accessor(a)->buffer->data, &pos, 1);

    return GRIB_SUCCESS;
}
int grib_accessor_class_bitmap_t::unpack_double_element_set(grib_accessor* a, const size_t* index_array, size_t len, double* val_array){
    for (size_t i=0; i<len; ++i) {
        unpack_double_element(a, index_array[i], val_array + i);
    }
    return GRIB_SUCCESS;
}

void grib_accessor_class_bitmap_t::update_size(grib_accessor* a, size_t s){
    a->length = s;
}

size_t grib_accessor_class_bitmap_t::string_length(grib_accessor* a){
    return a->length;
}

int grib_accessor_class_bitmap_t::unpack_string(grib_accessor* a, char* val, size_t* len){
    grib_handle* hand = grib_handle_of_accessor(a);
    const size_t l = a->length;

    if (*len < l) {
        const char* cclass_name = a->cclass->name;
        grib_context_log(a->context, GRIB_LOG_ERROR,
                         "%s: Buffer too small for %s. It is %zu bytes long (len=%zu)",
                         cclass_name, a->name, l, *len);
        *len = l;
        return GRIB_BUFFER_TOO_SMALL;
    }

    for (long i = 0; i < a->length; i++) {
        val[i] = hand->buffer->data[a->offset + i];
    }

    *len = a->length;

    return GRIB_SUCCESS;
}
