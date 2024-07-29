/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_accessor_class_mars_param.h"

grib_accessor_class_mars_param_t _grib_accessor_class_mars_param{ "mars_param" };
grib_accessor_class* grib_accessor_class_mars_param = &_grib_accessor_class_mars_param;


void grib_accessor_class_mars_param_t::init(grib_accessor* a, const long l, grib_arguments* c)
{
    grib_accessor_class_ascii_t::init(a, l, c);
    grib_accessor_mars_param_t* self = (grib_accessor_mars_param_t*)a;

    int n         = 0;
    self->paramId = grib_arguments_get_name(grib_handle_of_accessor(a), c, n++);
    self->table   = grib_arguments_get_name(grib_handle_of_accessor(a), c, n++);
    self->param   = grib_arguments_get_name(grib_handle_of_accessor(a), c, n++);
}

// For an implementation of pack_string, see
//   src/deprecated/grib_accessor_class_mars_param.cc

// For an alternative implementation of unpack_string, see
//   src/deprecated/grib_accessor_class_mars_param.cc
//
int grib_accessor_class_mars_param_t::unpack_string(grib_accessor* a, char* val, size_t* len)
{
    grib_accessor_mars_param_t* self = (grib_accessor_mars_param_t*)a;
    long param                       = 0;
    long table                       = 0;
    int ret                          = 0;

    if (self->table != NULL && (ret = grib_get_long_internal(grib_handle_of_accessor(a), self->table, &table)) != GRIB_SUCCESS)
        return ret;
    if (self->param != NULL && (ret = grib_get_long_internal(grib_handle_of_accessor(a), self->param, &param)) != GRIB_SUCCESS)
        return ret;

    /*if (table==200) table=128;*/
    snprintf(val, 32, "%ld.%ld", param, table);
    *len = strlen(val) + 1;

    return GRIB_SUCCESS;
}

size_t grib_accessor_class_mars_param_t::string_length(grib_accessor* a)
{
    return 7;
}
