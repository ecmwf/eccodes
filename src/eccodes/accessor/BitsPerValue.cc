/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "BitsPerValue.h"

eccodes::accessor::BitsPerValue _grib_accessor_bits_per_value;
eccodes::Accessor* grib_accessor_bits_per_value = &_grib_accessor_bits_per_value;

namespace eccodes::accessor
{

void BitsPerValue::init(const long l, grib_arguments* args)
{
    Long::init(l, args);
    int n           = 0;
    values_         = args->get_name(get_enclosing_handle(), n++);
    bits_per_value_ = args->get_name(get_enclosing_handle(), n++);
    flags_ |= GRIB_ACCESSOR_FLAG_FUNCTION;
    length_ = 0;
}

int BitsPerValue::unpack_long(long* val, size_t* len)
{
    int ret        = 0;
    grib_handle* h = get_enclosing_handle();

    if ((ret = grib_get_long_internal(h, bits_per_value_, val)) != GRIB_SUCCESS)
        return ret;

    *len = 1;
    return ret;
}

int BitsPerValue::pack_long(const long* val, size_t* len)
{
    double* values  = NULL;
    size_t size     = 0;
    int ret         = 0;
    grib_context* c = context_;
    grib_handle* h  = get_enclosing_handle();

    if ((ret = grib_get_size(h, values_, &size)) != GRIB_SUCCESS)
        return ret;

    values = (double*)grib_context_malloc(c, size * sizeof(double));
    if (!values)
        return GRIB_OUT_OF_MEMORY;

    if ((ret = grib_get_double_array_internal(h, values_, values, &size)) != GRIB_SUCCESS) {
        grib_context_free(c, values);
        return ret;
    }

    if ((ret = grib_set_long_internal(h, bits_per_value_, *val)) != GRIB_SUCCESS) {
        grib_context_free(c, values);
        return ret;
    }

    if ((ret = grib_set_double_array_internal(h, values_, values, size)) != GRIB_SUCCESS) {
        grib_context_free(c, values);
        return ret;
    }

    grib_context_free(c, values);

    return GRIB_SUCCESS;
}

}  // namespace eccodes::accessor
