/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_accessor_class_cf_var_name.h"

grib_accessor_class_cf_var_name_t _grib_accessor_class_cf_var_name{ "cf_var_name" };
grib_accessor_class* grib_accessor_class_cf_var_name = &_grib_accessor_class_cf_var_name;


void grib_accessor_class_cf_var_name_t::init(grib_accessor* a, const long l, grib_arguments* arg)
{
    grib_accessor_class_ascii_t::init(a, l, arg);

    grib_accessor_cf_var_name_t* self = (grib_accessor_cf_var_name_t*)a;
    grib_handle* h = grib_handle_of_accessor(a);

    self->defaultKey = grib_arguments_get_name(h, arg, 0);
}

int grib_accessor_class_cf_var_name_t::unpack_string(grib_accessor* a, char* val, size_t* len)
{
    grib_accessor_cf_var_name_t* self = (grib_accessor_cf_var_name_t*)a;

    grib_handle* h  = grib_handle_of_accessor(a);
    char defaultKey[256] = {0,};
    size_t size  = sizeof(defaultKey) / sizeof(*defaultKey);
    char* pDefaultKey = defaultKey;

    int err = grib_get_string(h, self->defaultKey, defaultKey, &size);
    if (err) return err;
    Assert(size > 0);
    Assert(strlen(defaultKey) > 0);

    if ( STR_EQUAL(defaultKey, "~") || isdigit(defaultKey[0]) ) {
        // NetCDF variables cannot start with a digit
        long paramId = 0;
        err = grib_get_long(h, "paramId", &paramId);
        if (err) snprintf(val, 1024, "%s", "unknown");
        else     snprintf(val, 1024, "p%ld", paramId);
    }
    else {
        snprintf(val, 1024, "%s", pDefaultKey);
    }
    size = strlen(val);
    *len = size + 1;
    return GRIB_SUCCESS;
}

size_t grib_accessor_class_cf_var_name_t::string_length(grib_accessor* a)
{
    return 1024;
}
