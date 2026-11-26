// SPDX-FileCopyrightText: 2023 European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "simple_packing/detail/Utils.h"

#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>

#include "simple_packing/detail/Compat.h"

namespace sp::detail
{

namespace
{

// The biased exponent of a double (0 for zero and subnormal values): floor(log2(|x|)) + 1023 for
// normal values
inline int32_t biased_exponent(double x)
{
    uint64_t bits = 0;
    std::memcpy(&bits, &x, sizeof bits);
    return static_cast<int32_t>((bits >> 52) & 0x7ff);
}

// 2^n for -1074 <= n <= 1023: the bits of a normal double directly (std::ldexp is a library call)
inline double power_of_two(int64_t n)
{
    if (n < -1022) {
        return std::ldexp(1.0, static_cast<int32_t>(n));  // subnormal
    }
    const uint64_t bits = static_cast<uint64_t>(n + 1023) << 52;
    double d = 0;
    std::memcpy(&d, &bits, sizeof d);
    return d;
}

// --- IEEE 754 single precision floats (GRIB edition 2), as in ecCodes grib_ieeefloat.cc ---

// The internal functions return NaN for no value (a result is never NaN): GCC passes a returned
// std::optional<double> through memory, slowly; nearest_smaller_value makes the optional once
constexpr double none = std::numeric_limits<double>::quiet_NaN();

// Largest IEEE 32 bit float <= a; NaN if there is none. Only zero and normal floats (no
// denormals): +0 for 0 <= a < FLT_MIN, -FLT_MIN for -FLT_MIN < a < 0
double nearest_smaller_ieee_float(double a)
{
    constexpr double flt_max = std::numeric_limits<float>::max();
    constexpr double flt_min = std::numeric_limits<float>::min();
    const double magnitude = std::fabs(a);
    if (!(magnitude <= flt_max)) {  // beyond the float range on either side, or NaN
        return none;
    }
    if (magnitude < flt_min) {
        return a < 0 ? -flt_min : 0.0;  // also +0 for -0
    }
    // The nearest float (in any rounding mode within one step), one step down if it is above a;
    // |a| >= FLT_MIN, so the result is a normal float
    float f = static_cast<float>(a);
    if (static_cast<double>(f) > a) {
        uint32_t bits = 0;
        std::memcpy(&bits, &f, sizeof bits);
        bits = f > 0 ? bits - 1 : bits + 1;  // sign and magnitude: one step towards -infinity
        std::memcpy(&f, &bits, sizeof f);
    }
    return f;
}


// --- IBM single precision floats (GRIB edition 1), as in ecCodes grib_ibmfloat.cc ---
// value = mantissa (24 bits) * 16^(exponent - 70), mantissa normalised to >= 0x100000

constexpr uint32_t ibm_mantissa_min = 0x100000;
constexpr uint32_t ibm_mantissa_max = 0xffffff;

struct IbmTable
{
    std::array<double, 128> e;        // 16^(i - 70)
    std::array<double, 128> inverse;  // 16^(70 - i): exact, x * inverse[i] == x / e[i]
    double vmin;
    double vmax;
};

constexpr IbmTable make_ibm_table()
{
    IbmTable t{};
    double e = 1;  // exact powers of 16: 16^0, then 16^-1, ... 16^-70 and 16^1, ... 16^57
    t.e[70] = e;
    for (size_t i = 70; i-- > 0;) {
        e /= 16;
        t.e[i] = e;
    }
    e = 1;
    for (size_t i = 71; i < 128; ++i) {
        e *= 16;
        t.e[i] = e;
    }
    for (size_t i = 0; i < 128; ++i) {
        t.inverse[i] = 1 / t.e[i];
    }
    t.vmin = t.e[0] * ibm_mantissa_min;
    t.vmax = t.e[127] * ibm_mantissa_max;
    return t;
}

constexpr IbmTable ibm_table = make_ibm_table();

// Largest IBM float <= a; NaN if there is none. +0 for 0 <= a < vmin, -vmin for
// -vmin < a < 0. The same results as legacy ecCodes (grib_nearest_smaller_ibm_float): the largest
// normalised value <= a, except for a mantissa of exactly 0xffffff.5 (halfway to the next power of
// 16), where the legacy rounding overflows (the mantissa becomes 0x800000 of the next exponent)
double nearest_smaller_ibm_float(double a)
{
    const double x = std::fabs(a);
    if (!(x <= ibm_table.vmax)) {  // also NaN
        return none;
    }
    if (a == 0) {
        return 0.0;
    }
    if (x < ibm_table.vmin) {
        return a < 0 ? -ibm_table.vmin : 0.0;
    }
    // The exponent e whose smallest normalised value 2^(4e - 260) is <= x < 2^(4e - 256), and the
    // mantissa x / 16^(e - 70) in [0x100000, 0x1000000) (x >= vmin = 2^-260 is a normal double)
    const int32_t e = (biased_exponent(x) - 1023 + 260) / 4;
    const double unit = ibm_table.e[e];
    const double mantissa = x * ibm_table.inverse[e];  // exact
    if (mantissa == ibm_mantissa_max + 0.5) {
        return a > 0 ? 0xfffff0 * unit : -0x8000000 * unit;  // legacy rounding overflow
    }
    // floor for a > 0, ceil for a < 0 (|a| rounded up); exact through the integer conversion (< 2^24)
    const double truncated = static_cast<double>(static_cast<int64_t>(mantissa));
    if (a > 0) {
        return truncated * unit;
    }
    return -(truncated < mantissa ? truncated + 1 : truncated) * unit;
}

}  // anonymous namespace


// --- Public API implementations ---

std::optional<double> nearest_smaller_value(double val, FloatType type)
{
    const double r = type == FloatType::IBM32 ? nearest_smaller_ibm_float(val) : nearest_smaller_ieee_float(val);
    if (std::isnan(r)) {
        return std::nullopt;
    }
    return r;
}

std::optional<int64_t> get_binary_scale_factor(double max, double min, int64_t bpval)
{
    constexpr int64_t last = 127;  // the largest binary scale factor (and -last the smallest)
    const double range = max - min;

    // Check the arguments before using them (ECC-246, ECC-1927 in ecCodes)
    if (bpval < 1 || bpval >= 64) {
        throw std::invalid_argument("Simple packing: unsupported number of bits per value " + std::to_string(bpval) +
                                    " (must be between 1 and 63)");
    }
    if (!std::isfinite(range)) {
        throw std::range_error("Simple packing: the range of the values is not finite");
    }
    if (range < 0) {  // maximum < minimum
        throw std::range_error("Simple packing: the maximum is smaller than the minimum");
    }
    if (range == 0) {
        return 0;
    }

    // The scale factor E with the largest factor 2^-E for which range * 2^-E fits bpval bits, as
    // legacy ecCodes finds it (grib_get_binary_scale_fact): first E with range * 2^-E <= max_code
    // (max_code as a double: 2^bpval - 1, or 2^bpval from 54 bits on), then E with
    // round(range * 2^-E) <= max_code. The products are exact (powers of two times the range).
    // Legacy ecCodes searches from E = 0 in steps of one; with ex the binary exponent of the range,
    // the first E is ex + 1 - bpval or ex + 2 - bpval and the result at most one above it, so the
    // search starts at ex + 1 - bpval and takes one or two steps per loop. A result below -last is
    // no value: also for subnormal ranges, and for all ranges with ex + 3 - bpval < -last (factors
    // up to 2^133 then remain: far from an overflow to infinity, which legacy ecCodes has to check).
    const int32_t biased = biased_exponent(range);
    const int64_t ex = static_cast<int64_t>(biased) - 1023;
    if (biased == 0 || ex + 3 - bpval < -last) {
        return std::nullopt;  // the range is too small even with the smallest binary scale factor
    }
    const uint64_t max_code = (uint64_t{ 1 } << bpval) - 1;
    int64_t scale = ex + 1 - bpval;
    double factor = power_of_two(-scale);  // 2^-scale, exact (-1023 <= -scale <= 131)

    while (range * factor <= max_code) {
        scale--;
        factor *= 2;
    }
    while (range * factor > max_code) {
        scale++;
        factor /= 2;
    }
    // Rounded, the scaled range can fit with one more doubling
    const auto rounded = [range](double f) { return static_cast<uint64_t>(range * f + 0.5); };
    while (rounded(factor) <= max_code) {
        scale--;
        factor *= 2;
    }
    while (rounded(factor) > max_code) {
        scale++;
        factor /= 2;
    }

    if (scale < -last) {
        return std::nullopt;  // the range is too small even with the smallest binary scale factor
    }
    if (scale > last) {  // ecCodes asserts this; an assert would be compiled out (NDEBUG)
        throw std::range_error("Simple packing: the range of the values is too large (binary scale factor " + std::to_string(scale) +
                               " beyond " + std::to_string(last) + ")");
    }
    return scale;
}

void check_data_values_range(double min_val, double max_val)
{
    constexpr double dbl_max = std::numeric_limits<double>::max();
    const auto within = [](double v) { return v < dbl_max && v > -dbl_max; };  // finite, and not +-DBL_MAX
    if (!within(min_val) || !within(max_val) || min_val > max_val) {
        throw std::runtime_error("Values out of range (minimum " + std::to_string(min_val) + ", maximum " + std::to_string(max_val) +
                                 "): values must be finite and the minimum must not be larger than the maximum");
    }
}

}  // namespace sp::detail
