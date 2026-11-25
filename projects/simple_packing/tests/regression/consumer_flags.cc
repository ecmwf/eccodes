// SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// A user who compiles with -ffast-math (or icx/icpx with its default floating-point model) gets
// the results and the checks of the library: the usage requirements of simple_packing turn the
// unsafe optimisations off for the header code the user instantiates. With -ffast-math the
// checks for infinity were removed and the search for the binary scale factor hung on an
// infinite range. This test is compiled with -ffast-math (GCC, Clang).

#include <iostream>
#include <stdexcept>
#include <unistd.h>
#include <vector>

#include "simple_packing/simple_packing.h"

int main()
{
    alarm(10);  // a hang is a failure
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
    return failures == 0 ? 0 : 1;
}
