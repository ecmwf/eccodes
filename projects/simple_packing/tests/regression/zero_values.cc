// SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// Packing and unpacking 0 values must do nothing: the number of blocks minus one wrapped around
// and the codecs ran off the end of their buffers.

#include <iostream>
#include <cstdint>
#include <vector>

#include "simple_packing/detail/simple/Binary.h"

using namespace sp::detail;

int main()
{
    for (int64_t bpv : { INT64_C(1), INT64_C(7), INT64_C(12), INT64_C(16), INT64_C(24), INT64_C(64) }) {
        Parameters params;
        params.bitsPerValue(bpv);
        params.referenceValue(1.0);
        SimplePackingBinary<double> sp(nullptr);

        std::vector<double> values(8, -1.0);
        std::vector<uint8_t> buffer(8, 0);
        sp.unpackTo(params, buffer.data(), buffer.size(), 0, values.data());
        sp.packTo(params, values.data(), 0, buffer.data());
        for (double v : values) {
            if (v != -1.0) {
                std::cerr << "FAIL: unpacking 0 values with " << bpv << " bits per value wrote values\n";
                return 1;
            }
        }
        for (uint8_t b : buffer) {
            if (b != 0) {
                std::cerr << "FAIL: packing 0 values with " << bpv << " bits per value wrote data\n";
                return 1;
            }
        }
    }
    std::cout << "OK\n";
    return 0;
}
