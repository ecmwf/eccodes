/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "TransientDarray.h"

eccodes::AccessorBuilder<eccodes::accessor::TransientDarray> _grib_accessor_transient_darray_builder{};

namespace eccodes::accessor
{

void TransientDarray::init(const long length, grib_arguments* args)
{
    Gen::init(length, args);
    arr_    = NULL;
    type_   = GRIB_TYPE_DOUBLE;
    length_ = 0;
}

void TransientDarray::dump(eccodes::Dumper* dumper)
{
    dumper->dump_double(this, NULL);
}

int TransientDarray::pack_double(const double* val, size_t* len)
{
    if (arr_)
        grib_darray_delete(arr_);
    arr_ = grib_darray_new(*len, 10);

    for (size_t i = 0; i < *len; i++)
        grib_darray_push(arr_, val[i]);

    return GRIB_SUCCESS;
}

int TransientDarray::pack_long(const long* val, size_t* len)
{
    if (arr_)
        grib_darray_delete(arr_);
    arr_ = grib_darray_new(*len, 10);

    for (size_t i = 0; i < *len; i++)
        grib_darray_push(arr_, (double)val[i]);

    return GRIB_SUCCESS;
}

int TransientDarray::unpack_double(double* val, size_t* len)
{
    long count = 0;

    value_count(&count);

    if (*len < count) {
        grib_context_log(context_, GRIB_LOG_ERROR, "Wrong size for %s (setting %ld, required %ld) ", name_, *len, count);
        return GRIB_ARRAY_TOO_SMALL;
    }

    *len = count;
    for (size_t i = 0; i < *len; i++)
        val[i] = arr_->v[i];

    return GRIB_SUCCESS;
}

int TransientDarray::unpack_long(long* val, size_t* len)
{
    long count = 0;

    value_count(&count);

    if (*len < count) {
        grib_context_log(context_, GRIB_LOG_ERROR, "Wrong size for %s (setting %ld, required %ld) ", name_, *len, count);
        return GRIB_ARRAY_TOO_SMALL;
    }

    *len = count;
    for (size_t i = 0; i < *len; i++)
        val[i] = (long)arr_->v[i];

    return GRIB_SUCCESS;
}

void TransientDarray::destroy(grib_context* c)
{
    if (arr_)
        grib_darray_delete(arr_);
    Gen::destroy(c);
}

int TransientDarray::value_count(long* count)
{
    if (arr_)
        *count = grib_darray_used_size(arr_);
    else
        *count = 0;

    return 0;
}

long TransientDarray::get_native_type()
{
    return type_;
}

}  // namespace eccodes::accessor
