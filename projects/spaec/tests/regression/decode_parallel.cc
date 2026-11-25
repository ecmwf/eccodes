// SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// Decoding with several threads must give the same values as decoding with one thread
// (the trailing partial reference sample interval was not decoded, and inputs smaller than
// one interval were not decoded at all) and must report errors in the compressed data.

#include <cstddef>
#include <exception>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "spaec/CodeOption.h"
#include "spaec/Span.h"
#include "spaec/SpAec.h"

using namespace spaec;

static int failures = 0;

int main()
{
    try {
        constexpr uint8_t bpv = 16, blockSize = 32;
        constexpr uint16_t rsi = 128;
        using SpAec = encoder::SpAec<double>;

        for (size_t n : { size_t(1), size_t(100), size_t(4096), size_t(4097), size_t(4 * 4096 + 5), size_t(1000000) }) {
            std::vector<double> data(n);
            for (size_t i = 0; i < n; ++i) {
                data[i] = 0.5 + 0.4 * std::sin(i * 1e-4);
            }
            SpAec seq{ bpv, blockSize, rsi, CodeOptionSet::Basic, 0.0, -16, 0, 1 };
            std::vector<uint8_t> encoded(SpAec::maxEncodedSize(n, bpv, blockSize));
            const size_t size = seq.encode(Span<double>(data), Span<uint8_t>(encoded));
            Span<uint8_t> stream(encoded.data(), size);

            std::vector<double> expected(n);
            seq.decode(stream, Span<double>(expected));

            for (uint8_t nThreads : { 2, 4, 7 }) {
                SpAec par{ bpv, blockSize, rsi, CodeOptionSet::Basic, 0.0, -16, 0, nThreads };
                std::vector<double> decoded(n, -999.0);
                try {
                    par.decode(stream, Span<double>(decoded));
                    if (decoded != expected) {
                        std::cerr << "FAIL: n=" << n << " threads=" << int(nThreads) << ": values differ from the sequential decoder\n";
                        failures++;
                    }
                }
                catch (const std::exception& e) {
                    std::cerr << "FAIL: n=" << n << " threads=" << int(nThreads) << ": " << e.what() << '\n';
                    failures++;
                }

                // Truncated stream
                if (size > 16) {
                    try {
                        par.decode(Span<uint8_t>(encoded.data(), size / 2), Span<double>(decoded));
                        std::cerr << "FAIL: n=" << n << " threads=" << int(nThreads) << ": truncated stream not reported\n";
                        failures++;
                    }
                    catch (const std::runtime_error&) {
                    }
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
