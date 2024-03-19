
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
#include "grib_accessor_class_bufrdc_expanded_descriptors.h"

grib_accessor_class_bufrdc_expanded_descriptors_t _grib_accessor_class_bufrdc_expanded_descriptors{"bufrdc_expanded_descriptors"};
grib_accessor_class* grib_accessor_class_bufrdc_expanded_descriptors = &_grib_accessor_class_bufrdc_expanded_descriptors;


void grib_accessor_class_bufrdc_expanded_descriptors_t::init(grib_accessor* a, const long len, grib_arguments* args){
    grib_accessor_class_long_t::init(a, len, args);
    grib_accessor_bufrdc_expanded_descriptors_t* self = (grib_accessor_bufrdc_expanded_descriptors_t*)a;
    int n                                           = 0;
    self->expandedDescriptors                       = grib_arguments_get_name(grib_handle_of_accessor(a), args, n++);
    self->expandedDescriptorsAccessor               = 0;
    a->length                                       = 0;
    a->flags |= GRIB_ACCESSOR_FLAG_READ_ONLY;
}

static grib_accessor* get_accessor(grib_accessor* a)
{
    grib_accessor_bufrdc_expanded_descriptors_t* self = (grib_accessor_bufrdc_expanded_descriptors_t*)a;
    if (!self->expandedDescriptorsAccessor) {
        self->expandedDescriptorsAccessor = grib_find_accessor(grib_handle_of_accessor(a), self->expandedDescriptors);
    }
    return self->expandedDescriptorsAccessor;
}

int grib_accessor_class_bufrdc_expanded_descriptors_t::unpack_long(grib_accessor* a, long* val, size_t* len){
    grib_accessor* descriptors = 0;
    size_t rlen  = 0, l;
    long lenall  = 0;
    size_t i     = 0;
    long* v      = 0;
    grib_context* c = a->context;

    descriptors = get_accessor(a);
    if (!descriptors) return GRIB_NOT_FOUND;

    a->value_count(&lenall);    v = (long*)grib_context_malloc_clear(c, sizeof(long) * lenall);
    l = lenall;
    descriptors->unpack_long(v, &l);
    rlen = 0;
    for (i = 0; i < l; i++) {
        if ((v[i] < 100000 || v[i] > 221999))
            val[rlen++] = v[i];
    }
    *len = rlen;
    grib_context_free(c,v);

    return GRIB_SUCCESS;
}

int grib_accessor_class_bufrdc_expanded_descriptors_t::unpack_string_array(grib_accessor* a, char** buffer, size_t* len){
    int err = 0;
    grib_accessor* descriptors = 0;
    size_t l     = 0;
    long lenall  = 0;
    size_t i     = 0;
    long* v      = 0;
    char buf[25] = {0,};
    grib_context* c = a->context;

    descriptors = get_accessor(a);
    if (!descriptors) return GRIB_NOT_FOUND;

    err = a->value_count(&lenall);    if (err) return err;
    l = lenall;
    if (l > *len) return GRIB_ARRAY_TOO_SMALL;

    v = (long*)grib_context_malloc_clear(c, sizeof(long) * l);
    err = descriptors->unpack_long(v, &l);    if (err) return err;

    for (i = 0; i < l; i++) {
        snprintf(buf, sizeof(buf), "%06ld", v[i]);
        buffer[i] = grib_context_strdup(c, buf);
    }
    *len = l;
    grib_context_free(c,v);

    return GRIB_SUCCESS;
}

int grib_accessor_class_bufrdc_expanded_descriptors_t::value_count(grib_accessor* a, long* rlen){
    grib_accessor* descriptors = get_accessor(a);

    return descriptors->value_count(rlen);}

void grib_accessor_class_bufrdc_expanded_descriptors_t::destroy(grib_context* c, grib_accessor* a){
}
