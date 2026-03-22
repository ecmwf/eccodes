#pragma once

#include <vector>
#include <assert.h>

#include "Span.h"
#include "CodeOption.h"
#include "BitBuffer.h"
#include "ThreadPool.h"
#include "DecoderSeq.h"

template <typename ValueType, typename DeltaType, typename DecodeFunc = Identity<DeltaType>>
class DecoderPar : public DecoderSeq<ValueType, DeltaType, DecodeFunc> {
    public:
        template <ReferenceType RType>
            std::pair<bool, size_t> skipBlock(const BitBuffer& buffer) const {
                auto id = buffer.peekBits(this->maxIdLength);
                bool isRos = false;
                size_t nBlocks = 1;

                if ( id == 0) {
                    buffer.skipBits(this->maxIdLength);
                    if constexpr (RType == ReferenceType::Ref) {
                        buffer.skipBits(bpv_);
                    }
                    nBlocks = buffer.readCodeword();
                    if (nBlocks == rosLength) { // All zeros segment
                        isRos = true;
                    }
                    else { // First n blocks are zeros
                        if (nBlocks > rosLength) {
                            nBlocks--;
                        }
                    }
                }
                else if (id == 1)
                {
                    buffer.skipBits(this->maxIdLength);
                    if constexpr (RType == ReferenceType::Ref) {
                        buffer.skipBits(bpv_);
                    }
                    buffer.skipCodewords(blockSizeV_ / 2);
                }
                else {
                    id >>= 1;
                    buffer.skipBits(this->maxIdLength - 1);

                    if (id == noCompressionId_) {
                        size_t size = blockSizeV_ * bpv_;
                        buffer.skipBits(size);
                    }
                    else {
                        if constexpr (RType == ReferenceType::Ref) {
                            buffer.skipBits(bpv_);
                        }
                        uint8_t k = static_cast<uint8_t>(id - 1);
                        constexpr size_t startIndex = (RType == ReferenceType::Ref) ? 1 : 0;
                        buffer.skipCodewords(blockSizeV_ - startIndex);
                        buffer.skipBits(k * (blockSizeV_ - startIndex));
                    }
                }

                return {isRos, nBlocks};
            }

    public:
        std::vector<size_t> decodeRsiOffsets(const BitBuffer& buffer, size_t nValues)
        {
            std::vector<size_t> rsiOffsets;
            const size_t nFullRsis = nValues / rsiSizeV_;
            const size_t nFullSegments = rsiSizeV_ / maxSegmentSizeV_;
            const size_t nSegmentsPerRsi = (rsiSizeV_ + maxSegmentSizeV_ - 1) / maxSegmentSizeV_;


            // LAMBDA for processing small and large RSIs
            auto processSmallRsi = [&](const BitBuffer& buffer) -> size_t {
                assert(rsiSizeV_ < maxSegmentSizeV_ && "RSI size must be smaller than segment size.");
                size_t i = 0;
                const auto [isRos, nBlocks] = skipBlock<ReferenceType::Ref>(buffer);
                if (!isRos) {
                    i += nBlocks;
                    for (; i < rsiSizeB_;) {
                        auto [isRos, nBlocks] = skipBlock<ReferenceType::NoRef>(buffer);
                        if (isRos) {
                            break;
                        }
                        i += nBlocks;
                    }
                }

                return buffer.bitPos();
            };

            // LAMBDA for processing large RSI
            auto processLargeRsi = [&](const BitBuffer& buffer) -> size_t {
                assert(rsiSizeV_ >= maxSegmentSizeV_ && "RSI size must be larger than or equal to segment size.");
                size_t i = 0;
                const auto [isRos, nBlocks] = skipBlock<ReferenceType::Ref>(buffer);
                if (!isRos) {
                    // Reference segment
                    if (!isRos) {
                        i += nBlocks;
                        for (; i < maxSegmentSizeB_;) {
                            auto [isRos, nBlocks] = skipBlock<ReferenceType::NoRef>(buffer);
                            if (isRos) {
                                break;
                            }
                            i += nBlocks;
                        }
                    }

                    // Full segments
                    for (size_t u = 1; u < nFullSegments; ++u) {
                        for (size_t i = 0; i < maxSegmentSizeB_;) {
                            const auto [isRos, nBlocks] = skipBlock<ReferenceType::NoRef>(buffer);
                            if (isRos) {
                                break;
                            }
                            i += nBlocks;
                        }
                    }
                    // Last segment
                    {
                        for (size_t i = 0; i < (rsiSizeB_) % (maxSegmentSizeB_);) {
                            const auto [isRos, nBlocks] = skipBlock<ReferenceType::NoRef>(buffer);
                            if (isRos) {
                                break;
                            }
                            i += nBlocks;
                        }
                    }
                }

                return buffer.bitPos();
            };



            // Process RSIs — scan only (no parallel decode to avoid shared state races)
            size_t prevOffset = 0;

            for (size_t i = 0; i < nFullRsis; ++i) {
                rsiOffsets.push_back(prevOffset);
                prevOffset = processLargeRsi(buffer);
            }

            return rsiOffsets;
        }

        void decodeSeq(Span<uint8_t> source, Span<ValueType> target)
        {
            const BitBuffer buffer{source};
            SplitBlock<ValueType> chunks(target, rsiSizeV_);
            for (size_t i = 0; i < chunks.nFull(); ++i) {
                this->decodeRefFullRsi(buffer, chunks.full(i));
            }
            if (chunks.hasPart()) {
                decodeRefPartRsi(buffer, chunks.part());
            }
        }

        void decodeSeq(size_t offset, Span<uint8_t> source, Span<ValueType> target)
        {
            BitBuffer buffer{source};
            buffer.bitPos(offset);

            SplitBlock<ValueType> chunks(target, rsiSizeV_);
            for (size_t i = 0; i < chunks.nFull(); ++i) {
                this->decodeRefFullRsi(buffer, chunks.full(i));
            }
            if (chunks.hasPart()) {
                this->decodeRefFullRsi(buffer, chunks.part());
            }
        }

        template <typename OutputType, typename UnmapFunc>
        void decodeParallel(Span<uint8_t> source, Span<OutputType> target, UnmapFunc unmapper) {
            size_t nRsi = target.size() / rsiSizeV_;
            size_t nRsiPerThread = (nRsi + nThreads_ - 1) / nThreads_;
            size_t chunkSizeV = nRsiPerThread * rsiSizeV_;

            // Incremental scan: launch decode threads as RSI boundaries are found
            BitBuffer scanBuf{source};
            size_t threadIdx = 0;
            size_t nextBoundary = 0;
            std::vector<std::future<void>> futures;
            futures.reserve(nThreads_);

            for (size_t rsi = 0; rsi < nRsi; ++rsi) {
                if (rsi == nextBoundary) {
                    size_t bitOffset = scanBuf.bitPos();
                    size_t startVal = threadIdx * chunkSizeV;
                    size_t endVal = std::min(startVal + chunkSizeV, target.size());
                    size_t chunkLen = endVal - startVal;

                    futures.push_back(threadPool_.enqueue(
                        [&source, &target, unmapper, bitOffset, startVal, chunkLen, this]() {
                            DecoderSeq<ValueType, DeltaType, DecodeFunc> localDecoder{
                                bpv_, blockSizeV_, static_cast<uint16_t>(rsiSizeB_), codeOptionSet_};
                            BitBuffer buffer{source};
                            buffer.bitPos(bitOffset);

                            size_t nFull = chunkLen / rsiSizeV_;
                            size_t remainder = chunkLen % rsiSizeV_;
                            std::vector<ValueType> localBuf(rsiSizeV_);

                            for (size_t i = 0; i < nFull; ++i) {
                                Span<ValueType> rsiSpan(localBuf.data(), rsiSizeV_);
                                localDecoder.decodeRefFullRsi(buffer, rsiSpan);
                                size_t base = startVal + i * rsiSizeV_;
                                for (size_t j = 0; j < rsiSizeV_; ++j) {
                                    target[base + j] = unmapper(localBuf[j]);
                                }
                            }

                            if (remainder > 0) {
                                Span<ValueType> rsiSpan(localBuf.data(), remainder);
                                localDecoder.decodeRefFullRsi(buffer, rsiSpan);
                                size_t base = startVal + nFull * rsiSizeV_;
                                for (size_t j = 0; j < remainder; ++j) {
                                    target[base + j] = unmapper(localBuf[j]);
                                }
                            }
                        }));

                    threadIdx++;
                    nextBoundary += nRsiPerThread;
                    if (threadIdx >= nThreads_) break;
                }
                this->skipFullRsi(scanBuf);
            }

            for (auto& f : futures) f.get();
        }

        void decode(Span<uint8_t> source, Span<ValueType> target)
        {
            std::vector<size_t> offsets = decodeRsiOffsets(BitBuffer{source}, target.size());

            auto nRsi = (target.size() + rsiSizeV_ - 1) / rsiSizeV_;
            auto nRsiPerThread = (nRsi + nThreads_ - 1) / nThreads_;
            auto chunkSizeV = nRsiPerThread * rsiSizeV_;
            auto chunks = SplitBlock<ValueType>(target, chunkSizeV);
            std::vector<std::future<void>> futures;
            futures.reserve(nThreads_);

            for (size_t t = 0; t < nThreads_; t++) {

                futures.push_back(
                threadPool_.enqueue([this, t, &source, &offsets, &chunks, nRsiPerThread]() {
                        // Per-thread decoder to avoid shared mutable state (rsiBuf_, paddedSegmentBuf_)
                        DecoderSeq<ValueType, DeltaType, DecodeFunc> localDecoder{
                            bpv_, blockSizeV_, static_cast<uint16_t>(rsiSizeB_), codeOptionSet_};

                        size_t bitOffset = offsets[t * nRsiPerThread];
                        assert(bitOffset / 8 <= source.size() && "Offset exceeds source size.");

                        BitBuffer buffer{source};
                        buffer.bitPos(bitOffset);

                        auto chunk = chunks.get(t);
                        SplitBlock<ValueType> rsis(chunk, rsiSizeV_);
                        for (size_t i = 0; i < rsis.nFull(); ++i) {
                            localDecoder.decodeRefFullRsi(buffer, rsis.full(i));
                        }
                        if (rsis.hasPart()) {
                            localDecoder.decodeRefFullRsi(buffer, rsis.part());
                        }
                }));
            }
            for (auto& future : futures) {
                future.get();
            }

        }

        DecoderPar(uint8_t bpv, uint8_t block_size, uint16_t rsiSize, CodeOptionSet codeOptionSet, size_t nThreads = 3) :
            DecoderSeq<ValueType, DeltaType, DecodeFunc>(bpv, block_size, rsiSize, codeOptionSet),
            nThreads_(nThreads)
        {}
        ~DecoderPar() {}

    private:
        using Base = DecoderSeq<ValueType, DeltaType, DecodeFunc>;

        const size_t nThreads_;
        mutable ThreadPool threadPool_{this->nThreads_};

        using Base::bpv_;
        using Base::blockSizeV_;
        using Base::codeOptionSet_;
        using Base::preprocessor_;
        using Base::maxIdLength;
        using Base::maxSegmentSizeV_;
        using Base::maxSegmentSizeB_;
        using Base::rsiSizeV_;
        using Base::rsiSizeB_;
        using Base::noCompressionId_;
};
