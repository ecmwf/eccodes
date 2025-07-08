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


#include "eccodes/geo/Geo.h"

#include <string>

#include "eckit/exception/Exceptions.h"


namespace eccodes::geo
{


bool codes_check_error(int e, const char* call)
{
    if (e != CODES_SUCCESS) {
        throw ::eckit::SeriousBug(std::string(call) + ": " + codes_get_error_message(e));
    }

    return true;
}


void codes_assertion(const char* message)
{
    throw ::eckit::SeriousBug(message);
}


grib_info::grib_info() :
    grid{}, packing{}, extra_settings_size_(sizeof(packing.extra_settings) / sizeof(packing.extra_settings[0]))
{
    // NOTE low-level initialisation only necessary for C interface
    std::memset(&grid, 0, sizeof(grid));
    std::memset(&packing, 0, sizeof(packing));

    strings_.reserve(extra_settings_size_);
}


void grib_info::extra_set(const char* key, long value)
{
    auto j = static_cast<size_t>(packing.extra_settings_count++);
    ASSERT(j < extra_settings_size_);

    auto& set      = packing.extra_settings[j];
    set.name       = key;
    set.type       = CODES_TYPE_LONG;
    set.long_value = value;
}


void grib_info::extra_set(const char* key, double value)
{
    auto j = static_cast<size_t>(packing.extra_settings_count++);
    ASSERT(j < extra_settings_size_);

    auto& set        = packing.extra_settings[j];
    set.name         = key;
    set.type         = CODES_TYPE_DOUBLE;
    set.double_value = value;
}


void grib_info::extra_set(const char* key, const char* value)
{
    auto j = static_cast<size_t>(packing.extra_settings_count++);
    ASSERT(j < extra_settings_size_);

    auto& set = packing.extra_settings[j];
    set.name  = key;
    set.type  = CODES_TYPE_STRING;

    strings_.emplace_back(value);
    set.string_value = strings_.back().c_str();
}


}  // namespace eccodes::geo
