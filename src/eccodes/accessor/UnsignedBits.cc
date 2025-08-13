/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "UnsignedBits.h"

eccodes::accessor::UnsignedBits _grib_accessor_unsigned_bits;
eccodes::Accessor* grib_accessor_unsigned_bits = &_grib_accessor_unsigned_bits;

namespace eccodes::accessor
{

long UnsignedBits::compute_byte_count()
{
    long numberOfBits;
    long numberOfElements;

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

    return (numberOfBits * numberOfElements + 7) / 8;
}

void UnsignedBits::init(const long len, grib_arguments* args)
{
    Long::init(len, args);
    int n             = 0;
    numberOfBits_     = args->get_name(get_enclosing_handle(), n++);
    numberOfElements_ = args->get_name(get_enclosing_handle(), n++);
    length_           = compute_byte_count();
}

int UnsignedBits::unpack_long(long* val, size_t* len)
{
    long pos          = offset_ * 8;
    long rlen         = 0;
    long numberOfBits = 0;

    int ret = value_count(&rlen);
    if (ret)
        return ret;

    if (*len < rlen) {
        grib_context_log(context_, GRIB_LOG_ERROR,
                         "Wrong size (%ld) for %s, it contains %ld values", *len, name_, rlen);
        *len = rlen;
        return GRIB_ARRAY_TOO_SMALL;
    }

    ret = grib_get_long(get_enclosing_handle(), numberOfBits_, &numberOfBits);
    if (ret)
        return ret;
    if (numberOfBits == 0) {
        int i;
        for (i = 0; i < rlen; i++)
            val[i] = 0;
        return GRIB_SUCCESS;
    }

    grib_decode_long_array(this->get_enclosing_handle()->buffer->data, &pos, numberOfBits, rlen, val);

    *len = rlen;

    return GRIB_SUCCESS;
}

int UnsignedBits::pack_long(const long* val, size_t* len)
{
    int ret            = 0;
    long off           = 0;
    long numberOfBits  = 0;
    size_t buflen      = 0;
    unsigned char* buf = NULL;
    unsigned long i    = 0;
    long rlen          = 0;
    ret                = value_count(&rlen);
    if (ret) return ret;

    /*
    if(*len < rlen)
    {
        grib_context_log(context_ , GRIB_LOG_ERROR,
            "Wrong size for %s it contains %d values ", name_ , rlen );
        return GRIB_ARRAY_TOO_SMALL;
    }
     */
    if (*len != rlen)
        ret = grib_set_long(get_enclosing_handle(), numberOfElements_, *len);
    if (ret) return ret;

    ret = grib_get_long(get_enclosing_handle(), numberOfBits_, &numberOfBits);
    if (ret) return ret;
    if (numberOfBits == 0) {
        grib_buffer_replace(this, NULL, 0, 1, 1);
        return GRIB_SUCCESS;
    }

    buflen = compute_byte_count();
    buf    = (unsigned char*)grib_context_malloc_clear(context_, buflen + sizeof(long));

    for (i = 0; i < *len; i++)
        grib_encode_unsigned_longb(buf, val[i], &off, numberOfBits);

    grib_buffer_replace(this, buf, buflen, 1, 1);

    grib_context_free(context_, buf);

    return ret;
}

long UnsignedBits::byte_count()
{
    return length_;
}

int UnsignedBits::value_count(long* numberOfElements)
{
    *numberOfElements = 0;

    int ret = grib_get_long(get_enclosing_handle(), numberOfElements_, numberOfElements);
    if (ret) {
        grib_context_log(context_, GRIB_LOG_ERROR,
                         "%s unable to get %s to compute size", name_, numberOfElements_);
    }

    return ret;
}

long UnsignedBits::byte_offset()
{
    return offset_;
}

void UnsignedBits::update_size(size_t s)
{
    length_ = s;
}

long UnsignedBits::next_offset()
{
    return byte_offset() + length_;
}

}  // namespace eccodes::accessor
