/*
 * SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "spaec/detail/Isa.h"

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <numeric>
#include <stdexcept>
#include <type_traits>
#include <vector>

#include "spaec/detail/encoder/EncoderBase.h"
#include "spaec/detail/CodeOption.h"
#include "spaec/detail/BitBuffer.h"
#include "spaec/detail/Preprocessor.h"
#include "spaec/detail/Span.h"
#include "spaec/detail/Utils.h"

namespace spaec::detail
{
inline namespace SPAEC_ISA
{

namespace encoder
{

template <typename ValueType>
struct SETmpState
{
    ValueType reference_;
    std::vector<uint64_t> encodedValues_;
};


template <typename ValueType, typename DeltaType, typename EncoderFunc = Identity<DeltaType>, SplitSelector Selector = SplitSelector::Search>
class EncoderSeq : EncoderBase<ValueType, DeltaType, Selector>
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
    // Sizes in bits are computed in uint64_t: the codewords of large samples exceed 32 bits
    template <ReferenceType RefType>
    uint64_t sePrepare(const Span<DeltaType> values)
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

        uint64_t size = seId_.length + std::accumulate(seState_.encodedValues_.begin(), seState_.encodedValues_.end(), uint64_t(0), [](uint64_t sum, uint64_t v) {
                            return sum + v + 1;
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

        buffer.writeCodewords(seState_.encodedValues_.data(), seState_.encodedValues_.size());
    }

    // Split Sample
    template <ReferenceType RType>
    void ssEncode(const IdType k, const Span<DeltaType> values, BitBuffer& buffer, uint64_t size)
    {
        const auto& ssId_ = ssIdStore[k];
        const IdType valueShifted = ssId_.valueShifted;

        buffer.writeId(valueShifted, ssId_.length);

        if constexpr (RType == ReferenceType::Ref) {
            buffer.writeBits(values[0], bpv_);
        }

        constexpr size_t startIndex = (RType == ReferenceType::Ref) ? 1 : 0;

        // size (from selectBestMethod) = ID + reference + codewords + (J - startIndex) k split bits
        const uint64_t nValues = blockSizeV_ - startIndex;
        const uint64_t fsBits = size - Base::ssId_.length - (RType == ReferenceType::Ref ? bpv_ : 0) - nValues * k;
        buffer.template writeSplitSample<DeltaType, static_cast<int>(Selector)>(values.data() + startIndex, nValues, k, fsBits);
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

    // Zero blocks up to the end of a segment or RSI: as CCSDS 121.0-B-3, 3.5.3, use the
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
        const uint64_t ncSize = ncId_.length + uint64_t{ blockSizeV_ } * bpv_;

        // SE lower bound: each pair costs >= max(d1+d2, 1) bits.
        // Total SE cost >= seId_.length + nPairs + blockSum [+ bpv_ for Ref].
        // Skip expensive sePrepare when this lower bound exceeds cheaper option.
        constexpr size_t startIndex = (RType == ReferenceType::Ref) ? 1 : 0;
        const size_t nPairs = (blockSizeV_ - startIndex) / 2;
        uint64_t seLowerBound = seId_.length + nPairs + blockSum;
        if constexpr (RType == ReferenceType::Ref) {
            seLowerBound += bpv_;
        }

        // SE wins some ties (see below), so it must be evaluated when it can be equal
        const uint64_t cheapest = std::min<uint64_t>(ssSize, ncSize);
        uint64_t seSize;
        if (seLowerBound <= cheapest) {
            seSize = sePrepare<RType>(block);
        }
        else {
            seSize = cheapest + 1;  // SE cannot win
        }

        // Selection and tie-breaking (CCSDS 121.0-B-3, 3.7): split sample if strictly smaller than
        // both other options, otherwise SE or NC (NC wins a tie with SE)
        if (ssSize < ncSize) {
            if (ssSize < seSize) {
                ssEncode<RType>(ssK, block, bitBuffer, ssSize);
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
        // The block is completed with copies of the last sample (as libaec does; CCSDS 121.0-B-3
        // leaves the fill samples to the user): with the unit-delay predictor their preprocessed
        // samples are 0, without reference sample they are the preprocessed last sample
        const DeltaType fill = preprocessing_.prediction != Prediction::UnitDelay && partBlock.size() > 0 ? partBlock[partBlock.size() - 1] : DeltaType{ 0 };
        fullBlock.assign(blockSizeV_, fill);
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

        // REF: the reference sample is not part of the zero test (CCSDS 121.0-B-3, 3.5.1)
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
    // Without reference sample (bypass predictor, no preprocessor), every segment is coded like the
    // alternative segments (CCSDS 121.0-B-3, 4.2.6 and 5.2.2).
    void encodeRsi(const Span<ValueType> data, BitBuffer& buffer)
    {
        const Span<DeltaType> mapped(preprocessBuffer_.data(), data.size());
        if (SPAEC_UNLIKELY(!preprocessing_.isDefault())) {
            encodeRsiOther(data, mapped, buffer);
            return;
        }
        preprocessor_.encode(data, mapped);
        encodeRefSegments(mapped, buffer);
    }

    // encodeRsi for a preprocessing other than the unit-delay predictor for unsigned samples (out
    // of line: the default path stays unchanged)
    __attribute__((noinline)) void encodeRsiOther(const Span<ValueType> data, Span<DeltaType> mapped, BitBuffer& buffer)
    {
        preprocessOther(data, mapped);
        if (preprocessing_.prediction == Prediction::UnitDelay) {
            encodeRefSegments(mapped, buffer);
        }
        else {
            SplitBlock<DeltaType> segmentBlocks(mapped, segmentSizeV_);
            for (size_t i = 0; i < segmentBlocks.nFull(); ++i) {
                encodeAltSegment(segmentBlocks.full(i), buffer);
            }
            if (segmentBlocks.hasPart()) {
                encodeAltSegment(segmentBlocks.part(), buffer);
            }
        }
    }

    // Preprocessing other than the unit-delay predictor for unsigned samples (CCSDS 121.0-B-3, 4)
    void preprocessOther(const Span<ValueType> data, Span<DeltaType> mapped)
    {
        const uint64_t mask = (uint64_t{ 1 } << bpv_) - 1;
        const uint64_t top = uint64_t{ 1 } << (bpv_ - 1);
        const size_t n = data.size();
        if (preprocessing_.prediction == Prediction::UnitDelay) {
            // Signed samples: with xmin = -2^(n-1), the prediction errors and theta (4.4) are those
            // of the unsigned samples x + 2^(n-1) (offset binary: the top bit inverted), so the
            // mapper for unsigned samples applies to them. The reference sample stays the n-bit
            // two's complement sample (4.2.6).
            offsetBuffer_.resize(n);
            for (size_t i = 0; i < n; ++i) {
                offsetBuffer_[i] = static_cast<ValueType>((static_cast<uint64_t>(data[i]) ^ top) & mask);
            }
            preprocessor_.encode(Span<ValueType>(offsetBuffer_.data(), n), mapped);
            if (n > 0) {
                mapped[0] = static_cast<DeltaType>(static_cast<uint64_t>(data[0]) & mask);
            }
        }
        else if (preprocessing_.signedSamples) {
            // Bypass predictor, signed samples: with the predicted value 0, theta = 2^(n-1) - 1 and
            // the mapper gives 2x for x >= 0 and 2|x| - 1 for x < 0 (also for x = -2^(n-1): theta + |x|)
            for (size_t i = 0; i < n; ++i) {
                const uint64_t v = static_cast<uint64_t>(data[i]) & mask;
                mapped[i] = static_cast<DeltaType>((v & top) ? 2 * ((mask - v) + 1) - 1 : 2 * v);
            }
        }
        else {
            // No preprocessor, or the bypass predictor for unsigned samples (theta = 0: the samples)
            for (size_t i = 0; i < n; ++i) {
                mapped[i] = static_cast<DeltaType>(data[i]);
            }
        }
    }

    // The segments of an RSI with a reference sample
    void encodeRefSegments(const Span<DeltaType> mapped, BitBuffer& buffer)
    {
        SplitBlock<DeltaType> segmentBlocks(mapped, segmentSizeV_);
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
        // An empty stream is one zero byte (the flushed, empty last byte)
        return std::max<size_t>((encodeBits(source, target) + 7) / 8, 1);
    }

    // Encodes source into the zeroed target and returns the exact number of bits written
    size_t encodeBits(const Span<ValueType> source, Span<uint8_t> target)
    {
        SplitBlock<ValueType> rsiBlocks(source, rsiSizeV_);
        BitBuffer buffer(target);
        this->splitK(0);  // the split-sample k state starts at 0 in every stream

        for (size_t i = 0; i < rsiBlocks.nFull(); ++i) {
            encodeRsi(rsiBlocks.full(i), buffer);
        }
        if (rsiBlocks.hasPart()) {
            encodeRsi(rsiBlocks.part(), buffer);
        }

        return buffer.bitPos();
    }

    EncoderSeq(uint8_t bpv, uint8_t block_size, uint16_t rsiSize, CodeOptionSet codeOptionSet, Preprocessing preprocessing = {}) :
        EncoderBase<ValueType, DeltaType, Selector>(bpv, block_size, rsiSize, codeOptionSet),
        preprocessor_(bpv),
        preprocessBuffer_(rsiSize * block_size),
        preprocessing_((validatePreprocessing(preprocessing), preprocessing))
    {
        static_assert(std::is_unsigned<ValueType>::value && std::is_unsigned<DeltaType>::value, "the samples are passed in unsigned types");
        if (!preprocessing_.isDefault() && !std::is_same<EncoderFunc, Identity<DeltaType>>::value) {
            throw std::invalid_argument("CCSDS: this preprocessing needs the identity sample function");
        }
        seState_.encodedValues_.resize(blockSizeV_ / 2);
    }


private:
    SETmpState<DeltaType> seState_;

    using Base = EncoderBase<ValueType, DeltaType, Selector>;

public:
    using Base::splitK;

private:
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
    std::vector<ValueType> offsetBuffer_;  // signed samples in offset binary (see preprocessOther)
    const Preprocessing preprocessing_;
};

}  // namespace encoder

}  // namespace SPAEC_ISA
}  // namespace spaec::detail
