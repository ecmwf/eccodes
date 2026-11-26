/*
 * SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

// SPAEC_ISA: the instruction set the code is compiled for, the name of an inline namespace around
// all functions and implementation classes of spaec. The code has variants for AVX2, SSE2 and
// other targets (#if defined(__AVX2__) ...): translation units compiled with different -march
// options get different names for them, instead of one definition chosen by the linker (One
// Definition Rule; e.g. an AVX2 instance called on a CPU without AVX2). The public types
// (spaec::Options, Scaling, ...) are outside and shared. The names in the code do not change.
// SSE2: GCC, Clang and icx define __SSE2__, MSVC _M_X64 (x86-64 always has SSE2) or _M_IX86_FP >= 2
#if defined(__SSE2__) || defined(_M_X64) || (defined(_M_IX86_FP) && _M_IX86_FP >= 2)
    #define SPAEC_HAVE_SSE2 1
#endif

#if defined(__AVX2__)
    #define SPAEC_ISA isa_avx2
#elif defined(SPAEC_HAVE_SSE2)
    #define SPAEC_ISA isa_sse2
#else
    #define SPAEC_ISA isa_generic
#endif
