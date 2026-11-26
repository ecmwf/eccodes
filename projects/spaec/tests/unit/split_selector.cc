// SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// Unit test of the split-sample selectors (spaec::Options::splitSelector):
// - SplitSelector::Estimate: the k of Kiely's rule (IPN Progress Report 42-159, 2004, IV.A), the
//   largest k with n * 128 * 2^k <= 128 * S + 49 * n, for small and the largest sums
// - its guarantees for every block: at most one k away from the k of minimum size, and at most
//   1/2 bit per sample larger (geometric, uniform, spiky, constant and alternating blocks)
// - round trips with both selectors: all sample types, 1 to 32 bits (also without split-sample
//   options, 1 and 2 bits with the restricted set), block sizes, reference intervals, all
//   preprocessors, signed samples, partial blocks and RSIs, threads, floating-point values
// - the data of Estimate are at least as large as those of Search and at most J/2 bits per block
//   larger; the multi-threaded encoder writes the same data as the single-threaded one
// - Search is the default, and its data are unchanged; an invalid selector is an error

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

#include "spaec/spaec.h"

static int32_t failures = 0;

static void check(bool ok, const std::string& what)
{
    if (!ok) {
        std::cerr << "FAIL: " << what << '\n';
        ++failures;
    }
}

using Base = spaec::detail::encoder::EncoderBase<uint32_t, uint32_t>;

// The rule by its definition: the largest k with n * 128 * 2^k <= 128 * S + 49 * n
static uint32_t kielyK(uint64_t sum, uint64_t n)
{
    // 128 * sum + 49 * n < 2^46 and (128 * n) << 41 <= 2^54: no overflow
    const uint64_t limit = 128 * sum + 49 * n;
    uint32_t k = 0;
    while (k < 40 && ((128 * n) << (k + 1)) <= limit) {
        ++k;
    }
    return k;  // 0 also if 128 * n > limit
}

// The size in bits of the split-sample data of a block with parameter k (without the ID)
static uint64_t splitBits(const std::vector<uint64_t>& block, uint32_t k)
{
    uint64_t bits = block.size() * (k + 1);
    for (uint64_t x : block)
        bits += x >> k;
    return bits;
}

static void estimateRule()
{
    for (uint64_t n : { 7, 8, 15, 16, 31, 32, 63, 64 }) {
        for (uint64_t sum = 0; sum < 20000; ++sum) {
            if (Base::estimateK(sum, n) != kielyK(sum, n)) {
                check(false, "estimateK(" + std::to_string(sum) + ", " + std::to_string(n) + ")");
                return;
            }
        }
        // The thresholds: k is reached exactly when mu + 49/128 >= 2^k
        for (uint32_t k = 1; k < 31; ++k) {
            // smallest S with 128 * S + 49 * n >= n * 128 * 2^k
            const uint64_t s = (n * 128 * (uint64_t{ 1 } << k) - 49 * n + 127) / 128;
            check(Base::estimateK(s, n) == k && Base::estimateK(s - 1, n) == k - 1, "estimateK at the threshold of k = " + std::to_string(k) + ", n = " + std::to_string(n));
        }
        // The largest sum: 64 samples of 32 bits
        const uint64_t maxSum = n * 0xFFFFFFFFull;
        check(Base::estimateK(maxSum, n) == kielyK(maxSum, n) && Base::estimateK(maxSum, n) == 31, "estimateK of the largest sum, n = " + std::to_string(n));
    }
    check(Base::estimateK(0, 32) == 0, "estimateK of a zero block");
    // k = 1 from mu + 49/128 >= 2, i.e. from mu = 207/128 (about 1.62): S = 52 with 32 samples
    check(Base::estimateK(51, 32) == 0 && Base::estimateK(52, 32) == 1, "k = 1 from mu = 207/128 (32 samples)");
}

// Kiely's guarantees (IV.A and IV.C) for blocks as sources with known mean
static void blockGuarantees()
{
    std::mt19937_64 rng(7);
    int64_t blocks = 0;
    for (int32_t shape = 0; shape < 6; ++shape) {
        for (double mean : { 0.1, 0.5, 1.0, 1.6, 3.0, 10.0, 100.0, 1e3, 1e5, 1e8 }) {
            for (size_t n : { 7, 8, 16, 31, 32, 64 }) {
                for (int32_t rep = 0; rep < 40; ++rep) {
                    std::vector<uint64_t> block(n);
                    std::geometric_distribution<uint64_t> geo(1.0 / (mean + 1.0));
                    for (size_t i = 0; i < n; ++i) {
                        switch (shape) {
                            case 0:
                                block[i] = geo(rng);
                                break;  // geometric
                            case 1:
                                block[i] = rng() % static_cast<uint64_t>(2 * mean + 1);
                                break;  // uniform
                            case 2:
                                block[i] = (rng() % 8 == 0) ? static_cast<uint64_t>(mean * 8) : rng() % 2;
                                break;  // spiky
                            case 3:
                                block[i] = static_cast<uint64_t>(mean);
                                break;  // constant
                            case 4:
                                block[i] = (i % 2) ? static_cast<uint64_t>(2 * mean) : 0;
                                break;  // alternating
                            default:
                                block[i] = static_cast<uint64_t>(mean) + rng() % 3;
                                break;  // narrow
                        }
                        block[i] = std::min<uint64_t>(block[i], 0xFFFFFFFFull);
                    }
                    uint64_t sum = 0;
                    for (uint64_t x : block)
                        sum += x;
                    const uint32_t k = Base::estimateK(sum, n);
                    uint64_t best = UINT64_MAX;
                    for (uint32_t kk = 0; kk <= 40; ++kk)
                        best = std::min(best, splitBits(block, kk));
                    // the nearest k of minimum size
                    uint32_t distance = 99;
                    for (uint32_t kk = 0; kk <= 40; ++kk) {
                        if (splitBits(block, kk) == best) distance = std::min(distance, kk > k ? kk - k : k - kk);
                    }
                    ++blocks;
                    if (distance > 1 || 2 * (splitBits(block, k) - best) > n) {
                        check(false, "block (shape " + std::to_string(shape) + ", mean " + std::to_string(mean) + ", n " + std::to_string(n) + "): estimated k " +
                                         std::to_string(k) + ", distance " + std::to_string(distance) + ", " + std::to_string(splitBits(block, k) - best) + " bits above the minimum");
                        return;
                    }
                }
            }
        }
    }
    check(blocks == 6 * 10 * 6 * 40, "all blocks checked");
}

template <typename Sample>
static std::vector<Sample> samples(size_t count, uint32_t bits, bool isSigned, int32_t shape, uint32_t seed)
{
    std::mt19937_64 rng(seed);
    const int64_t lo = isSigned ? -(int64_t{ 1 } << (bits - 1)) : 0;
    const int64_t hi = isSigned ? (int64_t{ 1 } << (bits - 1)) - 1 : (int64_t{ 1 } << bits) - 1;
    const uint64_t span = static_cast<uint64_t>(hi - lo) + 1;
    std::vector<Sample> s(count);
    int64_t v = (lo + hi) / 2;
    for (size_t i = 0; i < count; ++i) {
        switch (shape) {
            case 0:
                v += static_cast<int64_t>(rng() % 9) - 4;
                break;  // smooth
            case 1:
                v = lo + static_cast<int64_t>(rng() % span);
                break;  // noise
            case 2:
                v = (i / 50) % 2 ? hi : lo;
                break;  // steps: zero blocks
            case 3:
                v = (i % 2) ? hi : lo;
                break;  // alternating extremes
            default:
                v += (rng() % 64 == 0) ? static_cast<int64_t>(rng() % span) - (hi - lo) / 2 : 0;  // spikes
        }
        v = std::min(std::max(v, lo), hi);
        s[i] = static_cast<Sample>(v);
    }
    return s;
}

// Compresses with both selectors: round trips, sizes, threads
template <typename Sample>
static void roundTrips(uint32_t bits, bool isSigned)
{
    for (uint32_t blockSize : { 8u, 16u, 32u, 64u }) {
        for (auto pre : { spaec::Preprocessor::UnitDelay, spaec::Preprocessor::Bypass, spaec::Preprocessor::None }) {
            if (isSigned && pre == spaec::Preprocessor::None) continue;
            for (auto codes : { spaec::CodeOptions::Basic, spaec::CodeOptions::Restricted }) {
                if (codes == spaec::CodeOptions::Restricted && bits >= 5 && bits <= 8) continue;  // does not exist
                for (int32_t shape = 0; shape < 5; ++shape) {
                    spaec::Options options;
                    options.bitsPerSample = bits;
                    options.blockSize = blockSize;
                    options.referenceInterval = shape == 4 ? 1 : 7;
                    options.preprocessor = pre;
                    options.codeOptions = codes;
                    options.signedSamples = isSigned;
                    const size_t count = 3 * blockSize * options.referenceInterval + blockSize / 2 + 3;  // partial RSI and block
                    const std::vector<Sample> s = samples<Sample>(count, bits, isSigned, shape, bits * 100 + blockSize + shape);
                    const std::string what = std::to_string(bits) + " bits" + (isSigned ? " signed" : "") + ", J = " + std::to_string(blockSize) +
                                             ", preprocessor " + std::to_string(static_cast<int>(pre)) + ", code options " + std::to_string(static_cast<int>(codes)) +
                                             ", shape " + std::to_string(shape);

                    const std::vector<uint8_t> search = spaec::compress(s, options);
                    options.splitSelector = spaec::SplitSelector::Estimate;
                    const std::vector<uint8_t> estimate = spaec::compress(s, options);
                    check(spaec::decompress<Sample>(estimate, count, options) == s, "round trip with Estimate, " + what);
                    options.splitSelector = spaec::SplitSelector::Search;
                    check(spaec::decompress<Sample>(estimate, count, options) == s, "decompress does not depend on the selector, " + what);

                    // At least the data of Search, at most J/2 bits per block more (and the rounding to bytes)
                    const size_t blocks = (count + blockSize - 1) / blockSize;
                    check(estimate.size() >= search.size(), "Estimate not smaller than Search, " + what);
                    check(estimate.size() * 8 <= search.size() * 8 + blocks * (blockSize / 2) + 8, "Estimate at most J/2 bits per block larger, " + what);

                    options.splitSelector = spaec::SplitSelector::Estimate;
                    options.threads = 3;
                    check(spaec::compress(s, options) == estimate, "threads do not change the data of Estimate, " + what);
                }
            }
        }
    }
}

static void defaultsAndErrors()
{
    const spaec::Options options;
    check(options.splitSelector == spaec::SplitSelector::Search, "Search is the default");

    const std::vector<uint16_t> s = samples<uint16_t>(10000, 16, false, 0, 1);
    spaec::Options search;
    search.splitSelector = spaec::SplitSelector::Search;
    check(spaec::compress(s, options) == spaec::compress(s, search), "the default writes the data of Search");

    spaec::Options invalid;
    invalid.splitSelector = static_cast<spaec::SplitSelector>(7);
    try {
        spaec::compress(s, invalid);
        check(false, "an invalid selector is an error");
    }
    catch (const std::invalid_argument& e) {
        check(std::string(e.what()).find("Search or Estimate") != std::string::npos, "the message names the selectors");
    }

    // Zero blocks and constant fields are coded alike by both selectors
    spaec::Options estimate;
    estimate.splitSelector = spaec::SplitSelector::Estimate;
    const std::vector<uint16_t> zeros(5000, 0), constant(5000, 4321);
    check(spaec::compress(zeros, estimate) == spaec::compress(zeros, search), "zero blocks");
    check(spaec::compress(constant, estimate) == spaec::compress(constant, search), "a constant field");
}

// Floating-point values with a Scaling (GRIB CCSDS packing)
static void scaledValues()
{
    std::mt19937_64 rng(3);
    std::vector<double> values(20000);
    double v = 250;
    for (double& x : values) {
        v += static_cast<double>(rng() % 201) / 100.0 - 1.0;
        x = std::min(std::max(v, 200.0), 300.0);
    }
    const spaec::Scaling scaling{ 200.0, -6, 0 };  // 100 / 2^-6 = 6400 < 2^13
    spaec::Options options;
    options.bitsPerSample = 13;
    const std::vector<uint8_t> search = spaec::compress(values, scaling, options);
    options.splitSelector = spaec::SplitSelector::Estimate;
    const std::vector<uint8_t> estimate = spaec::compress(values, scaling, options);
    const std::vector<double> back = spaec::decompress<double>(estimate, values.size(), scaling, options);
    double worst = 0;
    for (size_t i = 0; i < values.size(); ++i)
        worst = std::max(worst, std::fabs(back[i] - values[i]));
    check(worst <= std::ldexp(1.0, -7) * 1.0000001, "scaled values within half a step with Estimate");
    check(estimate.size() >= search.size() && estimate.size() <= search.size() + search.size() / 50, "scaled values: Estimate near Search");
}

int main()
{
    estimateRule();
    blockGuarantees();
    for (uint32_t bits : { 1u, 2u, 3u, 4u, 5u, 7u, 8u }) {
        roundTrips<uint8_t>(bits, false);
    }
    for (uint32_t bits : { 2u, 4u, 8u }) {
        roundTrips<int8_t>(bits, true);
    }
    for (uint32_t bits : { 9u, 12u, 15u, 16u }) {
        roundTrips<uint16_t>(bits, false);
    }
    roundTrips<int16_t>(16, true);
    for (uint32_t bits : { 17u, 24u, 31u, 32u }) {
        roundTrips<uint32_t>(bits, false);
    }
    roundTrips<int32_t>(32, true);
    defaultsAndErrors();
    scaledValues();

    if (failures) {
        std::cerr << failures << " failure(s)\n";
        return 1;
    }
    std::cout << "split selectors ok\n";
    return 0;
}
