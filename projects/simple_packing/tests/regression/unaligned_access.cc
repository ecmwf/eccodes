// SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// The codecs must not load or store words at misaligned addresses (undefined behaviour, a crash
// on strict-alignment CPUs). This test is compiled with -fsanitize=alignment, which aborts at the
// first misaligned access.

#include <cmath>
#include <cstdint>
#include <iostream>
#include <vector>

#include "simple_packing/detail/simple/Binary.h"

using namespace sp::detail;

int main()
{
    for (int64_t bpv = 1; bpv <= 64; ++bpv) {
        const size_t n = 37;
        std::vector<double> values(n);
        const double maxCoded = std::ldexp(1.0, static_cast<int>(std::min(bpv, INT64_C(52)))) - 1;
        for (size_t i = 0; i < n; ++i) {
            values[i] = std::fmod(i * 7919.0, maxCoded + 1);
        }
        Parameters params;
        params.bitsPerValue(bpv);
        SimplePackingBinary<double> sp(nullptr);

        // Buffers at every offset from an aligned address
        for (size_t offset = 0; offset < 8; ++offset) {
            std::vector<unsigned char> storage(offset + (bpv * n + 7) / 8 + 16);
            unsigned char* buffer = storage.data() + offset;
            sp.packTo(params, values.data(), n, buffer);
            std::vector<double> decoded(n);
            sp.unpackTo(params, buffer, storage.size() - offset, n, decoded.data());
            if (decoded != values) {
                std::cerr << "FAIL: " << bpv << " bits per value at offset " << offset << " decoded wrongly\n";
                return 1;
            }
        }
    }
    std::cout << "OK\n";
    return 0;
}
