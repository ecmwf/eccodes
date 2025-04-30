/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "Spd.h"

eccodes::AccessorBuilder<eccodes::accessor::Spd> _grib_accessor_spd_builder{};

namespace eccodes::accessor
{

long Spd::byte_count()
{
    return length_;
}

long Spd::compute_byte_count()
{
    long numberOfBits         = 0;
    long numberOfElements     = 0;

    int ret = grib_get_long(get_enclosing_handle(), numberOfBits_, &numberOfBits);
    if (ret) {
        grib_context_log(context_, GRIB_LOG_ERROR,
                         "%s unable to get %s to compute size", name_, numberOfBits_);
        return 0;
    }

    ret = grib_get_long(get_enclosing_handle(), numberOfElements_, &numberOfElements);
    if (ret) {
        grib_context_log(context_, GRIB_LOG_ERROR,
                         "%s unable to get %s to compute size", name_, numberOfElements_);
        return 0;
    }
    numberOfElements++;

    return (numberOfBits * numberOfElements + 7) / 8;
}

void Spd::init(const long len, grib_arguments* args)
{
    Long::init(len, args);
    int n             = 0;
    numberOfBits_     = args->get_name(get_enclosing_handle(), n++);
    numberOfElements_ = args->get_name(get_enclosing_handle(), n++);
    length_           = compute_byte_count();
}

int Spd::unpack_long(long* val, size_t* len)
{
    long pos          = offset_ * 8;
    long rlen         = 0;
    long numberOfBits = 0;

    int ret = value_count(&rlen);
    if (ret)
        return ret;

    if (*len < rlen) {
        grib_context_log(context_, GRIB_LOG_ERROR,
                         "Wrong size (%zu) for %s, it contains %ld values", *len, name_, rlen);
        *len = rlen;
        return GRIB_ARRAY_TOO_SMALL;
    }

    ret = grib_get_long(get_enclosing_handle(), numberOfBits_, &numberOfBits);
    if (ret)
        return ret;
    if (numberOfBits > 64) {
        grib_context_log(context_, GRIB_LOG_ERROR, "Invalid number of bits: %ld", numberOfBits);
        return GRIB_DECODING_ERROR;
    }

    for (long i = 0; i < rlen - 1; i++)
        val[i] = grib_decode_unsigned_long(get_enclosing_handle()->buffer->data, &pos, numberOfBits);

    val[rlen - 1] = grib_decode_signed_longb(get_enclosing_handle()->buffer->data, &pos, numberOfBits);

    *len = rlen;

    return GRIB_SUCCESS;
}

int Spd::pack_long(const long* val, size_t* len)
{
    int ret            = 0;
    long off           = 0;
    long numberOfBits  = 0;
    size_t buflen      = 0;
    unsigned char* buf = NULL;
    unsigned long i    = 0;
    long rlen          = 0;

    ret = value_count(&rlen);
    if (ret)
        return ret;

    if (*len != rlen) {
        ret = grib_set_long(get_enclosing_handle(), numberOfElements_, (*len) - 1);
        if (ret) return ret;
    }

    ret = grib_get_long(get_enclosing_handle(), numberOfBits_, &numberOfBits);
    if (ret)
        return ret;

    buflen = compute_byte_count();
    buf    = (unsigned char*)grib_context_malloc_clear(context_, buflen);

    for (i = 0; i < rlen - 1; i++) {
        grib_encode_unsigned_longb(buf, val[i], &off, numberOfBits);
    }

    grib_encode_signed_longb(buf, val[rlen - 1], &off, numberOfBits);

    grib_buffer_replace(this, buf, buflen, 1, 1);

    grib_context_free(context_, buf);

    *len = rlen;
    return ret;
}

int Spd::value_count(long* numberOfElements)
{
    int ret;
    *numberOfElements = 0;

    ret = grib_get_long(get_enclosing_handle(), numberOfElements_, numberOfElements);
    if (ret) {
        grib_context_log(context_, GRIB_LOG_ERROR,
                         "%s unable to get %s to compute size", name_, numberOfElements_);
        return ret;
    }
    (*numberOfElements)++;

    return ret;
}

long Spd::byte_offset()
{
    return offset_;
}

void Spd::update_size(size_t s)
{
    length_ = s;
}

long Spd::next_offset()
{
    return byte_offset() + length_;
}

}  // namespace eccodes::accessor
