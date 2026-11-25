// SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// Unit test of nearest_smaller_value (the reference value is stored as an IEEE or IBM single
// precision float, rounded down): the result must be the largest value of the format that is not
// larger than x, compared with references built from the definitions of the formats:
// - IEEE: for values at, between and around the floats of every exponent (normal range); below
//   FLT_MIN like the legacy ecCodes table, which has no denormals
// - IBM: 24-bit mantissa, base-16 exponent, normalised; for values over the whole range 16^-60..16^60
#include <cfloat>
#include <cmath>
#include <limits>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <random>
#include <string>

#include "simple_packing/Utils.h"

using namespace sp;

static int failures = 0;

static void check(bool ok, const std::string& what)
{
    if (!ok && failures++ < 30) {
        std::cerr << "FAIL: " << what << '\n';
    }
}

// The largest float not larger than x (normal range)
static double ieeeReference(double x)
{
    float f = static_cast<float>(x);
    if (static_cast<double>(f) > x) {
        f = std::nextafter(f, -INFINITY);
    }
    return f;
}

// The largest normalised IBM single precision value not larger than x (m * 16^(e - 5), m in [2^20, 2^24))
static double ibmReference(double x)
{
    if (x == 0) {
        return 0;
    }
    const double a = std::fabs(x);
    int e = static_cast<int>(std::floor(std::log(a) / std::log(16.0)));
    while (std::ldexp(1.0, 4 * e) > a)
        --e;  // 16^e <= a < 16^(e + 1)
    while (std::ldexp(1.0, 4 * (e + 1)) <= a)
        ++e;
    const double ulp = std::ldexp(1.0, 4 * (e - 5));
    if (x > 0) {
        return std::floor(a / ulp) * ulp;
    }
    const double m = std::ceil(a / ulp);  // a larger magnitude: the next value below x
    return -(m * ulp);                    // m = 2^24 is 16^(e + 1), also representable
}

static void ieee()
{
    std::mt19937_64 rng(13);
    for (int exponent = -126; exponent <= 127; ++exponent) {
        for (int i = 0; i < 200; ++i) {
            const uint32_t mantissa = i < 2 ? (i == 0 ? 0u : 0x7FFFFFu) : static_cast<uint32_t>(rng() & 0x7FFFFF);
            const uint32_t bits = (static_cast<uint32_t>(exponent + 127) << 23) | mantissa;
            float f;
            std::memcpy(&f, &bits, 4);
            for (double sign : { 1.0, -1.0 }) {
                const double base = sign * f;
                for (double x : { base, std::nextafter(base, -INFINITY), std::nextafter(base, INFINITY) }) {
                    if (std::fabs(x) < FLT_MIN || std::fabs(x) > FLT_MAX) continue;
                    const auto r = nearest_smaller_value(x, FloatType::IEEE32);
                    if (!(r && *r == ieeeReference(x))) {
                        check(false, "IEEE nearest float <= " + std::to_string(x) + " (exponent " + std::to_string(exponent) + ")");
                    }
                }
            }
        }
    }
    // Below FLT_MIN (no denormals in the table): 0 for positive values, -FLT_MIN for negative ones
    for (double x : { 0x1p-130, 0x1p-149, FLT_MIN * 0.75 }) {
        const auto pos = nearest_smaller_value(x, FloatType::IEEE32), neg = nearest_smaller_value(-x, FloatType::IEEE32);
        check(pos && *pos == 0.0 && neg && *neg == -FLT_MIN, "IEEE nearest float <= +-" + std::to_string(x));
    }
}

static void ibm()
{
    std::mt19937_64 rng(17);
    std::uniform_real_distribution<double> mant(1.0, 16.0);
    for (int e = -60; e <= 60; ++e) {
        for (int i = 0; i < 300; ++i) {
            const double a = (i == 0 ? 1.0 : i == 1 ? 16.0 - 0x1p-40
                                                    : mant(rng)) *
                             std::ldexp(1.0, 4 * e);
            for (double x : { a, -a, std::ldexp(std::floor(std::ldexp(a, 20 - 4 * e)), 4 * e - 20) /* an IBM value */ }) {
                const auto r = nearest_smaller_value(x, FloatType::IBM32);
                const double expected = ibmReference(x);
                if (!(r && *r == expected)) {
                    check(false, "IBM nearest value <= " + std::to_string(x) + " (16^" + std::to_string(e) + "): " + (r ? std::to_string(*r) : "none") + " instead of " +
                                     std::to_string(expected));
                }
            }
        }
    }
}

// Edges of both formats: zero, the smallest and largest values, values beyond the range, values
// that round up into the next binade (halfway above the largest mantissa) and values between two
// representable values
static void edges()
{
    auto expectValue = [](double x, FloatType type, double expected, const std::string& what) {
        const auto r = nearest_smaller_value(x, type);
        check(r && *r == expected, what + ": nearest value <= " + std::to_string(x) + " is " + (r ? std::to_string(*r) : "none") + " instead of " + std::to_string(expected));
    };
    auto expectNone = [](double x, FloatType type, const std::string& what) {
        check(!nearest_smaller_value(x, type), what + ": a value <= " + std::to_string(x) + " was found");
    };

    const double inf = std::numeric_limits<double>::infinity(), nan = std::numeric_limits<double>::quiet_NaN();

    // IEEE
    expectValue(0.0, FloatType::IEEE32, 0.0, "IEEE zero");
    expectValue(-0.0, FloatType::IEEE32, 0.0, "IEEE -0");
    expectValue(FLT_MAX, FloatType::IEEE32, FLT_MAX, "IEEE FLT_MAX");
    expectValue(-FLT_MAX, FloatType::IEEE32, -FLT_MAX, "IEEE -FLT_MAX");
    expectValue(FLT_MIN, FloatType::IEEE32, FLT_MIN, "IEEE FLT_MIN");
    expectValue(-FLT_MIN, FloatType::IEEE32, -FLT_MIN, "IEEE -FLT_MIN");
    for (double x : { std::nextafter(static_cast<double>(FLT_MAX), inf), -std::nextafter(static_cast<double>(FLT_MAX), inf), 1e300, -1e300, inf, -inf, nan })
        expectNone(x, FloatType::IEEE32, "IEEE beyond the range");
    std::mt19937_64 rng(19);
    for (int exponent = -126; exponent <= 126; ++exponent) {
        // The largest float of the binade and the next power of two: halfway between them and just
        // below or above that point (ieee_to_long rounds them up into the next binade)
        const double top = std::ldexp(0xFFFFFF, exponent - 23), next = std::ldexp(1.0, exponent + 1);
        const double half = (top + next) / 2;
        for (double x : { half, std::nextafter(half, 0.0), std::nextafter(half, inf), std::nextafter(next, 0.0) }) {
            for (double s : { 1.0, -1.0 })
                expectValue(s * x, FloatType::IEEE32, ieeeReference(s * x), "IEEE halfway (2^" + std::to_string(exponent) + ")");
        }
        // Random values between two floats
        for (int i = 0; i < 20; ++i) {
            const double x = std::ldexp(1.0 + static_cast<double>(rng() >> 11) * 0x1p-53, exponent);
            for (double s : { 1.0, -1.0 })
                expectValue(s * x, FloatType::IEEE32, ieeeReference(s * x), "IEEE between floats");
        }
    }

    // IBM: value = m * 16^(e - 70), m in [0x100000, 0xffffff], e in 0..127
    const double ibmMin = std::ldexp(0x100000, -4 * 70), ibmMax = std::ldexp(0xFFFFFF, 4 * 57);
    expectValue(0.0, FloatType::IBM32, 0.0, "IBM zero");
    expectValue(ibmMax, FloatType::IBM32, ibmMax, "IBM largest value");
    expectValue(-ibmMax, FloatType::IBM32, -ibmMax, "IBM -largest value");
    expectValue(ibmMin, FloatType::IBM32, ibmMin, "IBM smallest value");
    expectValue(-ibmMin, FloatType::IBM32, -ibmMin, "IBM -smallest value");
    // Below the smallest normalised value: 0 for positive values, -smallest for negative ones
    for (double x : { ibmMin / 2, ibmMin * 0.999, 1e-300 }) {
        expectValue(x, FloatType::IBM32, 0.0, "IBM below the smallest value");
        expectValue(-x, FloatType::IBM32, -ibmMin, "IBM above -smallest value");
    }
    for (double x : { std::nextafter(ibmMax, inf), -std::nextafter(ibmMax, inf), 1e300, -1e300, inf, -inf, nan })
        expectNone(x, FloatType::IBM32, "IBM beyond the range");
    // Values that round up to 16^(e + 1) (halfway between the largest value of exponent e and
    // 16^(e + 1), and above): like legacy ecCodes (grib_nearest_smaller_ibm_float, which sets the
    // mantissa to 0x800000 instead of 0x100000 when rounding overflows) the result is an IBM value
    // not larger than x, but not the nearest one: the same reference value as legacy ecCodes
    for (int e = -64; e <= 56; ++e) {
        const double top = std::ldexp(0xFFFFFF, 4 * (e - 6) + 4), next = std::ldexp(1.0, 4 * (e + 1));
        const double half = (top + next) / 2;
        for (double x : { half, std::nextafter(half, 0.0), std::nextafter(half, inf), std::nextafter(next, 0.0) }) {
            for (double s : { 1.0, -1.0 }) {
                const auto r = nearest_smaller_value(s * x, FloatType::IBM32);
                check(r && *r <= s * x && ibmReference(*r) == *r, "IBM value <= " + std::to_string(s * x) + " (16^" + std::to_string(e) + ")");
            }
        }
    }
    // Pinned results of legacy ecCodes
    expectValue(0x1.ffffffp+3, FloatType::IBM32, 0x1.ffffep+3, "IBM legacy rounding");
    expectValue(-0x1.ffffffp+3, FloatType::IBM32, -0x1p+7, "IBM legacy rounding");
    expectValue(-0x1.fffffffffffffp+3, FloatType::IBM32, -0x1p+4, "IBM legacy rounding");
}

int main()
{
    ieee();
    ibm();
    edges();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }
    std::cout << "OK\n";
    return 0;
}
