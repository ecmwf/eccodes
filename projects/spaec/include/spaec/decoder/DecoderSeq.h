/*
 * SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <cmath>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include "spaec/Compat.h"
#include "spaec/Span.h"
#include "spaec/CodeOption.h"
#include "spaec/Preprocessor.h"
#include "spaec/BitBuffer.h"
#include "spaec/Utils.h"

namespace spaec
{

namespace detail
{

// The FS codewords (zero bits terminated by a one bit, bits MSB first) that end in a byte, for
// every byte value: the fast codeword reader of the decoder handles the stream byte by byte
struct FsByte
{
    uint8_t zeros[8];  // zeros before each one bit (for the first: from the start of the byte)
    uint8_t ones;      // number of one bits
    uint8_t trail;     // zeros after the last one bit (8 without a one bit)
    uint8_t pad[6];
};

struct FsTables
{
    FsByte bytes[256];
    uint8_t end[256][8];  // end[b][r]: bits of b up to and including its (r + 1)-th one bit
};

constexpr FsTables makeFsTables()
{
    FsTables t{};
    for (unsigned b = 0; b < 256; ++b) {
        unsigned ones = 0, zeros = 0;
        for (unsigned bit = 0; bit < 8; ++bit) {
            if ((b >> (7 - bit)) & 1u) {
                t.bytes[b].zeros[ones] = static_cast<uint8_t>(zeros);
                t.end[b][ones] = static_cast<uint8_t>(bit + 1);
                ++ones;
                zeros = 0;
            }
            else {
                ++zeros;
            }
        }
        t.bytes[b].ones = static_cast<uint8_t>(ones);
        t.bytes[b].trail = static_cast<uint8_t>(zeros);
    }
    return t;
}

inline constexpr FsTables fsTables = makeFsTables();

}  // namespace detail

template <typename ValueType, typename DeltaType, typename DecodeFunc = Identity<DeltaType>>
class DecoderSeq
{
private:
    static uint64_t loadU64(const uint8_t* ptr)
    {
        uint64_t val;
        std::memcpy(&val, ptr, sizeof(uint64_t));
        return val;
    }

    // Reads unary codewords (zero bits terminated by a one bit) from a 64-bit accumulator.
    // The shared inner loop of the SE and SS decoders: all three call sites decode one
    // codeword per iteration from the same (data, pos, acc, consumed) state.
    struct UnaryCodewordReader
    {
        const uint8_t* data;
        const BitBuffer& buffer;
        size_t pos;
        uint64_t acc;
        size_t consumed;

        explicit UnaryCodewordReader(const BitBuffer& bitBuffer) :
            data(bitBuffer.getBuffer().data()),
            buffer(bitBuffer),
            pos(bitBuffer.bitPos())
        {
            const size_t bitpos = pos & 7;
            acc = bigEndian64(loadU64(&data[pos >> 3]));
            acc <<= bitpos;
            consumed = bitpos;
        }

        // Returns the number of zeros before the terminating one bit and advances the state.
        size_t read()
        {
            const size_t cw_start = pos;
            while (SPAEC_UNLIKELY(acc == 0)) {
                buffer.checkPos(pos);
                pos += (64 - consumed);
                const size_t bitpos = pos & 7;
                acc = bigEndian64(loadU64(&data[pos >> 3]));
                acc <<= bitpos;
                consumed = bitpos;
            }
            const size_t lz = countlZero(acc);
            const size_t total_zeros = (pos - cw_start) + lz;
            pos += lz + 1;
            consumed += lz + 1;
            if (SPAEC_UNLIKELY(consumed >= 64)) {
                const size_t bitpos = pos & 7;
                acc = bigEndian64(loadU64(&data[pos >> 3]));
                acc <<= bitpos;
                consumed = bitpos;
            }
            else {
                acc <<= (lz + 1);
            }
            return total_zeros;
        }
    };

    // Fast reader of count FS codewords at the position of the buffer: stores their numbers of
    // zeros in zeros[0, count) (room for count + 8 values is needed) and advances the buffer.
    // Returns false, with the buffer unchanged, for a codeword of 240 or more zeros (also at the
    // end of truncated data, which is followed by zero bytes): the caller then uses
    // UnaryCodewordReader, which handles any length and checks the end of the data.
    static bool readCodewordsFast(const BitBuffer& buffer, uint8_t* zeros, size_t count)
    {
        const uint8_t* data = buffer.getBuffer().data();
        const size_t pos = buffer.bitPos();
        size_t p = pos >> 3;
        const unsigned bitOffset = static_cast<unsigned>(pos & 7);
        // The bits before the position are cleared and their number is taken from the first codeword
        int carry = -static_cast<int>(bitOffset);
        unsigned byte = data[p] & (0xFFu >> bitOffset);
        size_t n = 0;
        for (;;) {
            const detail::FsByte& e = detail::fsTables.bytes[byte];
            std::memcpy(zeros + n, e.zeros, 8);
            zeros[n] = static_cast<uint8_t>(e.zeros[0] + carry);
            const size_t before = n;
            n += e.ones;
            if (n >= count) {
                buffer.bitPos(p * 8 + detail::fsTables.end[byte][count - before - 1]);
                return true;
            }
            carry = e.ones != 0 ? e.trail : carry + 8;
            if (SPAEC_UNLIKELY(carry >= 240)) {
                return false;
            }
            byte = data[++p];
        }
    }

    // Reads the k low bits of count samples at bit position pos, G samples per 64-bit load
    // (G * k <= 57: the valid bits after a bit offset of up to 7). The trip counts are constant:
    // up to G - 1 samples more than count are written (lows has room for count + 8).
    template <size_t G>
    static void readLowBits(const uint8_t* data, size_t pos, unsigned k, ValueType* lows, size_t count)
    {
        const uint64_t mask = (uint64_t{ 1 } << k) - 1;
        for (size_t i = 0; i < count; i += G) {
            const uint64_t word = bigEndian64(loadU64(&data[pos >> 3])) << (pos & 7);
            for (size_t j = 0; j < G; ++j) {
                lows[i + j] = static_cast<ValueType>((word >> (64 - (j + 1) * k)) & mask);
            }
            pos += G * k;
        }
    }

protected:
    [[noreturn]] [[gnu::cold, gnu::noinline]] static void throwCorrupt(const std::string& what)
    {
        throw std::runtime_error("CCSDS decoding failed: the compressed data is corrupt (" + what + ")");
    }

    // The readers may load bytes beyond the end of the compressed data (see BitBuffer::guardBytes):
    // decode from a copy of the source followed by zero bytes.
    BitBuffer makeBuffer(Span<uint8_t> source) const
    {
        paddedSource_.assign(source.size() + BitBuffer::guardBytes, 0);
        if (source.size() > 0) {
            std::memcpy(paddedSource_.data(), source.data(), source.size());
        }
        return BitBuffer{ Span<uint8_t>(paddedSource_), 0, source.size() * 8 };
    }

public:
    // NC - batched readBits using accumulator
    template <ReferenceType RType>
    void ncDecode(const BitBuffer& buffer, Span<ValueType> values)
    {
        if constexpr (RType == ReferenceType::Ref) {
            values[0] = 0;
        }
        constexpr uint8_t startIndex = (RType == ReferenceType::Ref) ? 1 : 0;
        const uint8_t* data = buffer.getBuffer().data();
        const uint8_t nBits = bpv_;

        size_t pos = buffer.bitPos();
        size_t bytepos = pos >> 3;
        size_t bitpos = pos & 7;
        uint64_t acc = bigEndian64(loadU64(&data[bytepos]));
        acc <<= bitpos;
        int bitsAvail = 64 - static_cast<int>(bitpos);

        for (size_t i = startIndex; i < blockSizeV_; ++i) {
            if (SPAEC_UNLIKELY(bitsAvail < nBits)) {
                bytepos = pos >> 3;
                bitpos = pos & 7;
                acc = bigEndian64(loadU64(&data[bytepos]));
                acc <<= bitpos;
                bitsAvail = 64 - static_cast<int>(bitpos);
            }
            values[i] = static_cast<ValueType>(acc >> (64 - nBits));
            acc <<= nBits;
            bitsAvail -= nBits;
            pos += nBits;
        }
        buffer.bitPos(pos);
    }

    // SE - integer sqrt + batched codeword reading
    void seDecode(const BitBuffer& buffer, Span<ValueType> values) const
    {
        assert(values.size() == blockSizeV_ && "Decoded values size mismatch");

        UnaryCodewordReader codewords{ buffer };
        uint64_t all = 0;  // all samples ORed: larger than maxSample_ iff a sample is (it is 2^n - 1)
        for (size_t i = 0; i < blockSizeV_ / 2; ++i) {
            // gamma = (d1 + d2)(d1 + d2 + 1) / 2 + d2 (CCSDS 121.0-B-3, 3.4.1) may be 2^n or larger
            // (3.4.2): it is not truncated to the sample type
            const uint64_t gamma = codewords.read();

            // w = d1 + d2 = floor((sqrt(8*gamma+1) - 1) / 2)
            uint64_t w;
            if (SPAEC_LIKELY(gamma < (uint64_t{ 1 } << 21))) {
                // Fast integer sqrt: 8*gamma+1 < 2^24 is exact in float
                const uint32_t val = 8u * static_cast<uint32_t>(gamma) + 1;
                uint32_t s = static_cast<uint32_t>(std::sqrt(static_cast<float>(val)));
                s -= (s * s > val);  // adjust down if needed
                w = (s - 1) >> 1;
            }
            else {
                w = static_cast<uint64_t>((std::sqrt(8.0 * static_cast<double>(gamma) + 1.0) - 1.0) / 2.0);
                while (w * (w + 1) / 2 > gamma) {
                    --w;
                }
                while ((w + 1) * (w + 2) / 2 <= gamma) {
                    ++w;
                }
            }
            const uint64_t d2 = gamma - w * (w + 1) / 2;
            all |= (w - d2) | d2;
            values[2 * i] = static_cast<ValueType>(w - d2);
            values[2 * i + 1] = static_cast<ValueType>(d2);
        }
        if (SPAEC_UNLIKELY(all > maxSample_)) {
            throwCorrupt("second-extension sample beyond 2^" + std::to_string(bpv_) + " - 1");
        }
        buffer.bitPos(codewords.pos);
    }

    // SS
    template <ReferenceType RType>
    void ssDecode(const BitBuffer& buffer, uint8_t k, Span<ValueType> values) const
    {
        if constexpr (RType == ReferenceType::Ref) {
            values[0] = 0;
        }

        constexpr size_t startIndex = (RType == ReferenceType::Ref) ? 1 : 0;
        const uint8_t* data = buffer.getBuffer().data();  // also used by the phase 2 reader
        // At most 64 samples (the largest block size): bounds the local arrays for the compiler
        const size_t count = std::min<size_t>(blockSizeV_ - startIndex, 64);
        ValueType* out = values.data() + startIndex;

        // Phase 1: the FS codewords, with the fast reader (byte by byte) or, for long codewords
        // and at the end of the data, one by one
        uint8_t zeros[64 + 8];
        const bool fast = readCodewordsFast(buffer, zeros, count);
        if (SPAEC_UNLIKELY(!fast)) {
            UnaryCodewordReader codewords{ buffer };
            uint64_t all = 0;  // the FS codewords ORed: beyond 2^(n-k) - 1 iff one of them is
            for (size_t i = 0; i < count; ++i) {
                const uint64_t high = codewords.read();
                all |= high;
                out[i] = static_cast<ValueType>(high << k);
            }
            if (SPAEC_UNLIKELY(all > (maxSample_ >> k))) {
                throwCorrupt((k != 0 ? "split-sample sample beyond 2^" : "FS sample beyond 2^") + std::to_string(bpv_) + " - 1");
            }
            buffer.bitPos(codewords.pos);
        }

        // Phase 2: the k low bits of the samples
        ValueType lows[64 + 8];
        if (k != 0) {
            const size_t pos = buffer.bitPos();
            if (k <= 7) {
                readLowBits<8>(data, pos, k, lows, count);
            }
            else if (k <= 14) {
                readLowBits<4>(data, pos, k, lows, count);
            }
            else if (k <= 28) {
                readLowBits<2>(data, pos, k, lows, count);
            }
            else {
                readLowBits<1>(data, pos, k, lows, count);
            }
            buffer.bitPos(pos + count * k);
        }
        else {
            std::memset(lows, 0, count * sizeof(ValueType));
        }

        if (SPAEC_LIKELY(fast)) {
            unsigned all = 0;
            for (size_t i = 0; i < count; ++i) {
                all |= zeros[i];
                out[i] = static_cast<ValueType>((static_cast<uint64_t>(zeros[i]) << k) | lows[i]);
            }
            if (SPAEC_UNLIKELY(all > (maxSample_ >> k))) {
                throwCorrupt((k != 0 ? "split-sample sample beyond 2^" : "FS sample beyond 2^") + std::to_string(bpv_) + " - 1");
            }
        }
        else {
            for (size_t i = 0; i < count; ++i) {
                out[i] |= lows[i];
            }
        }
    }

    // ZB
    // maxBlocks caps the run length: the number of blocks still available at blockOffset,
    // in the segment or, for the last RSI of a shorter target, in the whole RSI (the run
    // may legitimately cover blocks beyond the requested values: decoding stops with success when
    // the output is full)
    template <ReferenceType RType>
    size_t zbDecode(const BitBuffer& buffer, Span<ValueType> segment, size_t blockOffset, size_t maxBlocks)
    {
        size_t length = buffer.readCodeword();
        assert(segment.size() % blockSizeV_ == 0 && "Segment size must be multiple of block size.");
        size_t blocksWritten = 0;
        const size_t nBlocksInSegment = segment.size() / blockSizeV_;
        if (blockOffset >= nBlocksInSegment) {
            throwCorrupt("zero block at block " + std::to_string(blockOffset) + " of a segment of " + std::to_string(nBlocksInSegment) + " blocks");
        }

        if (length == rosLength) {  // All zeros segment
            size_t start = blockOffset * blockSizeV_;
            std::memset(&segment[start], 0, (segment.size() - start) * sizeof(ValueType));
            blocksWritten = segment.size() / blockSizeV_ - blockOffset;
        }
        else {  // First n blocks are zeros
            if (length > rosLength) {
                length--;
            }
            if (length > maxBlocks - blockOffset) {
                throwCorrupt("run of " + std::to_string(length) + " zero blocks, but only " + std::to_string(maxBlocks - blockOffset) + " blocks remain");
            }

            // A run may reach beyond the segment if it still fits in the RSI: the requested
            // values are then exhausted, so the run is cut at the end of the segment
            blocksWritten = std::min(length, nBlocksInSegment - blockOffset);

            size_t start = blockOffset * blockSizeV_;
            std::memset(&segment[start], 0, blocksWritten * blockSizeV_ * sizeof(ValueType));
        }
        return blocksWritten;
    }


    // BLOCK

    ValueType handleRefBlock(const BitBuffer& buffer, size_t id, Span<ValueType> block)
    {
        ValueType reference = 0;
        if (id == 1) {
            buffer.skipBits(maxIdLength);
            reference = buffer.readBits<ValueType>(bpv_);
            seDecode(buffer, block);
        }
        else {
            id >>= 1;
            buffer.skipBits(maxIdLength - 1);
            reference = buffer.readBits<ValueType>(bpv_);

            if (id == noCompressionId_) {
                ncDecode<ReferenceType::Ref>(buffer, block);
            }
            else {
                ssDecode<ReferenceType::Ref>(buffer, id - 1, block);
            }
        }
        return reference;
    };


    void handleBlock(const BitBuffer& buffer, size_t id, Span<ValueType> block)
    {
        if (id == 1) {
            buffer.skipBits(maxIdLength);
            seDecode(buffer, block);
        }
        else {
            id >>= 1;
            buffer.skipBits(maxIdLength - 1);

            if (id == noCompressionId_) {
                ncDecode<ReferenceType::NoRef>(buffer, block);
            }
            else {
                ssDecode<ReferenceType::NoRef>(buffer, id - 1, block);
            }
        }
    };


    // SEGMENT
    // maxBlocks is the zero-block run limit handed to zbDecode (see there)
    ValueType decodeRefFullSegment(const BitBuffer& buffer, Span<ValueType> segment, size_t maxBlocks)
    {
        assert(segment.size() % blockSizeV_ == 0 && "Segment size must be multiple of block size.");

        SplitBlock<ValueType> blocks(segment, blockSizeV_);
        size_t pos = 0;

        // First Block containing a reference
        buffer.checkPos();
        auto id = buffer.peekBits(maxIdLength);
        ValueType reference = 0;

        if (id == 0) {
            buffer.skipBits(maxIdLength);
            reference = buffer.template readBits<ValueType>(bpv_);
            pos += zbDecode<ReferenceType::Ref>(buffer, segment, pos, maxBlocks);
        }
        else {
            reference = handleRefBlock(buffer, id, blocks.full(0));
            pos++;
        }

        // Remaining Blocks
        for (; pos < blocks.nFull();) {
            buffer.checkPos();
            const auto nextId = buffer.peekBits(maxIdLength);
            if (nextId == 0) {
                buffer.skipBits(maxIdLength);
                pos += zbDecode<ReferenceType::NoRef>(buffer, segment, pos, maxBlocks);
            }
            else {
                handleBlock(buffer, nextId, blocks.full(pos));
                pos++;
            }
            assert(pos <= segment.size() / blockSizeV_ && "Decoded blocks exceed segment size.");
        }

        assert(pos == segment.size() / blockSizeV_ && "Decoded blocks do not match segment size.");

        return reference;
    }

    void decodeAltFullSegment(const BitBuffer& buffer, Span<ValueType> segment, size_t maxBlocks)
    {
        assert(segment.size() % blockSizeV_ == 0 && "Segment size must be multiple of block size.");
        SplitBlock<ValueType> blocks(segment, blockSizeV_);

        // All Blocks
        for (size_t pos = 0; pos < blocks.nFull();) {
            buffer.checkPos();
            auto id = buffer.peekBits(maxIdLength);
            if (id == 0) {
                assert(pos < segment.size() / blockSizeV_ && "Position exceeds number of blocks in segment.");

                buffer.skipBits(maxIdLength);
                pos += zbDecode<ReferenceType::NoRef>(buffer, segment, pos, maxBlocks);
            }
            else {
                handleBlock(buffer, id, blocks.full(pos));
                pos++;
            }

            assert(pos <= segment.size() / blockSizeV_ && "Decoded blocks exceed segment size.");
        }
    }

    // Decodes a trailing partial segment: decode a zero-padded full segment and copy back.
    // maxBlocks is the zero-block run limit handed to the segment decoders (see zbDecode).
    template <bool HasReference>
    auto decodePartSegment(const BitBuffer& buffer, Span<ValueType> segment, size_t maxBlocks)
    {
        size_t newSize = (segment.size() + blockSizeV_ - 1) / blockSizeV_ * blockSizeV_;
        paddedSegmentBuf_.resize(newSize);
        std::memset(paddedSegmentBuf_.data(), 0, newSize * sizeof(ValueType));
        Span<ValueType> paddedSpan{ paddedSegmentBuf_.data(), paddedSegmentBuf_.size() };
        if constexpr (HasReference) {
            ValueType reference = decodeRefFullSegment(buffer, paddedSpan, maxBlocks);
            copyBackPartSegment(segment);
            return reference;
        }
        else {
            decodeAltFullSegment(buffer, paddedSpan, maxBlocks);
            copyBackPartSegment(segment);
            return ValueType(0);
        }
    }

    void copyBackPartSegment(Span<ValueType> segment)
    {
        for (size_t i = 0; i < segment.size(); ++i) {
            segment[i] = paddedSegmentBuf_[i];
        }
    }

    // RSI
    // maxRunBlocks is the number of blocks the zero-block runs of this RSI may cover: 0
    // limits them to the blocks of their segment, the block count of the RSI as encoded
    // (rsiSizeB_) allows them to reach beyond the requested values of a shorter target
    // (see zbDecode)
    // Without reference sample (HasReference false: bypass predictor, no preprocessor), all segments
    // are decoded like the alternative segments and the result is 0.
    template <bool HasReference = true>
    ValueType decodeRsiRaw(const BitBuffer& buffer, size_t rsiSize, size_t maxRunBlocks)
    {
        rsiBuf_.resize(rsiSize);
        Span<ValueType> rsiBufSpan(rsiBuf_);

        SplitBlock<ValueType> segments(rsiBufSpan, maxSegmentSizeV_);
        ValueType reference = 0;

        if (rsiSize < maxSegmentSizeV_) {
            // The whole RSI is one (padded) segment: its own blocks are the strict limit
            const size_t segmentBlocks = (rsiSize + blockSizeV_ - 1) / blockSizeV_;
            reference = decodePartSegment<HasReference>(buffer, segments.part(),
                                                        maxRunBlocks != 0 ? maxRunBlocks : segmentBlocks);
        }
        else {
            if constexpr (HasReference) {
                reference = decodeRefFullSegment(buffer, segments.full(0), maxSegmentSizeB_);
            }
            else {
                decodeAltFullSegment(buffer, segments.full(0), maxSegmentSizeB_);
            }

            for (size_t i = 1; i < segments.nFull(); ++i) {
                decodeAltFullSegment(buffer, segments.full(i), maxSegmentSizeB_);
            }

            if (segments.hasPart()) {
                const size_t segmentBlocks = (segments.part().size() + blockSizeV_ - 1) / blockSizeV_;
                const size_t maxBlocks = maxRunBlocks != 0
                                           ? maxRunBlocks - segments.nFull() * maxSegmentSizeB_
                                           : segmentBlocks;
                decodePartSegment<false>(buffer, segments.part(), maxBlocks);
            }
        }

        return reference;
    }

    void decodeRefFullRsi(const BitBuffer& buffer, Span<ValueType> rsi, size_t maxRunBlocks)
    {
        ValueType reference = decodeRsiRaw(buffer, rsi.size(), maxRunBlocks);
        preprocessor_.decode(Span<DeltaType>(rsiBuf_), reference, rsi);
    }

    // Decodes one RSI with a preprocessing other than the unit-delay predictor for unsigned samples
    // (the inverse of EncoderSeq::preprocessOther). Signed samples are returned sign-extended.
    __attribute__((noinline)) void decodeRsiOther(const BitBuffer& buffer, Span<ValueType> rsi, size_t maxRunBlocks)
    {
        const size_t n = rsi.size();
        const uint64_t mask = maxSample_;
        const uint64_t top = uint64_t{ 1 } << (bpv_ - 1);
        if (preprocessing_.prediction == Prediction::UnitDelay) {
            // Signed samples in offset binary (the top bit inverted), the reference sample in two's
            // complement
            const ValueType reference = decodeRsiRaw<true>(buffer, n, maxRunBlocks);
            preprocessor_.decode(Span<DeltaType>(rsiBuf_.data(), n), static_cast<ValueType>((static_cast<uint64_t>(reference) ^ top) & mask), rsi);
            for (size_t i = 0; i < n; ++i) {
                rsi[i] = static_cast<ValueType>(static_cast<int64_t>(rsi[i]) - static_cast<int64_t>(top));
            }
            return;
        }
        decodeRsiRaw<false>(buffer, n, maxRunBlocks);
        if (preprocessing_.signedSamples) {
            // Bypass predictor: 2x for x >= 0, 2|x| - 1 for x < 0
            for (size_t i = 0; i < n; ++i) {
                const uint64_t d = rsiBuf_[i];
                rsi[i] = static_cast<ValueType>(static_cast<int64_t>(d >> 1) ^ -static_cast<int64_t>(d & 1));
            }
        }
        else {
            std::copy(rsiBuf_.begin(), rsiBuf_.begin() + static_cast<ptrdiff_t>(n), rsi.begin());
        }
    }

    // Skip one full RSI in the bitstream without decoding (for offset scanning)
    void skipFullRsi(const BitBuffer& buffer) const
    {
        const size_t nFullSegments = rsiSizeV_ / maxSegmentSizeV_;
        const bool hasReference = preprocessing_.prediction == Prediction::UnitDelay;

        auto skipOneBlock = [&](bool withRef) -> std::pair<bool, size_t> {
            buffer.checkPos();
            auto id = buffer.peekBits(maxIdLength);
            bool isRos = false;
            size_t nBlocks = 1;

            if (id == 0) {
                buffer.skipBits(maxIdLength);
                if (withRef) buffer.skipBits(bpv_);
                nBlocks = buffer.readCodeword();
                if (nBlocks == rosLength) {
                    isRos = true;
                }
                else if (nBlocks > rosLength) {
                    nBlocks--;
                }
            }
            else if (id == 1) {
                buffer.skipBits(maxIdLength);
                if (withRef) buffer.skipBits(bpv_);
                buffer.skipCodewords(blockSizeV_ / 2);
            }
            else {
                id >>= 1;
                buffer.skipBits(maxIdLength - 1);
                if (id == noCompressionId_) {
                    buffer.skipBits(blockSizeV_ * bpv_);
                }
                else {
                    if (withRef) buffer.skipBits(bpv_);
                    uint8_t k = static_cast<uint8_t>(id - 1);
                    size_t startIndex = withRef ? 1 : 0;
                    buffer.skipCodewords(blockSizeV_ - startIndex);
                    buffer.skipBits(k * (blockSizeV_ - startIndex));
                }
            }
            return { isRos, nBlocks };
        };

        auto skipSegment = [&](bool isRef, size_t nBlocksInSeg) {
            size_t pos = 0;
            auto [isRos, nBlocks] = skipOneBlock(isRef);
            if (isRos) return;
            pos += nBlocks;
            for (; pos < nBlocksInSeg;) {
                auto [isRos2, nBlocks2] = skipOneBlock(false);
                if (isRos2) return;
                pos += nBlocks2;
            }
        };

        if (rsiSizeV_ < maxSegmentSizeV_) {
            skipSegment(hasReference, rsiSizeB_);
        }
        else {
            skipSegment(hasReference, maxSegmentSizeB_);
            for (size_t u = 1; u < nFullSegments; ++u) {
                skipSegment(false, maxSegmentSizeB_);
            }
            size_t lastSegBlocks = rsiSizeB_ % maxSegmentSizeB_;
            if (lastSegBlocks > 0) {
                skipSegment(false, lastSegBlocks);
            }
        }
    }


public:
    void decode(Span<uint8_t> source, Span<ValueType> target)
    {
        const BitBuffer buffer = makeBuffer(source);
        SplitBlock<ValueType> chunks(target, rsiSizeV_);
        if (SPAEC_LIKELY(preprocessing_.isDefault())) {
            for (size_t i = 0; i < chunks.nFull(); ++i) {
                decodeRefFullRsi(buffer, chunks.full(i), 0);
            }
            if (chunks.hasPart()) {
                decodeRefFullRsi(buffer, chunks.part(), rsiSizeB_);
            }
        }
        else {
            for (size_t i = 0; i < chunks.nFull(); ++i) {
                decodeRsiOther(buffer, chunks.full(i), 0);
            }
            if (chunks.hasPart()) {
                decodeRsiOther(buffer, chunks.part(), rsiSizeB_);
            }
        }
        buffer.checkPos();
        bitsDecoded_ = buffer.bitPos();
    }

    // Number of bits of the source used by the last decode(source, target)
    size_t bitsDecoded() const { return bitsDecoded_; }

    // Fused decode: preprocessor + output callback in one pass (no intermediate buffer)
    template <typename OutputFunc>
    void decodeFused(Span<uint8_t> source, size_t nValues, OutputFunc&& outputFunc)
    {
        decodeFusedAt(makeBuffer(source), 0, nValues, std::forward<OutputFunc>(outputFunc));
    }

    // Decodes nValues values RSI by RSI and passes each decoded RSI to
    // sink(const ValueType* values, size_t count, size_t offset): the caller can convert whole
    // RSIs in (vectorizable) loops instead of one callback per value
    template <typename SpanFunc>
    void decodeSpans(Span<uint8_t> source, size_t nValues, SpanFunc&& sink)
    {
        const BitBuffer buffer = makeBuffer(source);
        buffer.checkPos();
        valuesBuf_.resize(std::min(nValues, rsiSizeV_));
        if (SPAEC_UNLIKELY(!preprocessing_.isDefault())) {
            decodeOther(buffer, nValues, [&](size_t count, size_t offset) { sink(static_cast<const ValueType*>(valuesBuf_.data()), count, offset); });
            return;
        }
        for (size_t offset = 0; offset < nValues; offset += rsiSizeV_) {
            const size_t count = std::min(rsiSizeV_, nValues - offset);
            // A trailing partial RSI may end with a zero-block run beyond the requested values
            const ValueType reference = decodeRsiRaw(buffer, count, count < rsiSizeV_ ? rsiSizeB_ : 0);
            preprocessor_.decode(Span<DeltaType>(rsiBuf_.data(), count), reference, Span<ValueType>(valuesBuf_.data(), count));
            sink(static_cast<const ValueType*>(valuesBuf_.data()), count, offset);
        }
        buffer.checkPos();
    }

    // decodeSpans and decodeFusedAt for a preprocessing other than the unit-delay predictor for
    // unsigned samples: decodes nValues values RSI by RSI into valuesBuf_ (sized for one RSI) and
    // calls done(count, offset) after each RSI. Out of line: the default paths stay unchanged.
    template <typename Done>
    __attribute__((noinline)) void decodeOther(const BitBuffer& buffer, size_t nValues, Done&& done)
    {
        for (size_t offset = 0; offset < nValues; offset += rsiSizeV_) {
            const size_t count = std::min(rsiSizeV_, nValues - offset);
            decodeRsiOther(buffer, Span<ValueType>(valuesBuf_.data(), count), count < rsiSizeV_ ? rsiSizeB_ : 0);
            done(count, offset);
        }
        buffer.checkPos();
    }

    // Fused decode of nValues values starting at bitOffset (for parallel decoding).
    // paddedBuffer must be followed by BitBuffer::guardBytes readable bytes (see makeBuffer).
    template <typename OutputFunc>
    void decodeFusedAt(const BitBuffer& paddedBuffer, size_t bitOffset, size_t nValues, OutputFunc&& outputFunc)
    {
        const BitBuffer buffer{ paddedBuffer, bitOffset };
        buffer.checkPos();

        if (SPAEC_UNLIKELY(!preprocessing_.isDefault())) {
            valuesBuf_.resize(std::min(nValues, rsiSizeV_));
            decodeOther(buffer, nValues, [&](size_t count, size_t offset) {
                for (size_t j = 0; j < count; ++j) {
                    outputFunc(valuesBuf_[j], offset + j);
                }
            });
            return;
        }

        size_t nFull = nValues / rsiSizeV_;
        size_t remainder = nValues % rsiSizeV_;

        for (size_t i = 0; i < nFull; ++i) {
            ValueType reference = decodeRsiRaw(buffer, rsiSizeV_, 0);
            preprocessor_.decodeFused(
                rsiBuf_.data(), rsiSizeV_,
                reference, i * rsiSizeV_, outputFunc);
        }

        if (remainder > 0) {
            ValueType reference = decodeRsiRaw(buffer, remainder, rsiSizeB_);
            preprocessor_.decodeFused(
                rsiBuf_.data(), remainder,
                reference, nFull * rsiSizeV_, outputFunc);
        }
        buffer.checkPos();
    }

    DecoderSeq(uint8_t bpv, uint8_t block_size, uint16_t rsiSize, CodeOptionSet codeOptionSet, Preprocessing preprocessing = {}) :
        bpv_((validateCcsdsParameters(bpv, block_size, rsiSize, 8 * std::min(sizeof(ValueType), sizeof(DeltaType))), bpv)),
        blockSizeV_(block_size),
        rsiSizeB_(rsiSize),
        rsiSizeV_(rsiSize * block_size),
        codeOptionSet_(codeOptionSet),
        preprocessor_(bpv),
        preprocessing_((validatePreprocessing(preprocessing), preprocessing))
    {
        static_assert(std::is_unsigned<ValueType>::value && std::is_unsigned<DeltaType>::value, "the samples are passed in unsigned types");
        if (!preprocessing_.isDefault() && !std::is_same<DecodeFunc, Identity<DeltaType>>::value) {
            throw std::invalid_argument("CCSDS: this preprocessing needs the identity sample function");
        }
        noCompressionId_ = (1u << (maxIdLength - 1)) - 1;
    }

protected:
    const uint8_t bpv_;
    const uint8_t blockSizeV_;

    constexpr static size_t maxSegmentSizeB_ = 64;
    const size_t maxSegmentSizeV_ = maxSegmentSizeB_ * blockSizeV_;
    const size_t rsiSizeB_;
    const size_t rsiSizeV_;

    const CodeOptionSet codeOptionSet_;
    PredictivePreprocessingDecodeFast<ValueType, DeltaType, DecodeFunc> preprocessor_;

    CodeOptionBase codeOptionBase_{ bpv_, codeOptionSet_ };
    const uint8_t maxIdLength = codeOptionBase_.maxIdLength;

    // The largest preprocessed sample, 2^n - 1 (CCSDS 121.0-B-3, 4.4: they are n-bit values)
    const uint64_t maxSample_ = (uint64_t{ 1 } << bpv_) - 1;

    IdType noCompressionId_;

    mutable std::vector<ValueType> rsiBuf_;
    mutable std::vector<ValueType> paddedSegmentBuf_;
    std::vector<ValueType> valuesBuf_;
    mutable std::vector<uint8_t> paddedSource_;
    size_t bitsDecoded_ = 0;

protected:
    const Preprocessing preprocessing_;
};

}  // namespace spaec
