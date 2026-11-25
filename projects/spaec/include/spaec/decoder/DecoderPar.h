/*
 * SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <vector>

#include "spaec/Span.h"
#include "spaec/CodeOption.h"
#include "spaec/Preprocessor.h"
#include "spaec/BitBuffer.h"
#include "spaec/ThreadPool.h"
#include "spaec/decoder/DecoderSeq.h"

namespace spaec
{

template <typename ValueType, typename DeltaType, typename DecodeFunc = Identity<DeltaType>>
class DecoderPar : public DecoderSeq<ValueType, DeltaType, DecodeFunc>
{
public:
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

        // The pool is sized for this decode: oversized pools would make thread creation
        // fail when several decodes run concurrently
        ThreadPool threadPool{ nThreads };

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

                futures.push_back(threadPool.enqueue(
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

    DecoderPar(uint8_t bpv, uint8_t block_size, uint16_t rsiSize, CodeOptionSet codeOptionSet, size_t nThreads = 3) :
        DecoderSeq<ValueType, DeltaType, DecodeFunc>(bpv, block_size, rsiSize, codeOptionSet),
        nThreads_(nThreads)
    {
    }
    ~DecoderPar() {}

private:
    using Base = DecoderSeq<ValueType, DeltaType, DecodeFunc>;

    const size_t nThreads_;

    using Base::blockSizeV_;
    using Base::bpv_;
    using Base::codeOptionSet_;
    using Base::rsiSizeB_;
    using Base::rsiSizeV_;
};

}  // namespace spaec
