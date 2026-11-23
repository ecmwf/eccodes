#pragma once

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <numeric>
#include <vector>

#include "ccsds/encoder/EncoderBase.h"
#include "ccsds/CodeOption.h"
#include "ccsds/BitBuffer.h"
#include "ccsds/Preprocessor.h"
#include "ccsds/Span.h"
#include "ccsds/Utils.h"

namespace encoder
{

template <typename ValueType>
struct SETmpState
{
    ValueType reference_;
    std::vector<uint64_t> encodedValues_;
};


template <typename ValueType, typename DeltaType, typename EncoderFunc = Identity<DeltaType>>
class EncoderSeq : EncoderBase<ValueType, DeltaType>
{
public:
    // No Compression
    void ncEncode(const Span<DeltaType> values, BitBuffer& buffer)
    {
        buffer.writeId(ncId_.valueShifted, ncId_.length);
        for (size_t i = 0; i < blockSizeV_; ++i) {
            buffer.writeBits(values[i], bpv_);
        }
    }

    // Second Extension
    template <ReferenceType RefType>
    size_t sePrepare(const Span<DeltaType> values)
    {
        assert(values.size() % 2 == 0 && "Deltas size must be even for second extension encoding");

        if constexpr (RefType == ReferenceType::Ref) {
            seState_.reference_ = values[0];

            const uint64_t d1 = 0;
            const uint64_t d2 = values[1];
            seState_.encodedValues_[0] = (d1 + d2) * (d1 + d2 + 1) / 2 + d2;
        }

        size_t startIndex = (RefType == ReferenceType::Ref) ? 2 : 0;

        for (size_t i = startIndex; i < values.size(); i += 2) {
            const uint64_t d1 = values[i];
            const uint64_t d2 = values[i + 1];
            seState_.encodedValues_[i / 2] = (d1 + d2) * (d1 + d2 + 1) / 2 + d2;
        }

        size_t size = seId_.length + std::accumulate(seState_.encodedValues_.begin(), seState_.encodedValues_.end(), size_t(0), [](size_t sum, uint64_t v) {
                          return sum + static_cast<size_t>(v) + 1;
                      });

        if constexpr (RefType == ReferenceType::Ref) {
            size += bpv_;
        }

        return size;
    }

    template <ReferenceType RefType>
    void seEncode(BitBuffer& buffer)
    {
        buffer.writeId(seId_.valueShifted, seId_.length);
        if constexpr (RefType == ReferenceType::Ref) {
            buffer.writeBits(seState_.reference_, bpv_);
        }

        for (const auto& value : seState_.encodedValues_) {
            buffer.writeCodeword(value);
        }
    }

    // Split Sample
    template <ReferenceType RType>
    void ssEncode(const IdType k, const Span<DeltaType> values, BitBuffer& buffer)
    {
        const auto& ssId_ = ssIdStore[k];
        const IdType valueShifted = ssId_.valueShifted;

        buffer.writeId(valueShifted, ssId_.length);

        if constexpr (RType == ReferenceType::Ref) {
            buffer.writeBits(values[0], bpv_);
        }

        constexpr size_t startIndex = (RType == ReferenceType::Ref) ? 1 : 0;

        for (size_t i = startIndex; i < blockSizeV_; ++i) {
            buffer.writeCodeword(values[i] >> k);
        }

        buffer.writeBitsBatch(values.data() + startIndex, blockSizeV_ - startIndex, k);
    }

    // Zero Block
    template <ReferenceType RType>
    void zbEncode(BitBuffer& buffer, DeltaType reference, size_t nBlocks)
    {
        assert(nBlocks > 0 && "Number of zero blocks must be greater than 0.");

        size_t cwLength = nBlocks;
        if (nBlocks >= rosLength) {
            cwLength++;
        }

        buffer.writeId(zbId_.valueShifted, zbId_.length);
        if constexpr (RType == ReferenceType::Ref) {
            buffer.writeBits(reference, bpv_);
        }
        buffer.writeCodeword(cwLength - 1);
    }

    // Zero blocks up to the end of a segment or RSI: like libaec (and CCSDS 121.0-B), use the
    // "remainder of segment" codeword only for runs of at least rosLength blocks
    template <ReferenceType RType>
    void zbEncodeROS(BitBuffer& buffer, DeltaType reference, size_t nBlocks)
    {
        if (nBlocks < rosLength) {
            zbEncode<RType>(buffer, reference, nBlocks);
            return;
        }
        buffer.writeId(zbId_.valueShifted, zbId_.length);
        if constexpr (RType == ReferenceType::Ref) {
            buffer.writeBits(reference, bpv_);
        }
        buffer.writeCodeword(rosLength - 1);
    }


    // ---------------------------------------------------


    // BLOCK
    template <ReferenceType RType>
    void encodeFullBlock(const Span<DeltaType> block, BitBuffer& bitBuffer)
    {
        assert(block.size() == blockSizeV_ && "Block size does not match encoder block size.");

        const auto [ssK, ssSize, blockSum] = this->template selectBestMethod<RType>(block);
        size_t ncSize = ncId_.length + blockSizeV_ * bpv_;

        // SE lower bound: each pair costs >= max(d1+d2, 1) bits.
        // Total SE cost >= seId_.length + nPairs + blockSum [+ bpv_ for Ref].
        // Skip expensive sePrepare when this lower bound exceeds cheaper option.
        constexpr size_t startIndex = (RType == ReferenceType::Ref) ? 1 : 0;
        const size_t nPairs = (blockSizeV_ - startIndex) / 2;
        size_t seLowerBound = seId_.length + nPairs + blockSum;
        if constexpr (RType == ReferenceType::Ref) {
            seLowerBound += bpv_;
        }

        // SE wins some ties (see below), so it must be evaluated when it can be equal
        size_t cheapest = std::min(ssSize, ncSize);
        size_t seSize;
        if (seLowerBound <= cheapest) {
            seSize = sePrepare<RType>(block);
        }
        else {
            seSize = cheapest + 1;  // SE cannot win
        }

        // Selection and tie-breaking of libaec (m_select_code_option), for identical streams:
        // split sample if strictly smaller than both other options, otherwise SE or NC (NC wins
        // a tie with SE)
        if (ssSize < ncSize) {
            if (ssSize < seSize) {
                ssEncode<RType>(ssK, block, bitBuffer);
            }
            else {
                seEncode<RType>(bitBuffer);
            }
        }
        else if (ncSize <= seSize) {
            ncEncode(block, bitBuffer);
        }
        else {
            seEncode<RType>(bitBuffer);
        }
    }

    template <ReferenceType RType>
    void encodePartBlock(const Span<DeltaType> partBlock, BitBuffer& buffer)
    {
        std::vector<DeltaType>& fullBlock = partBlockBuf_;  // not static: encoders may run concurrently
        fullBlock.assign(blockSizeV_, 0);
        std::copy(partBlock.begin(), partBlock.end(), fullBlock.begin());
        encodeFullBlock<RType>(Span<DeltaType>(fullBlock.data(), blockSizeV_), buffer);
    }


    // SEGMENT
    static inline auto isZeroBlock = [](const Span<DeltaType> block) {
        return std::all_of(block.begin(), block.end(), [](DeltaType v) { return v == 0; });
    };


    void encodeRefFullSegment(const Span<DeltaType> segment, BitBuffer& buffer)
    {
        DeltaType reference = segment[0];

        size_t nZeroBlocks = 0;
        SplitBlock<DeltaType> segmentBlocks(segment, blockSizeV_);
        bool isRefZeroBlock = false;

        auto handleZeroBlocks = [&]() {
            if (nZeroBlocks != 0) {
                if (isRefZeroBlock == 1) {
                    zbEncode<ReferenceType::Ref>(buffer, reference, nZeroBlocks);
                    isRefZeroBlock = false;
                }
                else if (isRefZeroBlock == 0) {
                    zbEncode<ReferenceType::NoRef>(buffer, 0, nZeroBlocks);
                }
                nZeroBlocks = 0;
            }
        };

        // REF: the reference sample is not part of the zero test (as in libaec, where the first
        // preprocessed sample of an RSI is 0)
        if (isZeroBlock(segmentBlocks.full(0).drop(1))) {
            nZeroBlocks++;
            isRefZeroBlock = true;
        }
        else {
            handleZeroBlocks();
            encodeFullBlock<ReferenceType::Ref>(segmentBlocks.full(0), buffer);
        }

        // ALT
        for (size_t i = 1; i < segmentBlocks.nFull(); ++i) {
            const auto block = segmentBlocks.full(i);
            if (isZeroBlock(block)) {
                nZeroBlocks++;
            }
            else {
                handleZeroBlocks();
                encodeFullBlock<ReferenceType::NoRef>(block, buffer);
            }
        }

        // ROS
        if (nZeroBlocks != 0) {
            if (isRefZeroBlock) {
                zbEncodeROS<ReferenceType::Ref>(buffer, reference, nZeroBlocks);
            }
            else {
                zbEncodeROS<ReferenceType::NoRef>(buffer, 0, nZeroBlocks);
            }
        }
    }

    void encodeRefPartSegment(const Span<DeltaType> segment, BitBuffer& buffer)
    {
        DeltaType reference = segment[0];
        SplitBlock<DeltaType> segmentBlocks(segment, blockSizeV_);
        size_t nZeroBlocks = 0;
        bool isRefZeroBlock = false;

        auto handleZeroBlocks = [&]() {
            if (nZeroBlocks != 0) {
                if (isRefZeroBlock == 1) {
                    zbEncode<ReferenceType::Ref>(buffer, reference, nZeroBlocks);
                    isRefZeroBlock = false;
                }
                else if (isRefZeroBlock == 0) {
                    zbEncode<ReferenceType::NoRef>(buffer, 0, nZeroBlocks);
                }
                nZeroBlocks = 0;
            }
        };


        if (segmentBlocks.nFull() == 0 && segmentBlocks.nPart() == 1) {
            // REF PART
            auto block = segmentBlocks.part();
            if (isZeroBlock(block.drop(1))) {
                nZeroBlocks++;
                isRefZeroBlock = true;
            }
            else {
                handleZeroBlocks();
                encodePartBlock<ReferenceType::Ref>(block, buffer);
            }
        }
        else {
            // REF
            auto block = segmentBlocks.full(0);
            if (isZeroBlock(block.drop(1))) {
                nZeroBlocks++;
                isRefZeroBlock = true;
            }
            else {
                handleZeroBlocks();
                encodeFullBlock<ReferenceType::Ref>(block, buffer);
            }

            // ALT
            for (size_t i = 1; i < segmentBlocks.nFull(); ++i) {
                const auto altBlock = segmentBlocks.full(i);
                if (isZeroBlock(altBlock)) {
                    nZeroBlocks++;
                }
                else {
                    handleZeroBlocks();
                    encodeFullBlock<ReferenceType::NoRef>(altBlock, buffer);
                }
            }

            // ALT PART
            if (segmentBlocks.hasPart()) {
                const auto partBlock = segmentBlocks.part();
                if (isZeroBlock(partBlock)) {
                    nZeroBlocks++;
                }
                else {
                    handleZeroBlocks();
                    encodePartBlock<ReferenceType::NoRef>(partBlock, buffer);
                }
            }
        }

        // ROS
        if (nZeroBlocks != 0) {
            if (isRefZeroBlock) {
                zbEncodeROS<ReferenceType::Ref>(buffer, reference, nZeroBlocks);
            }
            else {
                zbEncodeROS<ReferenceType::NoRef>(buffer, 0, nZeroBlocks);
            }
        }
    }

    // Alternative segments carry no reference: all blocks are NoRef, a trailing partial
    // block is simply absent for full segments.
    void encodeAltSegment(const Span<DeltaType> segment, BitBuffer& buffer)
    {
        SplitBlock<DeltaType> segmentBlocks(segment, blockSizeV_);
        size_t nZeroBlocks = 0;

        auto handleZeroBlocks = [&]() {
            if (nZeroBlocks != 0) {
                zbEncode<ReferenceType::NoRef>(buffer, 0, nZeroBlocks);
                nZeroBlocks = 0;
            }
        };

        for (size_t i = 0; i < segmentBlocks.nFull(); ++i) {
            auto block = segmentBlocks.full(i);
            if (isZeroBlock(block)) {
                nZeroBlocks++;
            }
            else {
                handleZeroBlocks();
                encodeFullBlock<ReferenceType::NoRef>(block, buffer);
            }
        }

        if (segmentBlocks.hasPart()) {
            auto block = segmentBlocks.part();
            if (isZeroBlock(block)) {
                nZeroBlocks++;
            }
            else {
                handleZeroBlocks();
                encodePartBlock<ReferenceType::NoRef>(block, buffer);
            }
        }

        // ROS
        if (nZeroBlocks != 0) {
            zbEncodeROS<ReferenceType::NoRef>(buffer, 0, nZeroBlocks);
        }
    }


    // RSI: preprocess the values and encode their segments (the first segment carries the
    // reference, the rest are alternatives). A trailing partial RSI is encoded the same way.
    void encodeRsi(const Span<ValueType> data, BitBuffer& buffer)
    {
        preprocessor_.encode(data, Span<DeltaType>(preprocessBuffer_.data(), data.size()));

        SplitBlock<DeltaType> segmentBlocks(Span<DeltaType>(preprocessBuffer_.data(), data.size()), segmentSizeV_);
        if (segmentBlocks.nFull() == 0 && segmentBlocks.nPart() == 1) {
            encodeRefPartSegment(segmentBlocks.part(), buffer);
        }
        else {
            encodeRefFullSegment(segmentBlocks.full(0), buffer);
            for (size_t i = 1; i < segmentBlocks.nFull(); ++i) {
                encodeAltSegment(segmentBlocks.full(i), buffer);
            }
            if (segmentBlocks.hasPart()) {
                encodeAltSegment(segmentBlocks.part(), buffer);
            }
        }
    }

public:
    // Encodes source into the zeroed target and returns the number of bytes written
    size_t encode(const Span<ValueType> source, Span<uint8_t> target)
    {
        // Like libaec, an empty stream is one zero byte (the flushed, empty last byte)
        return std::max<size_t>((encodeBits(source, target) + 7) / 8, 1);
    }

    // Encodes source into the zeroed target and returns the exact number of bits written
    size_t encodeBits(const Span<ValueType> source, Span<uint8_t> target)
    {
        SplitBlock<ValueType> rsiBlocks(source, rsiSizeV_);
        BitBuffer buffer(target);

        for (size_t i = 0; i < rsiBlocks.nFull(); ++i) {
            encodeRsi(rsiBlocks.full(i), buffer);
        }
        if (rsiBlocks.hasPart()) {
            encodeRsi(rsiBlocks.part(), buffer);
        }

        return buffer.bitPos();
    }

    EncoderSeq(uint8_t bpv, uint8_t block_size, uint16_t rsiSize, CodeOptionSet codeOptionSet) : EncoderBase<ValueType, DeltaType>(bpv, block_size, rsiSize, codeOptionSet),
                                                                                                 preprocessor_(bpv),
                                                                                                 preprocessBuffer_(rsiSize * block_size)
    {
        seState_.encodedValues_.resize(blockSizeV_ / 2);
    }


private:
    SETmpState<DeltaType> seState_;

    using Base = EncoderBase<ValueType, DeltaType>;
    using Base::blockSizeV_;
    using Base::bpv_;
    using Base::ncId_;
    using Base::rsiSizeV_;
    using Base::segmentSizeV_;
    using Base::seId_;
    using Base::ssIdStore;
    using Base::zbId_;

    PredictivePreprocessingEncodeFast<ValueType, DeltaType, EncoderFunc> preprocessor_;
    std::vector<DeltaType> preprocessBuffer_;
    std::vector<DeltaType> partBlockBuf_;
};

}  // namespace encoder
