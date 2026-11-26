// SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// Explains test failures that come from missing floating-point compiler options (README.md,
// "Floating-point compiler options"). The checks run in the translation unit of the test, so they
// see the options it was compiled with. SPAEC_TEST_FP_FLAGS: the floating-point options of the
// test (set by the CMake files of the tests).

#pragma once

#include <cmath>
#include <iostream>
#include <limits>
#include <string>

#ifndef SPAEC_TEST_FP_FLAGS
    #define SPAEC_TEST_FP_FLAGS "(not known)"
#endif

namespace fp_diagnostics
{

// Whether this code fuses a * b - c into one multiply-add: a * b rounds to 1 (2^-54 is lost), the
// fused result is -2^-54
inline bool contracts()
{
    static volatile double a = 1.0 + 0x1p-27, b = 1.0 - 0x1p-27, c = 1.0;
    const double x = a, y = b, z = c;
    return x * y - z != 0.0;
}

// Whether -ffast-math or -ffinite-math-only is in effect: NaN is not detected
inline bool fastMath()
{
#if defined(__FAST_MATH__) || (defined(__FINITE_MATH_ONLY__) && __FINITE_MATH_ONLY__)
    return true;
#else
    static volatile double nan = std::numeric_limits<double>::quiet_NaN();
    const double x = nan;
    return !std::isnan(x);
#endif
}

// Whether the CPU flushes subnormal numbers to zero (FTZ/DAZ), which linking with -ffast-math
// (GCC, Clang: crtfastmath.o) or icx/icpx's default model sets at program start
inline bool flushesSubnormals()
{
    static volatile double tiny = 0x1p-1060;
    const double x = tiny;
    return x * 0.5 == 0.0 || x == 0.0;
}

// Whether icx/icpx (or icc) compiled this code with its default floating-point model (fast), which
// defines no macro: the options of the test tell
inline bool intelFastModel()
{
#if defined(__INTEL_LLVM_COMPILER) || defined(__INTEL_COMPILER)
    const std::string flags = SPAEC_TEST_FP_FLAGS;
    return flags.find("-fno-fast-math") == std::string::npos && flags.find("-fp-model") == std::string::npos;
#else
    return false;
#endif
}

// Prints the floating-point options missing in this build and how to enable them; to be called
// when a test fails. Returns whether an option is missing.
inline bool explain()
{
    bool missing = false;
    const auto head = [&missing] {
        if (!missing) {
            std::cerr << "\nThis failure comes from missing floating-point compiler options (README.md, \"Floating-point "
                         "compiler options\"; floating-point options of this test: " SPAEC_TEST_FP_FLAGS "):\n";
        }
        missing = true;
    };
    if (contracts()) {
        head();
        std::cerr << "- The compiler fuses multiplications and additions into multiply-add instructions, which round\n"
                     "  once instead of twice: the coded values then depend on the compiler and the CPU.\n"
                     "  Fix: compile with -ffp-contract=off. Configure with -DSPAEC_FP_CONTRACT_OFF=ON (the default; AUTO enables it\n"
                     "  when the probe at configure time sees the compiler fuse); in your own code that includes the\n"
                     "  spaec headers, add -ffp-contract=off (after -fno-fast-math).\n";
    }
    if (fastMath()) {
        head();
        std::cerr << "- -ffast-math (or -ffinite-math-only) is in effect: NaN and infinite values are not detected, and the\n"
                     "  values may be rounded differently.\n"
                     "  Fix: configure with -DSPAEC_NO_FAST_MATH=ON (adds -fno-fast-math after CMAKE_CXX_FLAGS), or remove\n"
                     "  -ffast-math; in your own code, add -fno-fast-math after it.\n";
    }
    if (flushesSubnormals()) {
        head();
        std::cerr << "- The CPU flushes subnormal numbers to zero: the program was linked with -ffast-math (GCC and\n"
                     "  Clang then link crtfastmath.o) or built by icx/icpx with its default floating-point model.\n"
                     "  -fno-fast-math at compile time does not undo it.\n"
                     "  Fix: link with -fno-fast-math after -ffast-math (configure with -DSPAEC_NO_FAST_MATH=ON, which\n"
                     "  also adds it to the link options), or remove -ffast-math.\n";
    }
    if (intelFastModel()) {
        head();
        std::cerr << "- icx/icpx (and icc) use the floating-point model 'fast' by default, which allows unsafe\n"
                     "  optimisations.\n"
                     "  Fix: compile with -fno-fast-math. Configure with -DSPAEC_NO_FAST_MATH=ON (the default) or AUTO\n"
                     "  (which enables it for these compilers).\n";
    }
    if (!missing) {
        std::cerr << "(No missing floating-point compiler option detected; floating-point options of this test: " SPAEC_TEST_FP_FLAGS ")\n";
    }
    return missing;
}

}  // namespace fp_diagnostics
