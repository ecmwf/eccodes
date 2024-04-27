
/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_accessor_class_budgdate.h"

grib_accessor_class_budgdate_t _grib_accessor_class_budgdate{"budgdate"};
grib_accessor_class* grib_accessor_class_budgdate = &_grib_accessor_class_budgdate;


void grib_accessor_class_budgdate_t::init(grib_accessor* a, const long l, grib_arguments* c){
    grib_accessor_class_long_t::init(a, l, c);
    grib_accessor_budgdate_t* self = (grib_accessor_budgdate_t*)a;
    int n                        = 0;

    self->year  = grib_arguments_get_name(grib_handle_of_accessor(a), c, n++);
    self->month = grib_arguments_get_name(grib_handle_of_accessor(a), c, n++);
    self->day   = grib_arguments_get_name(grib_handle_of_accessor(a), c, n++);
}

int grib_accessor_class_budgdate_t::unpack_long(grib_accessor* a, long* val, size_t* len){
    int ret                      = 0;
    grib_accessor_budgdate_t* self = (grib_accessor_budgdate_t*)a;

    long year  = 0;
    long month = 0;
    long day   = 0;

    if ((ret = grib_get_long_internal(grib_handle_of_accessor(a), self->day, &day)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_long_internal(grib_handle_of_accessor(a), self->month, &month)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_long_internal(grib_handle_of_accessor(a), self->year, &year)) != GRIB_SUCCESS)
        return ret;

    if (*len < 1)
        return GRIB_WRONG_ARRAY_SIZE;

    val[0] = (1900 + year) * 10000 + month * 100 + day;

    return ret;
}

/* TODO: Check for a valid date */
int grib_accessor_class_budgdate_t::pack_long(grib_accessor* a, const long* val, size_t* len){
    int ret                      = 0;
    long v                       = val[0];
    grib_accessor_budgdate_t* self = (grib_accessor_budgdate_t*)a;

    long year  = 0;
    long month = 0;
    long day   = 0;

    if (*len != 1)
        return GRIB_WRONG_ARRAY_SIZE;

    year = v / 10000;
    v %= 10000;
    month = v / 100;
    v %= 100;
    day = v;

    year -= 1900;

    Assert(year < 255);

    if ((ret = grib_set_long_internal(grib_handle_of_accessor(a), self->day, day)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_set_long_internal(grib_handle_of_accessor(a), self->month, month)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_set_long_internal(grib_handle_of_accessor(a), self->year, year)) != GRIB_SUCCESS)
        return ret;

    return ret;
}
