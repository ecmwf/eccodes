
/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_accessor_class_padtoeven.h"

grib_accessor_class_padtoeven_t _grib_accessor_class_padtoeven{"padtoeven"};
grib_accessor_class* grib_accessor_class_padtoeven = &_grib_accessor_class_padtoeven;


size_t grib_accessor_class_padtoeven_t::preferred_size(grib_accessor* a, int from_handle){
    grib_accessor_padtoeven_t* self = (grib_accessor_padtoeven_t*)a;
    long offset                   = 0;
    long length                   = 0;
    long seclen;

    grib_get_long_internal(grib_handle_of_accessor(a), self->section_offset, &offset);
    grib_get_long_internal(grib_handle_of_accessor(a), self->section_length, &length);

    if ((length % 2) && from_handle) {
        //  grib_context_log(a->context,
        //       GRIB_LOG_ERROR,"GRIB message has an odd length section (%ld, %s)",
        //       (long)length,a->name);
        return 0;
    }

    /* printf("EVEN %ld %ld\n",(long) a->offset,(long) offset);*/
    seclen = a->offset - offset;

    return (seclen % 2) ? 1 : 0;
}

void grib_accessor_class_padtoeven_t::init(grib_accessor* a, const long len, grib_arguments* args){
    grib_accessor_class_padding_t::init(a, len, args);
    grib_accessor_padtoeven_t* self = (grib_accessor_padtoeven_t*)a;

    self->section_offset = grib_arguments_get_name(grib_handle_of_accessor(a), args, 0);
    self->section_length = grib_arguments_get_name(grib_handle_of_accessor(a), args, 1);

    a->length = preferred_size(a, 1);
}
