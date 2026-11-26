/*
 * SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "spaec/detail/Isa.h"

// C++17 replacements for the C++20 features SpAec used: std::endian, std::countl_zero and the
// [[likely]]/[[unlikely]] attributes (GCC, Clang and the Intel compilers, like the __builtin_bswap
// functions in BitBuffer.h)

#include <climits>
#include <type_traits>

namespace spaec::detail
{
inline namespace SPAEC_ISA
{

#if defined(__GNUC__) || defined(__clang__)
    #define SPAEC_LIKELY(x)   __builtin_expect(!!(x), 1)
    #define SPAEC_UNLIKELY(x) __builtin_expect(!!(x), 0)
#else
    #define SPAEC_LIKELY(x)   (x)
    #define SPAEC_UNLIKELY(x) (x)
#endif

#if defined(__BYTE_ORDER__) && defined(__ORDER_LITTLE_ENDIAN__)
constexpr bool isLittleEndian = (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__);
#else
    #error "SpAec: unknown byte order"
#endif

// The number of leading zero bits of x, the width of T for 0 (like std::countl_zero)
template <typename T>
inline int countlZero(T x)
{
    static_assert(std::is_unsigned<T>::value, "countlZero requires an unsigned type");
    constexpr int digits = static_cast<int>(sizeof(T) * CHAR_BIT);
    if (x == 0) {
        return digits;
    }
    if constexpr (sizeof(T) <= sizeof(unsigned)) {
        return __builtin_clz(static_cast<unsigned>(x)) - (static_cast<int>(sizeof(unsigned) * CHAR_BIT) - digits);
    }
    else if constexpr (sizeof(T) <= sizeof(unsigned long)) {
        return __builtin_clzl(static_cast<unsigned long>(x)) - (static_cast<int>(sizeof(unsigned long) * CHAR_BIT) - digits);
    }
    else {
        return __builtin_clzll(static_cast<unsigned long long>(x)) - (static_cast<int>(sizeof(unsigned long long) * CHAR_BIT) - digits);
    }
}

}  // namespace SPAEC_ISA
}  // namespace spaec::detail
