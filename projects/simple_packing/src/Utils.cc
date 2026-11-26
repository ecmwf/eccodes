// SPDX-FileCopyrightText: 2023 European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "simple_packing/detail/Utils.h"

#include <array>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>

namespace sp::detail
{

namespace
{

constexpr double dbl_max = std::numeric_limits<double>::max();

// The index of the largest of the first n (sorted) values that is not larger than x; 0 if there is none
// (bisection)
template <size_t N>
size_t index_not_above(const std::array<double, N>& values, size_t n, double x)
{
    size_t low = 0, high = n;
    while (high - low > 1) {
        const size_t mid = (low + high) / 2;
        (x >= values[mid] ? low : high) = mid;
    }
    return low;
}


// --- IEEE 754 single precision floats (GRIB edition 2), as in ecCodes grib_ieeefloat.cc ---
// The bits of a float: sign (1), biased exponent (8), mantissa (23, without the leading one).
// Only zero and normal values are produced (no denormals).

constexpr uint32_t ieee_mantissa_min = 0x800000;  // 2^23: the mantissa with its leading one
constexpr uint32_t ieee_mantissa_max = 0xffffff;

struct IeeeTable
{
    std::array<double, 255> e;  // e[i] = 2^(i - 150)
    std::array<double, 255> v;  // v[i] = smallest value with exponent i
    double vmin;
    double vmax;
};

constexpr IeeeTable make_ieee_table()
{
    IeeeTable t{};
    double e = 1;
    for (size_t i = 1; i <= 104; ++i) {
        e *= 2;
        t.e[150 + i] = e;
        t.v[150 + i] = e * ieee_mantissa_min;
    }
    t.e[150] = 1;
    t.v[150] = ieee_mantissa_min;
    e = 1;
    for (size_t i = 1; i < 150; ++i) {
        e /= 2;
        t.e[150 - i] = e;
        t.v[150 - i] = e * ieee_mantissa_min;
    }
    t.vmin = t.v[1];                        // FLT_MIN
    t.vmax = t.e[254] * ieee_mantissa_max;  // FLT_MAX
    return t;
}

constexpr IeeeTable ieee_table = make_ieee_table();

// The bits of the float nearest to x (the callers check that |x| <= FLT_MAX); 0 or the sign for
// |x| < FLT_MIN
uint32_t ieee_bits(double x)
{
    constexpr double rounding_limit = ieee_mantissa_max + 0.5;
    uint32_t sign = 0;
    if (x < 0) {
        sign = 1;
        x = -x;
    }
    if (x < ieee_table.vmin) {
        return sign << 31;
    }
    assert(x <= ieee_table.vmax);  // the callers check the range

    uint32_t e = static_cast<uint32_t>(index_not_above(ieee_table.v, 254, x));
    x /= ieee_table.e[e];
    assert(x >= ieee_mantissa_min);  // v[e] = mantissa_min * e[e] <= x

    while (x > rounding_limit) {
        x /= 2;
        e++;
    }
    uint32_t m = static_cast<uint32_t>(x + 0.5);
    if (m > ieee_mantissa_max) {
        e++;
        m = ieee_mantissa_min;
    }
    return (sign << 31) | (e << 23) | (m & 0x7fffff);
}

double ieee_value(uint32_t bits)
{
    const uint32_t sign = bits & 0x80000000;
    const uint32_t e = (bits & 0x7f800000) >> 23;
    const uint32_t m = bits & 0x007fffff;

    assert(e != 0 || m == 0);  // only the bits of zero and of normal values are produced (no denormals)
    if (e == 0) {
        return 0;
    }
    const double value = (m | ieee_mantissa_min) * ieee_table.e[e];
    return sign ? -value : value;
}

// The bits of the largest float <= x (the callers check that |x| <= FLT_MAX)
uint32_t ieee_nearest_smaller_bits(double x)
{
    if (x == 0) {
        return 0;
    }
    uint32_t bits = ieee_bits(x);
    const double y = ieee_value(bits);
    if (!(x < y)) {
        return bits;
    }
    if (x < 0 && -x < ieee_table.vmin) {
        bits = 0x80800000;  // -FLT_MIN
    }
    else {
        uint32_t e = (bits & 0x7f800000) >> 23;
        const uint32_t m = (bits & 0x007fffff) | ieee_mantissa_min;
        const bool negative = (bits & 0x80000000) != 0;
        if (m == ieee_mantissa_min && !negative) {
            assert(e > 1);  // x >= FLT_MIN here
            e--;
        }
        bits = ieee_bits(y - ieee_table.e[e]);
    }
    assert(x >= ieee_value(bits));
    return bits;
}

// Largest IEEE 32 bit float <= a; std::nullopt if there is none
std::optional<double> nearest_smaller_ieee_float(double a)
{
    if (!(std::fabs(a) <= ieee_table.vmax)) {  // beyond the float range on either side, or NaN
        return std::nullopt;
    }
    return ieee_value(ieee_nearest_smaller_bits(a));
}


// --- IBM single precision floats (GRIB edition 1), as in ecCodes grib_ibmfloat.cc ---
// value = mantissa (24 bits) * 16^(exponent - 70), mantissa normalised to >= 0x100000

constexpr uint32_t ibm_mantissa_min = 0x100000;
constexpr uint32_t ibm_mantissa_max = 0xffffff;

struct IbmTable
{
    std::array<double, 128> e;  // 16^(i - 70)
    std::array<double, 128> v;  // smallest normalised value with exponent i
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
        t.v[i] = t.e[i] * ibm_mantissa_min;
    }
    t.vmin = t.e[0] * ibm_mantissa_min;
    t.vmax = t.e[127] * ibm_mantissa_max;
    return t;
}

constexpr IbmTable ibm_table = make_ibm_table();

// The bits of the IBM value nearest to x (|x| must not be larger than the largest IBM value: the
// callers check the range); 0 or the sign below the smallest normalised value
uint32_t ibm_bits(double x)
{
    constexpr double rounding_limit = ibm_mantissa_max + 0.5;
    constexpr uint32_t legacy_mantissa_min = 0x800000;  // as legacy ecCodes, not ibm_mantissa_min
    uint32_t sign = 0;
    if (x < 0) {
        sign = 1;
        x = -x;
    }
    if (x < ibm_table.vmin) {
        return sign << 31;
    }
    assert(x <= ibm_table.vmax);

    uint32_t e = static_cast<uint32_t>(index_not_above(ibm_table.v, 127, x));
    x /= ibm_table.e[e];
    while (x < legacy_mantissa_min) {
        x *= 16;
        e--;
    }
    while (x > rounding_limit) {
        x /= 16;
        e++;
    }
    uint32_t m = static_cast<uint32_t>(x + 0.5);
    if (m > ibm_mantissa_max) {
        e++;
        m = legacy_mantissa_min;
    }
    return (sign << 31) | (e << 24) | m;
}

double ibm_value(uint32_t bits)
{
    const uint32_t sign = bits & 0x80000000;
    const uint32_t e = (bits & 0x7f000000) >> 24;
    const uint32_t m = bits & 0x00ffffff;
    if (e == 0 && m <= 1) {
        return 0;
    }
    const double value = m * ibm_table.e[e];
    return sign ? -value : value;
}

// Largest IBM float <= a; std::nullopt if there is none
std::optional<double> nearest_smaller_ibm_float(double a)
{
    if (!(std::fabs(a) <= ibm_table.vmax)) {  // also NaN
        return std::nullopt;
    }
    if (a == 0) {
        return 0.0;
    }

    uint32_t bits = ibm_bits(a);
    const double y = ibm_value(bits);
    double eps = 0;
    if (a < y) {
        if (a < 0 && -a < ibm_table.vmin) {
            bits = 0x80100000;  // -(smallest normalised value)
        }
        else {
            uint32_t e = (bits & 0x7f000000) >> 24;
            const uint32_t m = bits & 0x00ffffff;
            const bool negative = (bits & 0x80000000) != 0;
            if (m == ibm_mantissa_min && !negative) {
                assert(e > 0);  // a >= vmin here
                e--;
            }
            eps = ibm_table.e[e];
            bits = ibm_bits(y - eps);
        }
    }
    if (a < ibm_value(bits)) {
        bits = ibm_bits(a - eps);
    }
    assert(a >= ibm_value(bits));
    return ibm_value(bits);
}

// 2^0 to 2^31: number_of_bits counts the entries that are not larger than x
constexpr std::array<uint64_t, 32> powers_of_two = [] {
    std::array<uint64_t, 32> p{};
    for (size_t i = 0; i < p.size(); ++i) {
        p[i] = uint64_t{ 1 } << i;
    }
    return p;
}();

}  // anonymous namespace


// --- Public API implementations ---

std::optional<int64_t> number_of_bits(uint64_t x)
{
    // Linear: the values are mostly small
    int64_t count = 0;
    for (const uint64_t p : powers_of_two) {
        if (x < p) {
            return count;
        }
        ++count;
    }
    return std::nullopt;  // x >= 2^31
}

std::optional<double> nearest_smaller_value(double val, FloatType type)
{
    return type == FloatType::IBM32 ? nearest_smaller_ibm_float(val) : nearest_smaller_ieee_float(val);
}

std::optional<int64_t> get_binary_scale_factor(double max, double min, int64_t bpval)
{
    constexpr int64_t last = 127;
    constexpr double doubling_limit = dbl_max / 2;  // zs must not overflow to infinity
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

    // 64-bit integers: up to 63 bits per value also where long has 32 bits
    const uint64_t maxint = static_cast<uint64_t>(sp_power<double>(bpval, 2)) - 1;
    const auto rounded = [range](double zs) { return static_cast<uint64_t>(range * zs + 0.5); };
    double zs = 1;
    int64_t scale = 0;

    // Scaling up must not overflow zs to infinity, otherwise the search below could never
    // halve it back (inf / 2 == inf): the range is too small for any binary scale factor
    while (range * zs <= maxint) {
        if (zs > doubling_limit) {
            return std::nullopt;
        }
        scale--;
        zs *= 2;
    }
    while (range * zs > maxint) {
        scale++;
        zs /= 2;
    }

    // Rounding can allow one more doubling: it must not overflow zs either (inf cannot be
    // converted to an integer, and the search below could never halve it back)
    while (rounded(zs) <= maxint) {
        if (zs > doubling_limit) {
            return std::nullopt;
        }
        scale--;
        zs *= 2;
    }
    while (rounded(zs) > maxint) {
        scale++;
        zs /= 2;
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
    const auto within = [](double v) { return v < dbl_max && v > -dbl_max; };  // finite, and not +-DBL_MAX
    if (!within(min_val) || !within(max_val) || min_val > max_val) {
        throw std::runtime_error("Values out of range (minimum " + std::to_string(min_val) + ", maximum " + std::to_string(max_val) +
                                 "): values must be finite and the minimum must not be larger than the maximum");
    }
}

}  // namespace sp::detail
