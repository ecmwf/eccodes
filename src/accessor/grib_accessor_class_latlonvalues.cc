
/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_accessor_class_latlonvalues.h"

grib_accessor_class_latlonvalues_t _grib_accessor_class_latlonvalues{"latlonvalues"};
grib_accessor_class* grib_accessor_class_latlonvalues = &_grib_accessor_class_latlonvalues;


void grib_accessor_class_latlonvalues_t::init(grib_accessor* a, const long l, grib_arguments* c){
    grib_accessor_class_double_t::init(a, l, c);
    grib_accessor_latlonvalues_t* self = (grib_accessor_latlonvalues_t*)a;
    int n                            = 0;

    self->values = grib_arguments_get_name(grib_handle_of_accessor(a), c, n++);

    a->flags |= GRIB_ACCESSOR_FLAG_READ_ONLY;
}

int grib_accessor_class_latlonvalues_t::unpack_double(grib_accessor* a, double* val, size_t* len){
    grib_context* c = a->context;
    int err         = 0;
    double* v       = val;
    double lat, lon, value;
    size_t size         = 0;
    long count          = 0;
    grib_iterator* iter = grib_iterator_new(grib_handle_of_accessor(a), 0, &err);
    if (err) {
        if (iter) grib_iterator_delete(iter);
        grib_context_log(c, GRIB_LOG_ERROR, "latlonvalues: Unable to create iterator");
        return err;
    }

    err  = value_count(a, &count);
    if (err) return err;
    size = count;

    if (*len < size) {
        if (iter) grib_iterator_delete(iter);
        return GRIB_ARRAY_TOO_SMALL;
    }

    while (grib_iterator_next(iter, &lat, &lon, &value)) {
        *(v++) = lat;
        *(v++) = lon;
        *(v++) = value;
    }

    grib_iterator_delete(iter);

    *len = size;

    return GRIB_SUCCESS;
}

int grib_accessor_class_latlonvalues_t::value_count(grib_accessor* a, long* count){
    grib_accessor_latlonvalues_t* self = (grib_accessor_latlonvalues_t*)a;
    grib_handle* h                   = grib_handle_of_accessor(a);
    int ret = GRIB_SUCCESS;
    size_t size;
    if ((ret = grib_get_size(h, self->values, &size)) != GRIB_SUCCESS) {
        grib_context_log(h->context, GRIB_LOG_ERROR, "latlonvalues: Unable to get size of %s", self->values);
        return ret;
    }

    *count = 3 * size;
    return GRIB_SUCCESS;
}
