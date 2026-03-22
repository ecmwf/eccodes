#pragma once

#include <array>
#include <vector>
#include <assert.h>
#include <algorithm>
#include <numeric>

#include "EncoderBase.h"
#include "CodeOption.h"
#include "BitBuffer.h"
#include "Span.h"
#include "Utils.h"

namespace encoder {

template <typename ValueType>
struct SETmpState
{
        ValueType reference_;
        std::vector<uint64_t> encodedValues_;
};


template <typename ValueType, typename DeltaType, typename EncoderFunc = Identity<DeltaType>>
class EncoderSeq : EncoderBase<ValueType, DeltaType> {
    public:
        // No Compression
        void ncEncode(const Span<DeltaType> values, BitBuffer& buffer) {
            ncCounter_++;
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
                seState_.encodedValues_[i/2] = (d1 + d2) * (d1 + d2 + 1) / 2 + d2;
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
            seCounter_++;
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
            ssCounter_[k]++;

            const auto& ssId_ = ssIdStore[k];
            const IdType valueShifted = ssId_.valueShifted;
            const DeltaType mask = ssId_.mask;

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
        void zbEncode(BitBuffer& buffer, DeltaType reference, size_t nBlocks) {
            assert(nBlocks > 0 && "Number of zero blocks must be greater than 0.");
            zbCounter_ += nBlocks;

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

        // Zero Block ROS
        template <ReferenceType RType>
        void zbEncodeROS(BitBuffer& buffer, DeltaType reference) {
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

            size_t cheapest = std::min(ssSize, ncSize);
            size_t seSize;
            if (seLowerBound < cheapest) {
                seSize = sePrepare<RType>(block);
            } else {
                seSize = cheapest + 1; // SE cannot win
            }

            // §3.7.4 tie-breaking: NC > SE > smallest k
            if (ncSize <= ssSize && ncSize <= seSize) {
                ncEncode(block, bitBuffer);
            }
            else if (seSize <= ssSize && seSize <= ncSize) {
                seEncode<RType>(bitBuffer);
            }
            else {
                ssEncode<RType>(ssK, block, bitBuffer);
            }
        }

        template <ReferenceType RType>
        void encodePartBlock(const Span<DeltaType> partBlock, BitBuffer& buffer)
        {
            static std::vector<DeltaType> fullBlock;
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

            // REF
            if (isZeroBlock(segmentBlocks.full(0))) {
                nZeroBlocks++;
                isRefZeroBlock = true;
            }
            else {
                handleZeroBlocks();
                encodeFullBlock<ReferenceType::Ref>(segmentBlocks.full(0), buffer);
            }

            // ALT
            size_t count = 1;
            for (size_t i = 1; i < segmentBlocks.nFull(); ++i) {
                const auto block = segmentBlocks.full(i);
                if (isZeroBlock(block)) {
                    nZeroBlocks++;
                }
                else {
                    handleZeroBlocks();
                    encodeFullBlock<ReferenceType::NoRef>(block, buffer);
                }
                count++;
            }

            // ROS
            if (nZeroBlocks != 0) {
                if (isRefZeroBlock) {
                    zbEncodeROS<ReferenceType::Ref>(buffer, reference);
                }
                else {
                    zbEncodeROS<ReferenceType::NoRef>(buffer, 0);
                }
            }
        }

        void encodeAltFullSegment(const Span<DeltaType> segment, BitBuffer& buffer)
        {
            size_t nZeroBlocks = 0;
            SplitBlock<DeltaType> segmentBlocks(segment, blockSizeV_);

            auto handleZeroBlocks = [&]() {
                if (nZeroBlocks != 0) {
                    zbEncode<ReferenceType::NoRef>(buffer, 0, nZeroBlocks);
                    nZeroBlocks = 0;
                }
            };

            // ALT
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

            // ROS
            if (nZeroBlocks != 0) {
                zbEncodeROS<ReferenceType::NoRef>(buffer, 0);
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
                    auto block = segmentBlocks.full(i);
                    if (isZeroBlock(block)) {
                        nZeroBlocks++;
                    }
                    else {
                        handleZeroBlocks();
                        encodeFullBlock<ReferenceType::NoRef>(block, buffer);
                    }
                }

                // ALT PART
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
            }

            // ROS
            if (nZeroBlocks != 0) {
                if (isRefZeroBlock) {
                    zbEncodeROS<ReferenceType::Ref>(buffer, reference);
                }
                else {
                    zbEncodeROS<ReferenceType::NoRef>(buffer, 0);
                }
            }
        }

        void encodeAltPartSegment(const Span<DeltaType> segment, BitBuffer& buffer)
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
                zbEncodeROS<ReferenceType::NoRef>(buffer, 0);
            }
        }


        // RSI
        void encodeRefFullRsi(const Span<ValueType> data, BitBuffer& buffer)
        {
            preprocessor_.encode(data, Span<DeltaType>(preprocessBuffer_.data(), data.size()));

            SplitBlock<DeltaType> segmentBlocks(Span<DeltaType>(preprocessBuffer_.data(), data.size()), segmentSizeV_);
            if (segmentBlocks.nFull() == 0 && segmentBlocks.nPart() == 1) {
                encodeRefPartSegment(segmentBlocks.part(), buffer);
            }
            else {
                encodeRefFullSegment(segmentBlocks.full(0), buffer);
                for (size_t i = 1; i < segmentBlocks.nFull(); ++i) {
                    encodeAltFullSegment(segmentBlocks.full(i), buffer);
                }
                if (segmentBlocks.hasPart()) {
                    encodeAltPartSegment(segmentBlocks.part(), buffer);
                }
            }
        }

        void encodeRefPartRsi(const Span<ValueType> data, BitBuffer& buffer)
        {
            preprocessor_.encode(data, Span<DeltaType>(preprocessBuffer_.data(), data.size()));

            SplitBlock<DeltaType> segmentBlocks(Span<DeltaType>(preprocessBuffer_.data(), data.size()), segmentSizeV_);
            if (segmentBlocks.nFull() == 0 && segmentBlocks.nPart() == 1) {
                encodeRefPartSegment(segmentBlocks.part(), buffer);
            }
            else {
                encodeRefFullSegment(segmentBlocks.full(0), buffer);
                for (size_t i = 1; i < segmentBlocks.nFull(); ++i) {
                    encodeAltFullSegment(segmentBlocks.full(i), buffer);
                }
                if (segmentBlocks.hasPart()) {
                    encodeAltPartSegment(segmentBlocks.part(), buffer);
                }
            }
        }

    public:
        size_t encode(const Span<ValueType> source, Span<uint8_t> target)
        {
            // reset counters
            ssCounter_.fill(0);
            zbCounter_ = 0;
            ncCounter_ = 0;
            seCounter_ = 0; 
            nBlocks = (source.size() + blockSizeV_ - 1) / blockSizeV_;

            SplitBlock<ValueType> rsiBlocks(source, rsiSizeV_);
            BitBuffer buffer(target);

            for (size_t i = 0; i < rsiBlocks.nFull(); ++i) {
                encodeRefFullRsi(rsiBlocks.full(i), buffer);
            }
            if (rsiBlocks.hasPart()) {
                encodeRefPartRsi(rsiBlocks.part(), buffer);
            }

            return (buffer.bitPos() + 7) / 8;
        }


        double compressibility() const {
            size_t nEncodedBits = 0;
            for (size_t k = 0; k < kMax; ++k) {
                nEncodedBits += ssCounter_[k] * (bpv_ - k) * blockSizeV_;
            }
            nEncodedBits += seCounter_ * bpv_ * blockSizeV_;
            nEncodedBits += zbCounter_ * bpv_ * blockSizeV_;
            return static_cast<double>(nEncodedBits) / (nBlocks * blockSizeV_ * bpv_);
        }


        template <typename... Args>
        EncoderSeq(uint8_t bpv, uint8_t block_size, uint16_t rsiSize, CodeOptionSet codeOptionSet, Args&&... args) : preprocessor_(bpv, std::forward<Args>(args)...),
            EncoderBase<ValueType, DeltaType>(bpv, block_size, rsiSize, codeOptionSet),
            preprocessBuffer_(rsiSize * block_size)
        {
            seState_.encodedValues_.resize(blockSizeV_ / 2);
        }
        ~EncoderSeq() {}


    private:
        SETmpState<DeltaType> seState_;

        using Base = EncoderBase<ValueType, DeltaType>;
        using Base::bpv_;
        using Base::blockSizeV_;
        using Base::rsiSizeB_;
        using Base::rsiSizeV_;
        using Base::zbId_;
        using Base::ncId_;
        using Base::ssId_;
        using Base::seId_;
        using Base::limits_;
        using Base::kMin;
        using Base::kMax;
        using Base::segmentSizeV_;
        using Base::segmentSizeB_;
        using Base::ssIdStore;

        PredictivePreprocessingEncodeFast<ValueType, DeltaType, EncoderFunc> preprocessor_;
        std::vector<DeltaType> preprocessBuffer_;
        std::array<size_t, 32> ssCounter_{}; // Counters for each k in split sample method
        size_t zbCounter_ = 0;
        size_t ncCounter_ = 0;
        size_t seCounter_ = 0;
        size_t nBlocks = 0;
};

}  // namespace encoder
