// SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// Unit tests of DecoderSeq::zbDecode derived from regression tests:
// - decode_zero_block_run: a zero-block run longer than the blocks that remain (maxBlocks) must
//   throw (it overflowed the segment buffer)
// - decode_prefix: a run may reach beyond the segment if it fits in the RSI (maxBlocks larger
//   than the segment): it is cut at the end of the segment, decoding stops when the output is
//   full
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "spaec/detail/BitBuffer.h"
#include "spaec/detail/Span.h"
#include "spaec/detail/decoder/DecoderSeq.h"

using namespace spaec::detail;

static int failures = 0;

static void check(bool ok, const std::string& what)
{
    if (!ok) {
        std::cerr << "FAIL: " << what << '\n';
        ++failures;
    }
}

constexpr size_t blockSize = 8;
constexpr size_t segmentBlocks = 4;

// The fundamental sequence codeword of a zero-block run of n blocks (like EncoderSeq::zbEncode),
// or the "remainder of segment" codeword, followed by guard bytes
static std::vector<uint8_t> runCodeword(size_t n, bool ros = false)
{
    std::vector<uint8_t> data(8 + BitBuffer::guardBytes, 0);
    BitBuffer writer{ Span<uint8_t>(data) };
    if (ros) {
        writer.writeCodeword(rosLength - 1);
    }
    else {
        writer.writeCodeword(n < rosLength ? n - 1 : n);
    }
    return data;
}

struct Result
{
    bool thrown = false;
    size_t blocks = 0;
    std::vector<uint16_t> values;
};

// Decodes the codeword into a segment of 4 blocks of 8 values (all 7 before) at blockOffset
static Result decode(const std::vector<uint8_t>& codeword, size_t blockOffset, size_t maxBlocks)
{
    DecoderSeq<uint16_t, uint16_t> decoder(16, blockSize, 64, CodeOptionSet::Basic);
    std::vector<uint8_t> data = codeword;
    const BitBuffer buffer{ Span<uint8_t>(data), 0, 64 };
    Result r;
    r.values.assign(segmentBlocks * blockSize, 7);
    try {
        r.blocks = decoder.zbDecode<ReferenceType::NoRef>(buffer, Span<uint16_t>(r.values), blockOffset, maxBlocks);
    }
    catch (const std::runtime_error&) {
        r.thrown = true;
    }
    return r;
}

// Values [from, to) are 0, all others 7
static bool zeroed(const Result& r, size_t from, size_t to)
{
    for (size_t i = 0; i < r.values.size(); ++i) {
        if (r.values[i] != ((i >= from && i < to) ? 0 : 7)) {
            return false;
        }
    }
    return true;
}

int main()
{
    // A run that fits in the segment
    Result r = decode(runCodeword(3), 0, segmentBlocks);
    check(!r.thrown && r.blocks == 3 && zeroed(r, 0, 24), "run of 3 blocks at block 0 of 4");

    // A run longer than the rest of the segment, which is also the limit (decode_zero_block_run)
    r = decode(runCodeword(3), 2, segmentBlocks);
    check(r.thrown, "run of 3 blocks at block 2 of 4 (limit 4) must throw");
    check(zeroed(r, 0, 0), "a rejected run must not write");
    r = decode(runCodeword(9), 0, 8);
    check(r.thrown, "run of 9 blocks with 8 available must throw");

    // The same run with a larger limit (the RSI continues): cut at the end of the segment (decode_prefix)
    r = decode(runCodeword(3), 2, 8);
    check(!r.thrown && r.blocks == 2 && zeroed(r, 16, 32), "run of 3 blocks at block 2 of 4 (limit 8): 2 blocks");

    // Remainder of segment
    r = decode(runCodeword(0, true), 1, segmentBlocks);
    check(!r.thrown && r.blocks == 3 && zeroed(r, 8, 32), "remainder of segment from block 1");

    // A run that starts beyond the segment
    r = decode(runCodeword(1), segmentBlocks, 8);
    check(r.thrown, "run starting after the last block must throw");

    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }
    std::cout << "OK\n";
    return 0;
}
