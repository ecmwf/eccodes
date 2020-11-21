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


void post_t::addBytes(const std::string& key, const std::string& value) {
    struct post_value_bytes_t final : post_value_t {
        post_value_bytes_t(const std::string& name, const std::string& value) :
            name_(name), bytes_(Bytes::to_bytes(value)) {}

        void set(codes_handle* h) override {
            size_t length = bytes_.size();
            auto bytes    = reinterpret_cast<const unsigned char*>(bytes_.data());
            GRIB_CALL(codes_set_bytes(h, name_.c_str(), bytes, &length));
        }

        post_value_bytes_t(const post_value_bytes_t&) = delete;
        void operator=(const post_value_bytes_t&) = delete;

        const std::string name_;
        const Bytes::bytes_t bytes_;
    };

    emplace_back(new post_value_bytes_t(key, value));
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


Bytes::byte_t Bytes::to_byte(char nibble_hi, char nibble_lo) {
    auto g = [](char n) {
        constexpr char ten = 10;
        return ('0' <= n && n <= '9' ? n - '0' : 0) | ('A' <= n && n <= 'F' ? n - 'A' + ten : 0) |
               ('a' <= n && n <= 'f' ? n - 'a' + ten : 0);
    };
    return byte_t(g(nibble_hi) << 4 | g(nibble_lo));
}


std::string Bytes::to_string(const Bytes::bytes_t& bytes) {
    static const char a[] = "0123456789abcdef";

    std::string s;
    s.reserve(bytes.size() * 2);
    for (auto b : bytes) {
        s.push_back(a[b >> 4]);
        s.push_back(a[b % 16]);
    }
    return s;
}


Bytes::bytes_t Bytes::to_bytes(const std::string& n) {
    ASSERT(n.length() % 2 == 0);
    bytes_t b(n.size() / 2);
    for (size_t i = 0, j = 0; i < n.size(); i += 2, ++j) {
        b[j] = to_byte(n[i], n[i + 1]);
    }
    return b;
}
