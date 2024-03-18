
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
#include "grib_accessor_class_long_vector.h"
#include "grib_accessor_class_abstract_long_vector.h"

grib_accessor_class_long_vector_t _grib_accessor_class_long_vector{"long_vector"};
grib_accessor_class* grib_accessor_class_long_vector = &_grib_accessor_class_long_vector;


void grib_accessor_class_long_vector_t::init(grib_accessor* a, const long l, grib_arguments* c){
    grib_accessor_long_vector_t* self       = (grib_accessor_long_vector_t*)a;
    grib_accessor* va                     = NULL;
    grib_accessor_abstract_long_vector_t* v = NULL;
    int n                                 = 0;

    self->vector = grib_arguments_get_name(grib_handle_of_accessor(a), c, n++);
    va = (grib_accessor*)grib_find_accessor(grib_handle_of_accessor(a), self->vector);
    v  = (grib_accessor_abstract_long_vector_t*)va;

    self->index = grib_arguments_get_long(grib_handle_of_accessor(a), c, n++);

    /* check self->index on init and never change it */
    Assert(self->index < v->number_of_elements && self->index >= 0);

    a->length = 0;
}

int grib_accessor_class_long_vector_t::unpack_long(grib_accessor* a, long* val, size_t* len){
    size_t size = 0;
    int err = 0;
    long* vector;
    grib_accessor_long_vector_t* self       = (grib_accessor_long_vector_t*)a;
    grib_accessor* va                     = NULL;
    grib_accessor_abstract_long_vector_t* v = NULL;

    va = (grib_accessor*)grib_find_accessor(grib_handle_of_accessor(a), self->vector);
    v  = (grib_accessor_abstract_long_vector_t*)va;

    /*TODO implement the dirty mechanism to avoid to unpack every time */
    err = grib_get_size(grib_handle_of_accessor(a), self->vector, &size);
    if (err) return err;
    DEBUG_ASSERT(size > 0);
    vector = (long*)grib_context_malloc(a->context, sizeof(long) * size);
    err = va->unpack_long(vector, &size);    grib_context_free(a->context, vector);
    if (err) return err;

    *val = v->v[self->index];

    return GRIB_SUCCESS;
}

int grib_accessor_class_long_vector_t::unpack_double(grib_accessor* a, double* val, size_t* len){
    long lval                             = 0;
    int err                               = 0;
    grib_accessor_long_vector_t* self       = (grib_accessor_long_vector_t*)a;
    grib_accessor* va                     = NULL;
    grib_accessor_abstract_long_vector_t* v = NULL;
    va                                    = (grib_accessor*)grib_find_accessor(grib_handle_of_accessor(a), self->vector);
    v                                     = (grib_accessor_abstract_long_vector_t*)va;

    err = unpack_long(a, &lval, len);

    *val = (double)v->v[self->index];

    return err;
}

int grib_accessor_class_long_vector_t::pack_long(grib_accessor* a, const long* val, size_t* len){
    int err                               = 0;
    grib_accessor_long_vector_t* self       = (grib_accessor_long_vector_t*)a;
    grib_accessor* va                     = NULL;
    grib_accessor_abstract_long_vector_t* v = NULL;

    va = (grib_accessor*)grib_find_accessor(grib_handle_of_accessor(a), self->vector);
    v  = (grib_accessor_abstract_long_vector_t*)va;

    v->pack_index = self->index;

    err = va->pack_long(val, len);
    return err;
}

int grib_accessor_class_long_vector_t::get_native_type(grib_accessor* a){
    return GRIB_TYPE_LONG;
}
