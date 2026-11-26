// SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// Unit test of blockShiftedSum (detail/encoder/BlockSums.h), the sums of the split-sample selector
// SplitSelector::Estimate: against a plain loop for samples of 8, 16 and 32 bits, every block size
// (8, 16, 32, 64), every bits per sample and shift k, with random samples and with every sample at
// its maximum (the limits of the SIMD variants: 16-bit lanes, 32-bit lanes, the split into 16-bit
// halves). Built with AVX2 (-march=native) and without (SSE2) by the test builds.

#include <cstdint>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include "spaec/detail/encoder/BlockSums.h"

using spaec::detail::encoder::blockShiftedSum;

static int32_t failures = 0;

template <size_t J, typename T>
static void check(const std::vector<T>& v, uint32_t k, uint32_t bits, const std::string& what)
{
    uint64_t expected = 0;
    for (size_t j = 0; j < J; ++j) {
        expected += static_cast<uint64_t>(v[j] >> k);
    }
    const uint64_t got = blockShiftedSum<J>(v.data(), k, bits);
    if (got != expected && failures++ < 20) {
        std::cerr << "FAIL: " << what << ", " << 8 * sizeof(T) << "-bit samples, J = " << J << ", " << bits << " bits, k = " << k << ": " << got
                  << " instead of " << expected << '\n';
    }
}

template <size_t J, typename T>
static void blockSize(std::mt19937_64& rng)
{
    for (uint32_t bits = 1; bits <= 8 * sizeof(T); ++bits) {
        const uint64_t max = bits == 64 ? ~uint64_t{ 0 } : (uint64_t{ 1 } << bits) - 1;
        for (uint32_t k = 0; k < bits; ++k) {
            std::vector<T> v(J);
            for (auto& x : v)
                x = static_cast<T>(max);
            check<J>(v, k, bits, "all samples at the maximum");
            for (int32_t rep = 0; rep < 20; ++rep) {
                for (auto& x : v)
                    x = static_cast<T>(rng() & max);
                check<J>(v, k, bits, "random samples");
                for (size_t j = 0; j < J; ++j)
                    v[j] = static_cast<T>(rng() % 4 == 0 ? max : rng() & max);
                check<J>(v, k, bits, "samples near the maximum");
            }
        }
    }
}

template <typename T>
static void sampleType(std::mt19937_64& rng)
{
    blockSize<8, T>(rng);
    blockSize<16, T>(rng);
    blockSize<32, T>(rng);
    blockSize<64, T>(rng);
}

int main()
{
    std::mt19937_64 rng(5);
    sampleType<uint8_t>(rng);
    sampleType<uint16_t>(rng);
    sampleType<uint32_t>(rng);
    if (failures != 0) {
        std::cerr << failures << " failure(s)\n";
        return 1;
    }
    std::cout << "block sums ok\n";
    return 0;
}
