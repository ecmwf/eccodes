#pragma once

#include "grib_api_internal.h"
#include "step.h"
#include <optional>

std::optional<Step> get_step(grib_handle* h, const std::string& value_key, const std::string& unit_key);
bool is_future_output_enabled(grib_handle* h);
int set_step(grib_handle* h, const std::string& value_key, const std::string& unit_key, const Step& step);
long get_step_units(grib_handle* h);

//template <typename T>
//int set_step(grib_handle* h, const std::string& value_key, const std::string& unit_key, const Step& step)
//{
//    int err;
//    if constexpr (std::is_same_v<T, long>) {
//        if ((err = grib_set_long_internal(h, value_key.c_str(), step.value<T>())) != GRIB_SUCCESS)
//            return err;
//    }
//    else if constexpr (std::is_same_v<T, double>) {
//        if ((err = grib_set_double_internal(h, value_key.c_str(), step.value<T>())) != GRIB_SUCCESS)
//            return err;
//    }
//    else {
//        return GRIB_NOT_IMPLEMENTED;
//    }

//    if ((err = grib_set_long_internal(h, unit_key.c_str(), step.unit().to_long())) != GRIB_SUCCESS)
//        return err;
//    return GRIB_SUCCESS;
//}


