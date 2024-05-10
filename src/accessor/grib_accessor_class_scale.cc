
/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_accessor_class_scale.h"

grib_accessor_class_scale_t _grib_accessor_class_scale{ "scale" };
grib_accessor_class* grib_accessor_class_scale = &_grib_accessor_class_scale;


void grib_accessor_class_scale_t::init(grib_accessor* a, const long l, grib_arguments* c)
{
    grib_accessor_class_double_t::init(a, l, c);
    grib_accessor_scale_t* self = (grib_accessor_scale_t*)a;
    int n = 0;

    self->value      = grib_arguments_get_name(grib_handle_of_accessor(a), c, n++);
    self->multiplier = grib_arguments_get_name(grib_handle_of_accessor(a), c, n++);
    self->divisor    = grib_arguments_get_name(grib_handle_of_accessor(a), c, n++);
    self->truncating = grib_arguments_get_name(grib_handle_of_accessor(a), c, n++);
}

int grib_accessor_class_scale_t::unpack_double(grib_accessor* a, double* val, size_t* len)
{
    grib_accessor_scale_t* self = (grib_accessor_scale_t*)a;
    int ret                     = 0;
    long value                  = 0;
    long multiplier             = 0;
    long divisor                = 0;

    if (*len < 1) {
        ret = GRIB_ARRAY_TOO_SMALL;
        grib_context_log(a->context, GRIB_LOG_ERROR,
                         "Accessor %s cannot gather value for %s and/or %s",
                         a->name, self->multiplier, self->divisor);
        return ret;
    }

    if ((ret = grib_get_long_internal(grib_handle_of_accessor(a), self->divisor, &divisor)) != GRIB_SUCCESS)
        return ret;

    if ((ret = grib_get_long_internal(grib_handle_of_accessor(a), self->multiplier, &multiplier)) != GRIB_SUCCESS)
        return ret;

    if ((ret = grib_get_long_internal(grib_handle_of_accessor(a), self->value, &value)) != GRIB_SUCCESS)
        return ret;

    if (value == GRIB_MISSING_LONG)
        *val = GRIB_MISSING_DOUBLE;
    else
        *val = ((double)(value * multiplier)) / divisor;

    if (ret == GRIB_SUCCESS)
        *len = 1;

    return ret;
}

int grib_accessor_class_scale_t::pack_long(grib_accessor* a, const long* val, size_t* len)
{
    const double dval = (double)*val;
    return pack_double(a, &dval, len);
}

int grib_accessor_class_scale_t::pack_double(grib_accessor* a, const double* val, size_t* len)
{
    grib_accessor_scale_t* self = (grib_accessor_scale_t*)a;
    int ret = 0;

    long value      = 0;
    long divisor    = 0;
    long multiplier = 0;
    long truncating = 0;
    double x;

    ret = grib_get_long_internal(grib_handle_of_accessor(a), self->divisor, &divisor);
    if (ret != GRIB_SUCCESS) return ret;

    ret = grib_get_long_internal(grib_handle_of_accessor(a), self->multiplier, &multiplier);
    if (ret != GRIB_SUCCESS) return ret;

    if (self->truncating) {
        ret = grib_get_long_internal(grib_handle_of_accessor(a), self->truncating, &truncating);
        if (ret != GRIB_SUCCESS) return ret;
    }

    if (multiplier == 0) {
        grib_context_log(a->context, GRIB_LOG_ERROR, "Accessor %s: cannot divide by a zero multiplier %s",
                         a->name, self->multiplier);
        return GRIB_ENCODING_ERROR;
    }

    x = *val * (double)divisor / (double)multiplier;
    if (*val == GRIB_MISSING_DOUBLE)
        value = GRIB_MISSING_LONG;
    else if (truncating) {
        value = (long)x;
    }
    else {
        value = x > 0 ? (long)(x + 0.5) : (long)(x - 0.5);
    }

    ret = grib_set_long_internal(grib_handle_of_accessor(a), self->value, value);
    if (ret)
        grib_context_log(a->context, GRIB_LOG_ERROR,
                         "Accessor %s: cannot pack value for %s (%s)\n",
                         a->name, self->value, grib_get_error_message(ret));

    if (ret == GRIB_SUCCESS)
        *len = 1;

    return ret;
}

int grib_accessor_class_scale_t::is_missing(grib_accessor* a)
{
    grib_accessor_scale_t* self = (grib_accessor_scale_t*)a;
    grib_accessor* av = grib_find_accessor(grib_handle_of_accessor(a), self->value);

    if (!av)
        return GRIB_NOT_FOUND;
    return av->is_missing_internal();
    //     int ret=0;
    //     long value=0;
    //     if((ret = grib_get_long_internal(grib_handle_of_accessor(a),self->value, &value))!= GRIB_SUCCESS){
    //         grib_context_log(a->context, GRIB_LOG_ERROR,
    //         "Accessor %s cannot gather value for %s error %d \n", a->name,
    //         self->value, ret);
    //         return 0;
    //     }
    //     return (value == GRIB_MISSING_LONG);
}
