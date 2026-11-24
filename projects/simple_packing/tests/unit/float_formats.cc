// Unit test of nearest_smaller_value (the reference value is stored as an IEEE or IBM single
// precision float, rounded down): the result must be the largest value of the format that is not
// larger than x, compared with references built from the definitions of the formats:
// - IEEE: for values at, between and around the floats of every exponent (normal range); below
//   FLT_MIN like the legacy ecCodes table, which has no denormals
// - IBM: 24-bit mantissa, base-16 exponent, normalised; for values over the whole range 16^-60..16^60
#include <cfloat>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <random>
#include <string>

#include "simple_packing/Utils.h"

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
    while (std::ldexp(1.0, 4 * e) > a) --e;  // 16^e <= a < 16^(e + 1)
    while (std::ldexp(1.0, 4 * (e + 1)) <= a) ++e;
    const double ulp = std::ldexp(1.0, 4 * (e - 5));
    if (x > 0) {
        return std::floor(a / ulp) * ulp;
    }
    const double m = std::ceil(a / ulp);  // a larger magnitude: the next value below x
    return -(m * ulp);                     // m = 2^24 is 16^(e + 1), also representable
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
            const double a = (i == 0 ? 1.0 : i == 1 ? 16.0 - 0x1p-40 : mant(rng)) * std::ldexp(1.0, 4 * e);
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

int main()
{
    ieee();
    ibm();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }
    std::cout << "OK\n";
    return 0;
}
