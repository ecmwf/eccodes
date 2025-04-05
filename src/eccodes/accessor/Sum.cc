/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "Sum.h"

eccodes::accessor::Sum _grib_accessor_sum;
eccodes::Accessor* grib_accessor_sum = &_grib_accessor_sum;

namespace eccodes::accessor
{

void Sum::init(const long l, grib_arguments* c)
{
    Double::init(l, c);
    int n   = 0;
    values_ = c->get_name(get_enclosing_handle(), n++);
    length_ = 0;
    flags_ |= GRIB_ACCESSOR_FLAG_READ_ONLY;
}

int Sum::unpack_long(long* val, size_t* len)
{
    int ret      = 0;
    size_t size  = 0;
    long* values = 0;
    long i;
    long count = 0;

    ret = value_count(&count);
    if (ret)
        return ret;
    size = count;

    if (size == 0) {
        *val = 0;
        return GRIB_SUCCESS;
    }
    values = (long*)grib_context_malloc_clear(context_, sizeof(long) * size);
    if (!values)
        return GRIB_OUT_OF_MEMORY;

    grib_get_long_array(grib_handle_of_accessor(this), values_, values, &size);

    *val = 0;
    for (i = 0; i < size; i++)
        *val += values[i];

    grib_context_free(context_, values);

    return GRIB_SUCCESS;
}

int Sum::unpack_double(double* val, size_t* len)
{
    int ret        = 0;
    size_t size    = 0;
    double* values = 0;
    long i;
    long count = 0;

    ret = value_count(&count);
    if (ret)
        return ret;
    size = count;

    if (size == 0) {
        *val = 0;
        return GRIB_SUCCESS;
    }
    values = (double*)grib_context_malloc_clear(context_, sizeof(double) * size);
    if (!values)
        return GRIB_OUT_OF_MEMORY;

    ret = grib_get_double_array(grib_handle_of_accessor(this), values_, values, &size);
    if (ret) {
        grib_context_free(context_, values);
        return ret;
    }
    *val = 0;
    for (i = 0; i < size; i++)
        *val += values[i];

    grib_context_free(context_, values);

    return GRIB_SUCCESS;
}

int Sum::value_count(long* count)
{
    size_t n = 0;
    int ret  = GRIB_SUCCESS;

    ret    = grib_get_size(get_enclosing_handle(), values_, &n);
    *count = n;

    if (ret)
        grib_context_log(context_, GRIB_LOG_ERROR,
                         "%s is unable to get size of %s", name_, values_);

    return ret;
}

}  // namespace eccodes::accessor
