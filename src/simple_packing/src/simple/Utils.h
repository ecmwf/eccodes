/*
 * (C) Copyright 2023- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#pragma once

#include <cstddef>
#include <cstdint>
#include <cassert>
#include <cmath>
#include <iostream>
#include <limits>
#include <utility>
#include <vector>

#include <optional>


// --- Error codes ---

#define SP_SUCCESS 0
#define SP_NO_VALUES 1
#define SP_INVALID_BPV 2
#define SP_INTERNAL_ERROR 3
#define SP_LOG_DEBUG 4
#define SP_LOG_ERROR 5
#define SP_ENCODING_ERROR 6
#define SP_OUT_OF_RANGE 7
#define SP_UNDERFLOW 8
#define SP_ARRAY_TOO_SMALL 9

// --- Utility functions ---

size_t nSplits(size_t totalSize, size_t splitSize);


// --- Power function ---

/* Return n to the power of s */
template <typename T>
constexpr T sp_power(long s, long n)
{
    T divisor = 1.0;
    if (s == 0)
        return 1.0;
    if (s == 1)
        return n;
    while (s < 0) {
        divisor /= n;
        s++;
    }
    while (s > 0) {
        divisor *= n;
        s--;
    }
    return divisor;
}


// --- IEEE float utilities ---

struct ieee_table_t
{
    int inited;
    double e[255];
    double v[255];
    double vmin;
    double vmax;
};

std::optional<long> number_of_bits(unsigned long x);
std::optional<double> nearest_smaller_value(double val);
long get_binary_scale_factor(double max, double min, long bpval, int* ret);
int check_data_values_range(double min_val, double max_val);


// --- Byte swap macros ---

#if defined(_MSC_VER)
  #include <stdlib.h>
  #define bswap16(x) _byteswap_ushort(x)
  #define bswap32(x) _byteswap_ulong(x)
  #define bswap64(x) _byteswap_uint64(x)
#elif defined(__GNUC__) || defined(__clang__)
#if defined(__APPLE__)
  #include <libkern/OSByteOrder.h>
  #define bswap16(x) OSSwapInt16(x)
  #define bswap32(x) OSSwapInt32(x)
  #define bswap64(x) OSSwapInt64(x)
#else
  #include <byteswap.h>
  #define bswap16(x) __builtin_bswap16(x)
  #define bswap32(x) __builtin_bswap32(x)
  #define bswap64(x) __builtin_bswap64(x)
#endif

#else
// Fallback (portable but slower)
static inline uint16_t bswap16(uint16_t x) {
    return ((x >> 8) & 0x00FF) |
        ((x << 8) & 0xFF00);
}
static inline uint32_t bswap32(uint32_t x) {
    return ((x >> 24) & 0x000000FF) |
        ((x >> 8)  & 0x0000FF00) |
        ((x << 8)  & 0x00FF0000) |
        ((x << 24) & 0xFF000000);
}
static inline uint64_t bswap64(uint64_t x) {
    return ((x >> 56) & 0x00000000000000FFULL) |
        ((x >> 40) & 0x000000000000FF00ULL) |
        ((x >> 24) & 0x0000000000FF0000ULL) |
        ((x >> 8)  & 0x00000000FF000000ULL) |
        ((x << 8)  & 0x000000FF00000000ULL) |
        ((x << 24) & 0x0000FF0000000000ULL) |
        ((x << 40) & 0x00FF000000000000ULL) |
        ((x << 56) & 0xFF00000000000000ULL);
}
#endif
