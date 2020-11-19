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


void post_t::addBytes(const char* key, const unsigned char* value, size_t length) {
    struct codes_values_post_bytes_t final : codes_values_post_t {
        codes_values_post_bytes_t(const char* name, const unsigned char* bytes_value, size_t length) :
            name_(name), value_(bytes_value), length_(length) {}
        void set(codes_handle* h) override { GRIB_CALL(codes_set_bytes(h, name_, value_, &length_)); }

        codes_values_post_bytes_t(const codes_values_post_bytes_t&) = delete;
        void operator=(const codes_values_post_bytes_t&) = delete;

        const char* name_;
        const unsigned char* value_;
        size_t length_;
    };

    emplace_back(new codes_values_post_bytes_t(key, value, length));
}


void post_t::set(codes_handle* h) {
    for (auto& p : *this) {
        p->set(h);
    }
}


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
    } {}


void GribExtraSetting::set(grib_info& info, const char* key, long value) {
    auto& set      = info.packing.extra_settings[info.packing.extra_settings_count++];
    set.name       = key;
    set.long_value = value;
    set.type       = CODES_TYPE_LONG;
}


void GribExtraSetting::set(grib_info& info, const char* key, double value) {
    auto& set        = info.packing.extra_settings[info.packing.extra_settings_count++];
    set.name         = key;
    set.double_value = value;
    set.type         = CODES_TYPE_DOUBLE;
}


void GribExtraSetting::set(grib_info& info, const char* key, const char* value) {
    auto& set        = info.packing.extra_settings[info.packing.extra_settings_count++];
    set.name         = key;
    set.string_value = value;
    set.type         = CODES_TYPE_STRING;
}
