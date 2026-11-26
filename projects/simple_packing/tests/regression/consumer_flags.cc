// SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// A user's code gets the floating-point options that this compiler needs from the usage
// requirements of simple_packing (README.md, "Floating-point compiler options"): compiled without the project
// flags (for the same CPU), the checks for NaN and infinity and the results stay those of the library.
// A hang is a failure (TIMEOUT of the test).

#include <iostream>
#include <stdexcept>
#include <vector>

#include "simple_packing/simple_packing.h"

#include "../fp_diagnostics.h"

int main()
{
    // With -ffast-math (no checks for infinity) or subnormal numbers flushed to zero the test hangs:
    // check that first
    if (fp_diagnostics::fastMath() || fp_diagnostics::flushesSubnormals() || fp_diagnostics::intelFastModel()) {
        fp_diagnostics::explain();
        return 1;
    }
    int failures = 0;
#ifdef __FAST_MATH__
    std::cerr << "FAIL: -ffast-math is in effect for code that uses simple_packing\n";
    ++failures;
#endif
    // 1e300 * 10^10 is infinite: the scaling must be refused
    const std::vector<double> values{ 1.0, 1e300 };
    try {
        sp::scaling(values, sp::Method::bitsAndDecimals(16, 10));
        std::cerr << "FAIL: an infinite scaled range was accepted\n";
        ++failures;
    }
    catch (const std::runtime_error&) {
    }
    if (failures == 0) {
        std::cout << "OK\n";
    }
    if (failures != 0) {
        fp_diagnostics::explain();
    }
    return failures == 0 ? 0 : 1;
}
