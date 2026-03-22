#pragma once

#include <vector>
#include <assert.h>
#include <algorithm>
#include <numeric>

#include "CodeOption.h"
#include "BitBuffer.h"
#include "Span.h"
#include "ThreadPool.h"
#include "BlockWriter.h"
#include "EncoderBase.h"

namespace encoder {

template <typename ValueType, typename DeltaType = ValueType, typename EncodeFunc = Identity<DeltaType>>
class EncoderPar : public EncoderBase<ValueType, DeltaType> {

    struct BWContainer {
        std::array<BlockWriter<DeltaType>, 64> bws_;
        uint8_t size = 0;
    };


    public:
        // Zero Block
        template <ReferenceType RType>
        BlockWriter<DeltaType> zbPrepare(DeltaType reference, size_t nBlocks) {
            size_t size = zbId_.length + bpv_;
            size_t cwLength = nBlocks;
            if (nBlocks >= rosLength) {
                cwLength++;
            }
            assert(cwLength > 0 && "Codeword length must be greater than 0.");
            assert(cwLength <= 64 && "Codeword length must be less than or equal to 64.");
            size += cwLength;

            BlockWriter<DeltaType> blockWriter;
            blockWriter.size = size;
            blockWriter.self = this;
            blockWriter.args = encoder::ZBArgs<DeltaType>{reference, static_cast<uint8_t>(nBlocks)};

            blockWriter.writeTo = [](const void* sself, const encoder::Args<ValueType>& args, size_t offset, BitBuffer& buffer) -> size_t {
                const auto& zbArgs = std::get<encoder::ZBArgs<DeltaType>>(args);
                const auto self = static_cast<const EncoderPar<ValueType, DeltaType>*>(sself);
                const size_t nBlocks = zbArgs.nBlocks;

                size_t cwLength = nBlocks;
                if (nBlocks >= rosLength) {
                    cwLength++;
                }

                buffer.writeId(self->zbId_.valueShifted, self->zbId_.length, offset);
                offset += self->zbId_.length;

                if constexpr (RType == ReferenceType::Ref) {
                    DeltaType reference = zbArgs.reference;
                    buffer.writeBits(reference, self->bpv_, offset);
                }

                buffer.writeCodeword(cwLength - 1, offset);
                offset += cwLength;

                return offset;
            };
            return blockWriter;
        }


        // Zero Block ROS
        template <ReferenceType RType>
        BlockWriter<DeltaType> zbPrepareROS(DeltaType reference) {
            BlockWriter<DeltaType> blockWriter;
            blockWriter.size = zbId_.length + bpv_ + rosLength;
            blockWriter.self = this;
            blockWriter.args = encoder::ZBArgs<DeltaType>{reference, 0};

            blockWriter.writeTo = [](const void* sself, const encoder::Args<ValueType>& args, size_t offset, BitBuffer& buffer) -> size_t {
                const auto& zbArgs = std::get<encoder::ZBArgs<DeltaType>>(args);
                const auto self = static_cast<const EncoderPar<ValueType, DeltaType>*>(sself);

                buffer.writeId(self->zbId_.valueShifted, self->zbId_.length, offset);
                offset += self->zbId_.length;

                if constexpr (RType == ReferenceType::Ref) {
                    DeltaType reference = zbArgs.reference;
                    buffer.writeBits(reference, self->bpv_, offset);
                }

                buffer.writeCodeword(rosLength - 1, offset);
                offset += rosLength;

                return offset;
            };
            return blockWriter;
        }


        // Split Sample
        template <ReferenceType RType>
        BlockWriter<DeltaType> ssPrepare(Span<DeltaType> values, size_t sizeHint, uint8_t k)
        {
            BlockWriter<DeltaType> blockWriter;
            blockWriter.size = sizeHint;
            blockWriter.self = this;
            blockWriter.args = encoder::SSArgs<DeltaType>{k, values};

            blockWriter.writeTo = [](const void* sself, const encoder::Args<ValueType>& args, size_t offset, BitBuffer& buffer) -> size_t {
                const auto& ssArgs = std::get<encoder::SSArgs<DeltaType>>(args);
                const auto self = static_cast<const EncoderPar<ValueType, DeltaType>*>(sself);
                const auto k = ssArgs.k;
                const auto& values = ssArgs.values;

                const SplitSampleCodeOption<DeltaType>& id = self->ssIdStore[k];

                buffer.writeId(id.valueShifted, id.length, offset);
                offset += id.length;

                if constexpr (RType == ReferenceType::Ref) {
                    buffer.writeBits(values[0], self->bpv_, offset);
                    offset += self->bpv_;
                }

                constexpr size_t startIndex = (RType == ReferenceType::Ref) ? 1 : 0;

                for (size_t i = startIndex; i < self->blockSizeV_; ++i) {
                    buffer.writeCodeword(values[i] >> k, offset);
                    offset += (values[i] >> k) + 1;
                }

                buffer.writeBitsBatch(values.data() + startIndex, self->blockSizeV_ - startIndex, k, offset);
                offset += k * (self->blockSizeV_ - startIndex);

                return offset;
            };
            return blockWriter;
        }


        // Second Extension
        template <ReferenceType RType>
        BlockWriter<DeltaType> sePrepare(Span<DeltaType> values)
        {
            assert(values.size() % 2 == 0 && "Deltas size must be even for second extension encoding");

            DeltaType reference = values[0];
            size_t vsize = blockSizeV_ / 2;
            std::array<DeltaType, 32> encodedValues;

            if constexpr (RType == ReferenceType::Ref) {
                const DeltaType d1 = 0;
                const DeltaType d2 = values[1];
                const DeltaType value = (d1 + d2) * (d1 + d2 + 1) / 2 + d2;
                encodedValues[0] = value;
            }

            constexpr size_t startIndex = (RType == ReferenceType::Ref) ? 2 : 0;

            for (size_t i = startIndex; i < vsize; i += 2) {
                const DeltaType d1 = values[i];
                const DeltaType d2 = values[i + 1];
                encodedValues[i/2] = (d1 + d2) * (d1 + d2 + 1) / 2 + d2;
            }

            size_t size = seId_.length + bpv_ + std::accumulate(encodedValues.begin(), encodedValues.end(), 0u, [](size_t sum, ValueType v) {
                return sum + v + 1;
            });

            BlockWriter<DeltaType> blockWriter;
            blockWriter.size = size;
            blockWriter.self = this;
            blockWriter.args = encoder::SEArgs<DeltaType>{reference, encodedValues};

            blockWriter.writeTo = [](const void* sself, const encoder::Args<ValueType>& args, size_t offset, BitBuffer& buffer) -> size_t {
                const auto& seArgs = std::get<encoder::SEArgs<DeltaType>>(args);
                const auto self = static_cast<const EncoderPar<ValueType, DeltaType>*>(sself);
                const auto& encodedValues = seArgs.encodedValues;
                const size_t vsize = self->blockSizeV_ / 2;

                buffer.writeId(self->seId_.valueShifted, self->seId_.length, offset);
                offset += self->seId_.length;
                if constexpr (RType == ReferenceType::Ref) {
                    buffer.writeBits(seArgs.reference, self->bpv_, offset);
                    offset += self->bpv_;
                }
                for (size_t i = 0; i < vsize; ++i) {
                    buffer.writeCodeword(encodedValues[i], offset);
                    offset += encodedValues[i] + 1;
                }
                return offset;
            };
            return blockWriter;
        }

        // No Compression
        template <ReferenceType RType>
        BlockWriter<DeltaType> ncPrepare(Span<DeltaType> values) {
            BlockWriter<DeltaType> blockWriter;
            blockWriter.size = ncId_.length + blockSizeV_ * bpv_;
            blockWriter.self = this;
            blockWriter.args = encoder::NCArgs<DeltaType>{values};

            blockWriter.writeTo = [](const void* sself, const encoder::Args<ValueType>& args, size_t offset, BitBuffer& buffer) -> size_t{
                const auto& ncArgs = std::get<encoder::NCArgs<DeltaType>>(args);
                const auto self = static_cast<const EncoderPar<ValueType, DeltaType>*>(sself);
                const auto& values = ncArgs.values;
                buffer.writeId(self->ncId_.valueShifted, self->ncId_.length, offset);
                offset += self->ncId_.length;
                for (size_t i = 0; i < self->blockSizeV_; ++i) {
                    buffer.writeBits(values[i], self->bpv_, offset);
                    offset += self->bpv_;
                }
                return offset;
            };
            return blockWriter;
        }


        // SEGMENT
        static inline auto isZeroBlock = [](const Span<DeltaType> block) {
            return std::all_of(block.begin(), block.end(), [](DeltaType v) { return v == 0; });
        };

    public:
        template <ReferenceType RType>
        BlockWriter<DeltaType> encodeFullBlockPrepare(Span<DeltaType> block) {
            assert(block.size() == blockSizeV_ && "Block size does not match encoder block size.");

            auto [ssK, ssSize, blockSum] = this-> template selectBestMethod<RType>(block);
            BlockWriter<DeltaType> ssIres = ssPrepare<RType>(block, ssSize, ssK);
            BlockWriter<DeltaType> ncIres = ncPrepare<RType>(block);

            // SE lower bound: skip expensive sePrepare when SE cannot win
            constexpr size_t startIndex = (RType == ReferenceType::Ref) ? 1 : 0;
            const size_t nPairs = (blockSizeV_ - startIndex) / 2;
            size_t seLowerBound = seId_.length + nPairs + blockSum;
            if constexpr (RType == ReferenceType::Ref) {
                seLowerBound += bpv_;
            }

            size_t cheapest = std::min(ssSize, ncIres.size);

            BlockWriter<DeltaType> seIres;
            if (seLowerBound < cheapest) {
                seIres = sePrepare<RType>(block);
            } else {
                seIres.size = cheapest + 1; // SE cannot win
            }

            // §3.7.4 tie-breaking: NC > SE > smallest k
            if (ncIres.size <= ssSize && ncIres.size <= seIres.size) {
                return ncIres;
            }
            else if (seIres.size <= ssSize && seIres.size <= ncIres.size) {
                return seIres;
            }
            else {
                return ssIres;
            }
        }

        void encodeRefFullSegmentPrepare(Span<DeltaType> segment, BWContainer& blockWriters) 
        {
            DeltaType reference = segment[0];
            size_t size = 0;

            size_t nZeroBlocks = 0;
            SplitBlock<DeltaType> segmentBlocks(segment, blockSizeV_);
            bool isRefZeroBlock = false;

            auto handleZeroBlocks = [&]() {
                if (nZeroBlocks != 0) {
                    if (isRefZeroBlock == 1) {
                        blockWriters.bws_[blockWriters.size++] = zbPrepare<ReferenceType::Ref>(reference, nZeroBlocks);
                        isRefZeroBlock = false;
                    }
                    else if (isRefZeroBlock == 0) {
                        blockWriters.bws_[blockWriters.size++] = zbPrepare<ReferenceType::NoRef>(0, nZeroBlocks);
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
                blockWriters.bws_[blockWriters.size++] = encodeFullBlockPrepare<ReferenceType::Ref>(segmentBlocks.full(0));
            }

            // ALT
            size_t count = 1;
            for (size_t i = 1; i < segmentBlocks.nFull(); ++i) {
                auto block = segmentBlocks.full(i);
                if (isZeroBlock(block)) {
                    nZeroBlocks++;
                }
                else {
                    handleZeroBlocks();
                    blockWriters.bws_[blockWriters.size++] = encodeFullBlockPrepare<ReferenceType::NoRef>(block);
                }
                count++;
            }

            // ROS
            if (nZeroBlocks != 0) {
                if (isRefZeroBlock) {
                    blockWriters.bws_[blockWriters.size++] = zbPrepareROS<ReferenceType::Ref>(reference);
                }
                else {
                    blockWriters.bws_[blockWriters.size++] = zbPrepareROS<ReferenceType::NoRef>(0);
                }
            }
        }

        void encodeAltFullSegmentPrepare(Span<DeltaType> segment, BWContainer& blockWriters)
        {
            size_t nZeroBlocks = 0;
            SplitBlock<DeltaType> segmentBlocks(segment, blockSizeV_);
            size_t size = 0;

            auto handleZeroBlocks = [&]() {
                if (nZeroBlocks != 0) {
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
                    blockWriters.bws_[blockWriters.size++] = encodeFullBlockPrepare<ReferenceType::NoRef>(block);
                }
            }

            // ROS
            if (nZeroBlocks != 0) {
                blockWriters.bws_[blockWriters.size++] = zbPrepareROS<ReferenceType::NoRef>(0);
            }
        }


        void encodeRefPartSegmentPrepare(Span<DeltaType> segment, BWContainer& blockWriters)
        {
            DeltaType reference = segment[0];
            SplitBlock<DeltaType> segmentBlocks(segment, blockSizeV_);
            size_t nZeroBlocks = 0;
            bool isRefZeroBlock = false;

            auto handleZeroBlocks = [&]() {
                if (nZeroBlocks != 0) {
                    if (isRefZeroBlock == 1) {
                        blockWriters.bws_[blockWriters.size++] = zbPrepare<ReferenceType::Ref>(reference, nZeroBlocks);
                        isRefZeroBlock = false;
                    }
                    else if (isRefZeroBlock == 0) {
                        blockWriters.bws_[blockWriters.size++] = zbPrepare<ReferenceType::NoRef>(0, nZeroBlocks);
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
                    blockWriters.bws_[blockWriters.size++] = encodeFullBlockPrepare<ReferenceType::Ref>(block);
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
                    blockWriters.bws_[blockWriters.size++] = encodeFullBlockPrepare<ReferenceType::Ref>(block);
                }

                // ALT
                for (size_t i = 1; i < segmentBlocks.nFull(); ++i) {
                    auto block = segmentBlocks.full(i);
                    if (isZeroBlock(block)) {
                        nZeroBlocks++;
                    }
                    else {
                        handleZeroBlocks();
                        blockWriters.bws_[blockWriters.size++] = encodeFullBlockPrepare<ReferenceType::NoRef>(block);
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
                        blockWriters.bws_[blockWriters.size++] = encodeFullBlockPrepare<ReferenceType::NoRef>(block);
                    }
                }
            }

            // ROS
            if (nZeroBlocks != 0) {
                if (isRefZeroBlock) {
                    blockWriters.bws_[blockWriters.size++] = zbPrepareROS<ReferenceType::Ref>(reference);
                }
                else {
                    blockWriters.bws_[blockWriters.size++] = zbPrepareROS<ReferenceType::NoRef>(0);
                }
            }
        }

        void encodeAltPartSegmentPrepare(Span<DeltaType> segment, BWContainer& blockWriters)
        {
            SplitBlock<DeltaType> segmentBlocks(segment, blockSizeV_);
            size_t nZeroBlocks = 0;

            auto handleZeroBlocks = [&]() {
                if (nZeroBlocks != 0) {
                    blockWriters.bws_[blockWriters.size++] = zbPrepare<ReferenceType::NoRef>(0, nZeroBlocks);
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
                    blockWriters.bws_[blockWriters.size++] = encodeFullBlockPrepare<ReferenceType::NoRef>(block);
                }
            }

            if (segmentBlocks.hasPart()) {
                auto block = segmentBlocks.part();
                if (isZeroBlock(block)) {
                    nZeroBlocks++;
                }
                else {
                    handleZeroBlocks();
                    blockWriters.bws_[blockWriters.size++] = encodeFullBlockPrepare<ReferenceType::NoRef>(block);
                }
            }

            // ROS
            if (nZeroBlocks != 0) {
                blockWriters.bws_[blockWriters.size++] = zbPrepareROS<ReferenceType::NoRef>(0);
            }
        }


        void encodeRefFullRsiPrepare(Span<DeltaType> data, Span<DeltaType> valuesPreprocessed, Span<BWContainer> blockWriters)
        {
            preprocessor_.encode(data, valuesPreprocessed);
            SplitBlock<DeltaType> segmentBlocks(Span<DeltaType>(valuesPreprocessed), segmentSizeV_);

            if (segmentBlocks.nFull() == 0 && segmentBlocks.nPart() == 1) {
                encodeRefPartSegmentPrepare(segmentBlocks.part(), blockWriters[0]);
            }
            else {
                encodeRefFullSegmentPrepare(segmentBlocks.full(0), blockWriters[0]);
                for (size_t i = 1; i < segmentBlocks.nFull(); ++i) {
                    encodeAltFullSegmentPrepare(segmentBlocks.full(i), blockWriters[i]);
                }
                if (segmentBlocks.hasPart()) {
                    encodeAltPartSegmentPrepare(segmentBlocks.part(), blockWriters[segmentBlocks.nFull()]);
                }
            }
        }


        void encodeRefPartRsiPrepare(Span<DeltaType> data, Span<DeltaType> valuesPreprocessed, Span<BWContainer> blockWriters)
        {
            encodeRefFullRsiPrepare(data, valuesPreprocessed, blockWriters);
        }


        size_t encodeParallel(Span<DeltaType> source, Span<uint8_t> target)
        {
            SplitBlock<DeltaType> rsiBlocks(source, rsiSizeV_);
            size_t nBlocks = rsiBlocks.nFull() + rsiBlocks.nPart();
            std::vector<std::future<void>> futures;
            futures.reserve(nBlocks);

            std::vector<ValueType> preprocessedData(source.size());
            SplitBlock<DeltaType> preprocessedRsiBlocks(Span<DeltaType>(preprocessedData), rsiSizeV_);

            size_t nSegPerRsi = (rsiSizeV_ + segmentSizeV_ - 1) / segmentSizeV_;
            size_t nSegTotal = nSegPerRsi * preprocessedRsiBlocks.n();

            std::vector<BWContainer> bwContainers(nSegTotal);
            SplitBlock<BWContainer> bwContainersSplit(Span<BWContainer>(bwContainers), nSegPerRsi);

            for (size_t i = 0; i < rsiBlocks.nFull(); ++i) {
                auto rsiBlock = rsiBlocks.full(i);
                auto preprocessedBlock = preprocessedRsiBlocks.full(i);
                auto bwBlock = bwContainersSplit.full(i);
                futures.emplace_back(
                    threadPool_.enqueue([this, rsiBlock, preprocessedBlock, bwBlock]() {
                        encodeRefFullRsiPrepare(rsiBlock, preprocessedBlock, bwBlock);
                    }));
            }

            if (rsiBlocks.hasPart()) {
                auto rsiBlock = rsiBlocks.part();
                auto preprocessedBlock = preprocessedRsiBlocks.part();
                auto bwBlock = bwContainersSplit.full(rsiBlocks.nFull());
                futures.emplace_back(
                    threadPool_.enqueue([this, rsiBlock, preprocessedBlock, bwBlock]() {
                        encodeRefPartRsiPrepare(rsiBlock, preprocessedBlock, bwBlock);
                    }));
            }

            for (auto& f : futures) {
                f.get();
            }

            size_t currentOffset = 0;
            for (size_t i = 0; i < nBlocks; ++i) {
                auto bwContainer = bwContainersSplit.full(i);
                for (auto& segmentBlockWriters : bwContainer) {
                    for (size_t j = 0; j < segmentBlockWriters.size; ++j) {
                        auto& bw = segmentBlockWriters.bws_[j];
                        bw.offset = currentOffset;
                        currentOffset += bw.size;
                    }
                }
            }

            BitBuffer buffer(target);

            auto writeTask = [&](size_t startIdx) {
                std::vector<std::future<void>> futures;
                futures.reserve((nBlocks + 1) / 2);

                for (size_t i = startIdx; i < nBlocks; i+=2) {
                    futures.emplace_back(
                    threadPool_.enqueue([&, i]() {
                        auto bwContainer = bwContainersSplit.full(i);
                        for (auto segmentBlockWriters : bwContainer) {
                            for (size_t j = 0; j < segmentBlockWriters.size; ++j) {
                                auto& bw = segmentBlockWriters.bws_[j];
                                bw.write(buffer);
                            }
                        }
                    }));
                }
                for (auto& f : futures) {
                    f.get();
                }
            };

            writeTask(0);
            writeTask(1);

            return (currentOffset + 7)/ 8;
        }


        size_t encodeParallelSeq(Span<DeltaType> source, Span<uint8_t> target)
        {
            SplitBlock<DeltaType> rsiBlocks(source, rsiSizeV_);
            std::vector<ValueType> preprocessedData(source.size());
            SplitBlock<DeltaType> preprocessedRsi(Span<DeltaType>(preprocessedData), rsiSizeV_);
            std::vector<std::vector<BWContainer>> rsiBlockWriters(preprocessedRsi.n());

            size_t nSegPerRsi = (rsiSizeV_ + segmentSizeV_ - 1) / segmentSizeV_;
            size_t nSegTotal = nSegPerRsi * preprocessedRsi.n();

            std::vector<BWContainer> bwContainers(nSegTotal);
            SplitBlock<BWContainer> bwContainersSplit(Span<BWContainer>(bwContainers), nSegPerRsi);

            for (size_t i = 0; i < rsiBlocks.nFull(); ++i) {
                auto rsiBlock = rsiBlocks.full(i);
                auto preprocessedBlock = preprocessedRsi.full(i);
                auto blockWriters = bwContainersSplit.full(i);
                encodeRefFullRsiPrepare(rsiBlock, preprocessedBlock, blockWriters);
            }
            if (rsiBlocks.hasPart()) {
                auto rsiBlock = rsiBlocks.part();
                auto preprocessedBlock = preprocessedRsi.part();
                auto blockWriters = bwContainersSplit.full(rsiBlocks.nFull());
                encodeRefPartRsiPrepare(rsiBlock, preprocessedBlock, blockWriters);
            }

            size_t currentOffset = 0;
            for (size_t i = 0; i < preprocessedRsi.n(); ++i) {
                auto bwContainer = bwContainersSplit.full(i);
                for (auto& segmentBlockWriters : bwContainer) {
                    for (size_t j = 0; j < segmentBlockWriters.size; ++j) {
                        auto& bw = segmentBlockWriters.bws_[j];
                        bw.offset = currentOffset;
                        currentOffset += bw.size;
                    }
                }
            }

            BitBuffer buffer(target);

            auto writeTask = [&](size_t startIdx) {
                for (size_t i = startIdx; i < preprocessedRsi.n(); i+=2) {
                    auto bwContainer = bwContainersSplit.full(i);
                    for (auto segmentBlockWriters : bwContainer) {
                        for (size_t j = 0; j < segmentBlockWriters.size; ++j) {
                            auto& bw = segmentBlockWriters.bws_[j];
                            auto newOffset = bw.write(buffer);
                            assert(newOffset == bw.offset + bw.size);
                        }
                    }
                };
            };

            writeTask(0);
            writeTask(1);

            return (currentOffset + 7)/ 8;
        }

        EncoderPar(uint8_t bpv, uint8_t block_size, uint16_t rsiSize, CodeOptionSet codeOptionSet, size_t nThreads = 8) : preprocessor_(bpv),
            EncoderBase<ValueType, DeltaType>(bpv, block_size, rsiSize, codeOptionSet),
            nThreads_(nThreads)
        {}
        ~EncoderPar() {}


    private:
        size_t nThreads_;

        ThreadPool threadPool_{nThreads_};
        PredictivePreprocessingEncodeFast<ValueType, DeltaType, EncodeFunc> preprocessor_;

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
        using Base::codeOptionSet_;
        using Base::ssIdStore;
};

}  // namespace encoder
