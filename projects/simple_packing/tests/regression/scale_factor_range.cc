// SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// Scale factors outside the GRIB range (+-32767) must be rejected with std::invalid_argument when
// packing and unpacking: they took seconds per call, INT_MIN overflowed when negated, and values
// beyond the range of int were silently truncated. The constructor of Parameters must
// initialise all members.

#include <chrono>
#include <climits>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "simple_packing/simple/Binary.h"

using namespace sp;

static int failures = 0;

int main()
{
    std::vector<unsigned char> buffer(64, 0x11);
    std::vector<double> values(16, 1.0);

    for (long sf : { long(INT_MIN), long(INT_MAX), 32768L, -32768L, (1L << 32) + 3 }) {
        for (bool binary : { true, false }) {
            Parameters params;
            params.bitsPerValue(8);
            if (binary) {
                params.binaryScaleFactor(sf);
            }
            else {
                params.decimalScaleFactor(sf);
            }
            const std::string what = std::string(binary ? "binary" : "decimal") + " scale factor " + std::to_string(sf);
            SimplePackingBinary<double> sp(nullptr);
            for (bool pack : { false, true }) {
                const auto start = std::chrono::steady_clock::now();
                try {
                    if (pack) {
                        sp.packTo(params, values.data(), values.size(), buffer.data());
                    }
                    else {
                        sp.unpackTo(params, buffer.data(), buffer.size(), values.size(), values.data());
                    }
                    std::cerr << "FAIL: " << (pack ? "packing" : "unpacking") << " with " << what << " was accepted\n";
                    failures++;
                }
                catch (const std::invalid_argument&) {
                }
                const double seconds = std::chrono::duration<double>(std::chrono::steady_clock::now() - start).count();
                if (seconds > 0.1) {
                    std::cerr << "FAIL: " << (pack ? "packing" : "unpacking") << " with " << what << " took " << seconds << " s\n";
                    failures++;
                }
            }
        }
    }

    // All members are initialised (also by the constructor with all values)
    Parameters params(16, 0, 0, 1.0, false);
    if (params.isConstantField()) {
        std::cerr << "FAIL: Parameters(..., false) is a constant field\n";
        failures++;
    }
    if (!Parameters(0, 0, 0, 1.0, true).isConstantField()) {
        std::cerr << "FAIL: Parameters(..., true) is not a constant field\n";
        failures++;
    }

    if (failures == 0) {
        std::cout << "OK\n";
    }
    return failures == 0 ? 0 : 1;
}
