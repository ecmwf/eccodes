/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_accessor_class_transient_darray.h"

grib_accessor_class_transient_darray_t _grib_accessor_class_transient_darray{ "transient_darray" };
grib_accessor_class* grib_accessor_class_transient_darray = &_grib_accessor_class_transient_darray;


void grib_accessor_class_transient_darray_t::init(grib_accessor* a, const long length, grib_arguments* args)
{
    grib_accessor_class_gen_t::init(a, length, args);
    grib_accessor_transient_darray_t* self = (grib_accessor_transient_darray_t*)a;
    self->arr  = NULL;
    self->type = GRIB_TYPE_DOUBLE;
    a->length  = 0;
}

void grib_accessor_class_transient_darray_t::dump(grib_accessor* a, grib_dumper* dumper)
{
    grib_dump_double(dumper, a, NULL);
}

int grib_accessor_class_transient_darray_t::pack_double(grib_accessor* a, const double* val, size_t* len)
{
    grib_accessor_transient_darray_t* self = (grib_accessor_transient_darray_t*)a;

    if (self->arr)
        grib_darray_delete(a->context, self->arr);
    self->arr = grib_darray_new(a->context, *len, 10);

    for (size_t i = 0; i < *len; i++)
        grib_darray_push(a->context, self->arr, val[i]);

    return GRIB_SUCCESS;
}

int grib_accessor_class_transient_darray_t::pack_long(grib_accessor* a, const long* val, size_t* len)
{
    grib_accessor_transient_darray_t* self = (grib_accessor_transient_darray_t*)a;

    if (self->arr)
        grib_darray_delete(a->context, self->arr);
    self->arr = grib_darray_new(a->context, *len, 10);

    for (size_t i = 0; i < *len; i++)
        grib_darray_push(a->context, self->arr, (double)val[i]);

    return GRIB_SUCCESS;
}

int grib_accessor_class_transient_darray_t::unpack_double(grib_accessor* a, double* val, size_t* len)
{
    grib_accessor_transient_darray_t* self = (grib_accessor_transient_darray_t*)a;
    long count = 0;

    value_count(a, &count);

    if (*len < count) {
        grib_context_log(a->context, GRIB_LOG_ERROR, "Wrong size for %s (setting %ld, required %ld) ", a->name, *len, count);
        return GRIB_ARRAY_TOO_SMALL;
    }

    *len = count;
    for (size_t i = 0; i < *len; i++)
        val[i] = self->arr->v[i];

    return GRIB_SUCCESS;
}

int grib_accessor_class_transient_darray_t::unpack_long(grib_accessor* a, long* val, size_t* len)
{
    grib_accessor_transient_darray_t* self = (grib_accessor_transient_darray_t*)a;
    long count = 0;

    value_count(a, &count);

    if (*len < count) {
        grib_context_log(a->context, GRIB_LOG_ERROR, "Wrong size for %s (setting %ld, required %ld) ", a->name, *len, count);
        return GRIB_ARRAY_TOO_SMALL;
    }

    *len = count;
    for (size_t i = 0; i < *len; i++)
        val[i] = (long)self->arr->v[i];

    return GRIB_SUCCESS;
}

void grib_accessor_class_transient_darray_t::destroy(grib_context* c, grib_accessor* a)
{
    grib_accessor_transient_darray_t* self = (grib_accessor_transient_darray_t*)a;
    if (self->arr)
        grib_darray_delete(a->context, self->arr);
    grib_accessor_class_gen_t::destroy(c, a);
}

int grib_accessor_class_transient_darray_t::value_count(grib_accessor* a, long* count)
{
    grib_accessor_transient_darray_t* self = (grib_accessor_transient_darray_t*)a;
    if (self->arr)
        *count = grib_darray_used_size(self->arr);
    else
        *count = 0;

    return 0;
}

int grib_accessor_class_transient_darray_t::get_native_type(grib_accessor* a)
{
    const grib_accessor_transient_darray_t* self = (grib_accessor_transient_darray_t*)a;
    return self->type;
}
