// SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// Compiler-specific attributes and builtins of simple_packing: GCC, Clang and the Intel compilers use
// their attributes and builtins, MSVC and other compilers portable equivalents.
#pragma once

#include <cmath>

#if defined(__GNUC__) || defined(__clang__)
    #define SIMPLE_PACKING_ALWAYS_INLINE __attribute__((always_inline)) inline
    // true for NaN, without raising floating-point exceptions (a quiet comparison)
    #define SIMPLE_PACKING_ISUNORDERED(a, b) __builtin_isunordered(a, b)
#elif defined(_MSC_VER)
    #define SIMPLE_PACKING_ALWAYS_INLINE     __forceinline
    #define SIMPLE_PACKING_ISUNORDERED(a, b) std::isunordered(a, b)
#else
    #define SIMPLE_PACKING_ALWAYS_INLINE     inline
    #define SIMPLE_PACKING_ISUNORDERED(a, b) std::isunordered(a, b)
#endif

#include <cstdint>
#if defined(_MSC_VER) && !defined(__clang__)
    #include <intrin.h>
#endif

namespace sp::detail
{

// The number of bits needed for x: 0 for 0, else 1 + the position of the highest set bit
// (std::bit_width of C++20)
inline int32_t bit_width64(uint64_t x)
{
    if (x == 0) {
        return 0;
    }
#if defined(__GNUC__) || defined(__clang__)
    static_assert(sizeof(unsigned long long) == 8, "unsigned long long has 64 bits");
    return 64 - __builtin_clzll(x);
#elif defined(_MSC_VER) && (defined(_M_X64) || defined(_M_ARM64))
    unsigned long index = 0;
    _BitScanReverse64(&index, x);
    return static_cast<int32_t>(index) + 1;
#else
    int32_t width = 0;
    while (x != 0) {
        x >>= 1;
        ++width;
    }
    return width;
#endif
}

}  // namespace sp::detail
