/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "OptimalStepUnits.h"
#include "step_utilities.h"

eccodes::accessor::OptimalStepUnits _grib_accessor_optimal_step_units;
eccodes::Accessor* grib_accessor_optimal_step_units = &_grib_accessor_optimal_step_units;

namespace eccodes::accessor
{

void OptimalStepUnits::init(const long l, grib_arguments* c)
{
    Gen::init(l, c);
    grib_handle* hand = get_enclosing_handle();
    int n = 0;

    forecast_time_value_ = c->get_name(hand, n++);
    forecast_time_unit_  = c->get_name(hand, n++);
    time_range_value_    = c->get_name(hand, n++);
    time_range_unit_     = c->get_name(hand, n++);
    length_              = 0;
    overwriteStepUnits_  = eccodes::Unit{ eccodes::Unit::Value::MISSING }.value<long>();
}

void OptimalStepUnits::dump(eccodes::Dumper* dumper)
{
    dumper->dump_string(this, NULL);
}

size_t OptimalStepUnits::string_length()
{
    return 255;
}

int OptimalStepUnits::pack_expression(grib_expression* e)
{
    const char* cval  = NULL;
    int ret           = 0;
    long lval         = 0;
    size_t len        = 1;
    grib_handle* hand = get_enclosing_handle();

    if (strcmp(e->class_name(), "long") == 0) {
        e->evaluate_long(hand, &lval); /* TODO: check return value */
        ret = pack_long(&lval, &len);
    }
    else {
        char tmp[1024];
        len  = sizeof(tmp);
        cval = e->evaluate_string(hand, tmp, &len, &ret);
        if (ret != GRIB_SUCCESS) {
            grib_context_log(context_, GRIB_LOG_ERROR,
                             "%s.%s: Unable to evaluate string %s to be set in %s",
                             class_name_, __func__, e->get_name(), name_);
            return ret;
        }
        len = strlen(cval) + 1;
        // if (hand->context->debug)
        //     printf("ECCODES DEBUG grib_accessor_codetable::pack_expression %s %s\n", name_ , cval);
        ret = pack_string(cval, &len);
    }
    return ret;
}

int OptimalStepUnits::pack_long(const long* val, size_t* len)
{
    grib_handle* h = get_enclosing_handle();

    long start_step      = 0;
    long start_step_unit = 0;
    long end_step        = 0;
    long end_step_unit   = 0;
    int ret;

    auto supported_units = eccodes::Unit::list_supported_units();
    try {
        eccodes::Unit unit{ *val };  // throws if not supported
        auto iter = std::find(supported_units.begin(), supported_units.end(), unit);
        if (iter == supported_units.end()) {
            throw std::runtime_error{ "eccodes::Unit not supported" };
        }
    }
    catch (std::exception& e) {
        std::string supported_units_str;
        for (auto& u : supported_units)
            supported_units_str += eccodes::Unit{ u }.value<std::string>() + ",";
        supported_units_str.pop_back();

        std::string msg = std::string{ "Invalid unit: " } + std::to_string(*val) + " (" + e.what() + ")" +
                          ". Available units are: " + supported_units_str;
        grib_context_log(context_, GRIB_LOG_ERROR, "%s", msg.c_str());
        return GRIB_INVALID_ARGUMENT;
    }

    // ECC-1813: When the stepUnits key is used without specifying a value, as in the command
    // "grib-set -s stepUnits=m in.grib out.grib", the following code initiates an indirect update
    // of the low-level keys: forecastTime,indicatorOfUnitOfTimeRange,indicatorOfUnitForTimeRange,lengthOfTimeRange

    overwriteStepUnits_ = *val;
    if ((ret = grib_set_long_internal(h, "forceStepUnits", *val)) != GRIB_SUCCESS)
        return ret;

    if ((ret = grib_get_long_internal(h, "startStep", &start_step)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_long_internal(h, "startStepUnit", &start_step_unit)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_long_internal(h, "endStep", &end_step)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_long_internal(h, "endStepUnit", &end_step_unit)) != GRIB_SUCCESS)
        return ret;

    try {
        eccodes::Step start{ start_step, start_step_unit };
        start.set_unit(*val);
        eccodes::Step end{ end_step, end_step_unit };
        end.set_unit(*val);

        if ((ret = grib_set_long_internal(h, "startStepUnit", start.unit().value<long>())) != GRIB_SUCCESS)
            return ret;
        if ((ret = grib_set_long_internal(h, "startStep", start.value<long>())) != GRIB_SUCCESS)
            return ret;
        if ((ret = grib_set_long_internal(h, "endStepUnit", end.unit().value<long>())) != GRIB_SUCCESS)
            return ret;
        if ((ret = grib_set_long_internal(h, "endStep", end.value<long>())) != GRIB_SUCCESS)
            return ret;
    }
    catch (std::exception& e) {
        std::string msg = std::string{ "Failed to convert steps to: " } + std::to_string(*val) + " (" + e.what() + ")";
        grib_context_log(context_, GRIB_LOG_ERROR, "%s", msg.c_str());
        return GRIB_INTERNAL_ERROR;
    }

    return GRIB_SUCCESS;
}

int OptimalStepUnits::unpack_long(long* val, size_t* len)
{
    grib_handle* h = get_enclosing_handle();

    try {
        if (eccodes::Unit{ overwriteStepUnits_ } != eccodes::Unit{ eccodes::Unit::Value::MISSING }) {
            *val = overwriteStepUnits_;
            return GRIB_SUCCESS;
        }

        auto forecast_time_opt = get_step(h, forecast_time_value_, forecast_time_unit_);
        auto time_range_opt    = get_step(h, time_range_value_, time_range_unit_);

        if (forecast_time_opt && time_range_opt) {
            auto [step_a, step_b] = find_common_units(forecast_time_opt.value().optimize_unit(),
                                                      (forecast_time_opt.value() + time_range_opt.value()).optimize_unit());
            *val = step_a.unit().value<long>();
        }
        else if (forecast_time_opt && !time_range_opt) {
            *val = forecast_time_opt.value().optimize_unit().unit().value<long>();
        }
        else if (!forecast_time_opt && time_range_opt) {
            *val = time_range_opt.value().optimize_unit().unit().value<long>();
        }
        else if (!forecast_time_opt && !time_range_opt) {
            *val = eccodes::Unit{ eccodes::Unit::Value::HOUR }.value<long>();
        }
    }
    catch (std::exception& e) {
        grib_context_log(context_, GRIB_LOG_ERROR, "%s", e.what());
        return GRIB_INTERNAL_ERROR;
    }

    return GRIB_SUCCESS;
}

int OptimalStepUnits::pack_string(const char* val, size_t* len)
{
    try {
        long unit = eccodes::Unit{ val }.value<long>();
        pack_long(&unit, len);
    }
    catch (std::exception& e) {
        auto supported_units = eccodes::Unit::list_supported_units();
        std::string supported_units_str;
        for (auto& u : supported_units)
            supported_units_str += eccodes::Unit{ u }.value<std::string>() + ",";
        supported_units_str.pop_back();

        std::string msg = "Invalid unit: " + std::string(val) + " (" + e.what() + ")" + ". Available units are: " + supported_units_str;
        grib_context_log(context_, GRIB_LOG_ERROR, "%s", msg.c_str());
        return GRIB_INVALID_ARGUMENT;
    }

    return GRIB_SUCCESS;
}

int OptimalStepUnits::unpack_string(char* val, size_t* len)
{
    int ret = 0;
    long unit = 0;
    size_t unit_len = 0;
    if ((ret = unpack_long(&unit, &unit_len)) != GRIB_SUCCESS)
        return ret;
    *len = snprintf(val, *len, "%s", eccodes::Unit{ unit }.value<std::string>().c_str());
    return GRIB_SUCCESS;
}

// Step units are never missing
// If the user does not specify a step unit, we default to hours
int OptimalStepUnits::is_missing()
{
    return 0;
}

long OptimalStepUnits::get_native_type()
{
    return GRIB_TYPE_LONG;
}

}  // namespace eccodes::accessor
