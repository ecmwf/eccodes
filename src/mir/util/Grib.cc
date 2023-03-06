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
#include <utility>
// #include <numeric> std::lcm, std::gcd (C++ 17)
#include <ostream>

#include "eckit/config/Resource.h"
#include "eckit/types/Fraction.h"

#include "mir/param/MIRParametrisation.h"
#include "mir/repres/Representation.h"
#include "mir/util/Angles.h"
#include "mir/util/Exceptions.h"
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


namespace mir::util::grib {


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
    BasicAngle(Fraction(info.grid.latitudeOfFirstGridPointInDegrees),
               Fraction(normalise_longitude(info.grid.longitudeOfFirstGridPointInDegrees, 0.)),
               Fraction(info.grid.latitudeOfLastGridPointInDegrees),
               Fraction(normalise_longitude(info.grid.longitudeOfLastGridPointInDegrees, 0.)),
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
    fill("longitudeOfFirstGridPoint", normalise_longitude(info.grid.longitudeOfFirstGridPointInDegrees, 0.));
    fill("latitudeOfLastGridPoint", info.grid.latitudeOfLastGridPointInDegrees);
    fill("longitudeOfLastGridPoint", normalise_longitude(info.grid.longitudeOfLastGridPointInDegrees, 0.));
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


Packing::Packing(const std::string& name, const param::MIRParametrisation& param) :
    bitsPerValue_(0),
    edition_(0),
    precision_(0),
    gridded_(param.userParametrisation().has("grid") || param.fieldParametrisation().has("gridded")) {
    const auto& user  = param.userParametrisation();
    const auto& field = param.fieldParametrisation();

    ASSERT(!name.empty());
    packing_ = name;
    std::string packing;

    bool gridded = false;
    field.get("gridded", gridded);

    definePacking_                   = !field.get("packing", packing) || packing_ != packing || gridded_ != gridded;
    defineBitsPerValueBeforePacking_ = definePacking_ && packing == "ieee";

    defineBitsPerValue_ = false;
    if (defineBitsPerValueBeforePacking_) {
        ASSERT(param.get("accuracy", bitsPerValue_));
        defineBitsPerValue_ = true;
    }
    else if (user.get("accuracy", bitsPerValue_)) {
        long accuracy       = 0;
        defineBitsPerValue_ = !field.get("accuracy", accuracy) || bitsPerValue_ != accuracy;
    }

    param.get("edition", edition_);
    long edition   = 0;
    defineEdition_ = !field.get("edition", edition) || edition_ != edition;
}


bool Packing::sameAs(const Packing* other) const {
    if (definePacking_ != other->definePacking_ || defineBitsPerValue_ != other->defineBitsPerValue_ ||
        defineEdition_ != other->defineEdition_ || definePrecision_ != other->definePrecision_) {
        return false;
    }

    bool samePacking   = !definePacking_ || packing_ == other->packing_;
    bool sameAccuracy  = !defineBitsPerValue_ || bitsPerValue_ == other->bitsPerValue_;
    bool sameEdition   = !defineEdition_ || edition_ == other->edition_;
    bool sameprecision = !definePrecision_ || precision_ == other->precision_;

    return samePacking && sameAccuracy && sameEdition && sameprecision;
}


bool Packing::printParametrisation(std::ostream& out) const {
    std::string sep;

    if (definePacking_) {
        out << sep << "packing=" << packing_;
        sep = ",";
    }

    if (defineEdition_) {
        out << sep << "edition=" << edition_;
        sep = ",";
    }

    if (defineBitsPerValue_) {
        out << sep << "accuracy=" << bitsPerValue_;
        sep = ",";
    }

    if (definePrecision_) {
        out << sep << "precision=" << precision_;
        sep = ",";
    }

    return !sep.empty();
}


bool Packing::empty() const {
    return !definePacking_ && !defineBitsPerValue_ && !defineEdition_ && !definePrecision_;
}


void Packing::fill(grib_info& info, long pack) const {
    info.packing.packing  = CODES_UTIL_PACKING_SAME_AS_INPUT;
    info.packing.accuracy = CODES_UTIL_ACCURACY_SAME_BITS_PER_VALUES_AS_INPUT;
    // (Representation can set edition, so it isn't reset)

    if (definePacking_) {
        info.packing.packing      = CODES_UTIL_PACKING_USE_PROVIDED;
        info.packing.packing_type = pack;
    }

    if (defineBitsPerValue_ && !definePrecision_) {
        info.packing.accuracy     = CODES_UTIL_ACCURACY_USE_PROVIDED_BITS_PER_VALUES;
        info.packing.bitsPerValue = bitsPerValue_;
    }

    if (defineEdition_) {
        info.packing.editionNumber = edition_;
    }

    if (definePrecision_) {
        info.extra_set("precision", precision_);
    }
}


void Packing::set(grib_handle* h, const std::string& type) const {
    // Note: order is important, it is not applicable to all packing's.

    if (defineEdition_) {
        GRIB_CALL(codes_set_long(h, "edition", edition_));
    }

    if (defineBitsPerValueBeforePacking_) {
        GRIB_CALL(codes_set_long(h, "bitsPerValue", bitsPerValue_));
    }

    if (definePacking_) {
        auto len = type.length();
        GRIB_CALL(codes_set_string(h, "packingType", type.c_str(), &len));
    }

    if (definePrecision_) {
        GRIB_CALL(codes_set_long(h, "precision", precision_));
    }
    else if (defineBitsPerValue_) {
        GRIB_CALL(codes_set_long(h, "bitsPerValue", bitsPerValue_));
    }
}


namespace packing {


struct ArchivedValue : Packing {
    ArchivedValue(const std::string& name, const param::MIRParametrisation& param) : Packing(name, param) {
        ASSERT(!definePacking_);
    }

    void fill(const repres::Representation*, grib_info& info) const override {
        Packing::fill(info, CODES_UTIL_PACKING_SAME_AS_INPUT);
    }

    void set(const repres::Representation*, grib_handle* handle) const override { Packing::set(handle, ""); }
};


struct CCSDS : Packing {
    CCSDS(const std::string& name, const param::MIRParametrisation& param) : Packing(name, param) {
        edition_       = 2;
        long edition   = 0;
        defineEdition_ = !param.fieldParametrisation().get("edition", edition) || edition_ != edition;
    }

    void fill(const repres::Representation*, grib_info& info) const override {
        Packing::fill(info, CODES_UTIL_PACKING_TYPE_CCSDS);
    }

    void set(const repres::Representation*, grib_handle* handle) const override { Packing::set(handle, "grid_ccsds"); }
};


struct Complex : Packing {
    using Packing::Packing;

    void fill(const repres::Representation*, grib_info& info) const override {
        Packing::fill(info, CODES_UTIL_PACKING_TYPE_SPECTRAL_COMPLEX);
    }

    void set(const repres::Representation*, grib_handle* handle) const override {
        Packing::set(handle, "spectral_complex");
    }
};


struct IEEE : Packing {
    IEEE(const std::string& name, const param::MIRParametrisation& param) : Packing(name, param) {
        constexpr long L32  = 32;
        constexpr long L64  = 64;
        constexpr long L128 = 128;

        // Accuracy set by user, otherwise by field (rounded up to a supported precision)
        long bits = L32;
        param.fieldParametrisation().get("accuracy", bits);

        if (!param.userParametrisation().get("accuracy", bitsPerValue_)) {
            bitsPerValue_ = bits <= L32 ? L32 : bits <= L64 ? L64 : L128;
        }

        definePrecision_ = bitsPerValue_ != bits || definePacking_ || !param.fieldParametrisation().has("accuracy");
        precision_       = bitsPerValue_ == L32 ? 1 : bitsPerValue_ == L64 ? 2 : bitsPerValue_ == L128 ? 3 : 0;

        if (precision_ == 0) {
            std::string msg = "packing=ieee: only supports accuracy=32, 64 and 128";
            Log::error() << msg << std::endl;
            throw exception::UserError(msg);
        }
    }

    void fill(const repres::Representation*, grib_info& info) const override {
        Packing::fill(info, CODES_UTIL_PACKING_TYPE_IEEE);
    }

    void set(const repres::Representation*, grib_handle* handle) const override {
        Packing::set(handle, gridded() ? "grid_ieee" : "spectral_ieee");
    }
};


struct Simple : Packing {
    using Packing::Packing;

    void fill(const repres::Representation*, grib_info& info) const override {
        Packing::fill(info, gridded() ? CODES_UTIL_PACKING_TYPE_GRID_SIMPLE : CODES_UTIL_PACKING_TYPE_SPECTRAL_SIMPLE);
    }
    void set(const repres::Representation*, grib_handle* handle) const override {
        Packing::set(handle, gridded() ? "grid_simple" : "spectral_simple");
    }
};


struct SecondOrder : Packing {
    SecondOrder(const std::string& name, const param::MIRParametrisation& param) :
        Packing(name, param), simple_(name, param) {}

    Simple simple_;

    static bool check(const repres::Representation* repres) {
        ASSERT(repres != nullptr);

        auto n = repres->numberOfPoints();
        if (n < 4) {
            Log::warning() << "packing=second-order: does not support less than 4 values, using packing=simple"
                           << std::endl;
            return false;
        }
        return true;
    }

    void fill(const repres::Representation* repres, grib_info& info) const override {
        if (!check(repres)) {
            simple_.fill(repres, info);
            return;
        }

        Packing::fill(info, CODES_UTIL_PACKING_TYPE_GRID_SECOND_ORDER);
    }
    void set(const repres::Representation* repres, grib_handle* handle) const override {
        if (!check(repres)) {
            simple_.set(repres, handle);
            return;
        }

        Packing::set(handle, "grid_second_order");
    }
};


}  // namespace packing


Packing* Packing::build(const param::MIRParametrisation& param) {
    const auto& user  = param.userParametrisation();
    const auto& field = param.fieldParametrisation();


    // Defaults
    static const bool grib_edition_conversion_default =
        eckit::Resource<bool>("$MIR_GRIB_EDITION_CONVERSION;mirGribEditionConversion", false);
    bool grib_edition_conversion = grib_edition_conversion_default;
    param.get("grib-edition-conversion", grib_edition_conversion);

    long edition = 2;
    param.get("edition", edition);

    std::string default_spectral = "complex";
    std::string default_gridded  = "ccsds";
    param.get("default-spectral-packing", default_spectral);
    param.get(edition <= 1 ? "default-grib1-gridded-packing" : "default-grib2-gridded-packing", default_gridded);

    auto check = [](bool ok, const std::string& message) {
        if (!ok) {
            Log::error() << message << std::endl;
            throw exception::UserError(message);
        }
    };


    // Converting spectral to gridded
    auto packing = user.has("grid") && field.has("spectral") ? default_gridded : "av";
    user.get("packing", packing);


    // Aliasing
    auto av      = packing == "av" || packing == "archived-value";
    auto gridded = user.has("grid") || field.has("gridded");

    if (av) {
        packing = field.has("spectral") ? default_spectral : default_gridded;
        field.get("packing", packing);
    }
    else if (packing == "co") {
        packing = "complex";
    }
    else if (packing == "so") {
        packing = "second-order";
    }


    // Instantiate packing method
    if (packing == "ccsds") {
        check(edition == 2 || grib_edition_conversion, "packing=ccsds requires edition conversion, which is disabled");
        check(gridded, "packing=ccsds requires gridded data");
        return new packing::CCSDS(packing, param);
    }

    if (packing == "complex") {
        check(!gridded, "packing=complex requires spectral data");
        return new packing::Complex(packing, param);
    }

    if (packing == "ieee") {
        return new packing::IEEE(packing, param);
    }

    if (packing.compare(0, 12, "second-order") == 0) {
        check(gridded, "packing=second-order requires gridded data");
        return new packing::SecondOrder(packing, param);
    }

    if (packing.compare(0, 6, "simple") == 0) {
        return new packing::Simple(packing, param);
    }

    if (av) {
        return new packing::ArchivedValue(packing, param);
    }


    list(Log::error() << "Packing: unknown packing '" << packing << "', choices are: ");
    throw exception::UserError("Packing: unknown packing '" + packing + "'");
}


void Packing::list(std::ostream& out) {
    out << "archived-value, av, ccsds, co, complex, ieee, second-order, simple, so" << std::endl;
}


}  // namespace mir::util::grib
