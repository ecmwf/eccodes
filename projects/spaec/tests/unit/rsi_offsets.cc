// SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// Unit test of the RSI offsets, adapted from libaec's tests/check_rsi_block_access.c and
// tests/check_seeking.c. A stream is the concatenation of its RSIs, so the bit offset of RSI i is
// the length in bits of the stream of the first i RSIs (EncoderSeq::encodeBits). For libaec's
// parameters (numbers of values, RSI and block sizes, bits per value; zero, random and increasing
// values):
// - DecoderSeq::skipFullRsi must find these offsets, and the end of the last full RSI
// - every RSI must decode from its offset (DecoderSeq::decodeFusedAt, as DecoderPar does)
// - three slices of the values must decode from the offset of the RSI that contains their start
// - a stream that starts at any bit of a buffer must decode from that bit
// - decoding an RSI whose data are cut off must throw (libaec check_decode_range_truncated)

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "spaec/detail/BitBuffer.h"
#include "spaec/detail/SpAec.h"
#include "spaec/detail/decoder/DecoderSeq.h"
#include "spaec/detail/encoder/EncoderSeq.h"

using namespace spaec::detail;

static int32_t failures = 0;

static void check(bool ok, const std::string& what)
{
    if (!ok) {
        if (failures < 20) std::cerr << "FAIL: " << what << '\n';
        ++failures;
    }
}

using Encoder = encoder::EncoderSeq<uint32_t, uint32_t>;
using Decoder = DecoderSeq<uint32_t, uint32_t>;

// Stream bits starting at bit `shift` of a buffer followed by the guard bytes the decoder needs
struct Padded
{
    std::vector<uint8_t> bytes;
    size_t limitBits;
    BitBuffer buffer() { return BitBuffer{ Span<uint8_t>(bytes), 0, limitBits }; }
};

static Padded padded(const std::vector<uint8_t>& stream, size_t nBits, size_t shift)
{
    Padded p{ std::vector<uint8_t>((shift + nBits + 7) / 8 + BitBuffer::guardBytes, 0), shift + nBits };
    for (size_t i = 0; i < nBits; ++i) {
        if ((stream[i / 8] >> (7 - i % 8)) & 1) {
            const size_t j = shift + i;
            p.bytes[j / 8] |= static_cast<uint8_t>(0x80 >> (j % 8));
        }
    }
    return p;
}

static size_t encodeBits(uint8_t bpv, uint8_t blockSize, uint16_t rsi, const uint32_t* values, size_t n, std::vector<uint8_t>& out)
{
    Encoder encoder(bpv, blockSize, rsi, CodeOptionSet::Basic);
    out.assign(encoder::SpAec<double>::maxEncodedSize(n, bpv, blockSize), 0);
    return encoder.encodeBits(Span<uint32_t>(const_cast<uint32_t*>(values), n), Span<uint8_t>(out));
}

// Decodes count values starting at bitOffset into a vector
static std::vector<uint32_t> decodeAt(Decoder& decoder, Padded& p, size_t bitOffset, size_t count)
{
    std::vector<uint32_t> out(count);
    decoder.decodeFusedAt(p.buffer(), bitOffset, count, [&out](uint32_t value, size_t i) { out[i] = value; });
    return out;
}

static void run(size_t n, uint16_t rsi, uint8_t blockSize, uint8_t bpv, int32_t generator, size_t shift)
{
    const std::string what = "n=" + std::to_string(n) + " rsi=" + std::to_string(rsi) + " bs=" + std::to_string(blockSize) + " bpv=" + std::to_string(bpv) + " generator=" + std::to_string(generator);

    // The value generators of libaec: zero, random below 2^(bpv-1), increasing modulo 2^(bpv-1)
    std::vector<uint32_t> values(n, 0);
    const uint64_t half = uint64_t{ 1 } << (bpv - 1);
    for (size_t i = 0; i < n; ++i) {
        if (generator == 1) values[i] = static_cast<uint32_t>(std::rand() & (half - 1));
        if (generator == 2) values[i] = static_cast<uint32_t>(i % half);
    }

    std::vector<uint8_t> stream;
    const size_t nBits = encodeBits(bpv, blockSize, rsi, values.data(), n, stream);
    Padded p = padded(stream, nBits, 0);
    Decoder decoder(bpv, blockSize, rsi, CodeOptionSet::Basic);

    // Offsets found by skipping the full RSIs; the last offset is the start of a partial RSI or
    // the end of the stream
    const size_t rsiValues = size_t{ rsi } * blockSize;
    const size_t nFull = n / rsiValues, nRsi = (n + rsiValues - 1) / rsiValues;
    std::vector<size_t> offsets;
    try {
        const BitBuffer scan = p.buffer();
        for (size_t i = 0; i < nFull; ++i) {
            offsets.push_back(scan.bitPos());
            decoder.skipFullRsi(scan);
        }
        offsets.push_back(scan.bitPos());
    }
    catch (const std::exception& e) {
        check(false, what + ": skipFullRsi threw: " + e.what());
        return;
    }
    if (nFull == nRsi) check(offsets.back() == nBits, what + ": the full RSIs do not end at the end of the stream");

    // Encoder offsets: of the first, second and last RSIs, and of one in the middle
    std::vector<uint8_t> prefix;
    for (size_t i : { size_t{ 1 }, size_t{ 2 }, nRsi / 2, nRsi - 1 }) {
        if (i == 0 || i >= nRsi) continue;
        check(offsets[i] == encodeBits(bpv, blockSize, rsi, values.data(), i * rsiValues, prefix),
              what + ": offset of RSI " + std::to_string(i));
    }

    try {
        // Every RSI from its offset
        for (size_t i = 0; i < nRsi; ++i) {
            const size_t count = std::min(rsiValues, n - i * rsiValues);
            const auto decoded = decodeAt(decoder, p, offsets[i], count);
            check(std::equal(decoded.begin(), decoded.end(), values.begin() + i * rsiValues),
                  what + ": RSI " + std::to_string(i) + " decoded from its offset");
        }

        // Three slices, each decoded from the offset of the RSI containing its first value
        const size_t slice = (n + 2) / 3;
        for (size_t first = 0; first < n; first += slice) {
            const size_t last = std::min(first + slice, n), r = first / rsiValues;
            const auto decoded = decodeAt(decoder, p, offsets[r], last - r * rsiValues);
            check(std::equal(values.begin() + first, values.begin() + last, decoded.begin() + (first - r * rsiValues)),
                  what + ": slice from value " + std::to_string(first));
        }

        // The stream starting at bit `shift` of the buffer
        Padded shifted = padded(stream, nBits, shift);
        check(decodeAt(decoder, shifted, shift, n) == values, what + ": stream starting at bit " + std::to_string(shift));
    }
    catch (const std::exception& e) {
        check(false, what + ": decoding threw: " + e.what());
        return;
    }

    // The last RSI cut off after its first bit: random values need more than one bit
    if (generator == 1 && bpv >= 8) {
        Padded truncated = padded(stream, offsets[nRsi - 1] + 1, 0);
        bool threw = false;
        try {
            decodeAt(decoder, truncated, offsets[nRsi - 1], n - (nRsi - 1) * rsiValues);
        }
        catch (const std::runtime_error&) {
            threw = true;
        }
        check(threw, what + ": decoding a truncated RSI must throw");
    }
}

int main()
{
    std::srand(42);
    const size_t ns[] = { 1, 255, 256, 255 * 10, 256 * 10, 67000 };
    const uint16_t rsis[] = { 1, 2, 255, 256, 512, 1024, 4095, 4096 };
    const uint8_t blockSizes[] = { 8, 16, 32, 64 };
    const uint8_t bpvs[] = { 1, 7, 8, 9, 15, 16, 17, 23, 24, 25, 31, 32 };

    size_t runs = 0;
    for (size_t n : ns)
        for (uint16_t rsi : rsis)
            for (uint8_t blockSize : blockSizes)
                for (uint8_t bpv : bpvs)
                    for (int32_t generator = 0; generator < 3; ++generator)
                        run(n, rsi, blockSize, bpv, generator, 1 + runs++ % 15);

    // libaec check_decode_range_truncated: a no-compression ID (8 bits per value: 111) without
    // the samples
    {
        std::vector<uint8_t> stream{ 0xe0 };
        Padded p = padded(stream, 8, 0);
        Decoder decoder(8, 8, 8, CodeOptionSet::Basic);
        bool threw = false;
        try {
            decodeAt(decoder, p, 0, 64);
        }
        catch (const std::runtime_error&) {
            threw = true;
        }
        check(threw, "a no-compression ID without samples must throw");
    }

    if (failures) {
        std::cerr << failures << " failure(s) in " << runs << " runs\n";
        return 1;
    }
    std::cout << runs << " runs passed\n";
    return 0;
}
