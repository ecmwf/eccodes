/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "Bytes.h"

eccodes::accessor::Bytes _grib_accessor_bytes;
eccodes::Accessor* grib_accessor_bytes = &_grib_accessor_bytes;

namespace eccodes::accessor
{

void Bytes::init(const long len, grib_arguments* arg)
{
    Gen::init(len, arg);
    /*grib_accessor_signed* self = (grib_accessor_signed*)a;  */
    length_ = len;
    ECCODES_ASSERT(length_ >= 0);
}

long Bytes::get_native_type()
{
    return GRIB_TYPE_BYTES;
}

int Bytes::compare(grib_accessor* b)
{
    int retval = GRIB_SUCCESS;

    size_t alen = (size_t)byte_count();
    size_t blen = (size_t)b->byte_count();
    if (alen != blen)
        return GRIB_COUNT_MISMATCH;

    return retval;
}

int Bytes::unpack_string(char* v, size_t* len)
{
    unsigned char* p   = NULL;
    char* s = v;
    const long length  = byte_count();
    const long slength = 2 * length;

    if (*len < (size_t)slength) {
        *len = slength;
        return GRIB_BUFFER_TOO_SMALL;
    }

    p = get_enclosing_handle()->buffer->data + byte_offset();
    for (long i = 0; i < length; i++) {
        snprintf(s, INT_MAX, "%02x", *(p++));
        s += 2;
    }

    *len = slength;

    return GRIB_SUCCESS;
}

int Bytes::pack_string(const char* val, size_t* len)
{
    /* The string representation (val) of the byte array will have two chars
     * per byte e.g. 4C5B means the 2 bytes 0114 and 0133 in octal
     * so has to be twice the length of the byte array
     */
    int err = 0;
    size_t nbytes = length_;
    const size_t expected_blen = nbytes;
    const size_t expected_slen = 2 * expected_blen;
    size_t slen = strlen(val);

    if (slen != expected_slen || *len != expected_slen) {
        grib_context_log(context_, GRIB_LOG_ERROR,
                         "%s: Key %s is %lu bytes. Expected a string with %lu characters (actual length=%zu)",
                         __func__, name_, expected_blen, expected_slen, *len);
        return GRIB_WRONG_ARRAY_SIZE;
    }

    unsigned char* bytearray = (unsigned char*)grib_context_malloc(context_, nbytes * (sizeof(unsigned char)));
    if (!bytearray) return GRIB_OUT_OF_MEMORY;

    for (size_t i = 0; i < (slen / 2); i++) {
        unsigned int byteVal = 0;
        if (sscanf(val + 2 * i, "%02x", &byteVal) != 1) {
            grib_context_log(context_, GRIB_LOG_ERROR, "%s: Invalid hex byte specfication '%.2s'", __func__, val + 2 * i);
            grib_context_free(context_, bytearray);
            return GRIB_INVALID_KEY_VALUE;
        }
        ECCODES_ASSERT(byteVal < 256);
        bytearray[i] = (int)byteVal;
    }

    /* Forward to base class to pack the byte array */
    err = Gen::pack_bytes(bytearray, &nbytes);
    grib_context_free(context_, bytearray);
    return err;
}

}  // namespace eccodes::accessor
