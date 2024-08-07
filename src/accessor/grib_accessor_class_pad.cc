/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_accessor_class_pad.h"

grib_accessor_class_pad_t _grib_accessor_class_pad{ "pad" };
grib_accessor_class* grib_accessor_class_pad = &_grib_accessor_class_pad;


void grib_accessor_class_pad_t::init(grib_accessor* a, const long len, grib_arguments* arg)
{
    grib_accessor_class_padding_t::init(a, len, arg);
    grib_accessor_pad_t* self = (grib_accessor_pad_t*)a;

    self->expression = grib_arguments_get_expression(grib_handle_of_accessor(a), arg, 0);
    a->length        = preferred_size(a, 1);
}

size_t grib_accessor_class_pad_t::preferred_size(grib_accessor* a, int from_handle)
{
    grib_accessor_pad_t* self = (grib_accessor_pad_t*)a;
    long length = 0;

    grib_expression_evaluate_long(grib_handle_of_accessor(a), self->expression, &length);

    return length > 0 ? length : 0;
}
