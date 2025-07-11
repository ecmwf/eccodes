/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "Bitmap.h"

eccodes::accessor::Bitmap _grib_accessor_bitmap;
eccodes::Accessor* grib_accessor_bitmap = &_grib_accessor_bitmap;

namespace eccodes::accessor
{

void Bitmap::compute_size()
{
    long slen = 0;
    long off = 0;
    grib_handle* hand = get_enclosing_handle();

    grib_get_long_internal(hand, offsetbsec_, &off);
    grib_get_long_internal(hand, sLength_, &slen);

    if (slen == 0) {
        grib_accessor* seclen;
        size_t size;
        /* Assume reparsing */
        ECCODES_ASSERT(hand->loader != 0);
        if (hand->loader != 0) {
            seclen = grib_find_accessor(hand, sLength_);
            ECCODES_ASSERT(seclen);
            grib_get_block_length(seclen->parent_, &size);
            slen = size;
        }
    }

    // printf("compute_size off=%ld slen=%ld a->offset_=%ld\n", (long)off,(long)slen,(long)offset_ );

    length_ = off + (slen - offset_);

    if (length_ < 0) {
        /* Assume reparsing */
        /*ECCODES_ASSERT(hand->loader != 0);*/
        length_ = 0;
    }

    ECCODES_ASSERT(length_ >= 0);
}

void Bitmap::init(const long len, grib_arguments* arg)
{
    Bytes::init(len, arg);
    grib_handle* hand = get_enclosing_handle();
    int n             = 0;

    tableReference_ = arg->get_name(hand, n++);
    missing_value_  = arg->get_name(hand, n++);
    offsetbsec_     = arg->get_name(hand, n++);
    sLength_        = arg->get_name(hand, n++);

    compute_size();
}

long Bitmap::next_offset()
{
    return byte_offset() + byte_count();
}

void Bitmap::dump(eccodes::Dumper* dumper)
{
    long len = 0;
    char label[1024] = {0,};

    value_count(&len);
    snprintf(label, sizeof(label), "Bitmap of %ld values", len);
    dumper->dump_bytes(this, label);
}

int Bitmap::unpack_long(long* val, size_t* len)
{
    long pos                = offset_ * 8;
    long tlen               = 0;
    const grib_handle* hand = get_enclosing_handle();

    int err = value_count(&tlen);
    if (err)
        return err;

    if (*len < tlen) {
        grib_context_log(context_, GRIB_LOG_ERROR, "Wrong size for %s, it contains %ld values", name_, tlen);
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
static int unpack(grib_accessor* a, T* val, size_t* len)
{
    static_assert(std::is_floating_point<T>::value, "Requires floating points numbers");
    long pos = a->offset_ * 8;
    long tlen;
    grib_handle* hand = a->get_enclosing_handle();

    int err = a->value_count(&tlen);
    if (err)
        return err;

    if (*len < tlen) {
        grib_context_log(a->context_, GRIB_LOG_ERROR, "Wrong size for %s, it contains %ld values", a->name_, tlen);
        *len = tlen;
        return GRIB_ARRAY_TOO_SMALL;
    }

    for (long i = 0; i < tlen; i++) {
        val[i] = (T)grib_decode_unsigned_long(hand->buffer->data, &pos, 1);
    }
    *len = tlen;
    return GRIB_SUCCESS;
}

int Bitmap::unpack_double(double* val, size_t* len)
{
    return unpack<double>(this, val, len);
}

int Bitmap::unpack_float(float* val, size_t* len)
{
    return unpack<float>(this, val, len);
}

int Bitmap::unpack_double_element(size_t idx, double* val)
{
    long pos = offset_ * 8;

    pos += idx;
    *val = (double)grib_decode_unsigned_long(this->get_enclosing_handle()->buffer->data, &pos, 1);

    return GRIB_SUCCESS;
}
int Bitmap::unpack_double_element_set(const size_t* index_array, size_t len, double* val_array)
{
    for (size_t i = 0; i < len; ++i) {
        unpack_double_element(index_array[i], val_array + i);
    }
    return GRIB_SUCCESS;
}

void Bitmap::update_size(size_t s)
{
    length_ = s;
}

size_t Bitmap::string_length()
{
    return length_;
}

int Bitmap::unpack_string(char* val, size_t* len)
{
    grib_handle* hand = this->get_enclosing_handle();
    const size_t l = length_;

    if (*len < l) {
        grib_context_log(context_, GRIB_LOG_ERROR,
                         "%s: Buffer too small for %s. It is %zu bytes long (len=%zu)",
                         class_name_, name_, l, *len);
        *len = l;
        return GRIB_BUFFER_TOO_SMALL;
    }

    for (long i = 0; i < length_; i++) {
        val[i] = hand->buffer->data[offset_ + i];
    }

    *len = length_;

    return GRIB_SUCCESS;
}

}  // namespace eccodes::accessor
