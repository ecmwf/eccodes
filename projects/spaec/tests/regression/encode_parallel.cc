// SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// Encoding with several threads must produce a valid stream, identical to the one produced
// with one thread (the parallel encoder lost zero-block runs and read past partial blocks).

#include <algorithm>
#include <cstddef>
#include <exception>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <vector>

#include "spaec/CodeOption.h"
#include "spaec/Span.h"
#include "spaec/SpAec.h"

using namespace spaec;

int main()
{
    try {
        int failures = 0;
        constexpr uint8_t bpv = 16, blockSize = 32;
        constexpr uint16_t rsi = 128;
        using SpAec = encoder::SpAec<double>;

        for (size_t n : { size_t(37), size_t(4096 * 3 + 7), size_t(1000000) }) {
            // Smooth data with runs of zeros and a partial last block
            std::vector<double> data(n);
            for (size_t i = 0; i < n; ++i) {
                data[i] = (i / 1000) % 3 == 0 ? 0.0 : 0.5 + 0.4 * std::sin(i * 1e-4);
            }

            SpAec seq{ bpv, blockSize, rsi, CodeOptionSet::Basic, 0.0, -16, 0, 1 };
            std::vector<uint8_t> expected(SpAec::maxEncodedSize(n, bpv, blockSize));
            const size_t expectedSize = seq.encode(Span<double>(data), Span<uint8_t>(expected));

            for (uint8_t nThreads : { 2, 4, 8 }) {
                SpAec par{ bpv, blockSize, rsi, CodeOptionSet::Basic, 0.0, -16, 0, nThreads };
                std::vector<uint8_t> encoded(expected.size());
                try {
                    const size_t size = par.encode(Span<double>(data), Span<uint8_t>(encoded));
                    if (size != expectedSize || !std::equal(encoded.begin(), encoded.begin() + size, expected.begin())) {
                        std::cerr << "FAIL: n=" << n << " threads=" << int(nThreads) << ": " << size << " bytes, expected " << expectedSize << '\n';
                        failures++;
                        continue;
                    }
                    std::vector<double> decoded(n);
                    seq.decode(Span<uint8_t>(encoded.data(), size), Span<double>(decoded));
                    for (size_t i = 0; i < n; ++i) {
                        if (std::fabs(decoded[i] - data[i]) > 1.0 / 65536) {
                            std::cerr << "FAIL: n=" << n << " threads=" << int(nThreads) << ": value " << i << " decoded wrongly\n";
                            failures++;
                            break;
                        }
                    }
                }
                catch (const std::exception& e) {
                    std::cerr << "FAIL: n=" << n << " threads=" << int(nThreads) << ": " << e.what() << '\n';
                    failures++;
                }
            }
        }

        if (failures == 0) {
            std::cout << "OK\n";
        }
        return failures == 0 ? 0 : 1;
    }
    catch (const std::exception& e) {
        std::cerr << "FAIL: " << e.what() << '\n';
        return 1;
    }
    return 0;
}
