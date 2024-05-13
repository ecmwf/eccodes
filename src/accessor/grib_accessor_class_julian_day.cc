
/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_accessor_class_julian_day.h"

grib_accessor_class_julian_day_t _grib_accessor_class_julian_day{"julian_day"};
grib_accessor_class* grib_accessor_class_julian_day = &_grib_accessor_class_julian_day;


void grib_accessor_class_julian_day_t::init(grib_accessor* a, const long l, grib_arguments* c){
    grib_accessor_class_double_t::init(a, l, c);
    grib_accessor_julian_day_t* self = (grib_accessor_julian_day_t*)a;
    int n                          = 0;

    self->date   = grib_arguments_get_name(grib_handle_of_accessor(a), c, n++);
    self->hour   = grib_arguments_get_name(grib_handle_of_accessor(a), c, n++);
    self->minute = grib_arguments_get_name(grib_handle_of_accessor(a), c, n++);
    self->second = grib_arguments_get_name(grib_handle_of_accessor(a), c, n++);

    a->length = 0;
}

void grib_accessor_class_julian_day_t::dump(grib_accessor* a, grib_dumper* dumper){
    grib_dump_string(dumper, a, NULL);
}

int grib_accessor_class_julian_day_t::pack_long(grib_accessor* a, const long* val, size_t* len){
    const double v = *val;
    return pack_double(a, &v, len);
}

int grib_accessor_class_julian_day_t::pack_double(grib_accessor* a, const double* val, size_t* len){
    grib_accessor_julian_day_t* self = (grib_accessor_julian_day_t*)a;
    int ret                        = 0;
    long hour                      = 0;
    long minute                    = 0;
    long second                    = 0;
    long date                      = 0;
    long year, month, day;

    ret = grib_julian_to_datetime(*val, &year, &month, &day, &hour, &minute, &second);
    if (ret != 0)
        return ret;

    date = year * 10000 + month * 100 + day;

    ret = grib_set_long_internal(grib_handle_of_accessor(a), self->date, date);
    if (ret != 0)
        return ret;
    ret = grib_set_long_internal(grib_handle_of_accessor(a), self->hour, hour);
    if (ret != 0)
        return ret;
    ret = grib_set_long_internal(grib_handle_of_accessor(a), self->minute, minute);
    if (ret != 0)
        return ret;
    ret = grib_set_long_internal(grib_handle_of_accessor(a), self->second, second);

    return ret;
}

int grib_accessor_class_julian_day_t::unpack_long(grib_accessor* a, long* val, size_t* len){
    int ret  = 0;
    double v = 0;

    ret  = unpack_double(a, &v, len);
    *val = (long)v;

    return ret;
}

int grib_accessor_class_julian_day_t::unpack_double(grib_accessor* a, double* val, size_t* len){
    int ret = 0;
    long date, hour, minute, second;
    long year, month, day;
    grib_accessor_julian_day_t* self = (grib_accessor_julian_day_t*)a;

    ret = grib_get_long_internal(grib_handle_of_accessor(a), self->date, &date);
    if (ret != GRIB_SUCCESS)
        return ret;
    ret = grib_get_long_internal(grib_handle_of_accessor(a), self->hour, &hour);
    if (ret != GRIB_SUCCESS)
        return ret;
    ret = grib_get_long_internal(grib_handle_of_accessor(a), self->minute, &minute);
    if (ret != GRIB_SUCCESS)
        return ret;
    ret = grib_get_long_internal(grib_handle_of_accessor(a), self->second, &second);
    if (ret != GRIB_SUCCESS)
        return ret;

    year = date / 10000;
    date %= 10000;
    month = date / 100;
    date %= 100;
    day = date;

    ret = grib_datetime_to_julian(year, month, day, hour, minute, second, val);

    return ret;
}
