
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
#include "grib_accessor_class_size.h"

grib_accessor_class_size_t _grib_accessor_class_size{"size"};
grib_accessor_class* grib_accessor_class_size = &_grib_accessor_class_size;


void grib_accessor_class_size_t::init(grib_accessor* a, const long l, grib_arguments* c){
    grib_accessor_class_long_t::init(a, l, c);
    grib_accessor_size_t* self = (grib_accessor_size_t*)a;
    self->accessor           = grib_arguments_get_name(grib_handle_of_accessor(a), c, 0);
    a->flags |= GRIB_ACCESSOR_FLAG_READ_ONLY;
    a->flags |= GRIB_ACCESSOR_FLAG_FUNCTION;
    a->length = 0;
}

int grib_accessor_class_size_t::unpack_long(grib_accessor* a, long* val, size_t* len){
    int ret                  = 0;
    size_t size              = 0;
    grib_accessor_size_t* self = (grib_accessor_size_t*)a;
    ret                      = grib_get_size(grib_handle_of_accessor(a), self->accessor, &size);
    *val                     = (long)size;
    *len                     = 1;
    return ret;
}
