/*
 * (C) Copyright 2023- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "Utils.h"

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

namespace {

constexpr double dbl_min = std::numeric_limits<double>::min();
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

ieee_table_t ieee_table = { 0, {0, }, {0, }, 0, 0 };


void init_ieee_table()
{
    if (!ieee_table.inited) {
        unsigned long i;
        unsigned long mmin = 0x800000;
        unsigned long mmax = 0xffffff;
        double e           = 1;
        for (i = 1; i <= 104; i++) {
            e *= 2;
            ieee_table.e[i + 150] = e;
            ieee_table.v[i + 150] = e * mmin;
        }
        ieee_table.e[150] = 1;
        ieee_table.v[150] = mmin;
        e                 = 1;
        for (i = 1; i < 150; i++) {
            e /= 2;
            ieee_table.e[150 - i] = e;
            ieee_table.v[150 - i] = e * mmin;
        }
        ieee_table.vmin   = ieee_table.v[1];
        ieee_table.vmax   = ieee_table.e[254] * mmax;
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
    unsigned long s    = 0;
    unsigned long mmax = 0xffffff;
    unsigned long mmin = 0x800000;
    unsigned long m    = 0;
    unsigned long e    = 0;
    double rmmax       = mmax + 0.5;

    init_table_if_needed();

    if (x < 0) {
        s = 1;
        x = -x;
    }

    if (x < ieee_table.vmin) {
        return (s << 31);
    }

    if (x > ieee_table.vmax) {
        fprintf(stderr, "ieee_to_long: Number is too large: x=%.20e > xmax=%.20e\n", x, ieee_table.vmax);
        assert(0);
        return 0;
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

    if (x < long_to_ieee(l)) {
        printf("ieee_nearest_smaller_to_long: x=%.20e long_to_ieee(0x%lX)=%.20e\n", x, l, long_to_ieee(l));
        assert(x >= long_to_ieee(l));
    }

    return l;
}


int nearest_smaller_ieee_float(double a, double* ret)
{
    unsigned long l = 0;

    init_table_if_needed();

    if (a > ieee_table.vmax) {
        std::cout << "Number is too large: x=" << a << " > xmax=" << ieee_table.vmax << " (IEEE float)";
        return SP_INTERNAL_ERROR;
    }

    l    = ieee_nearest_smaller_to_long(a);
    *ret = long_to_ieee(l);
    return SP_SUCCESS;
}


} // anonymous namespace


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


std::optional<double> nearest_smaller_value(double val)
{
    double nearest = 0;
    int err = nearest_smaller_ieee_float(val, &nearest);
    if (err != SP_SUCCESS) {
        return std::nullopt;
    }
    return nearest;
}


long get_binary_scale_factor(double max, double min, long bpval, int* ret)
{
    double range         = max - min;
    double zs            = 1;
    long scale           = 0;
    const long last      = 127;
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
        std::cout << "Minimum value out of range: " << min_val << std::endl;
        return SP_ENCODING_ERROR;
    }
    if (!(max_val < dbl_max && max_val > -dbl_max)) {
        std::cout << "Maximum value out of range: " << max_val << std::endl;
        return SP_ENCODING_ERROR;
    }

    return SP_SUCCESS;
}

