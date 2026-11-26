// SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// Unit test of corrupt streams, adapted from libaec's tests/check_corrupt_input.c to preprocessed
// streams (8 bits per value, block size 8, RSI 1: the first block carries an 8-bit reference
// sample after its 3-bit ID). Decoding must throw std::runtime_error, and the same decoder must
// then decode a valid stream correctly (libaec check_decode_after_error):
// - a split-sample block (k = 1) followed by zeros: the zeros are one fundamental sequence that
//   never ends and leaves nothing for the binary part (check_split_truncated_binary_part)
// - a second-extension block whose first codeword gives a preprocessed sample above 2^8 - 1, and
//   one whose first fundamental sequence is 2^20 zero bits long (check_se_huge_fs_small_output).
//   libaec also rejects gamma = 91, which is valid for 8 bits per value (beta = 13): SpAec decodes
//   it, as CCSDS 121.0-B-3 does not limit gamma beyond the sample range.

#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "spaec/detail/decoder/DecoderSeq.h"
#include "spaec/detail/encoder/EncoderSeq.h"

using namespace spaec::detail;

static int32_t failures = 0;

static void check(bool ok, const std::string& what)
{
    if (!ok) {
        std::cerr << "FAIL: " << what << '\n';
        ++failures;
    }
}

struct Bits
{
    std::vector<uint8_t> bytes;
    size_t pos = 0;

    void put(uint64_t value, int32_t n)
    {
        while (n--) {
            if (bytes.size() <= pos / 8) bytes.push_back(0);
            if ((value >> n) & 1) bytes[pos / 8] |= static_cast<uint8_t>(0x80 >> (pos % 8));
            ++pos;
        }
    }
    void fs(size_t zeros)
    {
        for (size_t i = 0; i < zeros; ++i)
            put(0, 1);
        put(1, 1);
    }
};

using Decoder = DecoderSeq<uint32_t, uint32_t>;

static void expectCorrupt(Decoder& decoder, std::vector<uint8_t> stream, const std::string& what)
{
    std::vector<uint32_t> values(8);
    bool threw = false;
    try {
        decoder.decode(Span<uint8_t>(stream), Span<uint32_t>(values));
    }
    catch (const std::runtime_error&) {
        threw = true;
    }
    check(threw, what + ": decoding must throw");

    // The same decoder decodes a valid stream afterwards
    std::vector<uint32_t> source{ 3, 1, 4, 1, 5, 9, 2, 6 }, decoded(8);
    encoder::EncoderSeq<uint32_t, uint32_t> enc(8, 8, 1, CodeOptionSet::Basic);
    std::vector<uint8_t> valid(64, 0);
    valid.resize(enc.encode(Span<uint32_t>(source), Span<uint8_t>(valid)));
    try {
        decoder.decode(Span<uint8_t>(valid), Span<uint32_t>(decoded));
        check(decoded == source, what + ": a valid stream decoded afterwards");
    }
    catch (const std::exception& e) {
        check(false, what + ": a valid stream afterwards threw: " + e.what());
    }
}

int main()
{
    Decoder decoder(8, 8, 1, CodeOptionSet::Basic);

    // Split sample, k = 1 (ID 010), reference 0, then zeros only
    {
        std::vector<uint8_t> stream(32, 0);
        stream[0] = 0x40;
        expectCorrupt(decoder, stream, "split-sample block with zeros instead of its codewords");
    }

    // Second extension (ID 000 1), reference 0, then gamma = 45150 (beta = 300, so a preprocessed
    // sample of 300 > 255) and valid codewords for the rest of the block
    {
        Bits b;
        b.put(1, 4);
        b.put(0, 8);
        b.fs(45150);
        for (int32_t i = 0; i < 4; ++i)
            b.fs(0);
        expectCorrupt(decoder, b.bytes, "second extension with a sample above 2^8 - 1");
    }

    // Second extension with a first fundamental sequence of 2^20 zero bits
    {
        Bits b;
        b.put(1, 4);
        b.put(0, 8);
        b.fs(size_t{ 1 } << 20);
        for (int32_t i = 0; i < 4; ++i)
            b.fs(0);
        expectCorrupt(decoder, b.bytes, "second extension with a fundamental sequence of 2^20 zeros");
    }

    // libaec rejects gamma = 91 (beyond its table): valid with 8 bits per value
    {
        Bits b;
        b.put(1, 4);
        b.put(0, 8);
        b.fs(91);
        for (int32_t i = 0; i < 4; ++i)
            b.fs(0);
        b.put(0, 7);
        std::vector<uint32_t> values(8);
        try {
            decoder.decode(Span<uint8_t>(b.bytes), Span<uint32_t>(values));
        }
        catch (const std::exception& e) {
            check(false, std::string("second extension with gamma = 91 must decode: ") + e.what());
        }
    }

    if (failures) {
        std::cerr << failures << " failure(s)\n";
        return 1;
    }
    std::cout << "corrupt streams rejected\n";
    return 0;
}
