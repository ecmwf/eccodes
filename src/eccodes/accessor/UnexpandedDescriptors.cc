/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "ExpandedDescriptors.h"
#include "UnexpandedDescriptors.h"

eccodes::accessor::UnexpandedDescriptors _grib_accessor_unexpanded_descriptors;
eccodes::Accessor* grib_accessor_unexpanded_descriptors = &_grib_accessor_unexpanded_descriptors;

namespace eccodes::accessor
{

void UnexpandedDescriptors::init(const long len, grib_arguments* args)
{
    Long::init(len, args);

    int n                         = 0;
    grib_handle* hand             = get_enclosing_handle();
    unexpandedDescriptorsEncoded_ = grib_find_accessor(hand, args->get_name(hand, n++));
    createNewData_                = args->get_name(hand, n++);
    length_                       = 0;
}

int UnexpandedDescriptors::unpack_long(long* val, size_t* len)
{
    int ret   = 0;
    long pos  = 0;
    long rlen = 0;
    long f, x, y;
    long* v = val;
    long i;
    grib_handle* hand = get_enclosing_handle();

    pos = accessor_raw_get_offset(unexpandedDescriptorsEncoded_) * 8;

    ret = value_count(&rlen);
    if (ret)
        return ret;

    if (rlen == 0) {
        grib_context_log(context_, GRIB_LOG_ERROR,
                         "No descriptors in section 3. Malformed message.");
        return GRIB_MESSAGE_MALFORMED;
    }

    if (*len < rlen) {
        /* grib_context_log(context_ , GRIB_LOG_ERROR, */
        /* " wrong size (%ld) for %s it contains %d values ",*len, name_ , rlen); */
        *len = 0;
        return GRIB_ARRAY_TOO_SMALL;
    }

    for (i = 0; i < rlen; i++) {
        f    = grib_decode_unsigned_long(hand->buffer->data, &pos, 2);
        x    = grib_decode_unsigned_long(hand->buffer->data, &pos, 6);
        y    = grib_decode_unsigned_long(hand->buffer->data, &pos, 8);
        *v++ = f * 100000 + x * 1000 + y;
    }
    *len = rlen;
    return GRIB_SUCCESS;
}

int UnexpandedDescriptors::pack_long(const long* val, size_t* len)
{
    int ret  = 0;
    long pos = 0;
    unsigned long f, x, y;
    unsigned char* buf      = NULL;
    ExpandedDescriptors* expanded = NULL;
    size_t buflen           = *len * 2;
    size_t i = 0, length = *len;
    long createNewData = 1;
    grib_handle* hand  = get_enclosing_handle();

    grib_get_long(hand, createNewData_, &createNewData);

    buf = (unsigned char*)grib_context_malloc_clear(context_, buflen);

    for (i = 0; i < length; i++) {
        const long tmp = val[i] % 100000;
        f              = val[i] / 100000;
        x              = tmp / 1000;
        y              = tmp % 1000;
        grib_encode_unsigned_longb(buf, f, &pos, 2);
        grib_encode_unsigned_longb(buf, x, &pos, 6);
        grib_encode_unsigned_longb(buf, y, &pos, 8);
    }

    unexpandedDescriptorsEncoded_->pack_bytes(buf, &buflen);
    grib_context_free(hand->context, buf);

    if (createNewData == 0)
        return ret;

    expanded = dynamic_cast<ExpandedDescriptors*>(grib_find_accessor(hand, "expandedCodes"));
    ECCODES_ASSERT(expanded != NULL);
    ret = expanded->grib_accessor_expanded_descriptors_set_do_expand(1);
    if (ret != GRIB_SUCCESS)
        return ret;

    ret = grib_set_long(hand, "unpack", 3); /* BUFR new data */
    if (ret != GRIB_SUCCESS)
        return ret;

    ret = grib_set_long(hand, "unpack", 1); /* Unpack structure */

    return ret;
}

int UnexpandedDescriptors::value_count(long* numberOfUnexpandedDescriptors)
{
    long n = 0;

    unexpandedDescriptorsEncoded_->value_count(&n);
    *numberOfUnexpandedDescriptors = n / 2;

    return 0;
}

long UnexpandedDescriptors::byte_offset()
{
    return offset_;
}

void UnexpandedDescriptors::update_size(size_t s)
{
    length_ = s;
}

long UnexpandedDescriptors::next_offset()
{
    return byte_offset() + length_;
}

}  // namespace eccodes::accessor
