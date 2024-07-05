
/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_accessor_class_g2end_step.h"
#include "step.h"
#include "step_utilities.h"
#include <stdexcept>

grib_accessor_class_g2end_step_t _grib_accessor_class_g2end_step{ "g2end_step" };
grib_accessor_class* grib_accessor_class_g2end_step = &_grib_accessor_class_g2end_step;


void grib_accessor_class_g2end_step_t::init(grib_accessor* a, const long l, grib_arguments* c)
{
    grib_accessor_class_long_t::init(a, l, c);
    grib_accessor_g2end_step_t* self = (grib_accessor_g2end_step_t*)a;
    int n                            = 0;
    grib_handle* h                   = grib_handle_of_accessor(a);

    self->start_step_value = grib_arguments_get_name(h, c, n++);
    self->step_units       = grib_arguments_get_name(h, c, n++);

    self->year   = grib_arguments_get_name(h, c, n++);
    self->month  = grib_arguments_get_name(h, c, n++);
    self->day    = grib_arguments_get_name(h, c, n++);
    self->hour   = grib_arguments_get_name(h, c, n++);
    self->minute = grib_arguments_get_name(h, c, n++);
    self->second = grib_arguments_get_name(h, c, n++);

    self->year_of_end_of_interval   = grib_arguments_get_name(h, c, n++);
    self->month_of_end_of_interval  = grib_arguments_get_name(h, c, n++);
    self->day_of_end_of_interval    = grib_arguments_get_name(h, c, n++);
    self->hour_of_end_of_interval   = grib_arguments_get_name(h, c, n++);
    self->minute_of_end_of_interval = grib_arguments_get_name(h, c, n++);
    self->second_of_end_of_interval = grib_arguments_get_name(h, c, n++);

    self->time_range_unit     = grib_arguments_get_name(h, c, n++);
    self->time_range_value    = grib_arguments_get_name(h, c, n++);
    self->typeOfTimeIncrement = grib_arguments_get_name(h, c, n++);
    self->numberOfTimeRanges  = grib_arguments_get_name(h, c, n++);
}

void grib_accessor_class_g2end_step_t::dump(grib_accessor* a, grib_dumper* dumper)
{
    grib_dump_double(dumper, a, NULL);
}

// See GRIB-488
bool is_special_expver(const grib_handle* h)
{
    int ret                = 0;
    char strMarsExpVer[50] = {0,};
    char strMarsClass[50] = {0,};
    size_t slen = 50;
    ret         = grib_get_string(h, "mars.class", strMarsClass, &slen);
    if (ret == GRIB_SUCCESS && STR_EQUAL(strMarsClass, "em")) {
        // em = ERA-CLIM model integration for the 20th-century (ERA-20CM)
        slen = 50;
        ret  = grib_get_string(h, "experimentVersionNumber", strMarsExpVer, &slen);
        if (ret == GRIB_SUCCESS && STR_EQUAL(strMarsExpVer, "1605")) {
            return true;  // Special case of expVer 1605 in class "em"
        }
    }

    return false;
}

static int convert_time_range_long_(
    grib_handle* h,
    long stepUnits,                   /* step_units */
    long indicatorOfUnitForTimeRange, /* time_range_unit */
    long* lengthOfTimeRange           /* time_range_value */
)
{
    Assert(lengthOfTimeRange != NULL);

    if (indicatorOfUnitForTimeRange != stepUnits) {
        eccodes::Step time_range{ *lengthOfTimeRange, indicatorOfUnitForTimeRange };
        time_range.set_unit(eccodes::Unit{ stepUnits });
        if (time_range.value<long>() != time_range.value<double>()) {
            return GRIB_DECODING_ERROR;
        }
        *lengthOfTimeRange = time_range.value<long>();
    }

    return GRIB_SUCCESS;
}

static int unpack_one_time_range_long_(grib_accessor* a, long* val, size_t* len)
{
    grib_accessor_g2end_step_t* self = (grib_accessor_g2end_step_t*)a;
    int err                          = 0;
    long start_step_value;
    long step_units;
    long time_range_unit;
    long time_range_value, typeOfTimeIncrement;
    int add_time_range = 1; /* whether we add lengthOfTimeRange */

    grib_handle* h = grib_handle_of_accessor(a);

    if ((err = grib_get_long_internal(h, self->start_step_value, &start_step_value)))
        return err;
    if ((err = grib_get_long_internal(h, self->step_units, &step_units)))
        return err;
    if ((err = grib_get_long_internal(h, self->time_range_unit, &time_range_unit)))
        return err;
    if ((err = grib_get_long_internal(h, self->time_range_value, &time_range_value)))
        return err;
    if ((err = grib_get_long_internal(h, self->typeOfTimeIncrement, &typeOfTimeIncrement)))
        return err;

    err = convert_time_range_long_(h, step_units, time_range_unit, &time_range_value);
    if (err != GRIB_SUCCESS)
        return err;

    if (typeOfTimeIncrement == 1) {
        /* See GRIB-488 */
        /* Note: For this case, lengthOfTimeRange is not related to step and should not be used to calculate step */
        add_time_range = 0;
        if (is_special_expver(h)) {
            add_time_range = 1;
        }
    }
    if (add_time_range) {
        *val = start_step_value + time_range_value;
        if ((err = grib_set_long_internal(h, "endStepUnit", step_units)) != GRIB_SUCCESS)
            return err;
    }
    else {
        *val = start_step_value;
        if ((err = grib_set_long_internal(h, "endStepUnit", step_units)) != GRIB_SUCCESS)
            return err;
    }

    return GRIB_SUCCESS;
}

static int unpack_one_time_range_double_(grib_accessor* a, double* val, size_t* len)
{
    grib_accessor_g2end_step_t* self = (grib_accessor_g2end_step_t*)a;
    int err                          = 0;
    double start_step_value;
    long start_step_unit;
    long step_units;
    long time_range_unit;
    double time_range_value;
    long typeOfTimeIncrement;
    int add_time_range = 1; /* whether we add lengthOfTimeRange */

    grib_handle* h = grib_handle_of_accessor(a);

    if ((err = grib_get_double_internal(h, self->start_step_value, &start_step_value)))
        return err;
    if ((err = grib_get_long_internal(h, "startStepUnit", &start_step_unit)))
        return err;
    if ((err = grib_get_long_internal(h, self->step_units, &step_units)))
        return err;
    if ((err = grib_get_long_internal(h, self->time_range_unit, &time_range_unit)))
        return err;
    if ((err = grib_get_double_internal(h, self->time_range_value, &time_range_value)))
        return err;
    if ((err = grib_get_long_internal(h, self->typeOfTimeIncrement, &typeOfTimeIncrement)))
        return err;

    eccodes::Step start_step{ start_step_value, start_step_unit };
    eccodes::Step time_range{ time_range_value, time_range_unit };

    if (typeOfTimeIncrement == 1) {
        /* See GRIB-488 */
        /* Note: For this case, lengthOfTimeRange is not related to step and should not be used to calculate step */
        add_time_range = 0;
        if (is_special_expver(h)) {
            add_time_range = 1;
        }
    }
    if (add_time_range) {
        *val = (start_step + time_range).value<double>(eccodes::Unit(step_units));
        if ((err = grib_set_long_internal(h, "endStepUnit", step_units)) != GRIB_SUCCESS)
            return err;
    }
    else {
        *val = start_step.value<double>(eccodes::Unit(start_step_unit));
        if ((err = grib_set_long_internal(h, "endStepUnit", start_step_unit)) != GRIB_SUCCESS)
            return err;
    }

    return GRIB_SUCCESS;
}

#define MAX_NUM_TIME_RANGES 16 /* maximum number of time range specifications */
static int unpack_multiple_time_ranges_long_(grib_accessor* a, long* val, size_t* len)
{
    grib_accessor_g2end_step_t* self = (grib_accessor_g2end_step_t*)a;
    int i = 0, err = 0;
    grib_handle* h         = grib_handle_of_accessor(a);
    long numberOfTimeRanges = 0, step_units = 0, start_step_value = 0;

    size_t count                                      = 0;
    long arr_typeOfTimeIncrement[MAX_NUM_TIME_RANGES] = {
        0,
    };
    long arr_coded_unit[MAX_NUM_TIME_RANGES] = {
        0,
    };
    long arr_coded_time_range[MAX_NUM_TIME_RANGES] = {
        0,
    };

    if ((err = grib_get_long_internal(h, self->start_step_value, &start_step_value)))
        return err;
    if ((err = grib_get_long_internal(h, self->step_units, &step_units)))
        return err;
    if ((err = grib_get_long_internal(h, self->numberOfTimeRanges, &numberOfTimeRanges)))
        return err;
    if (numberOfTimeRanges > MAX_NUM_TIME_RANGES) {
        grib_context_log(h->context, GRIB_LOG_ERROR, "Too many time range specifications!");
        return GRIB_DECODING_ERROR;
    }

    count = numberOfTimeRanges;
    /* Get the arrays for the N time ranges */
    if ((err = grib_get_long_array(h, self->typeOfTimeIncrement, arr_typeOfTimeIncrement, &count)))
        return err;
    if ((err = grib_get_long_array(h, self->time_range_unit, arr_coded_unit, &count)))
        return err;
    if ((err = grib_get_long_array(h, self->time_range_value, arr_coded_time_range, &count)))
        return err;

    /* Look in the array of typeOfTimeIncrements for first entry whose typeOfTimeIncrement == 2 */
    for (i = 0; i < count; i++) {
        if (arr_typeOfTimeIncrement[i] == 2) {
            /* Found the required time range. Get the other two keys from it */
            long the_coded_unit       = arr_coded_unit[i];
            long the_coded_time_range = arr_coded_time_range[i];

            err = convert_time_range_long_(h, step_units, the_coded_unit, &the_coded_time_range);
            if (err != GRIB_SUCCESS)
                return err;

            *val = start_step_value + the_coded_time_range;
            return GRIB_SUCCESS;
        }
    }

    grib_context_log(h->context, GRIB_LOG_ERROR,
                     "Cannot calculate endStep. No time range specification with typeOfTimeIncrement = 2");
    return GRIB_DECODING_ERROR;
}

static int unpack_multiple_time_ranges_double_(grib_accessor* a, double* val, size_t* len)
{
    grib_accessor_g2end_step_t* self = (grib_accessor_g2end_step_t*)a;
    int i = 0, err = 0;
    grib_handle* h          = grib_handle_of_accessor(a);
    long numberOfTimeRanges = 0;
    long step_units         = 0;
    long start_step_value   = 0;
    long start_step_unit    = 0;

    size_t count                                      = 0;
    long arr_typeOfTimeIncrement[MAX_NUM_TIME_RANGES] = {
        0,
    };
    long arr_coded_unit[MAX_NUM_TIME_RANGES] = {
        0,
    };
    long arr_coded_time_range[MAX_NUM_TIME_RANGES] = {
        0,
    };

    if ((err = grib_get_long_internal(h, self->start_step_value, &start_step_value)))
        return err;
    if ((err = grib_get_long_internal(h, "startStepUnit", &start_step_unit)))
        return err;

    eccodes::Step start_step{ start_step_value, start_step_unit };

    if ((err = grib_get_long_internal(h, self->step_units, &step_units)))
        return err;

    if ((err = grib_get_long_internal(h, self->numberOfTimeRanges, &numberOfTimeRanges)))
        return err;
    if (numberOfTimeRanges > MAX_NUM_TIME_RANGES) {
        grib_context_log(h->context, GRIB_LOG_ERROR, "Too many time range specifications!");
        return GRIB_DECODING_ERROR;
    }

    count = numberOfTimeRanges;
    /* Get the arrays for the N time ranges */
    if ((err = grib_get_long_array(h, self->typeOfTimeIncrement, arr_typeOfTimeIncrement, &count)))
        return err;
    if ((err = grib_get_long_array(h, self->time_range_unit, arr_coded_unit, &count)))
        return err;
    if ((err = grib_get_long_array(h, self->time_range_value, arr_coded_time_range, &count)))
        return err;

    /* Look in the array of typeOfTimeIncrements for first entry whose typeOfTimeIncrement == 2 */
    for (i = 0; i < count; i++) {
        if (arr_typeOfTimeIncrement[i] == 2) {
            /* Found the required time range. Get the other two keys from it */
            long the_coded_unit       = arr_coded_unit[i];
            long the_coded_time_range = arr_coded_time_range[i];

            eccodes::Step time_range{ the_coded_unit, the_coded_time_range };
            *val = (start_step + time_range).value<double>(eccodes::Unit(step_units));

            return GRIB_SUCCESS;
        }
    }

    grib_context_log(h->context, GRIB_LOG_ERROR,
                     "Cannot calculate endStep. No time range specification with typeOfTimeIncrement = 2");
    return GRIB_DECODING_ERROR;
}

// For the old implementation of unpack_long, see
//  src/deprecated/grib_accessor_class_g2end_step.unpack_long.cc
//
int grib_accessor_class_g2end_step_t::unpack_long(grib_accessor* a, long* val, size_t* len)
{
    grib_accessor_g2end_step_t* self = (grib_accessor_g2end_step_t*)a;
    grib_handle* h                   = grib_handle_of_accessor(a);
    int ret                          = 0;
    long start_step_value = 0;
    long start_step_unit = 0;
    long numberOfTimeRanges = 0;

    if ((ret = grib_get_long_internal(h, self->start_step_value, &start_step_value)))
        return ret;
    if ((ret = grib_get_long_internal(h, "startStepUnit", &start_step_unit)))
        return ret;

    /* point in time */
    if (self->year == NULL) {
        *val = start_step_value;
        if ((ret = grib_set_long_internal(h, "endStepUnit", start_step_unit)))
            return ret;
        return 0;
    }

    Assert(self->numberOfTimeRanges);
    if ((ret = grib_get_long_internal(h, self->numberOfTimeRanges, &numberOfTimeRanges)))
        return ret;
    Assert(numberOfTimeRanges == 1 || numberOfTimeRanges == 2);

    try {
        if (numberOfTimeRanges == 1) {
            ret = unpack_one_time_range_long_(a, val, len);
        }
        else {
            ret = unpack_multiple_time_ranges_long_(a, val, len);
        }
    }
    catch (std::exception& e) {
        grib_context_log(h->context, GRIB_LOG_ERROR, "grib_accessor_g2end_step_t::unpack_long: %s", e.what());
        ret = GRIB_DECODING_ERROR;
    }

    return ret;
}

int grib_accessor_class_g2end_step_t::unpack_double(grib_accessor* a, double* val, size_t* len)
{
    grib_accessor_g2end_step_t* self = (grib_accessor_g2end_step_t*)a;
    grib_handle* h                   = grib_handle_of_accessor(a);
    int ret                          = 0;
    long start_step_value;
    long start_step_unit;
    long numberOfTimeRanges;

    if ((ret = grib_get_long_internal(h, self->start_step_value, &start_step_value)))
        return ret;
    if ((ret = grib_get_long_internal(h, "startStepUnit", &start_step_unit)))
        return ret;

    /* point in time */
    if (self->year == NULL) {
        *val = start_step_value;
        if ((ret = grib_set_long_internal(h, "endStepUnit", start_step_unit)))
            return ret;
        return 0;
    }

    Assert(self->numberOfTimeRanges);
    if ((ret = grib_get_long_internal(h, self->numberOfTimeRanges, &numberOfTimeRanges)))
        return ret;
    Assert(numberOfTimeRanges == 1 || numberOfTimeRanges == 2);

    try {
        if (numberOfTimeRanges == 1) {
            ret = unpack_one_time_range_double_(a, val, len);
        }
        else {
            ret = unpack_multiple_time_ranges_double_(a, val, len);
        }
    }
    catch (std::exception& e) {
        grib_context_log(h->context, GRIB_LOG_ERROR, "grib_accessor_g2end_step_t::unpack_double: %s", e.what());
        ret = GRIB_DECODING_ERROR;
    }

    return ret;
}

static int pack_long_(grib_accessor* a, const long end_step_value, const long end_step_unit)
{
    grib_accessor_g2end_step_t* self = (grib_accessor_g2end_step_t*)a;
    grib_handle* h                   = grib_handle_of_accessor(a);
    int err                          = 0;

    long year;
    long month;
    long day;
    long hour;
    long minute;
    long second;

    long start_step_value;
    long start_step_unit;
    long time_range_unit;
    long year_of_end_of_interval;
    long month_of_end_of_interval;
    long day_of_end_of_interval;
    long hour_of_end_of_interval;
    long minute_of_end_of_interval = 0;
    long second_of_end_of_interval = 0;
    long typeOfTimeIncrement;

    double dend, dstep;
    const int show_units_for_hours = a->context->grib_hourly_steps_with_units;

    eccodes::Step end_step{ end_step_value, end_step_unit };

    /*point in time */
    if (self->year == NULL) {
        if ((err = grib_set_long_internal(h, "startStepUnit", end_step.unit().value<long>())) != GRIB_SUCCESS)
            return err;
        err = grib_set_long_internal(h, self->start_step_value, end_step.value<long>());
        return err;
    }

    if ((err = grib_get_long_internal(h, self->time_range_unit, &time_range_unit)))
        return err;
    if ((err = grib_get_long_internal(h, self->year, &year)))
        return err;
    if ((err = grib_get_long_internal(h, self->month, &month)))
        return err;
    if ((err = grib_get_long_internal(h, self->day, &day)))
        return err;
    if ((err = grib_get_long_internal(h, self->hour, &hour)))
        return err;
    if ((err = grib_get_long_internal(h, self->minute, &minute)))
        return err;
    if ((err = grib_get_long_internal(h, self->second, &second)))
        return err;

    if ((err = grib_get_long_internal(h, self->start_step_value, &start_step_value)))
        return err;
    if ((err = grib_get_long_internal(h, "startStepUnit", &start_step_unit)))
        return err;

    long force_step_units;
    if ((err = grib_get_long_internal(h, "forceStepUnits", &force_step_units)) != GRIB_SUCCESS)
        return err;

    if (eccodes::Unit{ start_step_unit } == eccodes::Unit{ eccodes::Unit::Value::MISSING }) {
        grib_context_log(h->context, GRIB_LOG_ERROR,
                         "missing start step unit");
        return GRIB_WRONG_STEP_UNIT;
    }

    if ((err = grib_get_long_internal(h, self->typeOfTimeIncrement, &typeOfTimeIncrement)))
        return err;

    eccodes::Step start_step{ start_step_value, start_step_unit };
    eccodes::Step time_range = end_step - start_step;

    if (time_range.value<double>() < 0) {
        grib_context_log(h->context, GRIB_LOG_ERROR,
                         "endStep < startStep (%s < %s)",
                         end_step.value<std::string>("%g", show_units_for_hours).c_str(),
                         start_step.value<std::string>("%g", show_units_for_hours).c_str());
        return GRIB_WRONG_STEP;
    }

    err = grib_datetime_to_julian(year, month, day, hour, minute, second, &dend);
    if (err != GRIB_SUCCESS)
        return err;

    dstep = end_step.value<double>(eccodes::Unit{ eccodes::Unit::Value::DAY });
    dend += dstep;

    err = grib_julian_to_datetime(dend, &year_of_end_of_interval, &month_of_end_of_interval,
                                  &day_of_end_of_interval, &hour_of_end_of_interval,
                                  &minute_of_end_of_interval, &second_of_end_of_interval);
    if (err != GRIB_SUCCESS)
        return err;

    if ((err = grib_set_long_internal(h, self->year_of_end_of_interval, year_of_end_of_interval)))
        return err;
    if ((err = grib_set_long_internal(h, self->month_of_end_of_interval, month_of_end_of_interval)))
        return err;
    if ((err = grib_set_long_internal(h, self->day_of_end_of_interval, day_of_end_of_interval)))
        return err;
    if ((err = grib_set_long_internal(h, self->hour_of_end_of_interval, hour_of_end_of_interval)))
        return err;
    if ((err = grib_set_long_internal(h, self->minute_of_end_of_interval, minute_of_end_of_interval)))
        return err;
    if ((err = grib_set_long_internal(h, self->second_of_end_of_interval, second_of_end_of_interval)))
        return err;

    const char* forecast_time_value_key = "forecastTime";
    const char* forecast_time_unit_key  = "indicatorOfUnitOfTimeRange";
    eccodes::Step forecast_time_opt;
    eccodes::Step time_range_opt;
    if (eccodes::Unit{ force_step_units } == eccodes::Unit{ eccodes::Unit::Value::MISSING }) {
        std::tie(forecast_time_opt, time_range_opt) = find_common_units(start_step.optimize_unit(), time_range.optimize_unit());
    }
    else {
        forecast_time_opt = eccodes::Step{ start_step.value<long>(eccodes::Unit{ force_step_units }), eccodes::Unit{ force_step_units } };
        time_range_opt    = eccodes::Step{ time_range.value<long>(eccodes::Unit{ force_step_units }), eccodes::Unit{ force_step_units } };
    }

    if ((err = grib_set_long_internal(grib_handle_of_accessor(a), self->time_range_value, time_range_opt.value<long>())) != GRIB_SUCCESS)
        return err;
    if ((err = grib_set_long_internal(grib_handle_of_accessor(a), self->time_range_unit, time_range_opt.unit().value<long>())) != GRIB_SUCCESS)
        return err;
    if ((err = grib_set_long_internal(grib_handle_of_accessor(a), forecast_time_value_key, forecast_time_opt.value<long>())) != GRIB_SUCCESS)
        return err;
    if ((err = grib_set_long_internal(grib_handle_of_accessor(a), forecast_time_unit_key, forecast_time_opt.unit().value<long>())) != GRIB_SUCCESS)
        return err;

    return GRIB_SUCCESS;
}

int grib_accessor_class_g2end_step_t::unpack_string(grib_accessor* a, char* val, size_t* len)
{
    grib_accessor_g2end_step_t* self = (grib_accessor_g2end_step_t*)a;
    grib_handle* h                   = grib_handle_of_accessor(a);
    int ret                          = 0;
    char fp_format[128]              = "%g";
    size_t fp_format_len             = sizeof(fp_format);
    size_t step_len                  = 0;
    long step_value;
    long step_units;
    const int show_units_for_hours = a->context->grib_hourly_steps_with_units;

    if ((ret = unpack_long(a, &step_value, &step_len)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_long_internal(h, self->step_units, &step_units)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_string(h, "formatForDoubles", fp_format, &fp_format_len)) != GRIB_SUCCESS)
        return ret;

    try {
        eccodes::Step step(step_value, step_units);
        step.set_unit(step_units);

        std::stringstream ss;

        ss << step.value<std::string>(fp_format, show_units_for_hours);

        size_t size = ss.str().size() + 1;

        if (*len < size)
            return GRIB_ARRAY_TOO_SMALL;

        *len = size;

        memcpy(val, ss.str().c_str(), size);
    }
    catch (std::exception& e) {
        grib_context_log(h->context, GRIB_LOG_ERROR, "grib_accessor_g2end_step_t::unpack_string: %s", e.what());
        return GRIB_DECODING_ERROR;
    }

    return GRIB_SUCCESS;
}

int grib_accessor_class_g2end_step_t::pack_long(grib_accessor* a, const long* val, size_t* len)
{
    grib_handle* h = grib_handle_of_accessor(a);
    int ret;

    long force_step_units;
    if ((ret = grib_get_long_internal(h, "forceStepUnits", &force_step_units)) != GRIB_SUCCESS)
        return ret;

    try {
        long end_step_unit;
        if (eccodes::Unit{ force_step_units } == eccodes::Unit{ eccodes::Unit::Value::MISSING }) {
            if ((ret = grib_get_long_internal(h, "endStepUnit", &end_step_unit)) != GRIB_SUCCESS)
                return ret;

            if (eccodes::Unit{ end_step_unit } == eccodes::Unit{ eccodes::Unit::Value::MISSING })
                end_step_unit = eccodes::Unit{ eccodes::Unit::Value::HOUR }.value<long>();
        }
        else {
            end_step_unit = force_step_units;
        }
        ret = pack_long_(a, *val, end_step_unit);
    }
    catch (std::exception& e) {
        grib_context_log(h->context, GRIB_LOG_ERROR, "grib_accessor_g2end_step_t::pack_long: %s", e.what());
        return GRIB_DECODING_ERROR;
    }
    return ret;
}

int grib_accessor_class_g2end_step_t::pack_string(grib_accessor* a, const char* val, size_t* len)
{
    grib_handle* h = grib_handle_of_accessor(a);
    int ret        = 0;
    long force_step_units;
    if ((ret = grib_get_long_internal(h, "forceStepUnits", &force_step_units)) != GRIB_SUCCESS)
        return ret;

    try {
        eccodes::Step end_step = step_from_string(val, eccodes::Unit{ force_step_units });
        end_step.optimize_unit();

        if ((ret = grib_set_long_internal(h, "endStepUnit", end_step.unit().value<long>())) != GRIB_SUCCESS)
            return ret;

        if ((ret = pack_long_(a, end_step.value<long>(), end_step.unit().value<long>())) != GRIB_SUCCESS)
            return ret;
    }
    catch (std::exception& e) {
        grib_context_log(h->context, GRIB_LOG_ERROR, "grib_accessor_g2end_step_t::pack_string: %s", e.what());
        return GRIB_DECODING_ERROR;
    }
    return GRIB_SUCCESS;
}

int grib_accessor_class_g2end_step_t::get_native_type(grib_accessor* a)
{
    grib_handle* h                 = grib_handle_of_accessor(a);
    const int show_units_for_hours = a->context->grib_hourly_steps_with_units;

    if (!show_units_for_hours) {
        long step_units = 0;
        if (grib_get_long_internal(h, "stepUnits", &step_units) == GRIB_SUCCESS) {
            if (eccodes::Unit{ step_units } == eccodes::Unit::Value::HOUR) {
                return GRIB_TYPE_LONG;  // For backward compatibility
            }
        }
    }

    return GRIB_TYPE_STRING;
}
