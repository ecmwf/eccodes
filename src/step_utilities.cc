#include "step_utilities.h"


std::optional<Step<long>> getStep(grib_handle* h, const std::string& value_key, const std::string& unit_key){
    if (grib_is_defined(h, unit_key.c_str()) && grib_is_defined(h, value_key.c_str())) {
        long unit = 0;
        if (grib_get_long_internal(h, unit_key.c_str(), &unit) != GRIB_SUCCESS)
            return {};

        long value = 0;
        if (grib_get_long_internal(h, value_key.c_str(), &value) != GRIB_SUCCESS)
            return {};

        return Step<long>(value, unit);
    }
    else {
        return {};
    }
}

std::optional<Step<long>> getForecastTime(grib_handle* h) {
    return getStep(h, "forecastTime", "indicatorOfUnitOfTimeRange");
}

std::optional<Step<long>> getLengthOfTimeRange(grib_handle* h) {
    return getStep(h, "lengthOfTimeRange", "indicatorOfUnitForTimeRange");
}

std::pair<Step<long>, Step<long>> getTimeRange(grib_handle* h) {
    auto forecast_time = getForecastTime(h);
    auto length_of_time_range = getLengthOfTimeRange(h);
    return {forecast_time.value(), forecast_time.value() + length_of_time_range.value_or(Step<long>())};
}

bool futureOutputEnabled(grib_handle* h) {
    int ret = 0;
    size_t stepOutputFormatSize = 128;
    char stepOutputFormat[stepOutputFormatSize];
    if ((ret = grib_get_string_internal(h, "stepOutputFormat", stepOutputFormat, &stepOutputFormatSize)) != GRIB_SUCCESS)
        return ret;
    return strcmp(stepOutputFormat, "future") == 0;
}
