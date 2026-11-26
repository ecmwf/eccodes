/*
 * SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "spaec/detail/Isa.h"

// C++17 replacements for C++20 features (std::endian, std::countl_zero, [[likely]]), and the
// compiler-specific attributes and intrinsics SpAec uses. GCC, Clang and the Intel compilers use their
// builtins and attributes, MSVC its intrinsics; other compilers portable code.

#include <climits>
#include <cstdint>
#include <type_traits>

#if defined(_MSC_VER) && !defined(__clang__)
    #include <intrin.h>
    #include <stdlib.h>
#endif

#if defined(__GNUC__) || defined(__clang__)
    #define SPAEC_LIKELY(x)     __builtin_expect(!!(x), 1)
    #define SPAEC_UNLIKELY(x)   __builtin_expect(!!(x), 0)
    #define SPAEC_ALWAYS_INLINE __attribute__((always_inline)) inline
    #define SPAEC_NOINLINE      __attribute__((noinline))
    #define SPAEC_COLD          __attribute__((cold, noinline))
    #define SPAEC_RESTRICT      __restrict__
#elif defined(_MSC_VER)
    #define SPAEC_LIKELY(x)     (x)
    #define SPAEC_UNLIKELY(x)   (x)
    #define SPAEC_ALWAYS_INLINE __forceinline
    #define SPAEC_NOINLINE      __declspec(noinline)
    #define SPAEC_COLD          __declspec(noinline)
    #define SPAEC_RESTRICT      __restrict
#else
    #define SPAEC_LIKELY(x)     (x)
    #define SPAEC_UNLIKELY(x)   (x)
    #define SPAEC_ALWAYS_INLINE inline
    #define SPAEC_NOINLINE
    #define SPAEC_COLD
    #define SPAEC_RESTRICT
#endif

namespace spaec::detail
{
inline namespace SPAEC_ISA
{

#if defined(__BYTE_ORDER__) && defined(__ORDER_LITTLE_ENDIAN__)
constexpr bool isLittleEndian = (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__);
#elif defined(_WIN32)
constexpr bool isLittleEndian = true;  // every Windows target is little-endian
#else
    #error "SpAec: unknown byte order"
#endif

// Byte swap of 16, 32 and 64 bits
inline uint16_t byteSwap16(uint16_t v)
{
#if defined(__GNUC__) || defined(__clang__)
    return __builtin_bswap16(v);
#elif defined(_MSC_VER)
    return _byteswap_ushort(v);
#else
    return static_cast<uint16_t>((v >> 8) | (v << 8));
#endif
}

inline uint32_t byteSwap32(uint32_t v)
{
#if defined(__GNUC__) || defined(__clang__)
    return __builtin_bswap32(v);
#elif defined(_MSC_VER)
    return _byteswap_ulong(v);
#else
    return ((v >> 24) & 0xFFu) | ((v >> 8) & 0xFF00u) | ((v << 8) & 0xFF0000u) | (v << 24);
#endif
}

inline uint64_t byteSwap64(uint64_t v)
{
#if defined(__GNUC__) || defined(__clang__)
    return __builtin_bswap64(v);
#elif defined(_MSC_VER)
    return _byteswap_uint64(v);
#else
    return (static_cast<uint64_t>(byteSwap32(static_cast<uint32_t>(v))) << 32) | byteSwap32(static_cast<uint32_t>(v >> 32));
#endif
}

// The number of leading zero bits of a non-zero value of 32 or 64 bits
inline int countlZero32(uint32_t x)
{
#if defined(__GNUC__) || defined(__clang__)
    static_assert(sizeof(unsigned) == 4, "unsigned int has 32 bits");
    return __builtin_clz(x);
#elif defined(_MSC_VER)
    unsigned long index = 0;
    _BitScanReverse(&index, x);
    return 31 - static_cast<int>(index);
#else
    int n = 0;
    while (!(x & 0x80000000u)) {
        x <<= 1;
        ++n;
    }
    return n;
#endif
}

inline int countlZero64(uint64_t x)
{
#if defined(__GNUC__) || defined(__clang__)
    static_assert(sizeof(unsigned long long) == 8, "unsigned long long has 64 bits");
    return __builtin_clzll(x);
#elif defined(_MSC_VER) && (defined(_M_X64) || defined(_M_ARM64))
    unsigned long index = 0;
    _BitScanReverse64(&index, x);
    return 63 - static_cast<int>(index);
#else
    const uint32_t high = static_cast<uint32_t>(x >> 32);
    return high != 0 ? countlZero32(high) : 32 + countlZero32(static_cast<uint32_t>(x));
#endif
}

// The number of leading zero bits of x, the width of T for 0 (like std::countl_zero)
template <typename T>
inline int countlZero(T x)
{
    static_assert(std::is_unsigned<T>::value && sizeof(T) <= 8, "countlZero requires an unsigned type of at most 64 bits");
    constexpr int digits = static_cast<int>(sizeof(T) * CHAR_BIT);
    if (x == 0) {
        return digits;
    }
    if constexpr (sizeof(T) <= 4) {
        return countlZero32(static_cast<uint32_t>(x)) - (32 - digits);
    }
    else {
        return countlZero64(static_cast<uint64_t>(x));
    }
}

}  // namespace SPAEC_ISA
}  // namespace spaec::detail
