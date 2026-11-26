/*
 * SPDX-FileCopyrightText: 2023 European Centre for Medium-Range Weather Forecasts (ECMWF)
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <cmath>
#include <optional>
#include <string>
#include <stdexcept>
#include <limits>
#include <utility>

#include "simple_packing/detail/Compat.h"

namespace sp::detail
{

// --- Power function ---

/* Return n to the power of s */
template <typename T>
constexpr T sp_power(int64_t s, int64_t n)
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
constexpr int64_t maxEncodingBitsPerValue = 63;
// GRIB stores the scale factors with 16 bits (sign and magnitude)
constexpr int64_t maxScaleFactor = 32767;

// Throws std::invalid_argument if value is not in [lo, hi]
inline void checkRange(const char* what, int64_t value, int64_t lo, int64_t hi)
{
    if (value < lo || value > hi) {
        throw std::invalid_argument(std::string("Simple packing: ") + what + " " + std::to_string(value) + " is out of range (must be between " +
                                    std::to_string(lo) + " and " + std::to_string(hi) + ")");
    }
}


// --- IEEE float utilities ---

// The number of bits of x (0 for 0); std::nullopt for x >= 2^31, as legacy ecCodes, whose table
// ends at 2^31. Inline: GCC passes a returned std::optional<int64_t> through memory, slowly.
inline std::optional<int64_t> number_of_bits(uint64_t x)
{
    if (x >= (uint64_t{ 1 } << 31)) {
        return std::nullopt;
    }
    return bit_width64(x);
}

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
 * the latency of vminsd/vmaxsd - roughly 4 cycles per element. The values are
 * therefore reduced in independent lanes. GCC, Clang and icpx do not vectorise
 * these lanes as scalar code (with the NaN test, about 1 ns per value), so they
 * are written with vector types (GCC vector extensions, portable to all
 * targets): 4 vectors of 32 bytes, which makes the scan as fast as reading the
 * values (3 to 4x faster). Other compilers use the scalar lanes.
 */
template <typename T>
std::pair<T, T> sp_minmax(const T* values, size_t n)
{
    if (n == 0)
        return { T(0), T(0) };

    T rmn = values[0];
    T rmx = values[0];
    bool rNan = false;
    size_t i = 0;

#if defined(__GNUC__) || defined(__clang__)
    // v < m ? v : m per element keeps m for a NaN v, like the scalar ternary; NaN values are
    // recorded in a mask (v != v), infinite values are found in the result
    typedef T V __attribute__((vector_size(32)));
    using Mask = decltype(V{} != V{});
    constexpr size_t width = sizeof(V) / sizeof(T);
    constexpr size_t nVectors = 4;
    V mn[nVectors];
    V mx[nVectors];
    Mask nan = {};
    for (size_t k = 0; k < nVectors; ++k) {
        for (size_t l = 0; l < width; ++l) {
            mn[k][l] = rmn;
            mx[k][l] = rmx;
        }
    }
    for (; i + nVectors * width <= n; i += nVectors * width) {
        for (size_t k = 0; k < nVectors; ++k) {
            V v;
            std::memcpy(&v, values + i + k * width, sizeof v);  // unaligned load
            mn[k] = v < mn[k] ? v : mn[k];
            mx[k] = v > mx[k] ? v : mx[k];
            nan |= v != v;  // NaN test (the vector types have no isunordered)
        }
    }
    for (size_t k = 0; k < nVectors; ++k) {
        for (size_t l = 0; l < width; ++l) {
            rmn = mn[k][l] < rmn ? mn[k][l] : rmn;
            rmx = mx[k][l] > rmx ? mx[k][l] : rmx;
        }
    }
    for (size_t l = 0; l < width; ++l) {
        rNan |= nan[l] != 0;
    }
#else
    constexpr size_t lanes = 8;
    T mn[lanes];
    T mx[lanes];
    bool nan[lanes];
    for (size_t l = 0; l < lanes; ++l) {
        mn[l] = rmn;
        mx[l] = rmx;
        nan[l] = false;
    }
    for (; i + lanes <= n; i += lanes) {
        for (size_t l = 0; l < lanes; ++l) {
            const T v = values[i + l];
            mn[l] = v < mn[l] ? v : mn[l];
            mx[l] = v > mx[l] ? v : mx[l];
            nan[l] |= SIMPLE_PACKING_ISUNORDERED(v, v);
        }
    }
    for (size_t l = 0; l < lanes; ++l) {
        rmn = mn[l] < rmn ? mn[l] : rmn;
        rmx = mx[l] > rmx ? mx[l] : rmx;
        rNan |= nan[l];
    }
#endif

    for (; i < n; ++i) {
        const T v = values[i];
        rmn = v < rmn ? v : rmn;
        rmx = v > rmx ? v : rmx;
        rNan |= SIMPLE_PACKING_ISUNORDERED(v, v);
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
std::optional<int64_t> get_binary_scale_factor(double max, double min, int64_t bpval);

// Throws std::runtime_error unless both values are finite and the minimum is not larger
// than the maximum
void check_data_values_range(double min_val, double max_val);

}  // namespace sp::detail


// --- Byte swap ---
// Compiler intrinsics where available (GCC, Clang, MinGW, icx: builtins; MSVC: _byteswap_*), else
// shifts. Functions in sp::detail, not macros, so they cannot clash with the names of a platform.

#if defined(_MSC_VER) && !defined(__clang__)
    #include <stdlib.h>
#endif

namespace sp::detail
{

inline uint16_t bswap16(uint16_t x)
{
#if defined(__GNUC__) || defined(__clang__)
    return __builtin_bswap16(x);
#elif defined(_MSC_VER)
    return _byteswap_ushort(x);
#else
    return static_cast<uint16_t>((x >> 8) | (x << 8));
#endif
}

inline uint32_t bswap32(uint32_t x)
{
#if defined(__GNUC__) || defined(__clang__)
    return __builtin_bswap32(x);
#elif defined(_MSC_VER)
    return _byteswap_ulong(x);
#else
    return ((x >> 24) & 0x000000FFu) | ((x >> 8) & 0x0000FF00u) | ((x << 8) & 0x00FF0000u) | ((x << 24) & 0xFF000000u);
#endif
}

inline uint64_t bswap64(uint64_t x)
{
#if defined(__GNUC__) || defined(__clang__)
    return __builtin_bswap64(x);
#elif defined(_MSC_VER)
    return _byteswap_uint64(x);
#else
    return (static_cast<uint64_t>(bswap32(static_cast<uint32_t>(x))) << 32) | bswap32(static_cast<uint32_t>(x >> 32));
#endif
}

}  // namespace sp::detail
