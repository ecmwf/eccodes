
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
#include "grib_accessor_class_bytes.h"

grib_accessor_class_bytes_t _grib_accessor_class_bytes{"bytes"};
grib_accessor_class* grib_accessor_class_bytes = &_grib_accessor_class_bytes;


void grib_accessor_class_bytes_t::init(grib_accessor* a, const long len, grib_arguments* arg){
    /*grib_accessor_signed* self = (grib_accessor_signed*)a;  */
    a->length = len;
    Assert(a->length >= 0);
}

int grib_accessor_class_bytes_t::get_native_type(grib_accessor* a){
    return GRIB_TYPE_BYTES;
}

int grib_accessor_class_bytes_t::compare(grib_accessor* a, grib_accessor* b){
    int retval = GRIB_SUCCESS;

    size_t alen = (size_t)a->byte_count();    size_t blen = (size_t)b->byte_count();
    if (alen != blen)
        return GRIB_COUNT_MISMATCH;

    return retval;
}

int grib_accessor_class_bytes_t::unpack_string(grib_accessor* a, char* v, size_t* len){
    unsigned char* p = NULL;
    char* s          = v;
    long i = 0;
    const long length = a->byte_count();    const long slength = 2 * length;

    if (*len < (size_t)slength) {
        *len = slength;
        return GRIB_BUFFER_TOO_SMALL;
    }

    p = grib_handle_of_accessor(a)->buffer->data + a->byte_offset();
    for (i = 0; i < length; i++) {
        snprintf(s, INT_MAX, "%02x", *(p++));
        s += 2;
    }

    *len = slength;

    return GRIB_SUCCESS;
}

int grib_accessor_class_bytes_t::pack_string(grib_accessor* a, const char* val, size_t* len){
    /* The string representation (val) of the byte array will have two chars
     * per byte e.g. 4C5B means the 2 bytes 0114 and 0133 in octal
     * so has to be twice the length of the byte array
     */
    int err = 0;
    grib_context* c = a->context;
    size_t nbytes = a->length;
    const size_t expected_blen = nbytes;
    const size_t expected_slen = 2 * expected_blen;
    unsigned char* bytearray = NULL;
    size_t i = 0, slen = strlen(val);

    if (slen != expected_slen || *len != expected_slen) {
        grib_context_log(c, GRIB_LOG_ERROR,
                        "%s: Key %s is %lu bytes. Expected a string with %lu characters (actual length=%zu)",
                        __func__, a->name, expected_blen, expected_slen, *len);
        return GRIB_WRONG_ARRAY_SIZE;
    }

    bytearray = (unsigned char*)grib_context_malloc(c, nbytes * (sizeof(unsigned char)));
    if (!bytearray) return GRIB_OUT_OF_MEMORY;

    for (i = 0; i < (slen/2); i++) {
        unsigned int byteVal = 0;
        if (sscanf(val + 2*i, "%02x", &byteVal) != 1) {
            grib_context_log(c, GRIB_LOG_ERROR,"%s: Invalid hex byte specfication '%.2s'", __func__, val + 2*i);
            grib_context_free(c, bytearray);
            return GRIB_INVALID_KEY_VALUE;
        }
        Assert(byteVal < 256);
        bytearray[i] = (int)byteVal;
    }

    /* Forward to base class to pack the byte array */
    err = grib_accessor_class_gen_t::pack_bytes(a, bytearray, &nbytes);
    grib_context_free(c, bytearray);
    return err;
}
