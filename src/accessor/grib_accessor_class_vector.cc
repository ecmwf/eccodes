
/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_accessor_class_vector.h"
#include "grib_accessor_class_abstract_vector.h"

grib_accessor_class_vector_t _grib_accessor_class_vector{"vector"};
grib_accessor_class* grib_accessor_class_vector = &_grib_accessor_class_vector;


void grib_accessor_class_vector_t::init(grib_accessor* a, const long l, grib_arguments* c){
    grib_accessor_class_abstract_vector_t::init(a, l, c);
    grib_accessor_vector_t* self = (grib_accessor_vector_t*)a;
    int n                      = 0;

    self->vector = grib_arguments_get_name(grib_handle_of_accessor(a), c, n++);
    self->index  = grib_arguments_get_long(grib_handle_of_accessor(a), c, n++);
    a->flags |= GRIB_ACCESSOR_FLAG_READ_ONLY;
    a->flags |= GRIB_ACCESSOR_FLAG_FUNCTION;
    a->length = 0;
}

int grib_accessor_class_vector_t::unpack_double(grib_accessor* a, double* val, size_t* len){
    int err     = 0;
    size_t size = 0;
    double* stat;
    grib_accessor_vector_t* self       = (grib_accessor_vector_t*)a;
    grib_accessor* va                = (grib_accessor*)grib_find_accessor(grib_handle_of_accessor(a), self->vector);
    grib_accessor_abstract_vector_t* v = (grib_accessor_abstract_vector_t*)va;

    Assert(self->index >= 0);

    if (self->index >= v->number_of_elements) {
        grib_context_log(a->context, GRIB_LOG_FATAL, "index=%d number_of_elements=%d for %s", self->index, v->number_of_elements, a->name);
        Assert(self->index < v->number_of_elements);
    }

    if (va->dirty) {
        grib_get_size(grib_handle_of_accessor(a), self->vector, &size);
        stat = (double*)grib_context_malloc_clear(a->context, sizeof(double) * size);
        err  = va->unpack_double(stat, &size);        grib_context_free(a->context, stat);
        if (err)
            return err;
    }

    *val = v->v[self->index];

    return err;
}
