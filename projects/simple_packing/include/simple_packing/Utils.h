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
#include <cmath>
#include <optional>
#include <string>
#include <stdexcept>
#include <limits>
#include <utility>

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


// --- Parameter limits ---

// Bits per value supported for encoding (2^bitsPerValue - 1 must fit into an unsigned long)
constexpr long maxEncodingBitsPerValue = 63;
// GRIB stores the scale factors with 16 bits (sign and magnitude)
constexpr long maxScaleFactor = 32767;

// Throws std::invalid_argument if value is not in [lo, hi]
inline void checkRange(const char* what, long value, long lo, long hi)
{
    if (value < lo || value > hi) {
        throw std::invalid_argument(std::string("Simple packing: ") + what + " " + std::to_string(value) + " is out of range (must be between " +
                                    std::to_string(lo) + " and " + std::to_string(hi) + ")");
    }
}


// --- IEEE float utilities ---

std::optional<long> number_of_bits(unsigned long x);

// Floating-point format in which the reference value is stored:
// IEEE 754 single precision (GRIB edition 2) or IBM single precision (GRIB edition 1)
enum class FloatType
{
    IEEE32,
    IBM32
};

// Largest value of the given format that is <= val (std::nullopt if there is none)
std::optional<double> nearest_smaller_value(double val, FloatType type = FloatType::IEEE32);


/* Minimum and maximum of an array.
 *
 * std::minmax_element has to return iterators and guarantees which element is
 * found first, which prevents the compiler from vectorising it. Only the
 * values are needed here.
 *
 * The naive reduction (mn = v < mn ? v : mn) is branchless but serialises on
 * the latency of vminsd/vmaxsd - roughly 4 cycles per element. Reducing into
 * a fixed number of independent lanes breaks that dependency chain and lets
 * gcc/clang SLP-vectorise the inner loop into vminpd/vmaxpd, which makes the
 * scan memory-bandwidth bound instead of latency bound.
 */
template <typename T>
std::pair<T, T> sp_minmax(const T* values, size_t n)
{
    constexpr size_t lanes = 8;

    if (n == 0)
        return { T(0), T(0) };

    // NaN values are detected with a quiet unordered comparison and infinite values by the
    // result (a floating-point addition per value, as in v - v, would lengthen the dependency
    // chains of the lanes and is not vectorised as well)
    T mn[lanes];
    T mx[lanes];
    bool nan[lanes];
    for (size_t l = 0; l < lanes; ++l) {
        mn[l] = values[0];
        mx[l] = values[0];
        nan[l] = false;
    }

    size_t i = 0;
    for (; i + lanes <= n; i += lanes) {
        for (size_t l = 0; l < lanes; ++l) {
            const T v = values[i + l];
            mn[l] = v < mn[l] ? v : mn[l];
            mx[l] = v > mx[l] ? v : mx[l];
            nan[l] |= __builtin_isunordered(v, v);
        }
    }

    T rmn = mn[0];
    T rmx = mx[0];
    bool rNan = nan[0];
    for (size_t l = 1; l < lanes; ++l) {
        rmn = mn[l] < rmn ? mn[l] : rmn;
        rmx = mx[l] > rmx ? mx[l] : rmx;
        rNan |= nan[l];
    }
    for (; i < n; ++i) {
        const T v = values[i];
        rmn = v < rmn ? v : rmn;
        rmx = v > rmx ? v : rmx;
        rNan |= __builtin_isunordered(v, v);
    }
    if (rNan || !std::isfinite(rmn) || !std::isfinite(rmx)) {  // a NaN or infinite value: make the range invalid
        return { std::numeric_limits<T>::quiet_NaN(), std::numeric_limits<T>::quiet_NaN() };
    }
    return { rmn, rmx };
}
// The binary scale factor E (a power of 2 exponent) such that (max - min) * 2^-E fills
// bpval bits. std::nullopt if the range is too small for bpval bits even with E = -127
// (underflow). Throws std::invalid_argument if bpval is not in [1, 63] and std::range_error
// if the range is not finite or max is smaller than min.
std::optional<long> get_binary_scale_factor(double max, double min, long bpval);

// Throws std::runtime_error unless both values are finite and the minimum is not larger
// than the maximum
void check_data_values_range(double min_val, double max_val);


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
static inline uint16_t bswap16(uint16_t x)
{
    return ((x >> 8) & 0x00FF) |
           ((x << 8) & 0xFF00);
}
static inline uint32_t bswap32(uint32_t x)
{
    return ((x >> 24) & 0x000000FF) |
           ((x >> 8) & 0x0000FF00) |
           ((x << 8) & 0x00FF0000) |
           ((x << 24) & 0xFF000000);
}
static inline uint64_t bswap64(uint64_t x)
{
    return ((x >> 56) & 0x00000000000000FFULL) |
           ((x >> 40) & 0x000000000000FF00ULL) |
           ((x >> 24) & 0x0000000000FF0000ULL) |
           ((x >> 8) & 0x00000000FF000000ULL) |
           ((x << 8) & 0x000000FF00000000ULL) |
           ((x << 24) & 0x0000FF0000000000ULL) |
           ((x << 40) & 0x00FF000000000000ULL) |
           ((x << 56) & 0xFF00000000000000ULL);
}
#endif
