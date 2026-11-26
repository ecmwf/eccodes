// SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// A user program of the install test: compresses and decompresses samples with the installed spaec
#include <cstdint>
#include <iostream>
#include <vector>

#include "spaec/spaec.h"

int main()
{
    spaec::Options options;
    options.bitsPerSample = 12;
    std::vector<uint16_t> samples(1000);
    for (size_t i = 0; i < samples.size(); ++i) {
        samples[i] = static_cast<uint16_t>((i * 37) % 4096);
    }
    const std::vector<uint8_t> data = spaec::compress(samples, options);
    if (spaec::decompress<uint16_t>(data, samples.size(), options) != samples) {
        std::cerr << "FAIL: round trip\n";
        return 1;
    }
    std::cout << "OK spaec " << spaec::version() << '\n';
    return 0;
}
