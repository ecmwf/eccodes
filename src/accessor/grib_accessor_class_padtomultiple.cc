/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_accessor_class_padtomultiple.h"

grib_accessor_class_padtomultiple_t _grib_accessor_class_padtomultiple{ "padtomultiple" };
grib_accessor_class* grib_accessor_class_padtomultiple = &_grib_accessor_class_padtomultiple;


size_t grib_accessor_class_padtomultiple_t::preferred_size(grib_accessor* a, int from_handle)
{
    grib_accessor_padtomultiple_t* self = (grib_accessor_padtomultiple_t*)a;
    long padding  = 0;
    long begin    = 0;
    long multiple = 0;

    grib_expression_evaluate_long(grib_handle_of_accessor(a), self->begin, &begin);
    grib_expression_evaluate_long(grib_handle_of_accessor(a), self->multiple, &multiple);

    padding = a->offset - begin;
    padding = ((padding + multiple - 1) / multiple) * multiple - padding;

    return padding == 0 ? multiple : padding;
}

void grib_accessor_class_padtomultiple_t::init(grib_accessor* a, const long len, grib_arguments* arg)
{
    grib_accessor_class_padding_t::init(a, len, arg);
    grib_accessor_padtomultiple_t* self = (grib_accessor_padtomultiple_t*)a;

    self->begin    = grib_arguments_get_expression(grib_handle_of_accessor(a), arg, 0);
    self->multiple = grib_arguments_get_expression(grib_handle_of_accessor(a), arg, 1);
    a->length      = preferred_size(a, 1);
}
