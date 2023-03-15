#pragma once

#include "grib_api_internal.h"
#include <typeinfo>
#include <type_traits>

template <typename T>
int grib_get_array(const grib_handle* h, const char* name, T* val, size_t* length);

template <typename T>
int grib_get_array_internal(const grib_handle* h, const char* name, T* val, size_t* length)
{
    static_assert(std::is_floating_point<T>::value, "Requires floating point numbers");
    int ret = grib_get_array<T>(h, name, val, length);

    if (ret != GRIB_SUCCESS)
        grib_context_log(h->context, GRIB_LOG_ERROR,
                         "unable to get %s as %s array (%s)",
                         name, typeid(T).name(), grib_get_error_message(ret));

    return ret;
}
