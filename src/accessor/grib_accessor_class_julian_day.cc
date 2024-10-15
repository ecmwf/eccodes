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

grib_accessor_julian_day_t _grib_accessor_julian_day{};
grib_accessor* grib_accessor_julian_day = &_grib_accessor_julian_day;

void grib_accessor_julian_day_t::init(const long l, grib_arguments* c)
{
    grib_accessor_double_t::init(l, c);
    int n = 0;

    date_   = grib_arguments_get_name(grib_handle_of_accessor(this), c, n++);
    hour_   = grib_arguments_get_name(grib_handle_of_accessor(this), c, n++);
    minute_ = grib_arguments_get_name(grib_handle_of_accessor(this), c, n++);
    second_ = grib_arguments_get_name(grib_handle_of_accessor(this), c, n++);

    length_ = 0;
}

void grib_accessor_julian_day_t::dump(grib_dumper* dumper)
{
    grib_dump_string(dumper, this, NULL);
}

int grib_accessor_julian_day_t::pack_long(const long* val, size_t* len)
{
    const double v = *val;
    return pack_double(&v, len);
}

int grib_accessor_julian_day_t::pack_double(const double* val, size_t* len)
{
    int ret     = 0;
    long hour   = 0;
    long minute = 0;
    long second = 0;
    long date   = 0;
    long year, month, day;

    ret = grib_julian_to_datetime(*val, &year, &month, &day, &hour, &minute, &second);
    if (ret != 0)
        return ret;

    date = year * 10000 + month * 100 + day;

    ret = grib_set_long_internal(grib_handle_of_accessor(this), date_, date);
    if (ret != 0)
        return ret;
    ret = grib_set_long_internal(grib_handle_of_accessor(this), hour_, hour);
    if (ret != 0)
        return ret;
    ret = grib_set_long_internal(grib_handle_of_accessor(this), minute_, minute);
    if (ret != 0)
        return ret;
    ret = grib_set_long_internal(grib_handle_of_accessor(this), second_, second);

    return ret;
}

int grib_accessor_julian_day_t::unpack_long(long* val, size_t* len)
{
    int ret  = 0;
    double v = 0;

    ret  = unpack_double(&v, len);
    *val = (long)v;

    return ret;
}

int grib_accessor_julian_day_t::unpack_double(double* val, size_t* len)
{
    int ret = 0;
    long date, hour, minute, second;
    long year, month, day;

    ret = grib_get_long_internal(grib_handle_of_accessor(this), date_, &date);
    if (ret != GRIB_SUCCESS)
        return ret;
    ret = grib_get_long_internal(grib_handle_of_accessor(this), hour_, &hour);
    if (ret != GRIB_SUCCESS)
        return ret;
    ret = grib_get_long_internal(grib_handle_of_accessor(this), minute_, &minute);
    if (ret != GRIB_SUCCESS)
        return ret;
    ret = grib_get_long_internal(grib_handle_of_accessor(this), second_, &second);
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
