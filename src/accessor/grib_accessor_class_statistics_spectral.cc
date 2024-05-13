
/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_accessor_class_statistics_spectral.h"

grib_accessor_class_statistics_spectral_t _grib_accessor_class_statistics_spectral{"statistics_spectral"};
grib_accessor_class* grib_accessor_class_statistics_spectral = &_grib_accessor_class_statistics_spectral;


void grib_accessor_class_statistics_spectral_t::init(grib_accessor* a, const long l, grib_arguments* c){
    grib_accessor_class_abstract_vector_t::init(a, l, c);
    grib_accessor_statistics_spectral_t* self = (grib_accessor_statistics_spectral_t*)a;
    int n                                   = 0;

    self->values = grib_arguments_get_name(grib_handle_of_accessor(a), c, n++);
    self->J      = grib_arguments_get_name(grib_handle_of_accessor(a), c, n++);
    self->K      = grib_arguments_get_name(grib_handle_of_accessor(a), c, n++);
    self->M      = grib_arguments_get_name(grib_handle_of_accessor(a), c, n++);
    self->JS     = grib_arguments_get_name(grib_handle_of_accessor(a), c, n++);

    a->flags |= GRIB_ACCESSOR_FLAG_READ_ONLY;
    a->flags |= GRIB_ACCESSOR_FLAG_FUNCTION;

    self->number_of_elements = 4;
    self->v                  = (double*)grib_context_malloc(a->context,
                                           sizeof(double) * self->number_of_elements);

    a->length = 0;
    a->dirty  = 1;
}

int grib_accessor_class_statistics_spectral_t::unpack_double(grib_accessor* a, double* val, size_t* len){
    grib_accessor_statistics_spectral_t* self = (grib_accessor_statistics_spectral_t*)a;
    int ret = 0, i = 0;
    double* values;
    size_t size = 0;
    long J, K, M, N;
    double avg, enorm, sd;
    grib_context* c = a->context;
    grib_handle* h  = grib_handle_of_accessor(a);

    if (!a->dirty)
        return GRIB_SUCCESS;

    if (*len != self->number_of_elements)
        return GRIB_ARRAY_TOO_SMALL;

    if ((ret = grib_get_size(h, self->values, &size)) != GRIB_SUCCESS)
        return ret;

    if ((ret = grib_get_long(grib_handle_of_accessor(a), self->J, &J)) != GRIB_SUCCESS)
        return ret;

    if ((ret = grib_get_long(grib_handle_of_accessor(a), self->K, &K)) != GRIB_SUCCESS)
        return ret;

    if ((ret = grib_get_long(grib_handle_of_accessor(a), self->M, &M)) != GRIB_SUCCESS)
        return ret;

    if (J != M || M != K)
        return GRIB_NOT_IMPLEMENTED;

    N = (M + 1) * (M + 2) / 2;

    if (2 * N != size) {
        grib_context_log(a->context, GRIB_LOG_ERROR,
                         "wrong number of components for spherical harmonics %ld != %ld", 2 * N, size);
        return GRIB_WRONG_ARRAY_SIZE;
    }

    values = (double*)grib_context_malloc(c, size * sizeof(double));
    if (!values)
        return GRIB_OUT_OF_MEMORY;

    if ((ret = grib_get_double_array_internal(h, self->values, values, &size)) != GRIB_SUCCESS) {
        grib_context_free(c, values);
        return ret;
    }

    avg   = values[0];
    sd    = 0;

    for (i = 2; i < 2 * J; i += 2)
        sd += values[i] * values[i];

    for (i = 2 * J; i < size; i += 2)
        sd += 2 * values[i] * values[i] + 2 * values[i + 1] * values[i + 1];

    enorm = sd + avg * avg;

    sd    = sqrt(sd);
    enorm = sqrt(enorm);

    a->dirty = 0;

    grib_context_free(c, values);

    self->v[0] = avg;
    self->v[1] = enorm;
    self->v[2] = sd;
    self->v[3] = sd == 0 ? 1 : 0;

    for (i = 0; i < self->number_of_elements; i++)
        val[i] = self->v[i];

    return ret;
}

int grib_accessor_class_statistics_spectral_t::value_count(grib_accessor* a, long* count){
    grib_accessor_statistics_spectral_t* self = (grib_accessor_statistics_spectral_t*)a;
    *count                                  = self->number_of_elements;
    return 0;
}

void grib_accessor_class_statistics_spectral_t::destroy(grib_context* c, grib_accessor* a){
    grib_accessor_statistics_spectral_t* self = (grib_accessor_statistics_spectral_t*)a;
    grib_context_free(c, self->v);
    grib_accessor_class_abstract_vector_t::destroy(c, a);
}

int grib_accessor_class_statistics_spectral_t::compare(grib_accessor* a, grib_accessor* b){
    int retval   = GRIB_SUCCESS;
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

    b->dirty = 1;
    a->dirty = 1;

    a->unpack_double(aval, &alen);    b->unpack_double(bval, &blen);
    retval = GRIB_SUCCESS;
    for (size_t i=0; i<alen && retval == GRIB_SUCCESS; ++i) {
        if (aval[i] != bval[i]) retval = GRIB_DOUBLE_VALUE_MISMATCH;
    }

    grib_context_free(a->context, aval);
    grib_context_free(b->context, bval);

    return retval;
}
