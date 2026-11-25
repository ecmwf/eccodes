// SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// Packing and unpacking 0 values must do nothing: the number of blocks minus one wrapped around
// and the codecs ran off the end of their buffers.

#include <iostream>
#include <vector>

#include "simple_packing/simple/Binary.h"

using namespace sp;

int main()
{
    for (long bpv : { 1L, 7L, 12L, 16L, 24L, 64L }) {
        Parameters params;
        params.bitsPerValue(bpv);
        params.referenceValue(1.0);
        SimplePackingBinary<double> sp(nullptr);

        std::vector<double> values(8, -1.0);
        std::vector<unsigned char> buffer(8, 0);
        sp.unpackTo(params, buffer.data(), buffer.size(), 0, values.data());
        sp.packTo(params, values.data(), 0, buffer.data());
        for (double v : values) {
            if (v != -1.0) {
                std::cerr << "FAIL: unpacking 0 values with " << bpv << " bits per value wrote values\n";
                return 1;
            }
        }
        for (unsigned char b : buffer) {
            if (b != 0) {
                std::cerr << "FAIL: packing 0 values with " << bpv << " bits per value wrote data\n";
                return 1;
            }
        }
    }
    std::cout << "OK\n";
    return 0;
}
