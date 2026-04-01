/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "DataRawPacking.h"
#include "grib_ieeefloat.h"

eccodes::AccessorBuilder<eccodes::accessor::DataRawPacking> _grib_accessor_data_raw_packing_builder{};

namespace eccodes::accessor
{

void DataRawPacking::init(const long v, grib_arguments* args)
{
    Values::init(v, args);

    number_of_values_ = args->get_name(get_enclosing_handle(), carg_++);
    precision_        = args->get_name(get_enclosing_handle(), carg_++);
    flags_ |= GRIB_ACCESSOR_FLAG_DATA;
}

int DataRawPacking::value_count(long* n_vals)
{
    *n_vals = 0;
    return grib_get_long_internal(get_enclosing_handle(), number_of_values_, n_vals);
}

int DataRawPacking::unpack_double(double* val, size_t* len)
{
    unsigned char* buf = NULL;
    int bytes          = 0;
    size_t nvals       = 0;
    long inlen         = byte_count();
    long precision     = 0;

    int code = GRIB_SUCCESS;

    if ((code = grib_get_long(get_enclosing_handle(), precision_, &precision)) != GRIB_SUCCESS)
        return code;

    dirty_ = 0;

    buf = get_enclosing_handle()->buffer->data;
    buf += byte_offset();
    switch (precision) {
        case 1:
            bytes = 4;
            break;
        case 2:
            bytes = 8;
            break;
        default:
            return GRIB_NOT_IMPLEMENTED;
    }

    nvals = inlen / bytes;

    if (*len < nvals)
        return GRIB_ARRAY_TOO_SMALL;

    code = grib_ieee_decode_array<double>(context_, buf, nvals, bytes, val);

    *len = nvals;

    return code;
}

int DataRawPacking::pack_double(const double* val, size_t* len)
{
    int bytes             = 0;
    unsigned char* buffer = NULL;

    long precision = 0;

    double* values = (double*)val;
    size_t inlen   = *len;

    /*int free_buffer = 0;
     *int free_values = 0;*/

    int code = GRIB_SUCCESS;

    size_t bufsize = 0;

    if (*len == 0)
        return GRIB_NO_VALUES;

    if ((code = grib_get_long_internal(get_enclosing_handle(), precision_, &precision)) != GRIB_SUCCESS)
        return code;

    dirty_ = 1;

    switch (precision) {
        case 1:
            bytes = 4;
            break;

        case 2:
            bytes = 8;
            break;

        default:
            code = GRIB_NOT_IMPLEMENTED;
            goto clean_up;
    }

    bufsize = bytes * inlen;

    buffer = (unsigned char*)grib_context_malloc(context_, bufsize);

    if (!buffer) {
        code = GRIB_OUT_OF_MEMORY;
        goto clean_up;
    }

    code = grib_ieee_encode_array(context_, values, inlen, bytes, buffer);

clean_up:
    /*if (free_buffer)
     *  free(buffer);
     * if (free_values)
     *  free(values);
     */

    grib_buffer_replace(this, buffer, bufsize, 1, 1);

    grib_context_buffer_free(context_, buffer);

    if (code == GRIB_SUCCESS) {
        code = grib_set_long(get_enclosing_handle(), number_of_values_, inlen);
        if (code == GRIB_READ_ONLY)
            code = 0;
    }

    return code;
}

int DataRawPacking::unpack_double_element(size_t idx, double* val)
{
    int ret            = 0;
    unsigned char* buf = NULL;
    int bytes          = 0;
    size_t nvals       = 0;
    long inlen         = byte_count();
    long pos           = 0;
    long precision     = 0;

    if ((ret = grib_get_long_internal(get_enclosing_handle(), precision_, &precision)) != GRIB_SUCCESS)
        return ret;

    dirty_ = 0;

    buf = get_enclosing_handle()->buffer->data;
    buf += byte_offset();
    switch (precision) {
        case 1:
            bytes = 4;
            break;

        case 2:
            bytes = 8;
            break;

        default:
            return GRIB_NOT_IMPLEMENTED;
    }

    pos = bytes * idx;

    ECCODES_ASSERT(pos <= inlen);

    nvals = 1;
    buf += pos;

    ret = grib_ieee_decode_array(context_, buf, nvals, bytes, val);

    return ret;
}

int DataRawPacking::unpack_double_element_set(const size_t* index_array, size_t len, double* val_array)
{
    int err  = 0;
    size_t i = 0;
    for (i = 0; i < len; ++i) {
        if ((err = unpack_double_element(index_array[i], val_array + i)) != GRIB_SUCCESS)
            return err;
    }
    return GRIB_SUCCESS;
}

}  // namespace eccodes::accessor
