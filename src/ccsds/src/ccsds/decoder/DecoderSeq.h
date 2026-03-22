#pragma once

#include <vector>
#include <cstring>
#include <assert.h>
#include <array>

#include "Span.h"
#include "CodeOption.h"
#include "Preprocessor.h"
#include "BitBuffer.h"
#include "Utils.h"
#include "Stats.h"

template <typename ValueType, typename DeltaType, typename DecodeFunc = Identity<DeltaType>>
class DecoderSeq {
    private:
        static uint64_t loadU64(const uint8_t* ptr) {
            uint64_t val;
            std::memcpy(&val, ptr, sizeof(uint64_t));
            return val;
        }

    public:
        // NC - batched readBits using accumulator
        template <ReferenceType RType>
        void ncDecode(const BitBuffer& buffer, Span<ValueType> values) {
            if constexpr (RType == ReferenceType::Ref) {
                values[0] = 0;
            }
            constexpr uint8_t startIndex = (RType == ReferenceType::Ref) ? 1 : 0;
            const uint8_t* data = buffer.getBuffer().data();
            const uint8_t nBits = bpv_;

            size_t pos = buffer.bitPos();
            size_t bytepos = pos >> 3;
            size_t bitpos = pos & 7;
            uint64_t acc = bswap_64(loadU64(&data[bytepos]));
            acc <<= bitpos;
            int bitsAvail = 64 - static_cast<int>(bitpos);

            for (size_t i = startIndex; i < blockSizeV_; ++i) {
                if (__builtin_expect(bitsAvail < nBits, 0)) {
                    bytepos = pos >> 3;
                    bitpos = pos & 7;
                    acc = bswap_64(loadU64(&data[bytepos]));
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
            const uint8_t* data = buffer.getBuffer().data();

            size_t pos = buffer.bitPos();
            size_t bytepos = pos >> 3;
            size_t bitpos = pos & 7;
            uint64_t acc = bswap_64(loadU64(&data[bytepos]));
            acc <<= bitpos;
            size_t consumed = bitpos;

            for (size_t i = 0; i < blockSizeV_ / 2; ++i) {
                // Batched codeword read
                size_t cw_start = pos;
                while (__builtin_expect(acc == 0, 0)) {
                    pos += (64 - consumed);
                    bytepos = pos >> 3;
                    bitpos = pos & 7;
                    acc = bswap_64(loadU64(&data[bytepos]));
                    acc <<= bitpos;
                    consumed = bitpos;
                }
                size_t lz = static_cast<size_t>(__builtin_clzll(acc));
                size_t total_zeros = (pos - cw_start) + lz;
                const ValueType tmp = static_cast<ValueType>(total_zeros);
                pos += lz + 1;
                consumed += lz + 1;
                if (__builtin_expect(consumed >= 64, 0)) {
                    bytepos = pos >> 3;
                    bitpos = pos & 7;
                    acc = bswap_64(loadU64(&data[bytepos]));
                    acc <<= bitpos;
                    consumed = bitpos;
                } else {
                    acc <<= (lz + 1);
                }

                // Fast integer sqrt: w = floor((sqrt(8*tmp+1) - 1) / 2)
                const uint32_t val = 8u * static_cast<uint32_t>(tmp) + 1;
                uint32_t s = static_cast<uint32_t>(__builtin_sqrtf(static_cast<float>(val)));
                s -= (s * s > val);  // adjust down if needed
                const ValueType w = static_cast<ValueType>((s - 1) >> 1);
                const ValueType t = w * (w + 1) / 2;
                const ValueType d2 = tmp - t;
                const ValueType d1 = w - d2;
                values[2 * i] = d1;
                values[2 * i + 1] = d2;
            }
            buffer.bitPos(pos);
        }

        // SS
        template <ReferenceType RType>
        void ssDecode(const BitBuffer& buffer, uint8_t k, Span<ValueType> values) const {
            if constexpr (RType == ReferenceType::Ref) {
                values[0] = 0;
            }

            constexpr size_t startIndex = (RType == ReferenceType::Ref) ? 1 : 0;
            const uint8_t* data = buffer.getBuffer().data();

            if (k != 0) {
                // Phase 1: Read codewords using batched accumulator
                {
                    size_t pos = buffer.bitPos();
                    size_t bytepos = pos >> 3;
                    size_t bitpos = pos & 7;
                    uint64_t acc = bswap_64(loadU64(&data[bytepos]));
                    acc <<= bitpos;
                    size_t consumed = bitpos;

                    for (size_t i = startIndex; i < blockSizeV_; ++i) {
                        size_t cw_start = pos;
                        while (__builtin_expect(acc == 0, 0)) {
                            pos += (64 - consumed);
                            bytepos = pos >> 3;
                            bitpos = pos & 7;
                            acc = bswap_64(loadU64(&data[bytepos]));
                            acc <<= bitpos;
                            consumed = bitpos;
                        }
                        size_t lz = static_cast<size_t>(__builtin_clzll(acc));
                        size_t total_zeros = (pos - cw_start) + lz;
                        values[i] = static_cast<ValueType>(total_zeros) << k;
                        pos += lz + 1;
                        consumed += lz + 1;
                        if (__builtin_expect(consumed >= 64, 0)) {
                            bytepos = pos >> 3;
                            bitpos = pos & 7;
                            acc = bswap_64(loadU64(&data[bytepos]));
                            acc <<= bitpos;
                            consumed = bitpos;
                        } else {
                            acc <<= (lz + 1);
                        }
                    }
                    buffer.bitPos(pos);
                }

                // Phase 2: Read k-bit LSBs using batched accumulator
                {
                    size_t pos = buffer.bitPos();
                    size_t bytepos = pos >> 3;
                    size_t bitpos = pos & 7;
                    uint64_t acc = bswap_64(loadU64(&data[bytepos]));
                    acc <<= bitpos;
                    int bitsAvail = 64 - static_cast<int>(bitpos);

                    for (size_t i = startIndex; i < blockSizeV_; ++i) {
                        if (__builtin_expect(bitsAvail < k, 0)) {
                            bytepos = pos >> 3;
                            bitpos = pos & 7;
                            acc = bswap_64(loadU64(&data[bytepos]));
                            acc <<= bitpos;
                            bitsAvail = 64 - static_cast<int>(bitpos);
                        }
                        values[i] |= static_cast<ValueType>(acc >> (64 - k));
                        acc <<= k;
                        bitsAvail -= k;
                        pos += k;
                    }
                    buffer.bitPos(pos);
                }
            }
            else { // FS - batched codeword reading
                size_t pos = buffer.bitPos();
                size_t bytepos = pos >> 3;
                size_t bitpos = pos & 7;
                uint64_t acc = bswap_64(loadU64(&data[bytepos]));
                acc <<= bitpos;
                size_t consumed = bitpos;

                for (size_t i = startIndex; i < blockSizeV_; ++i) {
                    size_t cw_start = pos;
                    while (__builtin_expect(acc == 0, 0)) {
                        pos += (64 - consumed);
                        bytepos = pos >> 3;
                        bitpos = pos & 7;
                        acc = bswap_64(loadU64(&data[bytepos]));
                        acc <<= bitpos;
                        consumed = bitpos;
                    }
                    size_t lz = static_cast<size_t>(__builtin_clzll(acc));
                    size_t total_zeros = (pos - cw_start) + lz;
                    values[i] = static_cast<ValueType>(total_zeros);
                    pos += lz + 1;
                    consumed += lz + 1;
                    if (__builtin_expect(consumed >= 64, 0)) {
                        bytepos = pos >> 3;
                        bitpos = pos & 7;
                        acc = bswap_64(loadU64(&data[bytepos]));
                        acc <<= bitpos;
                        consumed = bitpos;
                    } else {
                        acc <<= (lz + 1);
                    }
                }
                buffer.bitPos(pos);
            }
        }

        // ZB
        template <ReferenceType RType>
        size_t zbDecode(const BitBuffer& buffer, Span<ValueType> segment, size_t blockOffset) {
            size_t length = buffer.readCodeword();
            assert(segment.size() % blockSizeV_ == 0 && "Segment size must be multiple of block size.");
            size_t blocksWritten = 0;

            if (length == rosLength) { // All zeros segment
                size_t start = blockOffset * blockSizeV_;
                std::memset(&segment[start], 0, (segment.size() - start) * sizeof(ValueType));
                blocksWritten = segment.size() / blockSizeV_ - blockOffset;
                if constexpr (RType == ReferenceType::Ref) {
                    stats_.counter.zbRosRef[blocksWritten]++;
                }
                else {
                    stats_.counter.zbRosNoRef[blocksWritten]++;
                }

            }
            else { // First n blocks are zeros
                if (length > rosLength) {
                    length--;
                }

                blocksWritten = length;

                size_t start = blockOffset * blockSizeV_;
                std::memset(&segment[start], 0, blocksWritten * blockSizeV_ * sizeof(ValueType));

                if constexpr (RType == ReferenceType::Ref) {
                    stats_.counter.zbRef[blocksWritten]++;
                }
                else {
                    stats_.counter.zbNoRef[blocksWritten]++;
                }
            }
            return blocksWritten;
        }


        // BLOCK

        ValueType handleRefBlock(const BitBuffer& buffer, size_t id, Span<ValueType> block) {
            ValueType reference = 0;
            if (id == 1)
            {
                buffer.skipBits(maxIdLength);
                reference = buffer.readBits<ValueType>(bpv_);
                seDecode(buffer, block);
                stats_.counter.seRef++;
            }
            else {
                id >>= 1;
                buffer.skipBits(maxIdLength - 1);
                reference = buffer.readBits<ValueType>(bpv_);

                if (id == noCompressionId_) {
                    ncDecode<ReferenceType::Ref>(buffer, block);
                    stats_.counter.ncRef++;
                }
                else {
                    ssDecode<ReferenceType::Ref>(buffer, id - 1, block);
                    stats_.counter.ssRef[id - 1]++;
                }
            }
            return reference;
        };


        void handleBlock(const BitBuffer& buffer, size_t id, Span<ValueType> block) {
            if (id == 1)
            {
                buffer.skipBits(maxIdLength);
                seDecode(buffer, block);
                stats_.counter.seNoRef++;
            }
            else {
                id >>= 1;
                buffer.skipBits(maxIdLength - 1);

                if (id == noCompressionId_) {
                    ncDecode<ReferenceType::NoRef>(buffer, block);
                    stats_.counter.ncNoRef++;
                }
                else {
                    ssDecode<ReferenceType::NoRef>(buffer, id - 1, block);
                    stats_.counter.ssNoRef[id - 1]++;
                }
            }
        };


        // SEGMENT
        ValueType decodeRefFullSegment(const BitBuffer& buffer, Span<ValueType> segment)
        {
            assert(segment.size() % blockSizeV_ == 0 && "Segment size must be multiple of block size.");

            SplitBlock<ValueType> blocks(segment, blockSizeV_);
            size_t pos = 0;

            // First Block containing a reference
            auto id = buffer.peekBits(maxIdLength);
            ValueType reference = 0;

            if (id == 0) {
                buffer.skipBits(maxIdLength);
                reference = buffer.template readBits<ValueType>(bpv_);
                pos += zbDecode<ReferenceType::Ref>(buffer, segment, pos);
            }
            else {
                reference = handleRefBlock(buffer, id, blocks.full(0));
                pos++;
            }

            // Remaining Blocks
            for (; pos < blocks.nFull();) {
                auto id = buffer.peekBits(maxIdLength);
                if (id == 0) {
                    buffer.skipBits(maxIdLength);
                    pos += zbDecode<ReferenceType::NoRef>(buffer, segment, pos);
                }
                else {
                    handleBlock(buffer, id, blocks.full(pos));
                    pos++;
                }
                assert(pos <= segment.size() / blockSizeV_ && "Decoded blocks exceed segment size.");
            }

            assert(pos == segment.size() / blockSizeV_ && "Decoded blocks do not match segment size.");

            return reference;
        }

        void decodeAltFullSegment(const BitBuffer& buffer, Span<ValueType> segment)
        {
            assert(segment.size() % blockSizeV_ == 0 && "Segment size must be multiple of block size.");
            SplitBlock<ValueType> blocks(segment, blockSizeV_);

            // All Blocks
            for (size_t pos = 0; pos < blocks.nFull();) {
                auto id = buffer.peekBits(maxIdLength);
                if (id == 0) {
                    assert(pos < segment.size() / blockSizeV_ && "Position exceeds number of blocks in segment.");

                    buffer.skipBits(maxIdLength);
                    pos += zbDecode<ReferenceType::NoRef>(buffer, segment, pos);
                }
                else {
                    handleBlock(buffer, id, blocks.full(pos));
                    pos++;
                }

                assert(pos <= segment.size() / blockSizeV_ && "Decoded blocks exceed segment size.");
            }
        }

        ValueType decodeRefPartSegment(const BitBuffer& buffer, Span<ValueType> segment)
        {
            size_t newSize = (segment.size() + blockSizeV_ - 1) / blockSizeV_ * blockSizeV_;
            paddedSegmentBuf_.resize(newSize);
            std::memset(paddedSegmentBuf_.data(), 0, newSize * sizeof(ValueType));
            Span<ValueType> paddedSpan{paddedSegmentBuf_.data(), paddedSegmentBuf_.size()};
            ValueType reference = decodeRefFullSegment(buffer, paddedSpan);
            for (size_t i = 0; i < segment.size(); ++i) {
                segment[i] = paddedSegmentBuf_[i];
            }
            return reference;
        }

        void decodeAltPartSegment(const BitBuffer& buffer, Span<ValueType> segment)
        {
            size_t newSize = (segment.size() + blockSizeV_ - 1) / blockSizeV_ * blockSizeV_;
            paddedSegmentBuf_.resize(newSize);
            std::memset(paddedSegmentBuf_.data(), 0, newSize * sizeof(ValueType));
            Span<ValueType> paddedSpan{paddedSegmentBuf_.data(), paddedSegmentBuf_.size()};
            decodeAltFullSegment(buffer, paddedSpan);
            for (size_t i = 0; i < segment.size(); ++i) {
                segment[i] = paddedSegmentBuf_[i];
            }
        }

        // RSI
        ValueType decodeRsiRaw(const BitBuffer& buffer, size_t rsiSize)
        {
            rsiBuf_.resize(rsiSize);
            Span<ValueType> rsiBufSpan(rsiBuf_);

            SplitBlock<ValueType> segments(rsiBufSpan, maxSegmentSizeV_);
            ValueType reference = 0;

            if (rsiSize < maxSegmentSizeV_) {
                reference = decodeRefPartSegment(buffer, segments.part());
            }
            else {
                reference = decodeRefFullSegment(buffer, segments.full(0));

                for (size_t i = 1; i < segments.nFull(); ++i) {
                    decodeAltFullSegment(buffer, segments.full(i));
                }

                if (segments.hasPart()) {
                    decodeAltPartSegment(buffer, segments.part());
                }
            }

            return reference;
        }

        void decodeRefFullRsi(const BitBuffer& buffer, Span<ValueType> rsi)
        {
            ValueType reference = decodeRsiRaw(buffer, rsi.size());
            preprocessor_.decode(Span<DeltaType>(rsiBuf_), reference, rsi);
        }

        // Skip one full RSI in the bitstream without decoding (for offset scanning)
        void skipFullRsi(const BitBuffer& buffer) const {
            const size_t nFullSegments = rsiSizeV_ / maxSegmentSizeV_;

            auto skipOneBlock = [&](bool withRef) -> std::pair<bool, size_t> {
                auto id = buffer.peekBits(maxIdLength);
                bool isRos = false;
                size_t nBlocks = 1;

                if (id == 0) {
                    buffer.skipBits(maxIdLength);
                    if (withRef) buffer.skipBits(bpv_);
                    nBlocks = buffer.readCodeword();
                    if (nBlocks == rosLength) {
                        isRos = true;
                    } else if (nBlocks > rosLength) {
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
                    } else {
                        if (withRef) buffer.skipBits(bpv_);
                        uint8_t k = static_cast<uint8_t>(id - 1);
                        size_t startIndex = withRef ? 1 : 0;
                        buffer.skipCodewords(blockSizeV_ - startIndex);
                        buffer.skipBits(k * (blockSizeV_ - startIndex));
                    }
                }
                return {isRos, nBlocks};
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
                skipSegment(true, rsiSizeB_);
            } else {
                skipSegment(true, maxSegmentSizeB_);
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
            const BitBuffer buffer{source};
            SplitBlock<ValueType> chunks(target, rsiSizeV_);
            for (size_t i = 0; i < chunks.nFull(); ++i) {
                decodeRefFullRsi(buffer, chunks.full(i));
            }
            if (chunks.hasPart()) {
                decodeRefFullRsi(buffer, chunks.part());
            }

            setStats(target.size(), source.size());
        }

        // Per-RSI decode with callback for fused post-processing (avoids large temp buffers)
        template <typename PostProcess>
        void decode(Span<uint8_t> source, size_t nValues, PostProcess&& postProcess)
        {
            const BitBuffer buffer{source};

            size_t nFull = nValues / rsiSizeV_;
            size_t remainder = nValues % rsiSizeV_;

            std::vector<ValueType> perRsiBuf(rsiSizeV_);

            for (size_t i = 0; i < nFull; ++i) {
                Span<ValueType> rsiSpan(perRsiBuf.data(), rsiSizeV_);
                decodeRefFullRsi(buffer, rsiSpan);
                postProcess(rsiSpan, i * rsiSizeV_);
            }

            if (remainder > 0) {
                Span<ValueType> rsiSpan(perRsiBuf.data(), remainder);
                decodeRefFullRsi(buffer, rsiSpan);
                postProcess(rsiSpan, nFull * rsiSizeV_);
            }

            setStats(nValues, source.size());
        }

        // Fused decode: preprocessor + output callback in one pass (no intermediate buffer)
        template <typename OutputFunc>
        void decodeFused(Span<uint8_t> source, size_t nValues, OutputFunc&& outputFunc)
        {
            const BitBuffer buffer{source};

            size_t nFull = nValues / rsiSizeV_;
            size_t remainder = nValues % rsiSizeV_;

            for (size_t i = 0; i < nFull; ++i) {
                ValueType reference = decodeRsiRaw(buffer, rsiSizeV_);
                preprocessor_.decodeFused(
                    rsiBuf_.data(), rsiSizeV_,
                    reference, i * rsiSizeV_, outputFunc);
            }

            if (remainder > 0) {
                ValueType reference = decodeRsiRaw(buffer, remainder);
                preprocessor_.decodeFused(
                    rsiBuf_.data(), remainder,
                    reference, nFull * rsiSizeV_, outputFunc);
            }

            setStats(nValues, source.size());
        }

        // Fused decode starting from a specific bit offset (for parallel decoding)
        template <typename OutputFunc>
        void decodeFused(Span<uint8_t> source, size_t bitOffset, size_t nValues, OutputFunc&& outputFunc)
        {
            const BitBuffer buffer{source};
            buffer.bitPos(bitOffset);

            size_t nFull = nValues / rsiSizeV_;
            size_t remainder = nValues % rsiSizeV_;

            for (size_t i = 0; i < nFull; ++i) {
                ValueType reference = decodeRsiRaw(buffer, rsiSizeV_);
                preprocessor_.decodeFused(
                    rsiBuf_.data(), rsiSizeV_,
                    reference, i * rsiSizeV_, outputFunc);
            }

            if (remainder > 0) {
                ValueType reference = decodeRsiRaw(buffer, remainder);
                preprocessor_.decodeFused(
                    rsiBuf_.data(), remainder,
                    reference, nFull * rsiSizeV_, outputFunc);
            }

            setStats(nValues, source.size());
        }

        template <typename... Args>
        DecoderSeq(uint8_t bpv, uint8_t block_size, uint16_t rsiSize, CodeOptionSet codeOptionSet, Args&&... args) :
            bpv_(bpv),
            blockSizeV_(block_size),
            rsiSizeB_(rsiSize),
            rsiSizeV_(rsiSize * block_size),
            codeOptionSet_(codeOptionSet),
            preprocessor_(bpv, std::forward<Args>(args)...)
        {

            noCompressionId_ = (1u << (maxIdLength - 1)) - 1;
        }

        SpAecStats getStats() const {
            return stats_;
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

        CodeOptionBase codeOptionBase_{bpv_, codeOptionSet_};
        const uint8_t maxIdLength = codeOptionBase_.maxIdLength;

        IdType noCompressionId_;

        mutable std::vector<ValueType> rsiBuf_;
        mutable std::vector<ValueType> paddedSegmentBuf_;

        SpAecStats stats_;

        void setStats(size_t nValues, size_t compressedSize) {
            stats_.nBlocks = (nValues + blockSizeV_ - 1) / blockSizeV_;
            stats_.nValues = nValues;
            stats_.bpv = bpv_;
            stats_.blockSize = blockSizeV_;
            stats_.rsiSize = rsiSizeB_;
            stats_.codeOptionSet = codeOptionSet_;
            stats_.compressedSize = compressedSize;
        }
};
