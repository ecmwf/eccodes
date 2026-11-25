/*
 * SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <future>
#include <stdexcept>
#include <vector>

#include "spaec/detail/BitBuffer.h"
#include "spaec/detail/CodeOption.h"
#include "spaec/detail/Span.h"
#include "spaec/detail/ThreadPool.h"
#include "spaec/detail/encoder/EncoderSeq.h"

namespace spaec::detail
{

namespace encoder
{

// Multi-threaded encoder whose output is bit-identical to EncoderSeq.
//
// RSIs are packed back to back without padding, the preprocessor restarts at the
// first sample of each RSI, and zero-block runs end at the end of a segment or RSI. The only state
// carried from one RSI to the next is the split-sample k at which the search of the next block
// starts (see EncoderBase::selectBestMethod).
//
// Phase 1 (parallel): the input is split into contiguous chunks of whole RSIs (only the last chunk
// holds the trailing partial RSI). Each chunk is encoded by its own EncoderSeq into a zeroed scratch
// buffer, starting with the guess k = 0, and the k at the start of each of its RSIs is recorded.
//
// Fix-up (sequential, usually cheap): the real k at the start of chunk t is the final k of chunk
// t-1. If it differs from the guess, the leading RSIs of chunk t are encoded again from the real k
// until the k after an RSI equals the k recorded at the start of the next RSI. From there on the
// encoder state, and therefore the output, is the same as in phase 1 and is reused.
//
// Phase 2 (parallel): the resulting pieces of bit streams are concatenated at their exact bit
// offsets. Piece p owns the output bytes whose first bit lies in piece p, so no byte is written by
// two threads. All bits of an owned byte come from piece p, except in its last owned byte, which
// may also contain bits of the following pieces (a piece may be shorter than a byte) and is
// assembled bit by bit.
template <typename ValueType, typename DeltaType, typename EncoderFunc = Identity<DeltaType>>
class EncoderPar
{
public:
    EncoderPar(uint8_t bpv, uint8_t blockSize, uint16_t rsiSize, CodeOptionSet codeOptionSet, size_t nThreads, Preprocessing preprocessing = {}) :
        bpv_(bpv), blockSize_(blockSize), rsiSize_(rsiSize), codeOptionSet_(codeOptionSet), nThreads_(std::max<size_t>(nThreads, 1)), preprocessing_((validatePreprocessing(preprocessing), preprocessing))
    {
    }

    // Encodes source into the zeroed target, which must hold at least scratchSize(source.size())
    // bytes, like for EncoderSeq. Returns the number of bytes written (an empty stream is one zero
    // byte, like in EncoderSeq).
    size_t encode(const Span<ValueType> source, Span<uint8_t> target)
    {
        return std::max<size_t>((encodeBits(source, target) + 7) / 8, 1);
    }

    // Same as encode(), but returns the exact number of bits written
    size_t encodeBits(const Span<ValueType> source, Span<uint8_t> target)
    {
        const size_t rsiValues = static_cast<size_t>(rsiSize_) * blockSize_;
        const size_t nRsi = (source.size() + rsiValues - 1) / rsiValues;
        const size_t nChunks = std::min(nThreads_, nRsi);
        if (nChunks <= 1) {
            Encoder encoder{ bpv_, blockSize_, rsiSize_, codeOptionSet_, preprocessing_ };
            // The sequential encoder does not check the size of the target: give it one of the
            // worst-case size (every block uncompressed, plus the 8 bytes it may touch after the
            // data), and copy the result if the target is smaller
            const uint64_t nBlocks = (source.size() + blockSize_ - 1) / blockSize_;
            const uint64_t idLength = CodeOptionBase{ bpv_, codeOptionSet_ }.maxIdLength - 1u;
            const uint64_t worstCase = (nBlocks * (idLength + uint64_t{ blockSize_ } * bpv_) + 7) / 8 + 8;
            if (target.size() >= worstCase) {
                return encoder.encodeBits(source, target);
            }
            std::vector<uint8_t> scratch(static_cast<size_t>(worstCase), 0);
            const size_t bits = encoder.encodeBits(source, Span<uint8_t>(scratch));
            const size_t bytes = (bits + 7) / 8;
            if (target.size() < bytes) {
                throw std::invalid_argument("CCSDS encoding: the output buffer is too small");
            }
            std::memcpy(target.data(), scratch.data(), bytes);
            return bits;
        }

        // Balanced split of the RSIs: the first (nRsi % nChunks) chunks get one RSI more
        std::vector<size_t> firstRsi(nChunks + 1);
        for (size_t t = 0; t <= nChunks; ++t) {
            firstRsi[t] = t * (nRsi / nChunks) + std::min(t, nRsi % nChunks);
        }

        std::vector<Chunk> chunks(nChunks);
        ThreadPool pool{ nChunks };

        // Phase 1: encode the chunks, guessing k = 0 at their start
        runAll(pool, nChunks, [&](size_t t) {
            Chunk& c = chunks[t];
            const size_t first = firstRsi[t];
            const size_t nR = firstRsi[t + 1] - first;
            c.scratch.assign(scratchSize(rsiRangeValues(source.size(), first, nR)), 0);
            c.kIn.resize(nR);
            c.rsiEnd.resize(nR);
            Encoder encoder{ bpv_, blockSize_, rsiSize_, codeOptionSet_, preprocessing_ };
            encoder.splitK(0);
            BitBuffer buffer(Span<uint8_t>(c.scratch.data(), c.scratch.size()));
            for (size_t r = 0; r < nR; ++r) {
                c.kIn[r] = encoder.splitK();
                encoder.encodeRsi(rsiSpan(source, first + r), buffer);
                c.rsiEnd[r] = buffer.bitPos();
            }
            c.kOut = encoder.splitK();
            c.pieces.push_back(Piece{ c.scratch.data(), 0, c.rsiEnd[nR - 1] });
        });

        // Fix-up: re-encode the leading RSIs of a chunk whose real starting k differs from the guess
        for (size_t t = 1; t < nChunks; ++t) {
            Chunk& c = chunks[t];
            const uint8_t kReal = chunks[t - 1].kOut;
            if (kReal == c.kIn[0]) {
                continue;
            }
            const size_t first = firstRsi[t];
            const size_t nR = c.kIn.size();
            // The k usually converges after one RSI: first try with a small buffer, and only if
            // that is not enough, encode again with a buffer for the whole chunk
            size_t r = 0;
            bool converged = false;
            size_t prefixBits = 0;
            for (size_t limit = std::min<size_t>(nR, 4);; limit = nR) {
                Encoder encoder{ bpv_, blockSize_, rsiSize_, codeOptionSet_, preprocessing_ };
                encoder.splitK(kReal);
                c.prefix.assign(scratchSize(rsiRangeValues(source.size(), first, limit)), 0);
                BitBuffer buffer(Span<uint8_t>(c.prefix.data(), c.prefix.size()));
                for (r = 0; r < limit; ++r) {
                    encoder.encodeRsi(rsiSpan(source, first + r), buffer);
                    if (r + 1 < nR && encoder.splitK() == c.kIn[r + 1]) {
                        converged = true;
                        break;
                    }
                }
                prefixBits = buffer.bitPos();
                if (converged || limit == nR) {
                    c.kOut = converged ? c.kOut : encoder.splitK();
                    break;
                }
            }
            c.pieces.clear();
            c.pieces.push_back(Piece{ c.prefix.data(), 0, prefixBits });
            if (converged) {
                c.pieces.push_back(Piece{ c.scratch.data(), c.rsiEnd[r], c.rsiEnd[nR - 1] - c.rsiEnd[r] });
            }
        }

        // Output positions of the pieces
        std::vector<Piece> pieces;
        for (const Chunk& c : chunks) {
            for (const Piece& p : c.pieces) {
                if (p.nBits != 0) {
                    pieces.push_back(p);
                }
            }
        }
        std::vector<size_t> dst(pieces.size() + 1, 0);
        for (size_t p = 0; p < pieces.size(); ++p) {
            dst[p + 1] = dst[p] + pieces[p].nBits;
        }
        const size_t totalBits = dst[pieces.size()];
        const size_t totalBytes = (totalBits + 7) / 8;
        if (target.size() < totalBytes) {
            throw std::invalid_argument("CCSDS encoding: the output buffer is too small");
        }

        // Phase 2: concatenate. Piece p owns the bytes [ceil(dst[p]/8), ceil(dst[p+1]/8)).
        runAll(pool, pieces.size(), [&](size_t p) {
            const size_t firstByte = (dst[p] + 7) / 8;
            const size_t endByte = (dst[p + 1] + 7) / 8;
            const Piece& piece = pieces[p];
            for (size_t j = firstByte; j < endByte; ++j) {
                if (8 * j + 8 <= dst[p + 1]) {
                    // All bits of the byte are in piece p
                    const size_t s = piece.srcBit + (8 * j - dst[p]);
                    const unsigned sh = s % 8;
                    const size_t i = s / 8;
                    uint8_t b = static_cast<uint8_t>(piece.src[i] << sh);
                    if (sh != 0) {
                        b |= static_cast<uint8_t>(piece.src[i + 1] >> (8 - sh));
                    }
                    target[j] = b;
                }
                else {
                    target[j] = gatherByte(j, p, dst, pieces);
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
    using Encoder = EncoderSeq<ValueType, DeltaType, EncoderFunc>;

    // A part of the output: nBits bits of src starting at bit srcBit. src is followed by at least
    // one readable byte (scratch buffers have slack), and its bits after the piece are not used.
    struct Piece
    {
        const uint8_t* src;
        size_t srcBit;
        size_t nBits;
    };

    struct Chunk
    {
        std::vector<uint8_t> scratch;  // phase 1 output
        std::vector<uint8_t> prefix;   // re-encoded leading RSIs (fix-up)
        std::vector<uint8_t> kIn;      // split-sample k at the start of each RSI (phase 1)
        std::vector<size_t> rsiEnd;    // end bit of each RSI in scratch
        uint8_t kOut = 0;              // split-sample k after the last RSI
        std::vector<Piece> pieces;
    };

    Span<ValueType> rsiSpan(const Span<ValueType> source, size_t rsi) const
    {
        const size_t rsiValues = static_cast<size_t>(rsiSize_) * blockSize_;
        const size_t start = rsi * rsiValues;
        return Span<ValueType>(source.data() + start, std::min(rsiValues, source.size() - start));
    }

    size_t rsiRangeValues(size_t nValues, size_t firstRsi, size_t nR) const
    {
        const size_t rsiValues = static_cast<size_t>(rsiSize_) * blockSize_;
        return std::min(nValues, (firstRsi + nR) * rsiValues) - firstRsi * rsiValues;
    }

    // Assembles output byte j bit by bit from piece p and the pieces after it
    static uint8_t gatherByte(size_t j, size_t p, const std::vector<size_t>& dst, const std::vector<Piece>& pieces)
    {
        uint8_t b = 0;
        for (size_t k = 0; k < 8; ++k) {
            const size_t bit = 8 * j + k;
            while (p < pieces.size() && bit >= dst[p + 1]) {
                ++p;
            }
            if (p == pieces.size()) {
                break;  // beyond the end of the stream: zero bits
            }
            const size_t s = pieces[p].srcBit + (bit - dst[p]);
            const unsigned v = (pieces[p].src[s / 8] >> (7 - s % 8)) & 1u;
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
    Preprocessing preprocessing_;
};

}  // namespace encoder

}  // namespace spaec::detail
