/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "Raw.h"

eccodes::accessor::Raw _grib_accessor_raw;
eccodes::Accessor* grib_accessor_raw = &_grib_accessor_raw;

void accessor_raw_set_length(grib_accessor* a, size_t len)
{
    a->length_ = len;
}

long accessor_raw_get_offset(grib_accessor* a)
{
    return a->offset_;
}

namespace eccodes::accessor
{

void Raw::init(const long len, grib_arguments* arg)
{
    Gen::init(len, arg);
    int n   = 0;
    int err = 0;
    long sectionLength;
    grib_expression* e;
    grib_handle* hand = get_enclosing_handle();

    length_        = 0;
    totalLength_   = arg->get_name(hand, n++);
    sectionLength_ = arg->get_name(hand, n++);

    e   = arg->get_expression(hand, n++);
    err = e->evaluate_long(hand, &(relativeOffset_));
    if (err)
        grib_context_log(hand->context, GRIB_LOG_ERROR, "Unable to evaluate relativeOffset");

    grib_get_long(hand, sectionLength_, &sectionLength);

    length_ = sectionLength - relativeOffset_;
    if (length_ < 0)
        length_ = 0;

    /* ECCODES_ASSERT(length_ >=0); */
}

long Raw::get_native_type()
{
    return GRIB_TYPE_BYTES;
}

int Raw::compare(grib_accessor* b)
{
    int retval = GRIB_SUCCESS;

    size_t alen = (size_t)byte_count();
    size_t blen = (size_t)b->byte_count();
    if (alen != blen)
        return GRIB_COUNT_MISMATCH;

    return retval;
}

long Raw::byte_count()
{
    return length_;
}

int Raw::value_count(long* len)
{
    *len = length_;
    return 0;
}

int Raw::unpack_bytes(unsigned char* buffer, size_t* len)
{
    if (*len < length_) {
        *len = length_;
        return GRIB_ARRAY_TOO_SMALL;
    }
    *len = length_;

    memcpy(buffer, get_enclosing_handle()->buffer->data + offset_, *len);

    return GRIB_SUCCESS;
}

void Raw::update_size(size_t s)
{
    grib_context_log(context_, GRIB_LOG_DEBUG, "updating size of %s old %ld new %ld", name_, length_, s);
    length_ = s;
    ECCODES_ASSERT(length_ >= 0);
}

int Raw::pack_bytes(const unsigned char* val, size_t* len)
{
    size_t length = *len;
    long totalLength;
    long sectionLength;
    grib_handle* h = get_enclosing_handle();
    long dlen      = length - length_;

    grib_get_long(h, totalLength_, &totalLength);
    totalLength += dlen;
    grib_get_long(h, sectionLength_, &sectionLength);
    sectionLength += dlen;

    grib_buffer_replace(this, val, length, 1, 1);

    grib_set_long(h, totalLength_, totalLength);
    grib_set_long(h, sectionLength_, sectionLength);
    length_ = length;

    return GRIB_SUCCESS;
}

}  // namespace eccodes::accessor
