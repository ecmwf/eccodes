// SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// A user who compiles with -ffast-math (or icx/icpx with its default floating-point model) gets
// the results and the checks of the library: the usage requirements of spaec::spaec turn the
// unsafe optimisations off for the header code the user instantiates. With -ffast-math the
// checks for NaN and infinity were removed (-ffinite-math-only). This test is compiled with
// -ffast-math (GCC, Clang).

#include <cmath>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <vector>

#include "spaec/spaec.h"

int main()
{
    int failures = 0;
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
    return failures == 0 ? 0 : 1;
}
