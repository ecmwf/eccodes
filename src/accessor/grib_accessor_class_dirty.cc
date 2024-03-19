
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
#include "grib_accessor_class_dirty.h"

grib_accessor_class_dirty_t _grib_accessor_class_dirty{"dirty"};
grib_accessor_class* grib_accessor_class_dirty = &_grib_accessor_class_dirty;


void grib_accessor_class_dirty_t::init(grib_accessor* a, const long l, grib_arguments* c){
    grib_accessor_class_long_t::init(a, l, c);
    grib_accessor_dirty_t* self = (grib_accessor_dirty_t*)a;
    self->accessor            = grib_arguments_get_name(grib_handle_of_accessor(a), c, 0);
    a->flags |= GRIB_ACCESSOR_FLAG_FUNCTION;
    a->flags |= GRIB_ACCESSOR_FLAG_HIDDEN;
    a->length = 0;
}

int grib_accessor_class_dirty_t::pack_long(grib_accessor* a, const long* val, size_t* len){
    grib_accessor_dirty_t* self = (grib_accessor_dirty_t*)a;
    grib_accessor* x          = grib_find_accessor(grib_handle_of_accessor(a), self->accessor);

    if (x)
        x->dirty = *val;

    return GRIB_SUCCESS;
}

int grib_accessor_class_dirty_t::unpack_long(grib_accessor* a, long* val, size_t* len){
    grib_accessor_dirty_t* self = (grib_accessor_dirty_t*)a;
    grib_accessor* x          = grib_find_accessor(grib_handle_of_accessor(a), self->accessor);

    if (x)
        x->dirty = 1;

    *val = 1;

    return GRIB_SUCCESS;
}
