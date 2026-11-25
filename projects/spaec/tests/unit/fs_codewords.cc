// SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// Unit test of the decoding of FS codewords and split bits (DecoderSeq::ssDecode): streams built by
// hand (no preprocessor, so the samples are the coded values) must decode to their samples
// - codewords of 0 to 300 zeros (the decoder reads codewords of up to 239 zeros byte by byte from a
//   table, longer ones bit by bit), each length at every bit offset of the stream
// - every split-sample k: 0 (FS) to 13 with 16 bits per sample, 0 to 29 with 32 bits (the low bits
//   are read 8, 4, 2 or 1 samples per 64-bit word)
// - random mixes of short and long codewords, block sizes 8 to 64

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include "spaec/decoder/DecoderSeq.h"

using namespace spaec;

static int failures = 0;

static void check(bool ok, const std::string& what)
{
    if (!ok) {
        if (failures < 20) std::cerr << "FAIL: " << what << '\n';
        ++failures;
    }
}

struct Bits
{
    std::vector<uint8_t> bytes;
    size_t pos = 0;

    void put(uint64_t value, unsigned n)
    {
        while (n--) {
            if (bytes.size() <= pos / 8) bytes.push_back(0);
            if ((value >> n) & 1) bytes[pos / 8] |= static_cast<uint8_t>(0x80 >> (pos % 8));
            ++pos;
        }
    }
    void fs(uint64_t zeros)
    {
        pos += zeros;
        while (bytes.size() <= pos / 8)
            bytes.push_back(0);
        put(1, 1);
    }
};

// Codes the samples as split-sample blocks with the given k (CCSDS 121.0-B-3, 5.2.3: ID k + 1,
// the FS codewords of the high bits, then the k low bits) and decodes them
template <typename T>
static void run(unsigned n, unsigned J, unsigned k, const std::vector<T>& samples, const std::string& what)
{
    const unsigned idLength = n > 16 ? 5 : n > 8 ? 4
                                                 : 3;
    Bits b;
    for (size_t first = 0; first < samples.size(); first += J) {
        b.put(k + 1, idLength);
        for (size_t i = 0; i < J; ++i)
            b.fs(static_cast<uint64_t>(samples[first + i]) >> k);
        for (size_t i = 0; i < J; ++i)
            b.put(samples[first + i] & ((uint64_t{ 1 } << k) - 1), k);
    }
    std::vector<T> decoded(samples.size());
    try {
        DecoderSeq<T, T> decoder(static_cast<uint8_t>(n), static_cast<uint8_t>(J), 1, CodeOptionSet::Basic, Preprocessing{ Prediction::None });
        decoder.decode(Span<uint8_t>(b.bytes), Span<T>(decoded));
        check(decoded == samples, what);
    }
    catch (const std::exception& e) {
        check(false, what + ": " + e.what());
    }
}

template <typename T>
static void test(unsigned n, unsigned kMax)
{
    const uint64_t maxSample = (uint64_t{ 1 } << n) - 1;
    std::mt19937_64 rng(n);
    for (unsigned k = 0; k <= kMax; ++k) {
        const uint64_t maxZeros = std::min<uint64_t>(maxSample >> k, 300);
        const uint64_t lowMask = (uint64_t{ 1 } << k) - 1;
        const std::string name = "n=" + std::to_string(n) + " k=" + std::to_string(k);

        // Each length, preceded by 0 to 7 codewords of one bit (every bit offset) and followed by
        // codewords that must still be read at the right position
        for (uint64_t zeros = 0; zeros <= maxZeros; zeros += (zeros < 230 || zeros > 260 ? 7 : 1)) {
            for (unsigned offset = 0; offset < 8; ++offset) {
                std::vector<T> samples(16, 0);
                for (size_t i = 0; i < samples.size(); ++i) {
                    const uint64_t high = i == offset ? zeros : (i > offset ? (i % 3) & (maxSample >> k) : 0);
                    samples[i] = static_cast<T>((high << k) | (rng() & lowMask));
                }
                run<T>(n, 8, k, samples, name + " zeros=" + std::to_string(zeros) + " offset=" + std::to_string(offset));
            }
        }

        // Random mixes: mostly short codewords, some long ones
        for (unsigned J : { 8u, 16u, 32u, 64u }) {
            std::vector<T> samples(J * 40);
            for (auto& s : samples) {
                const uint64_t r = rng() % 100;
                const uint64_t high = r < 80 ? rng() % 3 : r < 97 ? rng() % 12
                                                                  : rng() % (maxZeros + 1);
                s = static_cast<T>((std::min(high, maxSample >> k) << k) | (rng() & lowMask));
            }
            run<T>(n, J, k, samples, name + " random J=" + std::to_string(J));
        }
    }
}

int main()
{
    test<uint16_t>(16, 13);
    test<uint32_t>(32, 29);
    test<uint8_t>(8, 5);

    if (failures) {
        std::cerr << failures << " failure(s)\n";
        return 1;
    }
    std::cout << "FS codewords and split bits decoded\n";
    return 0;
}
