#pragma once

#include "grib_api_internal.h"
#include "step.h"

#include <optional>

std::optional<Step<long>> getStep(grib_handle* h, const std::string& value_key, const std::string& unit_key);
std::optional<Step<long>> getForecastTime(grib_handle* h);
std::optional<Step<long>> getLengthOfTimeRange(grib_handle* h);
std::pair<Step<long>, Step<long>> getTimeRange(grib_handle* h);
//std::pair<Step<long>, Step<long>> getOptTimeRange(grib_handle* h);
bool futureOutputEnabled(grib_handle* h);

template <class T> int getOptTimeRange(grib_handle* h, Step<T>& s_a, Step<T>& s_b);
//std::pair<Step<long>, Step<long>> getOptTimeRange(grib_handle* h) {

template <class T>
int getOptTimeRange(grib_handle* h, Step<T>& s_a, Step<T>& s_b) {
    auto [step_a, step_b] = getTimeRange(h);

    long unit_code = 0;
    if (grib_get_long_internal(h, "stepUnits", &unit_code) != GRIB_SUCCESS)
        return {};

    UnitType wanted_unit{unit_code};
    try {
        if (wanted_unit == Unit::MISSING) {
            std::tie(s_a, s_b) = findCommonUnits(step_a.optimizeUnit(), step_b.optimizeUnit());
        }
        else {
            s_a = static_cast<Step<double>>(step_a).setUnit(unit_code);
            s_b = static_cast<Step<double>>(step_b).setUnit(unit_code);
        }
    }
    catch (...) {
        return GRIB_INVALID_ARGUMENT;
    }

    s_a.hide_hour_unit();
    s_b.hide_hour_unit();
    return GRIB_SUCCESS;
}

