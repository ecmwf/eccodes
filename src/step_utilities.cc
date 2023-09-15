#include "step_utilities.h"
#include <type_traits>


std::optional<Step> get_step(grib_handle* h, const std::string& value_key, const std::string& unit_key)
{
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


int set_step(grib_handle* h, const std::string& value_key, const std::string& unit_key, const Step& step)
{
    int err;
    Step step_copy = step.copy();
    step_copy.optimize_unit();
    if ((err = grib_set_long_internal(h, value_key.c_str(), step_copy.value<long>())) != GRIB_SUCCESS)
        return err;
    if ((err = grib_set_long_internal(h, unit_key.c_str(), step_copy.unit().to_long())) != GRIB_SUCCESS)
        return err;
    return GRIB_SUCCESS;
}


bool is_future_output_enabled(grib_handle* h)
{
    return h->context->is_future_step_format > 0;
}
