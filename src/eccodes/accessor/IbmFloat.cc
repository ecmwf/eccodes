/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "IbmFloat.h"

eccodes::accessor::IbmFloat _grib_accessor_ibmfloat;
eccodes::Accessor* grib_accessor_ibmfloat = &_grib_accessor_ibmfloat;

namespace eccodes::accessor
{

void IbmFloat::init(const long len, grib_arguments* arg)
{
    Double::init(len, arg);
    long count = 0;

    arg_ = arg;
    value_count(&count);
    length_ = 4 * count;
    ECCODES_ASSERT(length_ >= 0);
}

template <typename T>
static int unpack(grib_accessor* a, T* val, size_t* len)
{
    static_assert(std::is_floating_point<T>::value, "Requires floating point numbers");
    unsigned long rlen = 0;
    long count         = 0;
    int err            = 0;
    unsigned long i    = 0;
    long bitp          = a->offset_ * 8;
    grib_handle* hand  = a->get_enclosing_handle();

    err = a->value_count(&count);
    if (err)
        return err;
    rlen = count;

    if (*len < rlen) {
        grib_context_log(a->context_, GRIB_LOG_ERROR, "Wrong size (%zu) for %s, it contains %lu values", *len, a->name_, rlen);
        *len = 0;
        return GRIB_ARRAY_TOO_SMALL;
    }

    for (i = 0; i < rlen; i++)
        val[i] = (T)grib_long_to_ibm(grib_decode_unsigned_long(hand->buffer->data, &bitp, 32));

    *len = rlen;
    return GRIB_SUCCESS;
}

int IbmFloat::unpack_double(double* val, size_t* len)
{
    return unpack<double>(this, val, len);
}

int IbmFloat::unpack_float(float* val, size_t* len)
{
    return unpack<float>(this, val, len);
}

int IbmFloat::pack_double(const double* val, size_t* len)
{
    int ret            = 0;
    unsigned long i    = 0;
    unsigned long rlen = *len;
    size_t buflen      = 0;
    unsigned char* buf = NULL;
    long off           = 0;

    if (*len < 1) {
        grib_context_log(context_, GRIB_LOG_ERROR, "Wrong size for %s, it packs at least 1 value", name_);
        *len = 0;
        return GRIB_ARRAY_TOO_SMALL;
    }

    if (rlen == 1) {
        // double x = 0;
        // grib_nearest_smaller_ibm_float(val[0],&x);
        // double y = grib_long_to_ibm(grib_ibm_to_long(val[0]));
        // printf("IBMFLOAT val=%.20f nearest_smaller_ibm_float=%.20f long_to_ibm=%.20f\n",val[0],x ,y);

        off = byte_offset() * 8;
        ret = grib_encode_unsigned_long(get_enclosing_handle()->buffer->data, grib_ibm_to_long(val[0]), &off, 32);
        if (*len > 1)
            grib_context_log(context_, GRIB_LOG_WARNING, "ibmfloat: Trying to pack %zu values in a scalar %s, packing first value",
                             *len, name_);
        if (ret == GRIB_SUCCESS)
            len[0] = 1;
        return ret;
    }

    buflen = rlen * 4;

    buf = (unsigned char*)grib_context_malloc(context_, buflen);

    for (i = 0; i < rlen; i++) {
        grib_encode_unsigned_longb(buf, grib_ibm_to_long(val[i]), &off, 32);
    }
    ret = grib_set_long_internal(get_enclosing_handle(), arg_->get_name(parent_->h, 0), rlen);

    if (ret == GRIB_SUCCESS)
        grib_buffer_replace(this, buf, buflen, 1, 1);
    else
        *len = 0;

    grib_context_free(context_, buf);

    length_ = byte_count();

    return ret;
}

long IbmFloat::byte_count()
{
    return length_;
}

int IbmFloat::value_count(long* len)
{
    *len = 0;
    if (!arg_) {
        *len = 1;
        return 0;
    }
    return grib_get_long_internal(get_enclosing_handle(), arg_->get_name(parent_->h, 0), len);
}

long IbmFloat::byte_offset()
{
    return offset_;
}

void IbmFloat::update_size(size_t s)
{
    length_ = (long)s;
    ECCODES_ASSERT(length_ >= 0);
}

long IbmFloat::next_offset()
{
    return byte_offset() + byte_count();
}

int IbmFloat::nearest_smaller_value(double val, double* nearest)
{
    int ret = 0;
    if (grib_nearest_smaller_ibm_float(val, nearest) == GRIB_INTERNAL_ERROR) {
        grib_context_log(context_, GRIB_LOG_ERROR, "ibm_float:nearest_smaller_value overflow value=%g", val);
        grib_dump_content(get_enclosing_handle(), stderr, "wmo", GRIB_DUMP_FLAG_HEXADECIMAL, 0);
        ret = GRIB_INTERNAL_ERROR;
    }
    return ret;
}

}  // namespace eccodes::accessor
