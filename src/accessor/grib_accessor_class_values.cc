/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_accessor_class_values.h"

grib_accessor_class_values_t _grib_accessor_class_values{"values"};
grib_accessor_class* grib_accessor_class_values = &_grib_accessor_class_values;

long init_length(grib_accessor* a)
{
    grib_accessor_values_t* self = (grib_accessor_values_t*)a;
    int ret = 0;

    long seclen        = 0;
    long offsetsection = 0;
    long offsetdata    = 0;

    if ((ret = grib_get_long_internal(grib_handle_of_accessor(a), self->seclen, &seclen)))
        return ret;

    if (seclen == 0) {
        /* printf("init_length seclen=0\n"); */
        return 0;
    }

    if ((ret = grib_get_long_internal(grib_handle_of_accessor(a), self->offsetsection, &offsetsection)))
        return ret;

    if ((ret = grib_get_long_internal(grib_handle_of_accessor(a), self->offsetdata, &offsetdata)))
        return ret;

    /* When reparsing */
    if (offsetdata < offsetsection) {
        /* printf("init_length offsetdata < offsetsection=0\n"); */
        Assert(grib_handle_of_accessor(a)->loader);
        return 0;
    }

    return seclen - (offsetdata - offsetsection);
}

void grib_accessor_class_values_t::init(grib_accessor* a, const long v, grib_arguments* params)
{
    grib_accessor_class_gen_t::init(a, v, params);
    grib_accessor_values_t* self = (grib_accessor_values_t*)a;
    self->carg                 = 0;

    self->seclen        = grib_arguments_get_name(grib_handle_of_accessor(a), params, self->carg++);
    self->offsetdata    = grib_arguments_get_name(grib_handle_of_accessor(a), params, self->carg++);
    self->offsetsection = grib_arguments_get_name(grib_handle_of_accessor(a), params, self->carg++);
    self->dirty         = 1;

    a->length = init_length(a);
    /* Assert(a->length>=0); */
}

int grib_accessor_class_values_t::get_native_type(grib_accessor* a)
{
    return GRIB_TYPE_DOUBLE;
}

void grib_accessor_class_values_t::dump(grib_accessor* a, grib_dumper* dumper)
{
    grib_dump_values(dumper, a);
}

long grib_accessor_class_values_t::byte_count(grib_accessor* a)
{
    grib_context_log(a->context, GRIB_LOG_DEBUG, "byte_count of %s = %ld", a->name, a->length);
    return a->length;
}

long grib_accessor_class_values_t::byte_offset(grib_accessor* a)
{
    return a->offset;
}

long grib_accessor_class_values_t::next_offset(grib_accessor* a)
{
    return a->offset + a->length;
}

void grib_accessor_class_values_t::update_size(grib_accessor* a, size_t s)
{
    grib_context_log(a->context, GRIB_LOG_DEBUG, "updating size of %s old %ld new %ld", a->name, a->length, s);
    a->length = s;
    Assert(a->length >= 0);
}

int grib_accessor_class_values_t::compare(grib_accessor* a, grib_accessor* b)
{
    int retval   = 0;
    double* aval = 0;
    double* bval = 0;

    size_t alen = 0;
    size_t blen = 0;
    int err     = 0;
    long count  = 0;

    err = a->value_count(&count);    if (err)
        return err;
    alen = count;

    err = b->value_count(&count);    if (err)
        return err;
    blen = count;

    if (alen != blen)
        return GRIB_COUNT_MISMATCH;

    aval = (double*)grib_context_malloc(a->context, alen * sizeof(double));
    bval = (double*)grib_context_malloc(b->context, blen * sizeof(double));

    a->unpack_double(aval, &alen);    b->unpack_double(bval, &blen);
    retval = GRIB_SUCCESS;
    for (size_t i=0; i<alen && retval == GRIB_SUCCESS; ++i) {
        if (aval[i] != bval[i]) retval = GRIB_DOUBLE_VALUE_MISMATCH;
    }

    grib_context_free(a->context, aval);
    grib_context_free(b->context, bval);

    return retval;
}

int grib_accessor_class_values_t::pack_long(grib_accessor* a, const long* val, size_t* len)
{
    grib_accessor_values_t* self = (grib_accessor_values_t*)a;
    double* dval = (double*)grib_context_malloc(a->context, *len * sizeof(double));

    for (size_t i = 0; i < *len; i++)
        dval[i] = (double)val[i];

    int ret = a->pack_double(dval, len);
    grib_context_free(a->context, dval);

    self->dirty = 1;

    return ret;
}
