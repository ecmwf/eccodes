// SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// Exhaustive unit tests of the encoder and decoder:
// - every block of 8 samples with 1 and 2 bits per value, as the second block of a stream (no
//   reference sample) after blocks of different kinds, round trip through EncoderSeq/DecoderSeq,
//   and as the first block (with reference sample)
// - zero-block runs of every length at every block position of an RSI of two segments (runs at
//   the start, inside, at the end of a segment, across the segment boundary, "remainder of segment")
// - EncoderPar (any number of threads) produces the bytes of EncoderSeq
#include <cstdint>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include "spaec/detail/decoder/DecoderSeq.h"
#include "spaec/detail/encoder/EncoderPar.h"
#include "spaec/detail/encoder/EncoderSeq.h"

using namespace spaec::detail;

static int32_t failures = 0;

static void check(bool ok, const std::string& what)
{
    if (!ok && failures++ < 30) {
        std::cerr << "FAIL: " << what << '\n';
    }
}

static std::vector<uint8_t> encode(const std::vector<uint8_t>& samples, uint32_t bpv, uint32_t blockSize, uint32_t rsi)
{
    encoder::EncoderSeq<uint8_t, uint8_t> encoder(static_cast<uint8_t>(bpv), static_cast<uint8_t>(blockSize), static_cast<uint16_t>(rsi), CodeOptionSet::Basic);
    std::vector<uint8_t> input = samples, stream(samples.size() * 2 + 64, 0);
    stream.resize(encoder.encode(Span<uint8_t>(input), Span<uint8_t>(stream)));
    return stream;
}

static bool roundTrip(const std::vector<uint8_t>& samples, uint32_t bpv, uint32_t blockSize, uint32_t rsi)
{
    std::vector<uint8_t> stream = encode(samples, bpv, blockSize, rsi);
    DecoderSeq<uint8_t, uint8_t> decoder(static_cast<uint8_t>(bpv), static_cast<uint8_t>(blockSize), static_cast<uint16_t>(rsi), CodeOptionSet::Basic);
    std::vector<uint8_t> decoded(samples.size());
    try {
        decoder.decode(Span<uint8_t>(stream), Span<uint8_t>(decoded));
    }
    catch (const std::exception&) {
        return false;
    }
    return decoded == samples;
}

static void allBlocks(uint32_t bpv)
{
    const uint32_t nBlocks = 1u << (8 * bpv);
    const uint8_t maxv = static_cast<uint8_t>((1u << bpv) - 1);
    // First blocks: constant (zero block after preprocessing), small steps, limits
    const std::vector<std::vector<uint8_t>> firsts = { std::vector<uint8_t>(8, 0), std::vector<uint8_t>(8, maxv), { 0, 1, 0, 1, 0, 1, 0, 1 }, { 0, maxv, 0, maxv, 0, maxv, 0, maxv } };
    for (uint32_t b = 0; b < nBlocks; ++b) {
        std::vector<uint8_t> block(8);
        for (uint32_t i = 0; i < 8; ++i) {
            block[i] = static_cast<uint8_t>((b >> (bpv * i)) & maxv);
        }
        check(roundTrip(block, bpv, 8, 1), "bpv " + std::to_string(bpv) + ": block " + std::to_string(b) + " as first block");
        for (const auto& first : firsts) {
            std::vector<uint8_t> samples = first;
            samples.insert(samples.end(), block.begin(), block.end());
            check(roundTrip(samples, bpv, 8, 2), "bpv " + std::to_string(bpv) + ": block " + std::to_string(b) + " as second block");
        }
    }
}

// RSI of 128 blocks of 8 samples (two segments of 64 blocks): zeros (after preprocessing: a
// constant value) in blocks [start, start + length), noise elsewhere
static void zeroBlockRuns()
{
    std::mt19937_64 rng(5);
    const uint32_t rsi = 128;
    for (uint32_t start = 0; start < rsi; ++start) {
        for (uint32_t length = 1; start + length <= rsi; ++length) {
            std::vector<uint8_t> samples(rsi * 8);
            for (auto& s : samples)
                s = static_cast<uint8_t>(rng());
            const uint8_t constant = start == 0 ? 0 : samples[start * 8 - 1];
            for (uint32_t i = start * 8; i < (start + length) * 8; ++i)
                samples[i] = constant;
            if (!roundTrip(samples, 8, 8, rsi)) {
                check(false, "zero blocks " + std::to_string(start) + " .. " + std::to_string(start + length - 1));
            }
            // The same field ending after the run (runs to the end of the data)
            samples.resize((start + length) * 8);
            if (!roundTrip(samples, 8, 8, rsi)) {
                check(false, "field ending with zero blocks " + std::to_string(start) + " .. " + std::to_string(start + length - 1));
            }
        }
    }
}

static void parallelEncoder()
{
    std::mt19937_64 rng(6);
    for (uint32_t bpv : { 1, 8, 13, 16 }) {
        for (uint32_t blockSize : { 8, 32, 64 }) {
            for (uint32_t rsi : { 1, 7, 64 }) {
                for (size_t n : { size_t{ 0 }, size_t{ 1 }, size_t{ 9 }, size_t{ 1000 }, size_t{ 30000 } }) {
                    std::vector<uint16_t> samples(n);
                    for (size_t i = 0; i < n; ++i)
                        samples[i] = static_cast<uint16_t>((i % 400 < 100) ? 3 : rng() & ((1u << bpv) - 1));
                    encoder::EncoderSeq<uint16_t, uint16_t> seq(static_cast<uint8_t>(bpv), static_cast<uint8_t>(blockSize), static_cast<uint16_t>(rsi), CodeOptionSet::Basic);
                    std::vector<uint8_t> expected(n * 3 + 64, 0);
                    expected.resize(seq.encode(Span<uint16_t>(samples), Span<uint8_t>(expected)));
                    for (size_t threads : { 1, 2, 3, 8 }) {
                        encoder::EncoderPar<uint16_t, uint16_t> par(static_cast<uint8_t>(bpv), static_cast<uint8_t>(blockSize), static_cast<uint16_t>(rsi), CodeOptionSet::Basic, threads);
                        std::vector<uint8_t> stream(n * 3 + 64, 0);
                        stream.resize(par.encode(Span<uint16_t>(samples), Span<uint8_t>(stream)));
                        check(stream == expected, "EncoderPar bpv " + std::to_string(bpv) + " bs " + std::to_string(blockSize) + " rsi " + std::to_string(rsi) + " n " +
                                                      std::to_string(n) + ", " + std::to_string(threads) + " threads");
                    }
                }
            }
        }
    }
}

int main()
{
    allBlocks(1);
    allBlocks(2);
    zeroBlockRuns();
    parallelEncoder();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }
    std::cout << "OK\n";
    return 0;
}
