/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "SignedBits.h"

eccodes::accessor::SignedBits _grib_accessor_signed_bits;
eccodes::Accessor* grib_accessor_signed_bits = &_grib_accessor_signed_bits;

namespace eccodes::accessor
{

long SignedBits::byte_count()
{
    return length_;
}

long SignedBits::compute_byte_count()
{
    long numberOfBits;
    long numberOfElements;
    int ret = 0;

    ret = grib_get_long(get_enclosing_handle(), numberOfBits_, &numberOfBits);
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

void SignedBits::init(const long len, grib_arguments* args)
{
    Long::init(len, args);
    int n             = 0;
    numberOfBits_     = args->get_name(get_enclosing_handle(), n++);
    numberOfElements_ = args->get_name(get_enclosing_handle(), n++);
    length_           = compute_byte_count();
}

int SignedBits::unpack_long(long* val, size_t* len)
{
    return GRIB_NOT_IMPLEMENTED;
#if 0
    int i;
    int ret           = 0;
    long pos          = offset_ * 8;
    long rlen         = 0;
    long numberOfBits = 0;

    ret = value_count(a, &rlen);
    if (ret)
        return ret;

    if (*len < rlen) {
        grib_context_log(context_ , GRIB_LOG_ERROR,
                         "Wrong size (%ld) for %s it contains %ld values", *len, name_ , rlen);
        *len = 0;
        return GRIB_ARRAY_TOO_SMALL;
    }

    ret = grib_get_long(get_enclosing_handle(), numberOfBits_ , &numberOfBits);
    if (ret)
        return ret;

    if (numberOfBits == 0) {
        for (i = 0; i < rlen; i++)
            val[i] = 0;
        return GRIB_SUCCESS;
    }

    for (i = 0; i < rlen; i++)
        val[i] = grib_decode_signed_longb(get_enclosing_handle()->buffer->data, &pos, numberOfBits);

    *len = rlen;

    return GRIB_SUCCESS;
#endif
}

int SignedBits::pack_long(const long* val, size_t* len)
{
    return GRIB_NOT_IMPLEMENTED;
#if 0
    int ret            = 0;
    long off           = 0;
    long numberOfBits  = 0;
    size_t buflen      = 0;
    unsigned char* buf = NULL;
    unsigned long i    = 0;
    unsigned long rlen = 0;
    long count         = 0;

    ret = value_count(a, &count);
    if (ret)
        return ret;
    rlen = count;
    if (*len != rlen) {
        ret = grib_set_long(get_enclosing_handle(), numberOfElements_ , rlen);
        if (ret)
            return ret;
    }

    ret = grib_get_long(get_enclosing_handle(), numberOfBits_ , &numberOfBits);
    if (ret)
        return ret;

    buflen = compute_byte_count(a);
    buf    = (unsigned char*)grib_context_malloc_clear(context_ , buflen + sizeof(long));

    for (i = 0; i < rlen; i++)
        grib_encode_signed_longb(buf, val[i], &off, numberOfBits);

    grib_buffer_replace(a, buf, buflen, 1, 1);

    grib_context_free(context_ , buf);

    return ret;
#endif
}

int SignedBits::value_count(long* numberOfElements)
{
    *numberOfElements = 0;

    return grib_get_long(get_enclosing_handle(), numberOfElements_, numberOfElements);
}

long SignedBits::byte_offset()
{
    return offset_;
}

void SignedBits::update_size(size_t s)
{
    length_ = s;
}

long SignedBits::next_offset()
{
    return byte_offset() + byte_count();
}

}  // namespace eccodes::accessor
