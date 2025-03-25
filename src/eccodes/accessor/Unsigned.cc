/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "Unsigned.h"
#include "ecc_numeric_limits.h"

eccodes::AccessorBuilder<eccodes::accessor::Unsigned> _grib_accessor_unsigned_builder{};

namespace eccodes::accessor
{

void Unsigned::init(const long len, grib_arguments* arg)
{
    Long::init(len, arg);
    nbytes_ = len;
    arg_    = arg;

    if (flags_ & GRIB_ACCESSOR_FLAG_TRANSIENT) {
        length_ = 0;
        if (!vvalue_)
            vvalue_ = (grib_virtual_value*)grib_context_malloc_clear(context_, sizeof(grib_virtual_value));
        vvalue_->type   = GRIB_TYPE_LONG;
        vvalue_->length = len;
    }
    else {
        long count = 0;
        value_count(&count);

        length_ = len * count;
        vvalue_ = NULL;
    }
}

void Unsigned::dump(eccodes::Dumper* dumper)
{
    long rlen = 0;
    value_count(&rlen);
    if (rlen == 1)
        dumper->dump_long(this, NULL);
    else
        dumper->dump_values(this);
}

static const unsigned long ones[] = {
    0,
    0xff,
    0xffff,
    0xffffff,
    0xffffffff,
};

/* See GRIB-490 */
static const unsigned long all_ones = -1;

int value_is_missing(long val)
{
    return (val == GRIB_MISSING_LONG || val == all_ones);
}

int Unsigned::pack_long_unsigned_helper(const long* val, size_t* len, int check)
{
    int ret   = 0;
    long off  = 0;
    long rlen = 0;

    size_t buflen         = 0;
    unsigned char* buf    = NULL;
    unsigned long i       = 0;
    unsigned long missing = 0;

    int err = value_count(&rlen);
    if (err)
        return err;

    if (flags_ & GRIB_ACCESSOR_FLAG_CAN_BE_MISSING) {
        ECCODES_ASSERT(nbytes_ <= 4);
        missing = ones[nbytes_];
    }

    if (flags_ & GRIB_ACCESSOR_FLAG_TRANSIENT) {
        vvalue_->lval = val[0];

        if (missing && val[0] == GRIB_MISSING_LONG)
            vvalue_->missing = 1;
        else
            vvalue_->missing = 0;

        return GRIB_SUCCESS;
    }

    if (*len < 1) {
        grib_context_log(context_, GRIB_LOG_ERROR, "Wrong size for %s it contains %d values ", name_, 1);
        len[0] = 0;
        return GRIB_ARRAY_TOO_SMALL;
    }

    if (rlen == 1) {
        long v = val[0];

        if (missing)
            if (v == GRIB_MISSING_LONG)
                v = missing;

        /* Check if value fits into number of bits */
        if (check) {
            if (val[0] < 0) {
                grib_context_log(context_, GRIB_LOG_ERROR,
                                 "Key \"%s\": Trying to encode a negative value of %ld for key of type unsigned", name_, val[0]);
                return GRIB_ENCODING_ERROR;
            }
            /* See GRIB-23 and GRIB-262 */
            if (!value_is_missing(v)) {
                const long nbits = nbytes_ * 8;
                if (nbits < 33) {
                    unsigned long maxval = NumericLimits<unsigned long>::max(nbits);
                    if (maxval > 0 && v > maxval) { /* See ECC-1002 */
                        grib_context_log(context_, GRIB_LOG_ERROR,
                                         "Key \"%s\": Trying to encode value of %ld but the maximum allowable value is %lu (number of bits=%ld)",
                                         name_, v, maxval, nbits);
                        return GRIB_ENCODING_ERROR;
                    }
                }
            }
        }

        off = offset_ * 8;
        ret = grib_encode_unsigned_long(grib_handle_of_accessor(this)->buffer->data, v, &off, nbytes_ * 8);
        if (ret == GRIB_SUCCESS)
            len[0] = 1;
        if (*len > 1)
            grib_context_log(context_, GRIB_LOG_WARNING, "grib_accessor_unsigned : Trying to pack %d values in a scalar %s, packing first value", *len, name_);
        len[0] = 1;
        return ret;
    }

    /* TODO: We assume that there are no missing values if there are more that 1 value */
    buflen = *len * nbytes_;

    buf = (unsigned char*)grib_context_malloc(context_, buflen);

    for (i = 0; i < *len; i++)
        grib_encode_unsigned_long(buf, val[i], &off, nbytes_ * 8);

    ret = grib_set_long_internal(grib_handle_of_accessor(this), arg_->get_name(parent_->h, 0), *len);

    if (ret == GRIB_SUCCESS)
        grib_buffer_replace(this, buf, buflen, 1, 1);
    else
        *len = 0;

    grib_context_free(context_, buf);
    return ret;
}

int Unsigned::unpack_long(long* val, size_t* len)
{
    long rlen             = 0;
    unsigned long i       = 0;
    unsigned long missing = 0;
    long count            = 0;
    int err               = 0;
    long pos              = offset_ * 8;
    grib_handle* hand     = grib_handle_of_accessor(this);

    err = value_count(&count);
    if (err)
        return err;
    rlen = count;

    if (*len < rlen) {
        grib_context_log(context_, GRIB_LOG_ERROR, "Wrong size (%ld) for %s, it contains %ld values", *len, name_, rlen);
        *len = 0;
        return GRIB_ARRAY_TOO_SMALL;
    }

    if (flags_ & GRIB_ACCESSOR_FLAG_TRANSIENT) {
        *val = vvalue_->lval;
        *len = 1;
        return GRIB_SUCCESS;
    }

    if (flags_ & GRIB_ACCESSOR_FLAG_CAN_BE_MISSING) {
        ECCODES_ASSERT(nbytes_ <= 4);
        missing = ones[nbytes_];
    }

    for (i = 0; i < rlen; i++) {
        val[i] = (long)grib_decode_unsigned_long(hand->buffer->data, &pos, nbytes_ * 8);
        if (missing)
            if (val[i] == missing)
                val[i] = GRIB_MISSING_LONG;
    }

    *len = rlen;
    return GRIB_SUCCESS;
}

int Unsigned::pack_long(const long* val, size_t* len)
{
    /* See GRIB-262 as example of why we do the checks */
    return pack_long_unsigned_helper(val, len, /*check=*/1);
}

long Unsigned::byte_count()
{
    return length_;
}

int Unsigned::value_count(long* len)
{
    if (!arg_) {
        *len = 1;
        return 0;
    }
    return grib_get_long_internal(grib_handle_of_accessor(this), arg_->get_name(parent_->h, 0), len);
}

long Unsigned::byte_offset()
{
    return offset_;
}

void Unsigned::update_size(size_t s)
{
    length_ = s;
}

long Unsigned::next_offset()
{
    return byte_offset() + byte_count();
}

int Unsigned::is_missing()
{
    const unsigned char ff  = 0xff;
    unsigned long offset    = offset_;
    const grib_handle* hand = grib_handle_of_accessor(this);

    if (length_ == 0) {
        ECCODES_ASSERT(vvalue_ != NULL);
        return vvalue_->missing;
    }

    for (long i = 0; i < length_; i++) {
        if (hand->buffer->data[offset] != ff) {
            return 0;
        }
        offset++;
    }
    return 1;
}

void Unsigned::destroy(grib_context* context)
{
    if (vvalue_ != NULL)
        grib_context_free(context, vvalue_);

    vvalue_ = NULL;
    Long::destroy(context);
}

}  // namespace eccodes::accessor
