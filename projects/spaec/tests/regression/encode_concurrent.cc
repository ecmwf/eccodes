// SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// Encoders running at the same time in different threads must not share state
// (the encoder used a function-local static buffer for partial blocks).

#include <algorithm>
#include <cstddef>
#include <exception>
#include <atomic>
#include <cstdint>
#include <iostream>
#include <thread>
#include <vector>

#include "spaec/CodeOption.h"
#include "spaec/Span.h"
#include "spaec/encoder/EncoderSeq.h"

using namespace spaec;

int main()
{
    try {
        constexpr size_t nThreads = 8;
        constexpr size_t nIterations = 3000;
        constexpr uint8_t bpv = 16;
        constexpr uint8_t blockSize = 32;
        constexpr uint16_t rsi = 4;

        // Each thread encodes its own data, which ends with a partial block
        std::vector<std::vector<uint16_t>> data(nThreads);
        std::vector<std::vector<uint8_t>> expected(nThreads);
        for (size_t t = 0; t < nThreads; ++t) {
            data[t].resize(blockSize * 3 + 17);
            for (size_t i = 0; i < data[t].size(); ++i) {
                data[t][i] = static_cast<uint16_t>(t * 4099 + i * (t + 1) * 37);
            }
            expected[t].assign(data[t].size() * 4 + 64, 0);
            encoder::EncoderSeq<uint16_t, uint16_t> enc{ bpv, blockSize, rsi, CodeOptionSet::Basic };
            const size_t n = enc.encode(Span<uint16_t>(data[t]), Span<uint8_t>(expected[t]));
            expected[t].resize(n);
        }

        std::atomic<size_t> mismatches{ 0 };
        std::vector<std::thread> threads;
        for (size_t t = 0; t < nThreads; ++t) {
            threads.emplace_back([&, t] {
                encoder::EncoderSeq<uint16_t, uint16_t> enc{ bpv, blockSize, rsi, CodeOptionSet::Basic };
                std::vector<uint8_t> out(data[t].size() * 4 + 64);
                for (size_t it = 0; it < nIterations; ++it) {
                    std::fill(out.begin(), out.end(), 0);
                    const size_t n = enc.encode(Span<uint16_t>(data[t]), Span<uint8_t>(out));
                    if (n != expected[t].size() || !std::equal(expected[t].begin(), expected[t].end(), out.begin())) {
                        mismatches++;
                    }
                }
            });
        }
        for (auto& thread : threads) {
            thread.join();
        }

        if (mismatches > 0) {
            std::cerr << "FAIL: " << mismatches << " of " << nThreads * nIterations << " concurrent encodings differ\n";
            return 1;
        }
        std::cout << "OK\n";
        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "FAIL: " << e.what() << '\n';
        return 1;
    }
    return 0;
}
