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
