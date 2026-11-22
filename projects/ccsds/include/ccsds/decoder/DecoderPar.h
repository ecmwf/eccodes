#pragma once

#include <vector>
#include <assert.h>
#include <algorithm>
#include <exception>

#include "ccsds/Span.h"
#include "ccsds/CodeOption.h"
#include "ccsds/BitBuffer.h"
#include "ccsds/ThreadPool.h"
#include "ccsds/decoder/DecoderSeq.h"

template <typename ValueType, typename DeltaType, typename DecodeFunc = Identity<DeltaType>>
class DecoderPar : public DecoderSeq<ValueType, DeltaType, DecodeFunc>
{
public:
    // Bit offsets of all RSIs of nValues values, including a trailing partial RSI.
    // buffer must be followed by BitBuffer::guardBytes readable bytes (see DecoderSeq::makeBuffer).
    std::vector<size_t> decodeRsiOffsets(const BitBuffer& buffer, size_t nValues)
    {
        const size_t nRsis = (nValues + rsiSizeV_ - 1) / rsiSizeV_;
        std::vector<size_t> rsiOffsets;
        rsiOffsets.reserve(nRsis);
        for (size_t i = 0; i < nRsis; ++i) {
            buffer.checkPos();
            rsiOffsets.push_back(buffer.bitPos());
            if (i + 1 < nRsis) {
                this->skipFullRsi(buffer);
            }
        }
        return rsiOffsets;
    }

    std::vector<size_t> decodeRsiOffsets(Span<uint8_t> source, size_t nValues)
    {
        return decodeRsiOffsets(this->makeBuffer(source), nValues);
    }

    // Decodes target.size() values in parallel: each thread decodes a contiguous range of RSIs.
    // Errors in the compressed data are reported as exceptions (see DecoderSeq).
    template <typename OutputType, typename UnmapFunc>
    void decodeParallel(Span<uint8_t> source, Span<OutputType> target, UnmapFunc unmapper)
    {
        if (target.size() == 0) {
            return;
        }

        // Shared read-only copy of the source followed by zero guard bytes
        const BitBuffer padded = this->makeBuffer(source);

        // Count the trailing partial RSI, and never start more threads than there are RSIs
        const size_t nRsi = (target.size() + rsiSizeV_ - 1) / rsiSizeV_;
        const size_t nThreads = std::min(nThreads_, nRsi);
        const size_t nRsiPerThread = (nRsi + nThreads - 1) / nThreads;

        const BitBuffer scanBuf{ padded, 0 };
        std::vector<std::future<void>> futures;
        futures.reserve(nThreads);

        // Waits for all started threads (they reference local variables) and rethrows the first error
        auto waitAll = [&futures]() {
            std::exception_ptr firstError;
            for (auto& f : futures) {
                try {
                    f.get();
                }
                catch (...) {
                    if (!firstError) {
                        firstError = std::current_exception();
                    }
                }
            }
            futures.clear();
            if (firstError) {
                std::rethrow_exception(firstError);
            }
        };

        try {
            for (size_t t = 0; t < nThreads; ++t) {
                const size_t firstRsi = t * nRsiPerThread;
                if (firstRsi >= nRsi) {
                    break;
                }
                const size_t startVal = firstRsi * rsiSizeV_;
                const size_t endVal = std::min((firstRsi + nRsiPerThread) * rsiSizeV_, target.size());
                const size_t chunkLen = endVal - startVal;
                const size_t bitOffset = scanBuf.bitPos();

                futures.push_back(threadPool_.enqueue(
                    [&padded, &target, unmapper, bitOffset, startVal, chunkLen, this]() {
                        // Per-thread decoder to avoid shared mutable state (rsiBuf_, paddedSegmentBuf_)
                        DecoderSeq<ValueType, DeltaType, DecodeFunc> localDecoder{
                            bpv_, blockSizeV_, static_cast<uint16_t>(rsiSizeB_), codeOptionSet_
                        };
                        localDecoder.decodeFusedAt(padded, bitOffset, chunkLen,
                                                   [&](auto value, size_t idx) {
                                                       target[startVal + idx] = unmapper(value);
                                                   });
                    }));

                // Find the start of the next thread's first RSI (all RSIs before it are full)
                if (t + 1 < nThreads) {
                    for (size_t r = 0; r < nRsiPerThread && firstRsi + r + 1 < nRsi; ++r) {
                        scanBuf.checkPos();
                        this->skipFullRsi(scanBuf);
                    }
                }
            }
        }
        catch (...) {
            // Do not leave threads running that reference this stack frame
            try {
                waitAll();
            }
            catch (...) {
            }
            throw;
        }
        waitAll();
    }

    void decode(Span<uint8_t> source, Span<ValueType> target)
    {
        decodeParallel(source, target, [](ValueType value) { return value; });
    }

    DecoderPar(uint8_t bpv, uint8_t block_size, uint16_t rsiSize, CodeOptionSet codeOptionSet, size_t nThreads = 3) :
        DecoderSeq<ValueType, DeltaType, DecodeFunc>(bpv, block_size, rsiSize, codeOptionSet),
        nThreads_(nThreads)
    {
    }
    ~DecoderPar() {}

private:
    using Base = DecoderSeq<ValueType, DeltaType, DecodeFunc>;

    const size_t nThreads_;
    mutable ThreadPool threadPool_{ this->nThreads_ };

    using Base::blockSizeV_;
    using Base::bpv_;
    using Base::codeOptionSet_;
    using Base::maxIdLength;
    using Base::maxSegmentSizeB_;
    using Base::maxSegmentSizeV_;
    using Base::noCompressionId_;
    using Base::preprocessor_;
    using Base::rsiSizeB_;
    using Base::rsiSizeV_;
};
