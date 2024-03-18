
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
#include "grib_accessor_class_padto.h"

grib_accessor_class_padto_t _grib_accessor_class_padto{"padto"};
grib_accessor_class* grib_accessor_class_padto = &_grib_accessor_class_padto;



size_t grib_accessor_class_padto_t::preferred_size(grib_accessor* a, int from_handle){
    grib_accessor_padto_t* self = (grib_accessor_padto_t*)a;

    long length = 0;
    long theEnd;

    grib_expression_evaluate_long(grib_handle_of_accessor(a), self->expression, &theEnd);

    length = theEnd - a->offset;

    /* printf("preferred_size: prefered: %ld current:%ld %s %s %ld\n", (long)length,(long)a->length,a->cclass->name,a->name,(long)a->offset); */

    return length > 0 ? length : 0;
}


void grib_accessor_class_padto_t::init(grib_accessor* a, const long len, grib_arguments* arg){
    grib_accessor_padto_t* self = (grib_accessor_padto_t*)a;

    self->expression = grib_arguments_get_expression(grib_handle_of_accessor(a), arg, 0);
    a->length        = preferred_size(a, 1);
}

void grib_accessor_class_padto_t::dump(grib_accessor* a, grib_dumper* dumper){
    /*grib_dump_string(dumper,a,NULL);*/
}
