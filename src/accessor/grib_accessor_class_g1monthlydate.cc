/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_accessor_class_g1monthlydate.h"

grib_accessor_class_g1monthlydate_t _grib_accessor_class_g1monthlydate{ "g1monthlydate" };
grib_accessor_class* grib_accessor_class_g1monthlydate = &_grib_accessor_class_g1monthlydate;


void grib_accessor_class_g1monthlydate_t::init(grib_accessor* a, const long l, grib_arguments* c)
{
    grib_accessor_class_long_t::init(a, l, c);
    grib_accessor_g1monthlydate_t* self = (grib_accessor_g1monthlydate_t*)a;

    self->date = grib_arguments_get_name(grib_handle_of_accessor(a), c, 0);
    a->flags |= GRIB_ACCESSOR_FLAG_READ_ONLY;
}

int grib_accessor_class_g1monthlydate_t::unpack_long(grib_accessor* a, long* val, size_t* len)
{
    grib_accessor_g1monthlydate_t* self = (grib_accessor_g1monthlydate_t*)a;

    long date = 0;

    grib_get_long_internal(grib_handle_of_accessor(a), self->date, &date);

    date /= 100;
    date *= 100;
    date += 1;

    *val = date;

    return GRIB_SUCCESS;
}
