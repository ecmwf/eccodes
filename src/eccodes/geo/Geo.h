/*
 * (C) Copyright 2024- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#pragma once

#include <string>
#include <vector>

#include "eccodes.h"


namespace eckit::geo
{
class Grid;
class Spec;
}  // namespace eckit::geo


namespace eccodes::geo
{


using Grid = eckit::geo::Grid;
using Spec = eckit::geo::Spec;


bool codes_check_error(int e, const char* call);
[[noreturn]] void codes_assertion(const char* message);


#define CHECK_ERROR(a, b) codes_check_error(a, b)
#define CHECK_CALL(a)     codes_check_error(a, #a)


struct grib_info
{
    grib_info();
    grib_info(const grib_info&) = delete;
    grib_info(grib_info&&)      = delete;

    ~grib_info() = default;

    void operator=(grib_info&&)      = delete;
    void operator=(const grib_info&) = delete;

    void extra_set(const char* key, long);
    void extra_set(const char* key, double);
    void extra_set(const char* key, const char*);

    codes_util_grid_spec grid;
    codes_util_packing_spec packing;

private:
    std::vector<std::string> strings_;
    const size_t extra_settings_size_;
};


}  // namespace eccodes::geo
