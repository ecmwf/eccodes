#pragma once

#include "grib_api_internal.h"
#include "step.h"
#include <optional>
//#include "step_range.h"


std::optional<Step> getStep(grib_handle* h, const std::string& value_key, const std::string& unit_key);
//std::optional<Step> getForecastTime(grib_handle* h);
//std::optional<Step> getLengthOfTimeRange(grib_handle* h);
//StepRange getTimeRange(grib_handle* h);
bool futureOutputEnabled(grib_handle* h);

