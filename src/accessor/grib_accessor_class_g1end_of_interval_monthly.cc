
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
#include "grib_accessor_class_g1end_of_interval_monthly.h"

grib_accessor_class_g1end_of_interval_monthly_t _grib_accessor_class_g1end_of_interval_monthly{"g1end_of_interval_monthly"};
grib_accessor_class* grib_accessor_class_g1end_of_interval_monthly = &_grib_accessor_class_g1end_of_interval_monthly;


void grib_accessor_class_g1end_of_interval_monthly_t::init(grib_accessor* a, const long l, grib_arguments* c){
    grib_accessor_class_abstract_vector_t::init(a, l, c);
    grib_accessor_g1end_of_interval_monthly_t* self = (grib_accessor_g1end_of_interval_monthly_t*)a;
    int n                                         = 0;

    self->verifyingMonth = grib_arguments_get_name(grib_handle_of_accessor(a), c, n++);
    a->flags |= GRIB_ACCESSOR_FLAG_READ_ONLY;
    a->flags |= GRIB_ACCESSOR_FLAG_FUNCTION;
    a->flags |= GRIB_ACCESSOR_FLAG_HIDDEN;

    self->number_of_elements = 6;
    self->v = (double*)grib_context_malloc(a->context, sizeof(double) * self->number_of_elements);

    a->length = 0;
    a->dirty  = 1;
}

int grib_accessor_class_g1end_of_interval_monthly_t::unpack_double(grib_accessor* a, double* val, size_t* len){
    grib_accessor_g1end_of_interval_monthly_t* self = (grib_accessor_g1end_of_interval_monthly_t*)a;
    int ret = 0;
    char verifyingMonth[7] = {0,};
    size_t slen = 7;
    long year = 0, month = 0, date = 0;
    const long mdays[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    long days    = 0;

    if (!a->dirty)
        return GRIB_SUCCESS;

    if (*len != (size_t)self->number_of_elements)
        return GRIB_ARRAY_TOO_SMALL;

    if ((ret = grib_get_string(grib_handle_of_accessor(a), self->verifyingMonth, verifyingMonth, &slen)) != GRIB_SUCCESS)
        return ret;

    date  = atoi(verifyingMonth);
    if (date < 0) {
        return GRIB_INVALID_ARGUMENT;
    }
    year  = date / 100;
    month = date - year * 100;
    if (month == 2) {
        days = 28;
        if (year % 400 == 0 || (year % 4 == 0 && year % 100 != 0))
            days = 29;
    }
    else {
        if (month < 1 || month > 12) return GRIB_INVALID_ARGUMENT;
        days = mdays[month - 1];
    }
    self->v[0] = year;
    self->v[1] = month;

    self->v[2] = days;
    self->v[3] = 24;
    self->v[4] = 00;
    self->v[5] = 00;

    a->dirty = 0;

    val[0] = self->v[0];
    val[1] = self->v[1];
    val[2] = self->v[2];
    val[3] = self->v[3];
    val[4] = self->v[4];
    val[5] = self->v[5];

    return ret;
}

int grib_accessor_class_g1end_of_interval_monthly_t::value_count(grib_accessor* a, long* count){
    grib_accessor_g1end_of_interval_monthly_t* self = (grib_accessor_g1end_of_interval_monthly_t*)a;
    *count                                        = self->number_of_elements;
    return 0;
}

void grib_accessor_class_g1end_of_interval_monthly_t::destroy(grib_context* c, grib_accessor* a){
    grib_accessor_g1end_of_interval_monthly_t* self = (grib_accessor_g1end_of_interval_monthly_t*)a;
    grib_context_free(c, self->v);
}

int grib_accessor_class_g1end_of_interval_monthly_t::compare(grib_accessor* a, grib_accessor* b){
    int retval   = GRIB_SUCCESS;
    double* aval = 0;
    double* bval = 0;

    long count  = 0;
    size_t alen = 0;
    size_t blen = 0;
    int err     = 0;

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

    b->dirty = 1;
    a->dirty = 1;

    err = a->unpack_double(aval, &alen);    err = b->unpack_double(bval, &blen);
    for(size_t i=0; i<alen && retval == GRIB_SUCCESS; ++i) {
        if (aval[i] != bval[i]) retval = GRIB_DOUBLE_VALUE_MISMATCH;
    }

    grib_context_free(a->context, aval);
    grib_context_free(b->context, bval);

    return retval;
}
