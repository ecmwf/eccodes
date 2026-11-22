/*
 * (C) Copyright 2023- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "simple_packing/Utils.h"

#include <array>
#include <cmath>

#include <cmath>
#include <cstdio>
#include <limits>


size_t nSplits(size_t totalSize, size_t splitSize)
{
    size_t nSplits = totalSize / splitSize;
    size_t remainder = totalSize % splitSize;
    return nSplits + (remainder > 0 ? 1 : 0);
}


// --- Internal helpers ---

namespace
{

[[maybe_unused]] constexpr double dbl_min = std::numeric_limits<double>::min();
constexpr double dbl_max = std::numeric_limits<double>::max();

#define DebugAssert(a) assert(a)

const unsigned long nbits_table[32] = {
    0x1, 0x2, 0x4, 0x8, 0x10, 0x20,
    0x40, 0x80, 0x100, 0x200, 0x400, 0x800,
    0x1000, 0x2000, 0x4000, 0x8000, 0x10000, 0x20000,
    0x40000, 0x80000, 0x100000, 0x200000, 0x400000, 0x800000,
    0x1000000, 0x2000000, 0x4000000, 0x8000000, 0x10000000, 0x20000000,
    0x40000000, 0x80000000
};

ieee_table_t ieee_table = { 0, {
                                   0,
                               },
                            {
                                0,
                            },
                            0,
                            0 };


void init_ieee_table()
{
    if (!ieee_table.inited) {
        unsigned long i;
        unsigned long mmin = 0x800000;
        unsigned long mmax = 0xffffff;
        double e = 1;
        for (i = 1; i <= 104; i++) {
            e *= 2;
            ieee_table.e[i + 150] = e;
            ieee_table.v[i + 150] = e * mmin;
        }
        ieee_table.e[150] = 1;
        ieee_table.v[150] = mmin;
        e = 1;
        for (i = 1; i < 150; i++) {
            e /= 2;
            ieee_table.e[150 - i] = e;
            ieee_table.v[150 - i] = e * mmin;
        }
        ieee_table.vmin = ieee_table.v[1];
        ieee_table.vmax = ieee_table.e[254] * mmax;
        ieee_table.inited = 1;
    }
}

void init_table_if_needed()
{
    if (!ieee_table.inited)
        init_ieee_table();
}


void binary_search(const double xx[], const unsigned long n, double x, unsigned long* j)
{
    unsigned long ju, jm, jl;
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


unsigned long ieee_to_long(double x)
{
    unsigned long s = 0;
    unsigned long mmax = 0xffffff;
    unsigned long mmin = 0x800000;
    unsigned long m = 0;
    unsigned long e = 0;
    double rmmax = mmax + 0.5;

    init_table_if_needed();

    if (x < 0) {
        s = 1;
        x = -x;
    }

    if (x < ieee_table.vmin) {
        return (s << 31);
    }

    if (x > ieee_table.vmax) {
        return 0;  // not reached: the callers check the range
    }

    binary_search(ieee_table.v, 254, x, &e);

    x /= ieee_table.e[e];

    while (x < mmin) {
        x *= 2;
        e--;
    }

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


double long_to_ieee(unsigned long x)
{
    unsigned long s = x & 0x80000000;
    unsigned long c = (x & 0x7f800000) >> 23;
    unsigned long m = (x & 0x007fffff);

    double val;

    init_table_if_needed();

    if (c == 0 && m == 0)
        return 0;

    if (c == 0) {
        m |= 0x800000;
        c = 1;
    }
    else
        m |= 0x800000;

    val = m * ieee_table.e[c];
    if (s)
        val = -val;

    return val;
}


unsigned long ieee_nearest_smaller_to_long(double x)
{
    unsigned long l;
    unsigned long e;
    unsigned long m;
    unsigned long s;
    unsigned long mmin = 0x800000;

    if (x == 0)
        return 0;

    init_table_if_needed();

    l = ieee_to_long(x);
    double y = long_to_ieee(l);

    if (x < y) {
        if (x < 0 && -x < ieee_table.vmin) {
            l = 0x80800000;
        }
        else {
            e = (l & 0x7f800000) >> 23;
            m = (l & 0x007fffff) | 0x800000;
            s = l & 0x80000000;

            if (m == mmin) {
                e = s ? e : e - 1;
                if (e < 1)
                    e = 1;
                if (e > 254)
                    e = 254;
            }

            double eps = ieee_table.e[e];

            l = ieee_to_long(y - eps);
        }
    }
    else
        return l;

    assert(x >= long_to_ieee(l));

    return l;
}


int nearest_smaller_ieee_float(double a, double* ret)
{
    unsigned long l = 0;

    init_table_if_needed();

    if (!(std::fabs(a) <= ieee_table.vmax)) {  // beyond the float range on either side, or NaN
        return SP_INTERNAL_ERROR;
    }

    l = ieee_nearest_smaller_to_long(a);
    *ret = long_to_ieee(l);
    return SP_SUCCESS;
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

// Returns false if |x| is too large
bool ibm_to_long(double x, unsigned long& result)
{
    const IbmTable& t = ibm_table();
    const unsigned long mmax = 0xffffff;
    const unsigned long mmin = 0x800000;
    const double rmmax = mmax + 0.5;
    unsigned long s = 0;
    unsigned long e = 0;

    if (x < 0) {
        s = 1;
        x = -x;
    }
    if (x < t.vmin) {
        result = s << 31;
        return true;
    }
    if (x > t.vmax) {
        return false;
    }

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
    unsigned long m = static_cast<unsigned long>(x + 0.5);
    if (m > mmax) {
        e++;
        m = 0x800000;
    }
    result = (s << 31) | (e << 24) | m;
    return true;
}

double long_to_ibm(unsigned long x)
{
    const unsigned long s = x & 0x80000000;
    const unsigned long c = (x & 0x7f000000) >> 24;
    const unsigned long m = (x & 0x00ffffff);
    if (c == 0 && m <= 1) {
        return 0;
    }
    const double val = m * ibm_table().e[c];
    return s ? -val : val;
}

// Largest IBM float <= a; returns false if there is none
bool nearest_smaller_ibm_float(double a, double* ret)
{
    const IbmTable& t = ibm_table();
    if (!(std::fabs(a) <= t.vmax)) {  // also NaN
        return false;
    }
    if (a == 0) {
        *ret = 0;
        return true;
    }

    unsigned long l = 0;
    if (!ibm_to_long(a, l)) {
        return false;
    }
    const double y = long_to_ibm(l);
    double eps = 0;
    if (a < y) {
        if (a < 0 && -a < t.vmin) {
            l = 0x80100000;
        }
        else {
            unsigned long e = (l & 0x7f000000) >> 24;
            const unsigned long m = (l & 0x00ffffff);
            const unsigned long s = l & 0x80000000;
            if (m == 0x100000) {
                e = s ? e : e - 1;
                if (e > 127) {
                    e = 127;
                }
            }
            eps = t.e[e];
            if (!ibm_to_long(y - eps, l)) {
                return false;
            }
        }
    }
    if (a < long_to_ibm(l)) {
        if (!ibm_to_long(a - eps, l) || a < long_to_ibm(l)) {
            return false;
        }
    }
    *ret = long_to_ibm(l);
    return true;
}

}  // anonymous namespace


// --- Public API implementations ---


std::optional<long> number_of_bits(unsigned long x)
{
    const int count = sizeof(nbits_table) / sizeof(nbits_table[0]);
    const unsigned long* n = nbits_table;
    long result = 0;
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
    double nearest = 0;
    if (type == FloatType::IBM32) {
        if (!nearest_smaller_ibm_float(val, &nearest)) {
            return std::nullopt;
        }
        return nearest;
    }
    int err = nearest_smaller_ieee_float(val, &nearest);
    if (err != SP_SUCCESS) {
        return std::nullopt;
    }
    return nearest;
}


long get_binary_scale_factor(double max, double min, long bpval, int* ret)
{
    double range = max - min;
    double zs = 1;
    long scale = 0;
    const long last = 127;
    const unsigned long maxint = static_cast<unsigned long>(sp_power<double>(bpval, 2)) - 1;
    const size_t ulong_size = sizeof(maxint) * 8;

    if (bpval >= static_cast<long>(ulong_size)) {
        *ret = SP_OUT_OF_RANGE;
        return 0;
    }

    *ret = 0;
    if (bpval < 1) {
        *ret = SP_ENCODING_ERROR;
        return 0;
    }

    assert(bpval >= 1);
    if (range == 0)
        return 0;

    while ((range * zs) <= maxint) {
        scale--;
        zs *= 2;
    }

    while ((range * zs) > maxint) {
        scale++;
        zs /= 2;
    }

    while ((unsigned long)(range * zs + 0.5) <= maxint) {
        scale--;
        zs *= 2;
    }

    while ((unsigned long)(range * zs + 0.5) > maxint) {
        scale++;
        zs /= 2;
    }

    if (scale < -last) {
        *ret = SP_UNDERFLOW;
        scale = -last;
    }
    assert(scale <= last);
    return scale;
}


int check_data_values_range(double min_val, double max_val)
{
    if (!(min_val < dbl_max && min_val > -dbl_max)) {
        return SP_ENCODING_ERROR;
    }
    if (!(max_val < dbl_max && max_val > -dbl_max)) {
        return SP_ENCODING_ERROR;
    }

    return SP_SUCCESS;
}
