// SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// get_binary_scale_factor() must not return a binary scale factor beyond 127 (GRIB stores it in
// 15 bits plus sign, ecCodes allows -127..127): for ranges too large even with E = 127 it only
// asserted the limit, so release builds (NDEBUG) returned factors such as 1014, whose powers of
// 2 overflow. Like ecCodes (which aborts), it must fail: with std::range_error.
#include <iostream>
#include <stdexcept>

#include "simple_packing/detail/Utils.h"

#include "../fp_diagnostics.h"

using namespace sp::detail;

int main()
{
    // With -ffast-math (no checks for infinity) or subnormal numbers flushed to zero the test hangs:
    // check that first
    if (fp_diagnostics::fastMath() || fp_diagnostics::flushesSubnormals() || fp_diagnostics::intelFastModel()) {
        fp_diagnostics::explain();
        return 1;
    }
    int failures = 0;
    const double ranges[][2] = { { 1e300, 0 }, { 1e300, -1e300 }, { 8.9e307, -8.9e307 }, { 3e38, 0 } };
    for (const auto& r : ranges) {
        for (long bpv : { 1L, 8L, 16L, 24L, 32L, 63L }) {
            try {
                const auto scale = get_binary_scale_factor(r[0], r[1], bpv);
                if (scale && (*scale > 127 || *scale < -127)) {
                    std::cerr << "range " << r[1] << ".." << r[0] << ", " << bpv << " bits: binary scale factor " << *scale << '\n';
                    ++failures;
                }
            }
            catch (const std::range_error&) {
                // expected for ranges that cannot be packed
            }
        }
    }
    // A large range that fits: 2^127 * (2^bpv - 1) is the largest
    try {
        const auto scale = get_binary_scale_factor(1e38, 0, 16);
        if (!scale || *scale != 111) {
            std::cerr << "range 1e38, 16 bits: wrong binary scale factor\n";
            ++failures;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "range 1e38, 16 bits: " << e.what() << '\n';
        ++failures;
    }
    if (failures != 0) {
        fp_diagnostics::explain();
        std::cerr << failures << " cases failed\n";
        return 1;
    }
    std::cout << "OK\n";
    return 0;
}
