/*
 * (C) Copyright 2025- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#pragma once

#include <grib_api_internal.h>
#include <tl/expected.hpp>
#include <utility>

namespace eccodes {

// ExceptionHandler is a template class that handles exceptions thrown by a function.
// It catches specific exceptions and returns an expected value or an error status.
template <typename Func, typename... Args>
struct ExceptionHandler
{
    using ReturnType = decltype(std::declval<Func>()(std::declval<Args>()...));
    using ErrorType  = int;

    static tl::expected<ReturnType, ErrorType> handle(Func&& f, Args&&... args)
    {
        try {
            if constexpr (std::is_void_v<ReturnType>) {
                f(std::forward<Args>(args)...);
                return tl::expected<void, ErrorType>{};
            }
            else {
                return f(std::forward<Args>(args)...);
            }
        }
        catch (const std::exception& e) {
            const grib_context* c = grib_context_get_default();
            grib_context_log(c, GRIB_LOG_ERROR, "%s", e.what());
            return tl::unexpected{ GRIB_RUNTIME_ERROR };
        }
        catch (...) {
            const grib_context* c = grib_context_get_default();
            grib_context_log(c, GRIB_LOG_ERROR, "Unknown exception occurred.");
            return tl::unexpected{ GRIB_RUNTIME_ERROR };
        }
    }
};

// Function to handle exceptions and return expected values
template <typename Func, typename... Args>
auto handleExceptions(Func&& f, Args&&... args)
{
    return ExceptionHandler<Func, Args...>::handle(std::forward<Func>(f), std::forward<Args>(args)...);
}

// Helper functions to update error codes and return values
template <typename ErrorType>
int getErrorCode(tl::expected<ErrorType, ErrorType> result)
{
    if (result)
        return result.value();  // Error code originated from function return value
    else
        return result.error();  // Error code originated from exception handling
}

// Helper functions to update error codes and return values
// For functions that return error codes either as return value or through an output parameter
template <typename ValueType, typename ErrorType>
ValueType updateErrorAndReturnValue(tl::expected<ValueType, ErrorType> result, int* err)
{
    if (result) {
        return result.value();
    }
    else {
        *err = result.error();
        return {};
    }
}

template <typename ErrorType>
void updateErrorAndReturnValue(tl::expected<void, ErrorType> result, int* err)
{
    if (!result) {
        *err = result.error();
    }
}

// Helper functions to log errors and return values
// For function that do not return error codes
template <typename ValueType, typename ErrorType>
ValueType logErrorAndReturnValue(tl::expected<ValueType, ErrorType> result)
{
    if (result) {
        return result.value();
    }
    else {
        const grib_context* c = grib_context_get_default();
        grib_context_log(c, GRIB_LOG_ERROR, "%s", grib_get_error_message(result.error()));
        return {};
    }
}

template <typename ErrorType>
void logErrorAndReturnValue(tl::expected<void, ErrorType> result)
{
    if (!result) {
        const grib_context* c = grib_context_get_default();
        grib_context_log(c, GRIB_LOG_ERROR, "%s", grib_get_error_message(result.error()));
    }
}

} // namespace eccodes
