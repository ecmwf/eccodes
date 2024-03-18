
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
#include "grib_accessor_class_sum.h"

grib_accessor_class_sum_t _grib_accessor_class_sum{"sum"};
grib_accessor_class* grib_accessor_class_sum = &_grib_accessor_class_sum;


void grib_accessor_class_sum_t::init(grib_accessor* a, const long l, grib_arguments* c){
    grib_accessor_sum_t* self = (grib_accessor_sum_t*)a;
    int n                   = 0;
    self->values            = grib_arguments_get_name(grib_handle_of_accessor(a), c, n++);
    a->length               = 0;
    a->flags |= GRIB_ACCESSOR_FLAG_READ_ONLY;
}

int grib_accessor_class_sum_t::unpack_long(grib_accessor* a, long* val, size_t* len){
    grib_accessor_sum_t* self = (grib_accessor_sum_t*)a;
    int ret                 = 0;
    size_t size             = 0;
    long* values            = 0;
    long i;
    long count = 0;

    ret = value_count(a, &count);
    if (ret)
        return ret;
    size = count;

    if (size == 0) {
        *val = 0;
        return GRIB_SUCCESS;
    }
    values = (long*)grib_context_malloc_clear(a->context, sizeof(long) * size);
    if (!values)
        return GRIB_OUT_OF_MEMORY;

    grib_get_long_array(grib_handle_of_accessor(a), self->values, values, &size);

    *val = 0;
    for (i = 0; i < size; i++)
        *val += values[i];

    grib_context_free(a->context, values);

    return GRIB_SUCCESS;
}

int grib_accessor_class_sum_t::unpack_double(grib_accessor* a, double* val, size_t* len){
    grib_accessor_sum_t* self = (grib_accessor_sum_t*)a;
    int ret                 = 0;
    size_t size             = 0;
    double* values          = 0;
    long i;
    long count = 0;

    ret = value_count(a, &count);
    if (ret)
        return ret;
    size = count;

    if (size == 0) {
        *val = 0;
        return GRIB_SUCCESS;
    }
    values = (double*)grib_context_malloc_clear(a->context, sizeof(double) * size);
    if (!values)
        return GRIB_OUT_OF_MEMORY;

    ret = grib_get_double_array(grib_handle_of_accessor(a), self->values, values, &size);
    if (ret) {
        grib_context_free(a->context, values);
        return ret;
    }
    *val = 0;
    for (i = 0; i < size; i++)
        *val += values[i];

    grib_context_free(a->context, values);

    return GRIB_SUCCESS;
}

int grib_accessor_class_sum_t::value_count(grib_accessor* a, long* count){
    grib_accessor_sum_t* self = (grib_accessor_sum_t*)a;
    size_t n                = 0;
    int ret                 = 0;

    ret    = grib_get_size(grib_handle_of_accessor(a), self->values, &n);
    *count = n;

    if (ret)
        grib_context_log(a->context, GRIB_LOG_ERROR,
                         "%s is unable to get size of %s", a->name, self->values);

    return ret;
}
