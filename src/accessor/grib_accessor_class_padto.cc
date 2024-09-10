/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_accessor_class_padto.h"

AccessorBuilder<grib_accessor_padto_t> _grib_accessor_padto_builder{};

size_t grib_accessor_padto_t::preferred_size(int from_handle)
{
    long length = 0;
    long theEnd;

    grib_expression_evaluate_long(grib_handle_of_accessor(this), expression_, &theEnd);

    length = theEnd - offset_;

    /* printf("preferred_size: prefered: %ld current:%ld %s %s %ld\n", (long)length,(long)a->length,a->cclass->name,a->name,(long)offset_ ); */

    return length > 0 ? length : 0;
}

void grib_accessor_padto_t::init(const long len, grib_arguments* arg)
{
    grib_accessor_padding_t::init(len, arg);

    expression_ = grib_arguments_get_expression(grib_handle_of_accessor(this), arg, 0);
    length_     = preferred_size(1);
}

void grib_accessor_padto_t::dump(grib_dumper* dumper)
{
    /*grib_dump_string(dumper,a,NULL);*/
}
