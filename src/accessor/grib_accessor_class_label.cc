
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
#include "grib_accessor_class_label.h"

grib_accessor_class_label_t _grib_accessor_class_label{"label"};
grib_accessor_class* grib_accessor_class_label = &_grib_accessor_class_label;


void grib_accessor_class_label_t::init(grib_accessor* a, const long len, grib_arguments* arg){
    a->flags |= GRIB_ACCESSOR_FLAG_READ_ONLY;
    a->flags |= GRIB_ACCESSOR_FLAG_EDITION_SPECIFIC;
    a->length = 0;
}

void grib_accessor_class_label_t::dump(grib_accessor* a, grib_dumper* dumper){
    grib_dump_label(dumper, a, NULL);
}

int grib_accessor_class_label_t::get_native_type(grib_accessor* a){
    return GRIB_TYPE_LABEL;
}

int grib_accessor_class_label_t::unpack_string(grib_accessor* a, char* val, size_t* len){
    size_t vlen = strlen(a->name);
    if (vlen > *len)
        return GRIB_BUFFER_TOO_SMALL;
    *len = vlen;
    strcpy(val, a->name);
    return GRIB_SUCCESS;
}
