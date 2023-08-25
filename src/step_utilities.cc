//#include "step_range.h"
#include "step_utilities.h"


std::optional<Step> get_step(grib_handle* h, const std::string& value_key, const std::string& unit_key){
    if (grib_is_defined(h, unit_key.c_str()) && grib_is_defined(h, value_key.c_str())) {
        long unit = 0;
        if (grib_get_long_internal(h, unit_key.c_str(), &unit) != GRIB_SUCCESS)
            return {};

        long value = 0;
        if (grib_get_long_internal(h, value_key.c_str(), &value) != GRIB_SUCCESS)
            return {};

        return Step(value, unit);
    }
    else {
        return {};
    }
}


bool is_future_output_enabled(grib_handle* h) {
    size_t step_output_format_size = 128;
    char step_output_format[step_output_format_size];
    int ret = 0;
    if ((ret = grib_get_string_internal(h, "stepOutputFormat", step_output_format, &step_output_format_size)) != GRIB_SUCCESS)
        return ret;
    return strcmp(step_output_format, "future") == 0;
}
