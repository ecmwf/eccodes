// SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// Unit test of the predictive preprocessing (CCSDS 121.0-B, 4.3: unit-delay predictor, unsigned
// samples):
// - exhaustively for 1 to 10 bits per value: every pair (previous sample, sample) is mapped like
//   the reference implementation of the standard below, and decoded back
// - the chunked inverse of PredictivePreprocessingDecodeFast::decode (chunks of 64 values decoded
//   assuming the common case, checked, decoded again with the exact rule if needed): uncommon
//   values at every position of several chunks, runs of them (the next chunk is then decoded with
//   the exact rule directly) followed by common values again, for 8, 16 and 32-bit samples
#include <cstdint>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include "spaec/detail/Preprocessor.h"
#include "spaec/detail/Span.h"

using namespace spaec::detail;

static int failures = 0;

static void check(bool ok, const std::string& what)
{
    if (!ok && failures++ < 20) {
        std::cerr << "FAIL: " << what << '\n';
    }
}

// The mapped value of sample x with prediction p (CCSDS 121.0-B, equation 4.3-3)
static uint64_t referenceMap(int64_t x, int64_t p, int64_t xMax)
{
    const int64_t delta = x - p;
    const int64_t theta = std::min(p, xMax - p);
    if (delta >= 0 && delta <= theta) {
        return static_cast<uint64_t>(2 * delta);
    }
    if (delta < 0 && delta >= -theta) {
        return static_cast<uint64_t>(2 * -delta - 1);
    }
    return static_cast<uint64_t>(theta + (delta < 0 ? -delta : delta));
}

template <typename T>
static void exhaustivePairs(unsigned bpv)
{
    const int64_t xMax = (int64_t{ 1 } << bpv) - 1;
    PredictivePreprocessingEncodeFast<T, T> encoder(static_cast<uint8_t>(bpv));
    PredictivePreprocessingDecodeFast<T, T> decoder(static_cast<uint8_t>(bpv));
    for (int64_t p = 0; p <= xMax; ++p) {
        for (int64_t x = 0; x <= xMax; ++x) {
            std::vector<T> data = { static_cast<T>(p), static_cast<T>(x) };
            std::vector<T> mapped(2), restored(2);
            encoder.encode(Span<T>(data), Span<T>(mapped));
            const uint64_t expected = referenceMap(x, p, xMax);
            check(mapped[0] == data[0] && mapped[1] == expected,
                  "bpv " + std::to_string(bpv) + ": map " + std::to_string(x) + " after " + std::to_string(p) + ": " + std::to_string(mapped[1]) + " instead of " + std::to_string(expected));
            decoder.decode(Span<T>(mapped), data[0], Span<T>(restored));
            check(restored == data, "bpv " + std::to_string(bpv) + ": decode " + std::to_string(x) + " after " + std::to_string(p));
        }
    }
}

template <typename T>
static void roundTrip(const std::vector<T>& data, unsigned bpv, const std::string& what)
{
    PredictivePreprocessingEncodeFast<T, T> encoder(static_cast<uint8_t>(bpv));
    PredictivePreprocessingDecodeFast<T, T> decoder(static_cast<uint8_t>(bpv));
    std::vector<T> input = data, mapped(data.size()), restored(data.size());
    encoder.encode(Span<T>(input), Span<T>(mapped));
    decoder.decode(Span<T>(mapped), input[0], Span<T>(restored));
    check(restored == data, what);
}

// Sequences around the chunks of the inverse (64 values): smooth values (common case) with
// uncommon ones (a jump to a limit and back) at every position of the first 4 chunks, runs of
// uncommon values of different lengths, and a field ending in each state
template <typename T>
static void chunks(unsigned bpv)
{
    const uint64_t xMax = bpv == 64 ? ~uint64_t{ 0 } : (uint64_t{ 1 } << bpv) - 1;
    std::mt19937_64 rng(bpv);
    auto smooth = [&](size_t n) {
        std::vector<T> v(n);
        uint64_t x = xMax / 2;
        for (auto& e : v) {
            x = (x + rng() % 5 + xMax - 2) % (xMax + 1);  // steps of -2..2 around the middle
            e = static_cast<T>(x);
        }
        return v;
    };
    const std::string b = "bpv " + std::to_string(bpv) + ": ";
    for (size_t pos = 0; pos < 4 * 64 + 3; ++pos) {
        std::vector<T> v = smooth(300);
        v[pos] = static_cast<T>(pos % 2 ? xMax : 0);  // to a limit: |delta| > theta at the next value
        roundTrip(v, bpv, b + "uncommon value at " + std::to_string(pos));
    }
    for (size_t runStart : { 0, 1, 63, 64, 65, 127, 128, 200 }) {
        for (size_t runLength : { 1, 2, 63, 64, 65, 130 }) {
            std::vector<T> v = smooth(runStart + runLength + 200);
            for (size_t i = runStart; i < runStart + runLength; ++i) {
                v[i] = static_cast<T>(rng() % 2 ? xMax : 0);  // alternating limits: all uncommon
            }
            roundTrip(v, bpv, b + "run of " + std::to_string(runLength) + " limits at " + std::to_string(runStart));
            v.resize(runStart + runLength);  // ending inside the run
            if (!v.empty()) {
                roundTrip(v, bpv, b + "ending in a run of limits");
            }
        }
    }
    for (size_t n = 1; n <= 130; ++n) {  // every length up to two chunks
        roundTrip(smooth(n), bpv, b + std::to_string(n) + " smooth values");
        std::vector<T> limits(n);
        for (auto& e : limits)
            e = static_cast<T>(rng() % 2 ? xMax : 0);
        roundTrip(limits, bpv, b + std::to_string(n) + " values at the limits");
    }
}

int main()
{
    for (unsigned bpv = 1; bpv <= 8; ++bpv) {
        exhaustivePairs<uint8_t>(bpv);
    }
    exhaustivePairs<uint16_t>(9);
    exhaustivePairs<uint16_t>(10);
    chunks<uint8_t>(8);
    chunks<uint8_t>(3);
    chunks<uint16_t>(16);
    chunks<uint16_t>(12);
    chunks<uint32_t>(32);
    chunks<uint32_t>(24);
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }
    std::cout << "OK\n";
    return 0;
}
