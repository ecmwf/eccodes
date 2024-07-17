/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_accessor_class_offset_file.h"

grib_accessor_class_offset_file_t _grib_accessor_class_offset_file{ "offset_file" };
grib_accessor_class* grib_accessor_class_offset_file = &_grib_accessor_class_offset_file;


void grib_accessor_class_offset_file_t::init(grib_accessor* a, const long l, grib_arguments* c)
{
    grib_accessor_class_double_t::init(a, l, c);
    a->flags |= GRIB_ACCESSOR_FLAG_READ_ONLY;
    a->length = 0;
}

int grib_accessor_class_offset_file_t::unpack_double(grib_accessor* a, double* val, size_t* len)
{
    *val = (double)grib_handle_of_accessor(a)->offset;
    *len = 1;
    return GRIB_SUCCESS;
}

int grib_accessor_class_offset_file_t::unpack_string(grib_accessor* a, char* v, size_t* len)
{
    double val = 0;
    size_t l   = 1;
    char repres[1024] = {0,};
    int err = 0;
    const char* cclass_name = a->cclass->name;

    err = a->unpack_double(&val, &l);
    if (err) return err;

    snprintf(repres, sizeof(repres), "%.0f", val);

    l = strlen(repres) + 1;
    if (l > *len) {
        grib_context_log(a->context, GRIB_LOG_ERROR,
                         "%s: Buffer too small for %s. It is %zu bytes long (len=%zu)",
                         cclass_name, a->name, l, *len);
        *len = l;
        return GRIB_BUFFER_TOO_SMALL;
    }
    grib_context_log(a->context, GRIB_LOG_DEBUG, "%s: Casting double %s to string", __func__, a->name);

    *len = l;

    strcpy(v, repres);
    return GRIB_SUCCESS;
}
