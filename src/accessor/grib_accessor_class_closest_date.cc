
/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_api_internal.h"
#include "grib_accessor_class_closest_date.h"
#include <float.h>

grib_accessor_class_closest_date_t _grib_accessor_class_closest_date{"closest_date"};
grib_accessor_class* grib_accessor_class_closest_date = &_grib_accessor_class_closest_date;


void grib_accessor_class_closest_date_t::init(grib_accessor* a, const long l, grib_arguments* c){
    grib_accessor_class_double_t::init(a, l, c);
    grib_accessor_closest_date_t* self = (grib_accessor_closest_date_t*)a;
    grib_handle* h = grib_handle_of_accessor(a);
    int n = 0;

    self->dateLocal = grib_arguments_get_name(h, c, n++);
    self->timeLocal = grib_arguments_get_name(h, c, n++);
    self->numForecasts = grib_arguments_get_name(h, c, n++);
    self->year   = grib_arguments_get_name(h, c, n++);
    self->month  = grib_arguments_get_name(h, c, n++);
    self->day    = grib_arguments_get_name(h, c, n++);
    self->hour   = grib_arguments_get_name(h, c, n++);
    self->minute = grib_arguments_get_name(h, c, n++);
    self->second = grib_arguments_get_name(h, c, n++);

    a->length = 0;
}

void grib_accessor_class_closest_date_t::dump(grib_accessor* a, grib_dumper* dumper){
    grib_dump_string(dumper, a, NULL);
}

int grib_accessor_class_closest_date_t::unpack_long(grib_accessor* a, long* val, size_t* len){
    int ret  = 0;
    double v = 0;

    ret  = unpack_double(a, &v, len);
    *val = (long)v;

    return ret;
}

/* Sets val to the 'index' of the closes date */
int grib_accessor_class_closest_date_t::unpack_double(grib_accessor* a, double* val, size_t* len){
    const grib_accessor_closest_date_t* self = (grib_accessor_closest_date_t*)a;

    int err = 0;
    long num_forecasts = 0; /* numberOfForecastsUsedInLocalTime */
    /* These relate to the date and time in Section 1 */
    long ymdLocal, hmsLocal, yearLocal, monthLocal, dayLocal, hourLocal, minuteLocal, secondLocal;
    double jLocal = 0;
    double minDiff = DBL_MAX;
    size_t i = 0;
    size_t size = 0; /* number of elements in the array keys - should be = numberOfForecastsUsedInLocalTime */

    /* These relate to the forecast dates and times in Section 4 */
    long *yearArray, *monthArray, *dayArray, *hourArray, *minuteArray, *secondArray;

    grib_handle* h = grib_handle_of_accessor(a);
    const grib_context* c = a->context;
    *val = -1; /* initialise to an invalid index */

    if ((err = grib_get_long_internal(h, self->numForecasts, &num_forecasts)) != GRIB_SUCCESS) return err;
    Assert(num_forecasts > 1);

    if ((err = grib_get_long(h, self->dateLocal, &ymdLocal)) != GRIB_SUCCESS) return err;
    yearLocal = ymdLocal / 10000;
    ymdLocal %= 10000;
    monthLocal = ymdLocal / 100;
    ymdLocal %= 100;
    dayLocal = ymdLocal;

    if ((err= grib_get_long(h, self->timeLocal, &hmsLocal)) != GRIB_SUCCESS) return err;
    hourLocal = hmsLocal / 100;
    hmsLocal %= 100;
    minuteLocal = hmsLocal / 100;
    hmsLocal %= 100;
    secondLocal = hmsLocal;

    if ((err = grib_get_size(h, self->year, &size)) != GRIB_SUCCESS) return err;
    Assert(size == (size_t)num_forecasts);
    yearArray = (long*)grib_context_malloc_clear(c, size * sizeof(long));
    if ((err = grib_get_long_array_internal(h, self->year, yearArray, &size)) != GRIB_SUCCESS) return err;

    if ((err = grib_get_size(h, self->month, &size)) != GRIB_SUCCESS) return err;
    Assert(size == (size_t)num_forecasts);
    monthArray = (long*)grib_context_malloc_clear(c, size * sizeof(long));
    if ((err = grib_get_long_array_internal(h, self->month, monthArray, &size)) != GRIB_SUCCESS) return err;

    if ((err = grib_get_size(h, self->day, &size)) != GRIB_SUCCESS) return err;
    Assert(size == (size_t)num_forecasts);
    dayArray = (long*)grib_context_malloc_clear(c, size * sizeof(long));
    if ((err = grib_get_long_array_internal(h, self->day, dayArray, &size)) != GRIB_SUCCESS) return err;

    if ((err = grib_get_size(h, self->hour, &size)) != GRIB_SUCCESS) return err;
    Assert(size == (size_t)num_forecasts);
    hourArray = (long*)grib_context_malloc_clear(c, size * sizeof(long));
    if ((err = grib_get_long_array_internal(h, self->hour, hourArray, &size)) != GRIB_SUCCESS) return err;

    if ((err = grib_get_size(h, self->minute, &size)) != GRIB_SUCCESS) return err;
    Assert(size == (size_t)num_forecasts);
    minuteArray = (long*)grib_context_malloc_clear(c, size * sizeof(long));
    if ((err = grib_get_long_array_internal(h, self->minute, minuteArray, &size)) != GRIB_SUCCESS) return err;

    if ((err = grib_get_size(h, self->second, &size)) != GRIB_SUCCESS) return err;
    Assert(size == (size_t)num_forecasts);
    secondArray = (long*)grib_context_malloc_clear(c, size * sizeof(long));
    if ((err = grib_get_long_array_internal(h, self->second, secondArray, &size)) != GRIB_SUCCESS) return err;

    grib_datetime_to_julian(yearLocal, monthLocal, dayLocal, hourLocal, minuteLocal, secondLocal, &jLocal);
    for(i=0; i< size; ++i) {
        double jval = 0, diff = 0;
        grib_datetime_to_julian(yearArray[i], monthArray[i], dayArray[i],
                                hourArray[i], minuteArray[i], secondArray[i], &jval);
        diff = jLocal - jval;
        if (diff >= 0 && diff < minDiff) {
            minDiff = diff;
            *val = i;
        }
    }
    if (*val == -1) {
        grib_context_log(c, GRIB_LOG_ERROR, "Failed to find a date/time amongst forecasts used in local time");
        err = GRIB_DECODING_ERROR;
        goto cleanup;
    }

cleanup:
    grib_context_free(c, yearArray);
    grib_context_free(c, monthArray);
    grib_context_free(c, dayArray);
    grib_context_free(c, hourArray);
    grib_context_free(c, minuteArray);
    grib_context_free(c, secondArray);

    return err;
}
