
/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_accessor_class_gds_is_present.h"

grib_accessor_class_gds_is_present_t _grib_accessor_class_gds_is_present{ "gds_is_present" };
grib_accessor_class* grib_accessor_class_gds_is_present = &_grib_accessor_class_gds_is_present;


void grib_accessor_class_gds_is_present_t::init(grib_accessor* a, const long l, grib_arguments* c)
{
    grib_accessor_class_long_t::init(a, l, c);
    int n                                = 0;
    grib_accessor_gds_is_present_t* self = (grib_accessor_gds_is_present_t*)a;
    grib_handle* h                       = grib_handle_of_accessor(a);
    self->gds_present                    = grib_arguments_get_name(h, c, n++);
    self->grid_definition                = grib_arguments_get_name(h, c, n++);
    self->bitmap_present                 = grib_arguments_get_name(h, c, n++);
    self->values                         = grib_arguments_get_name(h, c, n++);

    a->flags |= GRIB_ACCESSOR_FLAG_FUNCTION;
    a->flags |= GRIB_ACCESSOR_FLAG_HIDDEN;
    a->length = 0;
}

int grib_accessor_class_gds_is_present_t::pack_long(grib_accessor* a, const long* val, size_t* len)
{
    grib_accessor_gds_is_present_t* self = (grib_accessor_gds_is_present_t*)a;
    long missing                         = 255;
    int ret                              = 0;
    size_t size                          = 0;
    double* values                       = NULL;
    grib_context* c                      = a->context;
    grib_handle* h                       = grib_handle_of_accessor(a);

    if (*val != 1)
        return GRIB_NOT_IMPLEMENTED;

    if ((ret = grib_get_size(h, self->values, &size)) != GRIB_SUCCESS)
        return ret;

    values = (double*)grib_context_malloc(c, size * sizeof(double));
    if (!values)
        return GRIB_OUT_OF_MEMORY;

    if ((ret = grib_get_double_array_internal(h, self->values, values, &size)) != GRIB_SUCCESS) {
        grib_context_free(c, values);
        return ret;
    }

    if ((ret = grib_set_long_internal(h, self->gds_present, *val)) != GRIB_SUCCESS)
        return ret;

    if ((ret = grib_set_long_internal(h, self->bitmap_present, *val)) != GRIB_SUCCESS)
        return ret;


    if ((ret = grib_set_long_internal(h, self->grid_definition, missing)) != GRIB_SUCCESS)
        return ret;

    if ((ret = grib_set_double_array_internal(h, self->values, values, size)) != GRIB_SUCCESS)
        return ret;

    grib_context_free(c, values);

    return GRIB_SUCCESS;
}

int grib_accessor_class_gds_is_present_t::unpack_long(grib_accessor* a, long* val, size_t* len)
{
    int ret = 0;
    grib_handle* h = grib_handle_of_accessor(a);
    grib_accessor_gds_is_present_t* self = (grib_accessor_gds_is_present_t*)a;

    if ((ret = grib_get_long_internal(h, self->gds_present, val)) != GRIB_SUCCESS)
        return ret;

    *len = 1;

    return GRIB_SUCCESS;
}
