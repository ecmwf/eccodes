// SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// Shifting a 128-bit integer by 128 bits or more must give 0: the shift count was reduced
// modulo 128, so x << 128 returned x.

#include <iostream>

#include "simple_packing/detail/U128.h"

using namespace sp::detail;

int main()
{
    int failures = 0;
    for (unsigned s : { 128u, 129u, 200u, 256u }) {
        if ((UInt128(1) << s) != UInt128(0, 0) || (UInt128(~0ULL, ~0ULL) >> s) != UInt128(0, 0)) {
            std::cerr << "FAIL: shift by " << s << " is not 0\n";
            failures++;
        }
    }
    if ((UInt128(1) << 127) != UInt128(1ULL << 63, 0) || (UInt128(1ULL << 63, 0) >> 127) != UInt128(1)) {
        std::cerr << "FAIL: shift by 127\n";
        failures++;
    }
    if (failures == 0) {
        std::cout << "OK\n";
    }
    return failures == 0 ? 0 : 1;
}
