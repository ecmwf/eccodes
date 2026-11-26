// SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// A user's code gets the floating-point options that this compiler needs from the usage
// requirements of spaec::spaec (README.md, "Floating-point compiler options"): compiled without the project
// flags (for the same CPU), the checks for NaN and infinity and the results stay those of the library.
// A -ffast-math of the user is not undone: the user adds -fno-fast-math.

#include <cmath>
#include <cstdint>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <vector>

#include "spaec/spaec.h"

#include "../fp_diagnostics.h"

int main()
{
    int32_t failures = 0;
#ifdef __FAST_MATH__
    std::cerr << "FAIL: -ffast-math is in effect for code that uses spaec\n";
    ++failures;
#endif
    spaec::Options options;
    const std::vector<double> values{ 1.0, std::numeric_limits<double>::quiet_NaN(), 3.0 };
    try {
        spaec::compress(values, spaec::Scaling{ 0.0, 0, 0 }, options);
        std::cerr << "FAIL: a NaN value was compressed\n";
        ++failures;
    }
    catch (const std::invalid_argument&) {
    }
    if (failures == 0) {
        std::cout << "OK\n";
    }
    if (failures != 0) {
        fp_diagnostics::explain();
    }
    return failures == 0 ? 0 : 1;
}
