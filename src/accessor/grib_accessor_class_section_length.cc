
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
#include "grib_accessor_class_section_length.h"

grib_accessor_class_section_length_t _grib_accessor_class_section_length{"section_length"};
grib_accessor_class* grib_accessor_class_section_length = &_grib_accessor_class_section_length;


void grib_accessor_class_section_length_t::init(grib_accessor* a, const long len, grib_arguments* arg){
    a->parent->aclength = a;
    a->length           = len;
    a->flags |= GRIB_ACCESSOR_FLAG_READ_ONLY;
    a->flags |= GRIB_ACCESSOR_FLAG_EDITION_SPECIFIC;
    Assert(a->length >= 0);
}

void grib_accessor_class_section_length_t::dump(grib_accessor* a, grib_dumper* dumper){
    grib_dump_long(dumper, a, NULL);
}

int grib_accessor_class_section_length_t::value_count(grib_accessor* a, long* c){
    *c = 1;
    return 0;
}
