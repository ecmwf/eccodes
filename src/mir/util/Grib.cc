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


#include "mir/util/Grib.h"

#include <algorithm>
#include <cstring>
#include <ios>
#include <limits>
// #include <numeric> std::lcm, std::gcd (C++ 17)
#include <ostream>

#include "eckit/types/Fraction.h"

#include "mir/util/Log.h"


void grib_reorder(std::vector<double>& values, long scanningMode, size_t Ni, size_t Nj) {
    using mir::Log;

    enum
    {
        iScansNegatively      = 1 << 7,
        jScansPositively      = 1 << 6,
        jPointsAreConsecutive = 1 << 5,
        alternateRowScanning  = 1 << 4
    };

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
    os << "grib_reorder " << current << " not supported";
    Log::error() << os.str() << std::endl;
    throw mir::exception::SeriousBug(os.str());
}


void grib_get_unique_missing_value(const std::vector<double>& values, double& missingValue) {
    ASSERT(!values.empty());

    // check if it's unique, otherwise a high then a low value
    if (std::find(values.begin(), values.end(), missingValue) == values.end()) {
        return;
    }

    auto mm = std::minmax_element(values.begin(), values.end());

    missingValue = *(mm.second) + 1.;
    if (missingValue == missingValue) {
        return;
    }

    missingValue = *(mm.first) - 1.;
    if (missingValue == missingValue) {
        return;
    }

    throw mir::exception::SeriousBug("grib_get_unique_missing_value: failed to get a unique missing value.");
}


grib_info::grib_info() :
    grid{}, packing{}, extra_settings_size_(sizeof(packing.extra_settings) / sizeof(packing.extra_settings[0])) {
    // NOTE low-level initialisation only necessary for C interface
    std::memset(&grid, 0, sizeof(grid));
    std::memset(&packing, 0, sizeof(packing));

    strings_.reserve(extra_settings_size_);
}


void grib_info::extra_set(const char* key, long value) {
    auto j = static_cast<size_t>(packing.extra_settings_count++);
    ASSERT(j < extra_settings_size_);

    auto& set      = packing.extra_settings[j];
    set.name       = key;
    set.type       = CODES_TYPE_LONG;
    set.long_value = value;
}


void grib_info::extra_set(const char* key, double value) {
    auto j = static_cast<size_t>(packing.extra_settings_count++);
    ASSERT(j < extra_settings_size_);

    auto& set        = packing.extra_settings[j];
    set.name         = key;
    set.type         = CODES_TYPE_DOUBLE;
    set.double_value = value;
}


void grib_info::extra_set(const char* key, const char* value) {
    auto j = static_cast<size_t>(packing.extra_settings_count++);
    ASSERT(j < extra_settings_size_);

    auto& set = packing.extra_settings[j];
    set.name  = key;
    set.type  = CODES_TYPE_STRING;

    strings_.emplace_back(value);
    set.string_value = strings_.back().c_str();
}


namespace mir {
namespace util {
namespace grib {


Fraction::Fraction(double d) : Fraction(eckit::Fraction(d)) {}


Fraction::Fraction(const eckit::Fraction& frac) :
    num(static_cast<value_type>(frac.numerator())), den(static_cast<value_type>(frac.denominator())) {
    constexpr auto min = static_cast<eckit::Fraction::value_type>(std::numeric_limits<value_type>::lowest());
    constexpr auto max = static_cast<eckit::Fraction::value_type>(std::numeric_limits<value_type>::max());
    ASSERT(frac.denominator() != 0);
    ASSERT(min <= frac.denominator() && frac.denominator() <= max);
    ASSERT(min <= frac.numerator() && frac.numerator() <= max);
}


Fraction::value_type lcm(Fraction::value_type a, Fraction::value_type b) {
    if (a == 0 || b == 0) {
        return 0;
    }

    return a * (b / gcd(a, b));
}


Fraction::value_type gcd(Fraction::value_type a, Fraction::value_type b) {
    if (a == 0 && b == 0) {
        return 0;
    }

    // Euclidean algorithm
    while (b != 0) {
        auto r = a % b;
        a      = b;
        b      = r;
    }
    return a;
}


BasicAngle::BasicAngle(Fraction a, Fraction b, Fraction c, Fraction d, Fraction e, Fraction f) :
    Fraction(gcd(a.num, b.num, c.num, d.num, e.num, f.num), lcm(a.den, b.den, c.den, d.den, e.den, f.den)) {
    ASSERT(den != 0);
}


BasicAngle::BasicAngle(const grib_info& info) :
    BasicAngle(
        Fraction(info.grid.latitudeOfFirstGridPointInDegrees), Fraction(info.grid.longitudeOfFirstGridPointInDegrees),
        Fraction(info.grid.latitudeOfLastGridPointInDegrees), Fraction(info.grid.longitudeOfLastGridPointInDegrees),
        Fraction(info.grid.iDirectionIncrementInDegrees), Fraction(info.grid.jDirectionIncrementInDegrees)) {}


void BasicAngle::fillGrib(grib_info& info) const {
    auto fill = [this, &info](const char* key, double value) {
        Fraction f(value);
        ASSERT(f.den != 0);
        if (f.num != 0) {
            info.extra_set(key, numerator(f));
        }
    };

    info.extra_set("basicAngleOfTheInitialProductionDomain", num);
    info.extra_set("subdivisionsOfBasicAngle", den);

    fill("latitudeOfFirstGridPoint", info.grid.latitudeOfFirstGridPointInDegrees);
    fill("longitudeOfFirstGridPoint", info.grid.longitudeOfFirstGridPointInDegrees);
    fill("latitudeOfLastGridPoint", info.grid.latitudeOfLastGridPointInDegrees);
    fill("longitudeOfLastGridPoint", info.grid.longitudeOfLastGridPointInDegrees);
    fill("iDirectionIncrement", info.grid.iDirectionIncrementInDegrees);
    fill("jDirectionIncrement", info.grid.jDirectionIncrementInDegrees);
}


Fraction::value_type BasicAngle::numerator(const Fraction& f) const {
    Fraction x(f.num * den, f.den * num);
    const auto div = gcd(x.num, x.den);

    ASSERT(x.den == div);  // if BasicAngle is adequate
    return x.num / div;
}


void BasicAngle::list(std::ostream& out) {
    out << "as-input, decimal, fraction";
}


}  // namespace grib
}  // namespace util
}  // namespace mir
