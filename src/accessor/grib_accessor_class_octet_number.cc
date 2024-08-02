/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_accessor_class_octet_number.h"

grib_accessor_class_octet_number_t _grib_accessor_class_octet_number{ "octet_number" };
grib_accessor_class* grib_accessor_class_octet_number = &_grib_accessor_class_octet_number;


void grib_accessor_class_octet_number_t::init(grib_accessor* a, const long l, grib_arguments* c)
{
    grib_accessor_class_long_t::init(a, l, c);
    grib_accessor_octet_number_t* self = (grib_accessor_octet_number_t*)a;

    int n       = 0;
    self->left  = grib_arguments_get_name(grib_handle_of_accessor(a), c, n++);
    self->right = grib_arguments_get_long(grib_handle_of_accessor(a), c, n++);

    a->length = 0;
}

int grib_accessor_class_octet_number_t::unpack_long(grib_accessor* a, long* val, size_t* len)
{
    grib_accessor_octet_number_t* self = (grib_accessor_octet_number_t*)a;
    int ret = GRIB_SUCCESS;
    long offset = a->offset + self->right;

    if ((ret = grib_set_long_internal(grib_handle_of_accessor(a), self->left, offset)) != GRIB_SUCCESS)
        return ret;

    *val = offset;
    *len = 1;

    return ret;
}

int grib_accessor_class_octet_number_t::pack_long(grib_accessor* a, const long* val, size_t* len)
{
    return GRIB_SUCCESS;
}
