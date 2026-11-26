// SPDX-FileCopyrightText: 2023 European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "simple_packing/detail/Utils.h"

#include <array>
#include <cstdint>
#include <cassert>
#include <cmath>
#include <limits>

namespace sp::detail
{


// --- Internal helpers ---

namespace
{

constexpr double dbl_max = std::numeric_limits<double>::max();

const uint64_t nbits_table[32] = {
    0x1, 0x2, 0x4, 0x8, 0x10, 0x20,
    0x40, 0x80, 0x100, 0x200, 0x400, 0x800,
    0x1000, 0x2000, 0x4000, 0x8000, 0x10000, 0x20000,
    0x40000, 0x80000, 0x100000, 0x200000, 0x400000, 0x800000,
    0x1000000, 0x2000000, 0x4000000, 0x8000000, 0x10000000, 0x20000000,
    0x40000000, 0x80000000
};

// Lookup table for IEEE 754 single precision floats, filled on first use
struct IeeeTable
{
    std::array<double, 255> e;  // e[i] = 2^(i - 150)
    std::array<double, 255> v;  // v[i] = smallest value with exponent i
    double vmin;
    double vmax;
};

// Thread-safe: the first caller fills the table, concurrent callers wait for it
const IeeeTable& ieee_table()
{
    static const IeeeTable table = [] {
        IeeeTable t{};
        const uint64_t mmin = 0x800000;
        const uint64_t mmax = 0xffffff;
        double e = 1;
        for (uint64_t i = 1; i <= 104; i++) {
            e *= 2;
            t.e[i + 150] = e;
            t.v[i + 150] = e * mmin;
        }
        t.e[150] = 1;
        t.v[150] = mmin;
        e = 1;
        for (uint64_t i = 1; i < 150; i++) {
            e /= 2;
            t.e[150 - i] = e;
            t.v[150 - i] = e * mmin;
        }
        t.vmin = t.v[1];
        t.vmax = t.e[254] * mmax;
        return t;
    }();
    return table;
}


void binary_search(const double xx[], const uint64_t n, double x, uint64_t* j)
{
    uint64_t ju, jm, jl;
    jl = 0;
    ju = n;
    while (ju - jl > 1) {
        jm = (ju + jl) >> 1;
        if (x >= xx[jm])
            jl = jm;
        else
            ju = jm;
    }
    *j = jl;
}


uint64_t ieee_to_long(double x)
{
    const IeeeTable& t = ieee_table();
    uint64_t s = 0;
    uint64_t mmax = 0xffffff;
    uint64_t mmin = 0x800000;
    uint64_t m = 0;
    uint64_t e = 0;
    double rmmax = mmax + 0.5;

    if (x < 0) {
        s = 1;
        x = -x;
    }

    if (x < t.vmin) {
        return (s << 31);
    }

    assert(x <= t.vmax);  // the callers check the range

    binary_search(t.v.data(), 254, x, &e);

    x /= t.e[e];
    assert(x >= mmin);  // v[e] = mmin * e[e] <= x

    while (x > rmmax) {
        x /= 2;
        e++;
    }

    m = x + 0.5;
    if (m > mmax) {
        e++;
        m = 0x800000;
    }

    return (s << 31) | (e << 23) | (m & 0x7fffff);
}


double long_to_ieee(uint64_t x)
{
    uint64_t s = x & 0x80000000;
    uint64_t c = (x & 0x7f800000) >> 23;
    uint64_t m = (x & 0x007fffff);

    double val;

    // Only the bits of zero and of normal values are produced (no denormals)
    assert(c != 0 || m == 0);
    if (c == 0)
        return 0;

    m |= 0x800000;

    val = m * ieee_table().e[c];
    if (s)
        val = -val;

    return val;
}


uint64_t ieee_nearest_smaller_to_long(double x)
{
    const IeeeTable& t = ieee_table();
    uint64_t l;
    uint64_t e;
    uint64_t m;
    uint64_t s;
    uint64_t mmin = 0x800000;

    if (x == 0)
        return 0;

    l = ieee_to_long(x);
    double y = long_to_ieee(l);

    if (x < y) {
        if (x < 0 && -x < t.vmin) {
            l = 0x80800000;
        }
        else {
            e = (l & 0x7f800000) >> 23;
            m = (l & 0x007fffff) | 0x800000;
            s = l & 0x80000000;

            if (m == mmin && !s) {
                assert(e > 1);  // x >= FLT_MIN here
                e = e - 1;
            }

            double eps = t.e[e];

            l = ieee_to_long(y - eps);
        }
    }
    else
        return l;

    assert(x >= long_to_ieee(l));

    return l;
}


// Largest IEEE 32 bit float <= a; std::nullopt if there is none
std::optional<double> nearest_smaller_ieee_float(double a)
{
    if (!(std::fabs(a) <= ieee_table().vmax)) {  // beyond the float range on either side, or NaN
        return std::nullopt;
    }

    return long_to_ieee(ieee_nearest_smaller_to_long(a));
}


// --- IBM single precision floats (GRIB edition 1), as in ecCodes grib_ibmfloat.cc ---
// value = mantissa (24 bits) * 16^(exponent - 70), mantissa normalised to >= 0x100000

struct IbmTable
{
    std::array<double, 128> e;  // 16^(i - 70)
    std::array<double, 128> v;  // smallest normalised value with exponent i
    double vmin;
    double vmax;
};

const IbmTable& ibm_table()
{
    static const IbmTable table = [] {
        IbmTable t{};
        for (int i = 0; i < 128; ++i) {
            t.e[i] = std::ldexp(1.0, 4 * (i - 70));
            t.v[i] = t.e[i] * 0x100000;
        }
        t.vmin = t.e[0] * 0x100000;
        t.vmax = t.e[127] * 0xffffff;
        return t;
    }();
    return table;
}

// |x| must not be larger than the largest IBM value (the callers check the range)
uint64_t ibm_to_long(double x)
{
    const IbmTable& t = ibm_table();
    const uint64_t mmax = 0xffffff;
    const uint64_t mmin = 0x800000;
    const double rmmax = mmax + 0.5;
    uint64_t s = 0;
    uint64_t e = 0;

    if (x < 0) {
        s = 1;
        x = -x;
    }
    if (x < t.vmin) {
        return s << 31;
    }
    assert(x <= t.vmax);

    binary_search(t.v.data(), 127, x, &e);
    x /= t.e[e];
    while (x < mmin) {
        x *= 16;
        e--;
    }
    while (x > rmmax) {
        x /= 16;
        e++;
    }
    uint64_t m = static_cast<uint64_t>(x + 0.5);
    if (m > mmax) {
        e++;
        m = 0x800000;
    }
    return (s << 31) | (e << 24) | m;
}

double long_to_ibm(uint64_t x)
{
    const uint64_t s = x & 0x80000000;
    const uint64_t c = (x & 0x7f000000) >> 24;
    const uint64_t m = (x & 0x00ffffff);
    if (c == 0 && m <= 1) {
        return 0;
    }
    const double val = m * ibm_table().e[c];
    return s ? -val : val;
}

// Largest IBM float <= a; std::nullopt if there is none
std::optional<double> nearest_smaller_ibm_float(double a)
{
    const IbmTable& t = ibm_table();
    if (!(std::fabs(a) <= t.vmax)) {  // also NaN
        return std::nullopt;
    }
    if (a == 0) {
        return 0.0;
    }

    uint64_t l = ibm_to_long(a);
    const double y = long_to_ibm(l);
    double eps = 0;
    if (a < y) {
        if (a < 0 && -a < t.vmin) {
            l = 0x80100000;
        }
        else {
            uint64_t e = (l & 0x7f000000) >> 24;
            const uint64_t m = (l & 0x00ffffff);
            const uint64_t s = l & 0x80000000;
            if (m == 0x100000 && !s) {
                assert(e > 0);  // a >= vmin here
                e = e - 1;
            }
            eps = t.e[e];
            l = ibm_to_long(y - eps);
        }
    }
    if (a < long_to_ibm(l)) {
        l = ibm_to_long(a - eps);
    }
    assert(a >= long_to_ibm(l));
    return long_to_ibm(l);
}

}  // anonymous namespace


// --- Public API implementations ---


std::optional<int64_t> number_of_bits(uint64_t x)
{
    const int count = sizeof(nbits_table) / sizeof(nbits_table[0]);
    const uint64_t* n = nbits_table;
    int64_t result = 0;
    while (x >= *n) {
        n++;
        result++;
        if (result >= count) {
            return std::nullopt;
        }
    }
    return result;
}


std::optional<double> nearest_smaller_value(double val, FloatType type)
{
    return type == FloatType::IBM32 ? nearest_smaller_ibm_float(val) : nearest_smaller_ieee_float(val);
}


std::optional<int64_t> get_binary_scale_factor(double max, double min, int64_t bpval)
{
    const double range = max - min;
    double zs = 1;
    int64_t scale = 0;
    const int64_t last = 127;

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

    // 64-bit integers: up to 63 bits per value also where long has 32 bits
    const uint64_t maxint = static_cast<uint64_t>(sp_power<double>(bpval, 2)) - 1;

    assert(bpval >= 1);
    if (range == 0)
        return 0;

    // Scaling up must not overflow zs to infinity, otherwise the search below could never
    // halve it back (inf / 2 == inf): the range is too small for any binary scale factor
    while ((range * zs) <= maxint) {
        if (zs > (std::numeric_limits<double>::max() / 2)) {
            return std::nullopt;
        }
        scale--;
        zs *= 2;
    }

    while ((range * zs) > maxint) {
        scale++;
        zs /= 2;
    }

    // Rounding can allow one more doubling: it must not overflow zs either (inf cannot be
    // converted to an integer, and the search below could never halve it back)
    while (static_cast<uint64_t>(range * zs + 0.5) <= maxint) {
        if (zs > (std::numeric_limits<double>::max() / 2)) {
            return std::nullopt;
        }
        scale--;
        zs *= 2;
    }

    while (static_cast<uint64_t>(range * zs + 0.5) > maxint) {
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
    if (!(min_val < dbl_max && min_val > -dbl_max) || !(max_val < dbl_max && max_val > -dbl_max) || min_val > max_val) {
        throw std::runtime_error("Values out of range (minimum " + std::to_string(min_val) + ", maximum " + std::to_string(max_val) +
                                 "): values must be finite and the minimum must not be larger than the maximum");
    }
}

}  // namespace sp::detail
