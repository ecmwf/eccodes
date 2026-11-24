// Unit tests of the code option selection of EncoderSeq derived from regression tests:
// - encode_large_samples: sePrepare must return the exact size (64 bits, it wrapped around in a
//   32-bit size_t) and encodeFullBlock must not choose the second extension for large samples
// - encode_parallel_identity: the streams must be identical to libaec's: the split-sample k is
//   searched like libaec, a block that is zero after its reference sample is a zero block, an
//   empty stream is one zero byte; ties are broken like libaec (split sample only if strictly
//   smaller than both other options, otherwise no compression if not larger than the second
//   extension)
// - encode_zero_blocks_libaec: zbEncodeROS uses the "remainder of segment" codeword only for runs
//   of at least 5 blocks, shorter runs at the end of a segment are written as normal runs
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

#include "spaec/BitBuffer.h"
#include "spaec/CodeOption.h"
#include "spaec/Span.h"
#include "spaec/encoder/EncoderSeq.h"

static int failures = 0;

static void check(bool ok, const std::string& what)
{
    if (!ok) {
        std::cerr << "FAIL: " << what << '\n';
        ++failures;
    }
}

// Encodes one block (no reference sample) and returns the bits written; id gets the first idBits bits
template <typename T>
static size_t encodeBlock(unsigned bpv, std::vector<T> block, unsigned idBits, unsigned& id)
{
    encoder::EncoderSeq<T, T> encoder(static_cast<uint8_t>(bpv), static_cast<uint8_t>(block.size()), 1, CodeOptionSet::Basic);
    std::vector<uint8_t> out(block.size() * 8 + 64, 0);
    BitBuffer buffer{ Span<uint8_t>(out) };
    encoder.template encodeFullBlock<ReferenceType::NoRef>(Span<T>(block), buffer);
    const size_t bits = buffer.bitPos();
    BitBuffer reader{ Span<uint8_t>(out) };
    id = reader.readBits<uint8_t>(idBits);
    return bits;
}

static void tieBreaking()
{
    unsigned id = 0;
    // bpv 8: ID length 3 (second extension: 4), no compression 3 + 16 * 8 = 131 bits.
    // Values in [64, 95]: split sample with k = 5 or 6 also needs 131 bits: no compression
    std::vector<uint8_t> tieSsNc(16);
    for (size_t i = 0; i < 16; ++i) {
        tieSsNc[i] = static_cast<uint8_t>(64 + (i * 7) % 32);
    }
    size_t bits = encodeBlock<uint8_t>(8, tieSsNc, 3, id);
    check(id == 7 && bits == 131, "split sample == no compression: no compression (ID " + std::to_string(id) + ", " + std::to_string(bits) + " bits)");

    // 7 pairs (0, 1) and a pair (0, 0): second extension 4 + 7 * 3 + 1 = 26 bits, split sample
    // with k = 0: 3 + 7 + 16 = 26 bits: second extension (ID 0001)
    std::vector<uint8_t> tieSsSe = { 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0 };
    bits = encodeBlock<uint8_t>(8, tieSsSe, 4, id);
    check(id == 1 && bits == 26, "split sample == second extension: second extension (ID " + std::to_string(id) + ", " + std::to_string(bits) + " bits)");

    // One more 1: split sample 27 bits, second extension 4 + 7 * 3 + 3 = 28 bits: split sample k = 0 (ID 001)
    std::vector<uint8_t> ss = { 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1 };
    bits = encodeBlock<uint8_t>(8, ss, 3, id);
    check(id == 1 && bits == 3 + 8 + 16, "split sample smaller than both: split sample k = 0 (ID " + std::to_string(id) + ")");
}

static void largeSamples()
{
    // bpv 24, alternating 0 and 2^24 - 1: exact second extension size in 64 bits
    const uint64_t m = (uint64_t{ 1 } << 24) - 1;
    std::vector<uint32_t> block(16);
    for (size_t i = 0; i < 16; ++i) {
        block[i] = static_cast<uint32_t>((i % 2) ? m : 0);
    }
    encoder::EncoderSeq<uint32_t, uint32_t> encoder(24, 16, 1, CodeOptionSet::Basic);
    const uint64_t pair = m * (m + 1) / 2 + m;                                           // codeword of the pair (0, m)
    const uint64_t seIdLength = CodeOptionBase{ 24, CodeOptionSet::Basic }.maxIdLength;  // the longest ID: second extension
    const uint64_t expected = seIdLength + 8 * (pair + 1);
    const uint64_t size = encoder.sePrepare<ReferenceType::NoRef>(Span<uint32_t>(block));
    check(size == expected, "sePrepare of alternating 0 and 2^24 - 1: " + std::to_string(size) + " bits instead of " + std::to_string(expected));

    // bpv 31, alternating 0 and 2^31 - 1: no compression (ID 11111), 5 + 16 * 31 bits
    std::vector<uint32_t> extremes(16);
    for (size_t i = 0; i < 16; ++i) {
        extremes[i] = (i % 2) ? 0x7FFFFFFFu : 0u;
    }
    unsigned id = 0;
    const size_t bits = encodeBlock<uint32_t>(31, extremes, 5, id);
    check(id == 31 && bits == 5 + 16 * 31, "alternating 0 and 2^31 - 1: no compression (ID " + std::to_string(id) + ", " + std::to_string(bits) + " bits)");
}

static void zeroBlockRuns()
{
    // bpv 8: zero-block ID 0000; a run of n blocks is the codeword of n - 1 zeros (n < 5) or n zeros
    // (n > 5, the codeword of 4 zeros means "remainder of segment")
    for (size_t n = 1; n <= 7; ++n) {
        encoder::EncoderSeq<uint8_t, uint8_t> encoder(8, 16, 64, CodeOptionSet::Basic);
        std::vector<uint8_t> out(64, 0);
        BitBuffer buffer{ Span<uint8_t>(out) };
        encoder.zbEncodeROS<ReferenceType::NoRef>(buffer, 0, n);
        const size_t zeros = buffer.bitPos() - 4 - 1;
        const size_t expected = n < 5 ? n - 1 : 4;  // the remainder of segment from 5 blocks on
        check(zeros == expected, "zbEncodeROS of " + std::to_string(n) + " blocks: codeword of " + std::to_string(zeros) + " zeros instead of " + std::to_string(expected));
    }
}

// Streams whose encoding depends on details of libaec's option selection (expected: libaec's bytes)
static void likeLibaec()
{
    struct Case
    {
        const char* what;
        unsigned bpv, blockSize, rsi;
        std::vector<uint8_t> values;
        std::vector<uint8_t> expected;
    };
    const Case cases[] = {
        // split-sample k: searched from the k of the previous block, up while it decreases
        { "split-sample k selection", 3, 8, 1, { 0, 0, 1, 0, 0, 3, 0, 1 }, { 0x43, 0xd3, 0x6e } },
        // a block that is zero after its reference sample is a zero block (with reference), also
        // in a full segment (64 blocks): here the whole RSI is one zero-block run
        { "zero block after the reference sample", 8, 8, 1, std::vector<uint8_t>(16, 5), { 0x00, 0x58, 0x02, 0xc0 } },
        { "zero block after the reference sample, full segment", 8, 8, 64, std::vector<uint8_t>(512, 5), { 0x00, 0x50, 0x80 } },
        // an empty stream is one zero byte
        { "empty stream", 8, 8, 1, {}, { 0x00 } },
    };
    for (const auto& c : cases) {
        encoder::EncoderSeq<uint8_t, uint8_t> encoder(static_cast<uint8_t>(c.bpv), static_cast<uint8_t>(c.blockSize), static_cast<uint16_t>(c.rsi), CodeOptionSet::Basic);
        std::vector<uint8_t> values = c.values;
        std::vector<uint8_t> out(64, 0);
        const size_t length = encoder.encode(Span<uint8_t>(values), Span<uint8_t>(out));
        out.resize(length);
        check(out == c.expected, std::string(c.what) + ": stream differs from libaec");
    }
}

int main()
{
    likeLibaec();
    tieBreaking();
    largeSamples();
    zeroBlockRuns();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }
    std::cout << "OK\n";
    return 0;
}
