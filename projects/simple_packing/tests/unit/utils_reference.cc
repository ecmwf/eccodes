// SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// Unit test: the functions of src/Utils.cc give exactly the results of a frozen copy of their
// earlier implementation (utils_reference_impl.h), so that they can be modernised and optimised
// without changing a result. Compared: the value as a bit pattern (also the sign of zero), no
// value (std::nullopt), or the type and the message of the exception, for
// - nearest_smaller_value (IEEE and IBM): every exponent of both formats with characteristic
//   mantissas, their neighbours and the points halfway between two values, special values, and
//   random values over the whole double range
// - number_of_bits: the powers of two and their neighbours, random values
// - get_binary_scale_factor: ranges at every power of two of the double range and between them,
//   for bits per value of 1 to 63 (and invalid ones), with a minimum of zero and with other minima,
//   special values, random ranges
// - check_data_values_range: all pairs of special and ordinary values
#include <cfloat>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <limits>
#include <optional>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

#include "simple_packing/detail/Utils.h"
#include "utils_reference_impl.h"

namespace
{

int64_t failures = 0;
int64_t cases = 0;

// The outcome of a call: a value (as bits), no value, or an exception
struct Outcome
{
    enum Kind
    {
        Value,
        None,
        InvalidArgument,
        RangeError,
        RuntimeError,
        OtherException
    } kind = None;
    uint64_t bits = 0;
    std::string message;

    bool operator==(const Outcome& o) const { return kind == o.kind && bits == o.bits && message == o.message; }
};

uint64_t bitsOf(double d)
{
    uint64_t b = 0;
    std::memcpy(&b, &d, sizeof b);
    return b;
}

template <typename F>
Outcome outcomeOf(F&& f)
{
    Outcome o;
    try {
        const auto r = f();
        if (r) {
            o.kind = Outcome::Value;
            if constexpr (std::is_same<std::decay_t<decltype(*r)>, double>::value) {
                o.bits = bitsOf(*r);
            }
            else {
                o.bits = static_cast<uint64_t>(*r);
            }
        }
    }
    catch (const std::invalid_argument& e) {
        o = Outcome{ Outcome::InvalidArgument, 0, e.what() };
    }
    catch (const std::range_error& e) {
        o = Outcome{ Outcome::RangeError, 0, e.what() };
    }
    catch (const std::runtime_error& e) {
        o = Outcome{ Outcome::RuntimeError, 0, e.what() };
    }
    catch (const std::exception& e) {
        o = Outcome{ Outcome::OtherException, 0, e.what() };
    }
    return o;
}

std::string describe(const Outcome& o)
{
    static const char* kinds[] = { "value", "none", "invalid_argument", "range_error", "runtime_error", "other exception" };
    std::string s = kinds[o.kind];
    if (o.kind == Outcome::Value) {
        s += " bits 0x" + [&] {
            char buf[17];
            std::snprintf(buf, sizeof buf, "%016llx", static_cast<unsigned long long>(o.bits));
            return std::string(buf);
        }();
    }
    if (!o.message.empty()) {
        s += " \"" + o.message + "\"";
    }
    return s;
}

void compare(const Outcome& got, const Outcome& expected, const std::string& what)
{
    ++cases;
    if (!(got == expected)) {
        if (++failures <= 20) {
            std::cerr << "FAIL: " << what << ": " << describe(got) << " instead of " << describe(expected) << '\n';
        }
    }
}

std::string hex(double d)
{
    char buf[64];
    std::snprintf(buf, sizeof buf, "%a", d);
    return buf;
}

// --- nearest_smaller_value ---

void nearestSmaller(double x, sp::detail::FloatType type)
{
    for (double v : { x, -x }) {
        compare(outcomeOf([&] { return sp::detail::nearest_smaller_value(v, type); }),
                outcomeOf([&] { return utils_reference::nearest_smaller_value(v, type); }),
                std::string("nearest_smaller_value(") + hex(v) + (type == sp::detail::FloatType::IBM32 ? ", IBM32)" : ", IEEE32)"));
    }
}

// x and its neighbours
void nearestSmallerAround(double x, sp::detail::FloatType type)
{
    const double inf = std::numeric_limits<double>::infinity();
    double down = x, up = x;
    nearestSmaller(x, type);
    for (int32_t i = 0; i < 2; ++i) {
        down = std::nextafter(down, -inf);
        up = std::nextafter(up, inf);
        nearestSmaller(down, type);
        nearestSmaller(up, type);
    }
}

void ieee(std::mt19937_64& rng)
{
    using sp::detail::FloatType;
    // Every biased exponent of a float (0: zero and denormals, 255: infinity and NaN) with
    // characteristic mantissas, and the points halfway between two floats
    const uint32_t mantissas[] = { 0, 1, 2, 3, 0x3FFFFF, 0x400000, 0x400001, 0x7FFFFD, 0x7FFFFE, 0x7FFFFF };
    for (uint32_t e = 0; e < 256; ++e) {
        std::vector<uint32_t> ms(std::begin(mantissas), std::end(mantissas));
        for (int32_t i = 0; i < 8; ++i) {
            ms.push_back(static_cast<uint32_t>(rng()) & 0x7FFFFF);
        }
        for (uint32_t m : ms) {
            const uint32_t bits = (e << 23) | m;
            float f = 0;
            std::memcpy(&f, &bits, sizeof f);
            const double x = f;
            nearestSmallerAround(x, FloatType::IEEE32);
            if (std::isfinite(x)) {
                const double next = std::nextafter(f, std::numeric_limits<float>::infinity());
                if (std::isfinite(next)) {
                    nearestSmallerAround((x + next) / 2, FloatType::IEEE32);
                }
            }
        }
    }
    // The float range as doubles: one step beyond FLT_MAX, below FLT_MIN, 2^-149 (denorm_min)
    for (double x : { static_cast<double>(FLT_MAX), static_cast<double>(FLT_MIN), static_cast<double>(std::numeric_limits<float>::denorm_min()),
                      static_cast<double>(FLT_MAX) * (1 + 0x1p-25), static_cast<double>(FLT_MAX) * (1 + 0x1p-24) }) {
        nearestSmallerAround(x, FloatType::IEEE32);
    }
}

void ibm(std::mt19937_64& rng)
{
    using sp::detail::FloatType;
    // value = m * 16^(e - 70), m normalised to [0x100000, 0xffffff], e in 0..127; also the
    // exponents just outside, and mantissas that are not normalised
    const uint32_t mantissas[] = { 0, 1, 0xFFFFF, 0x100000, 0x100001, 0x7FFFFF, 0x800000, 0x800001, 0xFFFFFE, 0xFFFFFF, 0x1000000 };
    for (int32_t e = -2; e <= 129; ++e) {
        std::vector<uint32_t> ms(std::begin(mantissas), std::end(mantissas));
        for (int32_t i = 0; i < 8; ++i) {
            ms.push_back(0x100000 + static_cast<uint32_t>(rng() % 0xF00000));
        }
        const double unit = std::ldexp(1.0, 4 * (e - 70));
        for (uint32_t m : ms) {
            const double x = m * unit;
            nearestSmallerAround(x, FloatType::IBM32);
            nearestSmallerAround((m + 0.5) * unit, FloatType::IBM32);  // halfway to the next value
        }
        nearestSmallerAround(std::ldexp(1.0, 4 * (e - 64)), FloatType::IBM32);  // 16^(e - 64), and just below it
    }
}

void specialValues()
{
    const double inf = std::numeric_limits<double>::infinity(), nan = std::numeric_limits<double>::quiet_NaN();
    for (auto type : { sp::detail::FloatType::IEEE32, sp::detail::FloatType::IBM32 }) {
        for (double x : { 0.0, std::numeric_limits<double>::denorm_min(), DBL_MIN, DBL_MAX, inf, nan, 1.0, 0.1, 1e-40, 1e-80, 1e40, 1e80 }) {
            nearestSmaller(x, type);
        }
    }
}

void randomDoubles(std::mt19937_64& rng, int64_t n)
{
    std::uniform_int_distribution<int32_t> exponent(-1080, 1030);
    for (int64_t i = 0; i < n; ++i) {
        const double x = std::ldexp(1.0 + static_cast<double>(rng() >> 11) * 0x1p-53, exponent(rng));
        nearestSmaller(x, sp::detail::FloatType::IEEE32);
        nearestSmaller(x, sp::detail::FloatType::IBM32);
    }
    // Doubles near the float range: the region where the rounding matters
    std::uniform_int_distribution<int32_t> near(-160, 130);
    for (int64_t i = 0; i < n; ++i) {
        const double x = std::ldexp(1.0 + static_cast<double>(rng() >> 11) * 0x1p-53, near(rng));
        nearestSmaller(x, sp::detail::FloatType::IEEE32);
        nearestSmaller(x, sp::detail::FloatType::IBM32);
    }
}

// --- number_of_bits ---

void numberOfBits(std::mt19937_64& rng)
{
    auto one = [](uint64_t x) {
        compare(outcomeOf([&] { return sp::detail::number_of_bits(x); }), outcomeOf([&] { return utils_reference::number_of_bits(x); }),
                "number_of_bits(" + std::to_string(x) + ")");
    };
    one(0);
    one(UINT64_MAX);
    for (int32_t k = 0; k < 64; ++k) {
        const uint64_t p = uint64_t{ 1 } << k;
        one(p - 1);
        one(p);
        one(p + 1);
    }
    for (int32_t i = 0; i < 100000; ++i) {
        one(rng() >> (rng() % 64));
    }
}

// --- get_binary_scale_factor ---

void binaryScale(double max, double min, int64_t bpv)
{
    compare(outcomeOf([&] { return sp::detail::get_binary_scale_factor(max, min, bpv); }),
            outcomeOf([&] { return utils_reference::get_binary_scale_factor(max, min, bpv); }),
            "get_binary_scale_factor(" + hex(max) + ", " + hex(min) + ", " + std::to_string(bpv) + ")");
}

void binaryScaleFactors(std::mt19937_64& rng)
{
    const double inf = std::numeric_limits<double>::infinity(), nan = std::numeric_limits<double>::quiet_NaN();
    // Invalid bits per value, and invalid ranges
    for (int64_t bpv : { int64_t{ -1 }, int64_t{ 0 }, int64_t{ 64 }, int64_t{ 65 }, INT64_MIN, INT64_MAX }) {
        binaryScale(1.0, 0.0, bpv);
    }
    for (double max : { 0.0, 1.0, -1.0, DBL_MAX, -DBL_MAX, inf, -inf, nan }) {
        for (double min : { 0.0, 1.0, -1.0, DBL_MAX, -DBL_MAX, inf, -inf, nan }) {
            binaryScale(max, min, 16);
        }
    }
    // Ranges at every power of two of the double range (subnormal to the largest) and between two
    // powers; bits per value of all sizes (the search for 63 bits uses 2^63 as a double)
    const int64_t bpvs[] = { 1, 2, 3, 7, 8, 9, 12, 15, 16, 17, 23, 24, 25, 31, 32, 33, 48, 52, 53, 54, 62, 63 };
    const double factors[] = { 1.0, std::nextafter(1.0, 0.0), std::nextafter(1.0, 2.0), 1.5, std::nextafter(2.0, 0.0) };
    for (int32_t k = -1074; k <= 1023; ++k) {
        for (double f : factors) {
            const double range = std::ldexp(f, k);
            if (!std::isfinite(range) || range == 0) {
                continue;
            }
            for (int64_t bpv : bpvs) {
                binaryScale(range, 0.0, bpv);
            }
        }
    }
    // Ranges near the rounding boundaries: (2^b - 1 +- 0.5) * 2^E
    for (int64_t bpv = 1; bpv <= 63; ++bpv) {
        const double maxint = std::ldexp(1.0, static_cast<int32_t>(bpv)) - 1;
        for (int32_t e : { -1074 + 64, -1000, -127, -126, -60, -1, 0, 1, 60, 126, 127, 128, 900 }) {
            for (double m : { maxint, maxint + 0.5, maxint - 0.5, maxint + 1 }) {
                const double base = std::ldexp(m, e);
                if (!std::isfinite(base) || base == 0) {
                    continue;
                }
                binaryScale(base, 0.0, bpv);
                binaryScale(std::nextafter(base, inf), 0.0, bpv);
                binaryScale(std::nextafter(base, 0.0), 0.0, bpv);
            }
        }
    }
    // Other minima: the range is max - min
    for (double min : { -1e300, -273.15, -1.0, 1e-300, 0.25, 1e5, 1e300 }) {
        for (double range : { 0.0, 1e-310, 1e-30, 1e-5, 1.0, 3.0, 1e10, 1e300 }) {
            for (int64_t bpv : { 1, 8, 16, 24, 32, 63 }) {
                binaryScale(min + range, min, bpv);
            }
        }
    }
    // Random ranges and bits per value
    std::uniform_int_distribution<int32_t> exponent(-1074, 1023);
    std::uniform_int_distribution<int64_t> bits(1, 63);
    for (int32_t i = 0; i < 200000; ++i) {
        const double range = std::ldexp(1.0 + static_cast<double>(rng() >> 11) * 0x1p-53, exponent(rng));
        binaryScale(range, 0.0, bits(rng));
    }
}

// --- check_data_values_range ---

void dataValuesRange()
{
    const double inf = std::numeric_limits<double>::infinity(), nan = std::numeric_limits<double>::quiet_NaN();
    const double values[] = { -inf, -DBL_MAX, std::nextafter(-DBL_MAX, 0.0), -1.0, -0.0, 0.0, DBL_MIN, 1.0, std::nextafter(DBL_MAX, 0.0), DBL_MAX, inf, nan };
    for (double mn : values) {
        for (double mx : values) {
            auto outcome = [&](auto check) {
                return outcomeOf([&] {
                    check(mn, mx);
                    return std::optional<int64_t>(1);
                });
            };
            compare(outcome([](double a, double b) { sp::detail::check_data_values_range(a, b); }),
                    outcome([](double a, double b) { utils_reference::check_data_values_range(a, b); }),
                    "check_data_values_range(" + hex(mn) + ", " + hex(mx) + ")");
        }
    }
}

}  // namespace

int main()
{
    std::mt19937_64 rng(20261127);
    ieee(rng);
    ibm(rng);
    specialValues();
    randomDoubles(rng, 500000);
    numberOfBits(rng);
    binaryScaleFactors(rng);
    dataValuesRange();
    if (failures != 0) {
        std::cerr << failures << " of " << cases << " cases differ from the reference\n";
        return 1;
    }
    std::cout << "OK " << cases << " cases\n";
    return 0;
}
