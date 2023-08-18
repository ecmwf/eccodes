#pragma once

#include "grib_api_internal.h"
#include "step.h"

#include <optional>

std::optional<Step<long>> getStep(grib_handle* h, const std::string& value_key, const std::string& unit_key);
std::optional<Step<long>> getForecastTime(grib_handle* h);
std::optional<Step<long>> getLengthOfTimeRange(grib_handle* h);
std::pair<Step<long>, Step<long>> getTimeRange(grib_handle* h);
bool futureOutputEnabled(grib_handle* h);
