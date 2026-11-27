/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "IsoDateTime.h"

namespace eccodes::accessor
{

void IsoDateTime::init(const long len, grib_arguments* args)
{
    Gen::init(len, args);
    const int count = args->get_count();
    int n = 0;

    if (count == 6) {
        year_ = args->get_name(n++);
        month_ = args->get_name(n++);
        day_ = args->get_name(n++);
        hour_ = args->get_name(n++);
        minute_ = args->get_name(n++);
        second_ = args->get_name(n++);
    }
    else {
        ECCODES_ASSERT(count == 2 || count == 3 || count == 5);
        date_ = args->get_name(n++);
        time_ = args->get_name(n++);
        if (count >= 3)
            second_ = args->get_name(n++);
        if (count == 5) {
            step_ = args->get_name(n++);
            stepUnits_ = args->get_name(n++);
        }
    }
    length_ = 0;
    flags_ |= GRIB_ACCESSOR_FLAG_READ_ONLY;
}

// Read the date (YYYYMMDD), the time and the seconds to add (from a step in seconds)
int IsoDateTime::get_components(long* date, long* hour, long* minute, long* second, long* extra_seconds)
{
    grib_handle* h = get_enclosing_handle();
    int err = 0;
    *second = *extra_seconds = 0;

    if (year_) {
        long year = 0, month = 0, day = 0;
        if ((err = grib_get_long_internal(h, year_, &year)) ||
            (err = grib_get_long_internal(h, month_, &month)) ||
            (err = grib_get_long_internal(h, day_, &day)) ||
            (err = grib_get_long_internal(h, hour_, hour)) ||
            (err = grib_get_long_internal(h, minute_, minute)) ||
            (err = grib_get_long_internal(h, second_, second)))
            return err;
        // Out-of-range components must not turn into another valid date
        *date = (month < 1 || month > 12 || day < 1 || day > 31) ? -1 : year * 10000 + month * 100 + day;
        return GRIB_SUCCESS;
    }

    long time = 0;  // hhmm
    if ((err = grib_get_long_internal(h, date_, date)) ||
        (err = grib_get_long_internal(h, time_, &time)))
        return err;
    if (second_ && (err = grib_get_long_internal(h, second_, second)))
        return err;
    *hour = time / 100;
    *minute = time % 100;

    if (step_) {
        // validityTime has a resolution of minutes: a step in seconds is truncated there
        const long stepUnitSecond = 13;  // stepUnits.table
        long step = 0, stepUnits = 0;
        if (grib_get_long(h, stepUnits_, &stepUnits) == GRIB_SUCCESS && stepUnits == stepUnitSecond &&
            grib_get_long(h, step_, &step) == GRIB_SUCCESS && step > 0 && step != GRIB_MISSING_LONG)
            *extra_seconds = step % 60;
    }
    return GRIB_SUCCESS;
}

int IsoDateTime::unpack_string(char* val, size_t* len)
{
    long date = 0, hour = 0, minute = 0, second = 0, extra_seconds = 0;
    int err = get_components(&date, &hour, &minute, &second, &extra_seconds);
    if (err) return err;

    // The year must have 4 digits (GRIB1 gives dataDate=MMDD when the year is missing).
    // Invalid days (e.g. 31 February) do not survive the round trip through the julian day.
    const long year = date / 10000;
    const bool date_ok = year >= 1 && year <= 9999 && grib_julian_to_date(grib_date_to_julian(date)) == date;
    const bool time_ok = hour >= 0 && minute >= 0 && minute < 60 && second >= 0 && second < 60 &&
                         (hour < 24 || (hour == 24 && minute == 0 && second == 0));
    if (!date_ok || !time_ok) {
        grib_context_log(context_, GRIB_LOG_ERROR, "%s: Invalid date/time: date=%ld hour=%ld minute=%ld second=%ld",
                         name_, date, hour, minute, second);
        return GRIB_DECODING_ERROR;
    }

    // 24:00 and the extra seconds can move the time to the next day
    long seconds_of_day = hour * 3600 + minute * 60 + second + extra_seconds;
    if (seconds_of_day >= 86400) {
        date = grib_julian_to_date(grib_date_to_julian(date) + seconds_of_day / 86400);
        seconds_of_day %= 86400;
        if (date / 10000 > 9999) {
            grib_context_log(context_, GRIB_LOG_ERROR, "%s: Date after the year 9999", name_);
            return GRIB_DECODING_ERROR;
        }
    }

    const size_t needed = string_length() + 1;
    if (*len < needed) {
        grib_context_log(context_, GRIB_LOG_ERROR, "%s: Buffer too small for %s. It is %zu bytes long (len=%zu)",
                         accessor_type().c_str(), name_, needed, *len);
        *len = needed;
        return GRIB_BUFFER_TOO_SMALL;
    }
    snprintf(val, needed, "%04ld-%02ld-%02ldT%02ld:%02ld:%02ld+00:00",
             date / 10000, (date / 100) % 100, date % 100,
             seconds_of_day / 3600, (seconds_of_day / 60) % 60, seconds_of_day % 60);
    *len = needed - 1;
    return GRIB_SUCCESS;
}

}  // namespace eccodes::accessor
