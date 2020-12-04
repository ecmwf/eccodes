/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include <sstream>

#include "mir/util/Grib.h"
#include "mir/util/Log.h"


void GribReorder::reorder(std::vector<double>& values, long scanningMode, size_t Ni, size_t Nj) {
    using mir::Log;

    auto scanningModeAsString = [](long mode) {
        std::ostringstream os;
        os << "scanningMode=" << mode << " (0x" << std::hex << mode << std::dec << ")";
        return os.str();
    };

    auto current(scanningModeAsString(scanningMode));
    auto canonical(scanningModeAsString(0));

    ASSERT(Ni > 0);
    ASSERT(Nj > 0);
    ASSERT(values.size() == Ni * Nj);

    std::vector<double> out(values.size());

    if (scanningMode == jScansPositively) {
        Log::warning() << "LatLon::reorder " << current << " to " << canonical << std::endl;
        size_t count = 0;
        for (size_t j = Nj; j > 0; --j) {
            for (size_t i = 0; i < Ni; ++i) {
                out[count++] = values[(j - 1) * Ni + i];
            }
        }
        ASSERT(count == out.size());
        std::swap(values, out);
        return;
    }

    if (scanningMode == iScansNegatively) {
        Log::warning() << "LatLon::reorder " << current << " to " << canonical << std::endl;
        size_t count = 0;
        for (size_t j = 0; j < Nj; ++j) {
            for (size_t i = Ni; i > 0; --i) {
                out[count++] = values[j * Ni + (i - 1)];
            }
        }
        ASSERT(count == out.size());
        std::swap(values, out);
        return;
    }

    if (scanningMode == (iScansNegatively | jScansPositively)) {
        Log::warning() << "LatLon::reorder " << current << " to " << canonical << std::endl;
        size_t count = 0;
        for (size_t j = Nj; j > 0; --j) {
            for (size_t i = Ni; i > 0; --i) {
                out[count++] = values[(j - 1) * Ni + (i - 1)];
            }
        }
        ASSERT(count == out.size());
        std::swap(values, out);
        return;
    }

    std::ostringstream os;
    os << "LatLon::reorder " << current << " not supported";
    Log::error() << os.str() << std::endl;
    throw mir::exception::SeriousBug(os.str());
}


grib_info::grib_info() :
    grid{
        0,
    },
    packing{
        0,
    } {
    constexpr size_t extra_settings_size = 80;  // ecCodes extra_settings array size
    strings_.reserve(extra_settings_size);
}


void grib_info::extra_set(const char* key, long value) {
    auto& set      = packing.extra_settings[packing.extra_settings_count++];
    set.name       = key;
    set.type       = CODES_TYPE_LONG;
    set.long_value = value;
}


void grib_info::extra_set(const char* key, double value) {
    auto& set        = packing.extra_settings[packing.extra_settings_count++];
    set.name         = key;
    set.type         = CODES_TYPE_DOUBLE;
    set.double_value = value;
}


void grib_info::extra_set(const char* key, const char* value) {
    auto& set = packing.extra_settings[packing.extra_settings_count++];
    set.name  = key;
    set.type  = CODES_TYPE_STRING;

    strings_.emplace_back(value);
    set.string_value = strings_.back().c_str();
}
