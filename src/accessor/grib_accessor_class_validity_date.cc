/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_accessor_class_validity_date.h"

grib_accessor_class_validity_date_t _grib_accessor_class_validity_date{ "validity_date" };
grib_accessor_class* grib_accessor_class_validity_date = &_grib_accessor_class_validity_date;

void grib_accessor_class_validity_date_t::init(grib_accessor* a, const long l, grib_arguments* c)
{
    grib_accessor_class_long_t::init(a, l, c);
    grib_accessor_validity_date_t* self = (grib_accessor_validity_date_t*)a;
    grib_handle* hand                   = grib_handle_of_accessor(a);
    int n                               = 0;

    self->date      = grib_arguments_get_name(hand, c, n++);
    self->time      = grib_arguments_get_name(hand, c, n++);
    self->step      = grib_arguments_get_name(hand, c, n++);
    self->stepUnits = grib_arguments_get_name(hand, c, n++);
    self->year      = grib_arguments_get_name(hand, c, n++);
    self->month     = grib_arguments_get_name(hand, c, n++);
    self->day       = grib_arguments_get_name(hand, c, n++);

    a->flags |= GRIB_ACCESSOR_FLAG_READ_ONLY;
}

int grib_accessor_class_validity_date_t::unpack_long(grib_accessor* a, long* val, size_t* len)
{
    grib_accessor_validity_date_t* self = (grib_accessor_validity_date_t*)a;
    grib_handle* hand                   = grib_handle_of_accessor(a);
    int ret                             = 0;
    long date                           = 0;
    long time                           = 0;
    long step                           = 0;
    long stepUnits                      = 0;
    long hours = 0, minutes = 0, step_mins = 0, tmp, tmp_hrs;

    if (self->year) {
        long year, month, day;
        if ((ret = grib_get_long_internal(hand, self->year, &year)) != GRIB_SUCCESS)
            return ret;
        if ((ret = grib_get_long_internal(hand, self->month, &month)) != GRIB_SUCCESS)
            return ret;
        if ((ret = grib_get_long_internal(hand, self->day, &day)) != GRIB_SUCCESS)
            return ret;
        *val = year * 10000 + month * 100 + day;
        return GRIB_SUCCESS;
    }
    if ((ret = grib_get_long_internal(hand, self->date, &date)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_long_internal(hand, self->time, &time)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_long(hand, self->step, &step)) != GRIB_SUCCESS) {
        if ((ret = grib_get_long_internal(hand, "endStep", &step)) != GRIB_SUCCESS) {
            return ret; /* See ECC-817 */
        }
    }

    if (self->stepUnits) {
        if ((ret = grib_get_long_internal(hand, self->stepUnits, &stepUnits)) != GRIB_SUCCESS)
            return ret;
        step_mins = convert_to_minutes(step, stepUnits);
    }

    minutes = time % 100;
    hours   = time / 100;
    tmp     = minutes + step_mins; /* add the step to our minutes */
    tmp_hrs = tmp / 60;            /* how many hours and mins is that? */
    hours += tmp_hrs;              /* increment hours */

    date = grib_date_to_julian(date);
    /* does the new 'hours' exceed 24? if so increment julian */
    while (hours >= 24) {
        date++;
        hours -= 24;
    }
    /* GRIB-29: Negative forecast time */
    while (hours < 0) {
        date--;
        hours += 24;
    }

    if (*len < 1)
        return GRIB_ARRAY_TOO_SMALL;

    *val = grib_julian_to_date(date);

    return GRIB_SUCCESS;
}
