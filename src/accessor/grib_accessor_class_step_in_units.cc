
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
#include "grib_accessor_class_step_in_units.h"
#include "step.h"
#include "step_utilities.h"
#include <stdexcept>

grib_accessor_class_step_in_units_t _grib_accessor_class_step_in_units{"step_in_units"};
grib_accessor_class* grib_accessor_class_step_in_units = &_grib_accessor_class_step_in_units;


void grib_accessor_class_step_in_units_t::init(grib_accessor* a, const long l, grib_arguments* c){
    grib_accessor_class_long_t::init(a, l, c);
    grib_accessor_step_in_units_t* self = (grib_accessor_step_in_units_t*)a;
    grib_handle* hand = grib_handle_of_accessor(a);
    int n = 0;

    self->forecast_time_value = grib_arguments_get_name(hand, c, n++);
    self->forecast_time_unit  = grib_arguments_get_name(hand, c, n++);
    self->step_units          = grib_arguments_get_name(hand, c, n++);
    self->time_range_unit     = grib_arguments_get_name(hand, c, n++);
    self->time_range_value    = grib_arguments_get_name(hand, c, n++);
}

void grib_accessor_class_step_in_units_t::dump(grib_accessor* a, grib_dumper* dumper){
    grib_dump_double(dumper, a, NULL);
}

int grib_accessor_class_step_in_units_t::unpack_long(grib_accessor* a, long* val, size_t* len){
    const grib_accessor_step_in_units_t* self = (grib_accessor_step_in_units_t*)a;
    int err = 0;
    long forecast_time_value, forecast_time_unit, step_units;
    grib_handle* h = grib_handle_of_accessor(a);

    if ((err= grib_get_long_internal(h, "stepUnits", &step_units)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_long_internal(h, self->forecast_time_unit, &forecast_time_unit)))
        return err;
    if ((err = grib_get_long_internal(h, self->forecast_time_value, &forecast_time_value)))
        return err;

    try {
        eccodes::Step step{forecast_time_value, forecast_time_unit};
        step.optimize_unit();

        if ((err = grib_set_long_internal(h, "startStepUnit", eccodes::Unit{step_units}.value<long>())) != GRIB_SUCCESS)
            return err;

        *val = step.value<long>(eccodes::Unit{step_units});
    }
    catch (std::exception& e) {
        grib_context_log(h->context, GRIB_LOG_ERROR, "step_in_units: %s", e.what());
        return GRIB_DECODING_ERROR;
    }

    return GRIB_SUCCESS;
}

int grib_accessor_class_step_in_units_t::unpack_double(grib_accessor* a, double * val, size_t* len){
    const grib_accessor_step_in_units_t* self = (grib_accessor_step_in_units_t*)a;
    int err = 0;
    long forecast_time_value, forecast_time_unit, step_units;
    grib_handle* h = grib_handle_of_accessor(a);

    if ((err= grib_get_long_internal(h, "stepUnits", &step_units)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_long_internal(h, self->forecast_time_unit, &forecast_time_unit)))
        return err;
    if ((err = grib_get_long_internal(h, self->forecast_time_value, &forecast_time_value)))
        return err;

    try {
        eccodes::Step step{forecast_time_value, forecast_time_unit};

        if ((err = grib_set_long_internal(h, "startStepUnit", eccodes::Unit{step_units}.value<long>())) != GRIB_SUCCESS)
            return err;

        *val = step.value<double>(eccodes::Unit{step_units});
    }
    catch (std::exception& e) {
        grib_context_log(h->context, GRIB_LOG_ERROR, "step_in_units: %s", e.what());
        return GRIB_DECODING_ERROR;
    }

    return GRIB_SUCCESS;
}

int pack_long_new_(grib_accessor* a, const long start_step_value, const long start_step_unit, const long force_step_units){
    const grib_accessor_step_in_units_t* self = (grib_accessor_step_in_units_t*)a;
    grib_handle* h = grib_handle_of_accessor(a);
    int err = 0;
    long forecast_time_unit;
    long start_step_value_old;
    long start_step_unit_old;
    size_t len = 0;

    if ((err = grib_get_long_internal(h, self->forecast_time_unit, &forecast_time_unit)) != GRIB_SUCCESS)
        return err;
    if ((err = a->unpack_long(&start_step_value_old, &len)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_long_internal(h, "startStepUnit", &start_step_unit_old)) != GRIB_SUCCESS)
        return err;

    eccodes::Step start_step_old(start_step_value_old, start_step_unit_old);
    eccodes::Step forecast_time(start_step_value, start_step_unit);
    eccodes::Step time_range_new{};

    auto time_range_opt = get_step(h, self->time_range_value, self->time_range_unit);

    if (time_range_opt) {
        auto time_range = time_range_opt.value();
        time_range = time_range - (forecast_time - start_step_old);
        if (time_range.value<long>() < 0)
            time_range = eccodes::Step{0l, time_range.unit()};
        auto [sa, sb] = find_common_units(forecast_time.optimize_unit(), time_range.optimize_unit());
        if ((err = set_step(h, self->forecast_time_value, self->forecast_time_unit, sa)) != GRIB_SUCCESS)
            return err;
        if ((err = grib_set_long_internal(h, "startStepUnit", forecast_time.unit().value<long>())) != GRIB_SUCCESS)
            return err;
        if ((err = set_step(h, self->time_range_value, self->time_range_unit, sb)) != GRIB_SUCCESS)
            return err;
        return GRIB_SUCCESS;
    }

    if (eccodes::Unit{force_step_units} == eccodes::Unit{eccodes::Unit::Value::MISSING}) {
        forecast_time.optimize_unit();
    }

    if ((err = grib_set_long_internal(h, "startStepUnit", forecast_time.unit().value<long>())) != GRIB_SUCCESS)
        return err;
    if ((err = set_step(h, self->forecast_time_value, self->forecast_time_unit, forecast_time)) != GRIB_SUCCESS)
        return err;

    return GRIB_SUCCESS;
}

int grib_accessor_class_step_in_units_t::pack_long(grib_accessor* a, const long* val, size_t* len){
    grib_handle* h = grib_handle_of_accessor(a);
    int ret = GRIB_SUCCESS;

    long force_step_units;
    if ((ret = grib_get_long_internal(h, "forceStepUnits", &force_step_units)) != GRIB_SUCCESS)
        return ret;

    long start_step_unit;
    try {
        if (eccodes::Unit{force_step_units} == eccodes::Unit{eccodes::Unit::Value::MISSING}) {
            if ((ret = grib_get_long_internal(h, "startStepUnit", &start_step_unit)) != GRIB_SUCCESS)
                return ret;

            if (eccodes::Unit{start_step_unit} == eccodes::Unit{eccodes::Unit::Value::MISSING})
                start_step_unit = eccodes::Unit{eccodes::Unit::Value::HOUR}.value<long>();
        }
        else {
            start_step_unit = force_step_units;
        }
    }
    catch (std::exception& e) {
        grib_context_log(h->context, GRIB_LOG_ERROR, "step_in_units: %s", e.what());
        return GRIB_DECODING_ERROR;
    }

    ret = pack_long_new_(a, *val, start_step_unit, force_step_units);

    return ret;
}

int grib_accessor_class_step_in_units_t::pack_string(grib_accessor* a, const char* val, size_t* len){
    grib_handle* h = grib_handle_of_accessor(a);
    //long force_step_units = eccodes::Unit(eccodes::Unit::Value::MISSING).value<long>();
    int ret = GRIB_SUCCESS;
    long force_step_units;
    if ((ret = grib_get_long_internal(h, "forceStepUnits", &force_step_units)) != GRIB_SUCCESS)
        return ret;

    try {
        eccodes::Step step = step_from_string(val, eccodes::Unit{force_step_units});
        if ((ret = pack_long_new_(a, step.value<long>(), step.unit().value<long>(), force_step_units)) != GRIB_SUCCESS)
            return ret;
    }
    catch (std::exception& e) {
        grib_context_log(a->context, GRIB_LOG_ERROR, "step_in_units: %s", e.what());
        return GRIB_DECODING_ERROR;
    }

    return GRIB_SUCCESS;
}

int grib_accessor_class_step_in_units_t::unpack_string(grib_accessor* a, char* val, size_t* len){
    const grib_accessor_step_in_units_t* self = (grib_accessor_step_in_units_t*)a;
    grib_handle* h = grib_handle_of_accessor(a);
    int ret = GRIB_SUCCESS;
    long start_step_value = 0;
    long start_step_unit = 0;
    long step_units = 0;
    char fp_format[128] = "%g";
    size_t fp_format_len = sizeof(fp_format);
    int show_hours = a->context->grib_hourly_steps_with_units;

    if ((ret = grib_get_long_internal(h, "startStep", &start_step_value)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_long_internal(h, "startStepUnit", &start_step_unit)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_long_internal(h, self->step_units, &step_units)))
        return ret;
    if ((ret = grib_get_string_internal(h, "formatForDoubles", fp_format, &fp_format_len)) != GRIB_SUCCESS)
        return ret;

    try {
        eccodes::Step step{start_step_value, start_step_unit};
        std::stringstream ss;

        ss << step.value<std::string>(fp_format, show_hours);

        size_t size = ss.str().size() + 1;

        if (*len < size)
            return GRIB_ARRAY_TOO_SMALL;

        *len = size;

        memcpy(val, ss.str().c_str(), size);
    }
    catch (std::exception& e) {
        grib_context_log(h->context, GRIB_LOG_ERROR, "step_in_units: %s", e.what());
        return GRIB_DECODING_ERROR;
    }

    return GRIB_SUCCESS;
}

int grib_accessor_class_step_in_units_t::get_native_type(grib_accessor* a){
    grib_handle* h = grib_handle_of_accessor(a);
    const int show_units_for_hours = a->context->grib_hourly_steps_with_units;

    if (!show_units_for_hours) {
        long step_units = 0;
        if (grib_get_long_internal(h, "stepUnits", &step_units) == GRIB_SUCCESS) {
            if (eccodes::Unit{step_units} == eccodes::Unit::Value::HOUR) {
                return GRIB_TYPE_LONG; // For backward compatibility
            }
        }
    }
    return GRIB_TYPE_STRING;
}
