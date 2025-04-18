/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "G1Bitmap.h"

eccodes::accessor::G1Bitmap _grib_accessor_g1bitmap;
eccodes::Accessor* grib_accessor_g1bitmap = &_grib_accessor_g1bitmap;

namespace eccodes::accessor
{

void G1Bitmap::init(const long len, grib_arguments* arg)
{
    Bitmap::init(len, arg);
    unusedBits_ = arg->get_name(get_enclosing_handle(), 4);
}

int G1Bitmap::pack_double(const double* val, size_t* len)
{
    size_t tlen;

    unsigned char* buf = NULL;
    size_t i;
    int err  = 0;
    long pos = 0;
    // long bmaplen          = 0;
    const int bit_padding = 16;
    double miss_values    = 0;
    tlen                  = ((*len + bit_padding - 1) / bit_padding * bit_padding) / 8;

    if ((err = grib_get_double_internal(get_enclosing_handle(), missing_value_, &miss_values)) != GRIB_SUCCESS)
        return err;

    buf = (unsigned char*)grib_context_malloc_clear(context_, tlen);
    if (!buf)
        return GRIB_OUT_OF_MEMORY;
    pos = 0;
    for (i = 0; i < *len; i++) {
        if (val[i] == miss_values)
            pos++;
        else {
            // bmaplen++;
            grib_set_bit_on(buf, &pos);
        }
    }

    if ((err = grib_set_long_internal(get_enclosing_handle(), unusedBits_, tlen * 8 - *len)) != GRIB_SUCCESS)
        return err;

    err = grib_buffer_replace(this, buf, tlen, 1, 1);
    if (err) return err;

    grib_context_free(context_, buf);

    return GRIB_SUCCESS;
}

int G1Bitmap::value_count(long* count)
{
    long tlen;
    int err;

    if ((err = grib_get_long_internal(get_enclosing_handle(), unusedBits_, &tlen)) != GRIB_SUCCESS)
        grib_context_log(context_, GRIB_LOG_ERROR, "grib_accessor_bitmap.value_count : cannot get %s err=%d", unusedBits_, err);

    *count = (length_ * 8) - tlen;
    return err;
}

int G1Bitmap::unpack_bytes(unsigned char* val, size_t* len)
{
    unsigned char* buf = get_enclosing_handle()->buffer->data;
    long tlen;
    int err;
    long length = byte_count();
    long offset = byte_offset();
    if (*len < (size_t)length) {
        grib_context_log(context_, GRIB_LOG_ERROR, "Wrong size for %s it is %ld bytes long\n", name_, length);
        *len = length;
        return GRIB_ARRAY_TOO_SMALL;
    }

    if ((err = grib_get_long_internal(get_enclosing_handle(), unusedBits_, &tlen)) != GRIB_SUCCESS)
        grib_context_log(context_, GRIB_LOG_ERROR,
                         "grib_accessor_bitmap.unpack_bytes : cannot get %s err=%d", unusedBits_, err);

    length -= tlen / 8;
    memcpy(val, buf + offset, length);
    *len = length;

    return GRIB_SUCCESS;
}

}  // namespace eccodes::accessor
