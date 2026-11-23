#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <future>
#include <vector>

#include "ccsds/CodeOption.h"
#include "ccsds/Span.h"
#include "ccsds/ThreadPool.h"
#include "ccsds/encoder/EncoderSeq.h"

namespace encoder
{

// Multi-threaded encoder whose output is bit-identical to EncoderSeq.
//
// Every RSI is encoded independently of the others: the preprocessor restarts at the first sample
// of each RSI, zero-block runs end at the end of a segment or RSI, and RSIs are packed back to back
// without padding (no AEC_PAD_RSI). The input is therefore split into contiguous chunks of whole
// RSIs (only the last chunk holds the trailing partial RSI). Each chunk is encoded by its own
// EncoderSeq into a zeroed scratch buffer, and the chunk bit streams are concatenated at their
// exact bit offsets. The encoding itself is shared with EncoderSeq, so the two cannot diverge.
//
// The concatenation is parallel too. Chunk t owns the output bytes whose first bit lies in chunk t.
// All bits of these bytes come from chunk t, except in its last owned byte, which may also contain
// bits of the following chunks (a chunk may be shorter than a byte, e.g. one all-zero RSI with few
// bits per value). Owned bytes are disjoint, so the threads never write the same byte.
template <typename ValueType, typename DeltaType, typename EncoderFunc = Identity<DeltaType>>
class EncoderPar
{
public:
    EncoderPar(uint8_t bpv, uint8_t blockSize, uint16_t rsiSize, CodeOptionSet codeOptionSet, size_t nThreads) :
        bpv_(bpv), blockSize_(blockSize), rsiSize_(rsiSize), codeOptionSet_(codeOptionSet), nThreads_(std::max<size_t>(nThreads, 1))
    {
    }

    // Encodes source into the zeroed target, which must hold at least scratchSize(source.size())
    // bytes, like for EncoderSeq. Returns the number of bytes written.
    size_t encode(const Span<ValueType> source, Span<uint8_t> target)
    {
        return (encodeBits(source, target) + 7) / 8;
    }

    // Same as encode(), but returns the exact number of bits written
    size_t encodeBits(const Span<ValueType> source, Span<uint8_t> target)
    {
        const size_t rsiValues = static_cast<size_t>(rsiSize_) * blockSize_;
        const size_t nRsi = (source.size() + rsiValues - 1) / rsiValues;
        const size_t nChunks = std::min(nThreads_, nRsi);
        if (nChunks <= 1) {
            EncoderSeq<ValueType, DeltaType, EncoderFunc> encoder{ bpv_, blockSize_, rsiSize_, codeOptionSet_ };
            return encoder.encodeBits(source, target);
        }

        // Balanced split of the RSIs: the first (nRsi % nChunks) chunks get one RSI more
        std::vector<size_t> firstValue(nChunks + 1);
        for (size_t t = 0; t <= nChunks; ++t) {
            const size_t firstRsi = t * (nRsi / nChunks) + std::min(t, nRsi % nChunks);
            firstValue[t] = std::min(firstRsi * rsiValues, source.size());
        }

        std::vector<std::vector<uint8_t>> scratch(nChunks);
        std::vector<size_t> nBits(nChunks, 0);
        ThreadPool pool{ nChunks };

        // Phase 1: encode the chunks
        runAll(pool, nChunks, [&](size_t t) {
            const size_t len = firstValue[t + 1] - firstValue[t];
            scratch[t].assign(scratchSize(len), 0);
            EncoderSeq<ValueType, DeltaType, EncoderFunc> encoder{ bpv_, blockSize_, rsiSize_, codeOptionSet_ };
            nBits[t] = encoder.encodeBits(Span<ValueType>(source.data() + firstValue[t], len),
                                          Span<uint8_t>(scratch[t].data(), scratch[t].size()));
        });

        std::vector<size_t> offset(nChunks + 1, 0);
        for (size_t t = 0; t < nChunks; ++t) {
            offset[t + 1] = offset[t] + nBits[t];
        }
        const size_t totalBits = offset[nChunks];
        const size_t totalBytes = (totalBits + 7) / 8;
        if (target.size() < totalBytes) {
            throw std::invalid_argument("CCSDS encoding: the output buffer is too small");
        }

        // Phase 2: concatenate. Chunk t owns the bytes [ceil(offset[t]/8), ceil(offset[t+1]/8)),
        // the last chunk also owns the final partial byte.
        runAll(pool, nChunks, [&](size_t t) {
            const size_t firstByte = (offset[t] + 7) / 8;
            const size_t endByte = (t + 1 == nChunks) ? totalBytes : (offset[t + 1] + 7) / 8;
            const uint8_t* src = scratch[t].data();
            for (size_t j = firstByte; j < endByte; ++j) {
                if (8 * j + 8 <= offset[t + 1] || t + 1 == nChunks) {
                    // All bits of the byte are in chunk t (bits after the end of the stream are zero)
                    const size_t s = 8 * j - offset[t];
                    const unsigned sh = s % 8;
                    const size_t i = s / 8;
                    uint8_t b = static_cast<uint8_t>(src[i] << sh);
                    if (sh != 0) {
                        b |= static_cast<uint8_t>(src[i + 1] >> (8 - sh));
                    }
                    target[j] = b;
                }
                else {
                    target[j] = gatherByte(j, t, offset, scratch);
                }
            }
        });

        return totalBits;
    }

    // Scratch/target size for n values: the same bound as SpAec::maxEncodedSize
    size_t scratchSize(size_t nValues) const
    {
        const size_t nBlocks = (nValues + blockSize_ - 1) / blockSize_;
        const size_t idLength = CodeOptionBase{ bpv_, CodeOptionSet::Basic }.maxIdLength - 1u;
        const size_t bitsPerBlock = idLength + static_cast<size_t>(blockSize_) * bpv_;
        return (nBlocks * bitsPerBlock + 7) / 8 + 8;
    }

private:
    // Assembles output byte j bit by bit from chunk t and the chunks after it
    static uint8_t gatherByte(size_t j, size_t t, const std::vector<size_t>& offset,
                              const std::vector<std::vector<uint8_t>>& scratch)
    {
        const size_t nChunks = scratch.size();
        uint8_t b = 0;
        size_t c = t;
        for (size_t k = 0; k < 8; ++k) {
            const size_t bit = 8 * j + k;
            while (c < nChunks && bit >= offset[c + 1]) {
                ++c;
            }
            if (c == nChunks) {
                break;  // beyond the end of the stream
            }
            const size_t s = bit - offset[c];
            const unsigned v = (scratch[c][s / 8] >> (7 - s % 8)) & 1u;
            b = static_cast<uint8_t>(b | (v << (7 - k)));
        }
        return b;
    }

    // Runs f(0..n-1) on the pool, waits for all tasks and rethrows the first error
    template <typename F>
    static void runAll(ThreadPool& pool, size_t n, F f)
    {
        std::vector<std::future<void>> futures;
        futures.reserve(n);
        std::exception_ptr firstError;
        try {
            for (size_t t = 0; t < n; ++t) {
                futures.push_back(pool.enqueue([&f, t]() { f(t); }));
            }
        }
        catch (...) {
            firstError = std::current_exception();
        }
        // Tasks reference this stack frame: always wait for all of them
        for (auto& fut : futures) {
            try {
                fut.get();
            }
            catch (...) {
                if (!firstError) {
                    firstError = std::current_exception();
                }
            }
        }
        if (firstError) {
            std::rethrow_exception(firstError);
        }
    }

    uint8_t bpv_;
    uint8_t blockSize_;
    uint16_t rsiSize_;
    CodeOptionSet codeOptionSet_;
    size_t nThreads_;
};

}  // namespace encoder
