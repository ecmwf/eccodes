/*
 * SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <vector>

#if defined(__AVX2__)
    #include <immintrin.h>
#endif

#include "spaec/detail/Utils.h"
#include "spaec/detail/CodeOption.h"
#include "spaec/detail/Span.h"

namespace spaec::detail
{


namespace encoder
{

template <typename ValueType, typename DeltaType>
class EncoderBase
{
public:
    // Selects the split-sample parameter k. The size of a split-sample block as a function of k has a
    // single minimum (Kiely, "Selecting the Golomb Parameter in Rice Coding", 2004). The search
    // starts at the k of the previous split-sample assessment (splitK_, carried across blocks and
    // RSIs, 0 at the start of a stream) and moves up, then down, while the size strictly decreases.
    // When two k give the same size, the result therefore depends on the previous k.
    // One pass over the block computes the sum and the sizes for the previous k and its neighbours
    // (CCSDS 120.0-G-4, 5.5: by convexity, the signs of the differences of neighbouring sizes
    // identify the minimum); further passes are needed only when the minimum is farther away.
    // Returns k, the total block size (ID, reference and data bits) and the sum of the values.
    template <ReferenceType RType>
    auto selectBestMethod(Span<DeltaType> values)
    {
        constexpr size_t startIndex = (RType == ReferenceType::Ref) ? 1 : 0;
        const uint64_t thisBs = blockSizeV_ - startIndex;
        const DeltaType* v = values.data() + startIndex;

        struct Result
        {
            uint8_t k;
            uint64_t size;
            uint64_t blockSum;
        };

        // The restricted set for 1 and 2 bits has no split-sample options: only the sum
        if (!limits_.available_) {
            uint64_t sum = 0;
            for (size_t j = 0; j < thisBs; ++j) {
                sum += v[j];
            }
            return Result{ 0, UINT64_MAX, sum };
        }

        const unsigned k0 = splitK_;
        const unsigned kUp = k0 < kMax ? k0 + 1 : k0;
        const unsigned kDown = k0 > 0 ? k0 - 1 : k0;
        uint64_t sum = 0, fsAt = 0, fsUp = 0, fsDown = 0;
        bool summed = false;
#if defined(__AVX2__)
        if constexpr (sizeof(DeltaType) == 2) {
            if (thisBs % 16 == 0) {
                // 16 samples per step, the sums in 32-bit lanes (at most 64 samples of 16 bits)
                const __m256i zero = _mm256_setzero_si256();
                const __m128i shiftAt = _mm_cvtsi32_si128(static_cast<int>(k0));
                const __m128i shiftUp = _mm_cvtsi32_si128(static_cast<int>(kUp));
                const __m128i shiftDown = _mm_cvtsi32_si128(static_cast<int>(kDown));
                __m256i aSum = zero, aAt = zero, aUp = zero, aDown = zero;
                const auto widen = [&zero](__m256i w) { return _mm256_add_epi32(_mm256_unpacklo_epi16(w, zero), _mm256_unpackhi_epi16(w, zero)); };
                for (size_t j = 0; j < thisBs; j += 16) {
                    const __m256i x = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(v + j));
                    aSum = _mm256_add_epi32(aSum, widen(x));
                    aAt = _mm256_add_epi32(aAt, widen(_mm256_srl_epi16(x, shiftAt)));
                    aUp = _mm256_add_epi32(aUp, widen(_mm256_srl_epi16(x, shiftUp)));
                    aDown = _mm256_add_epi32(aDown, widen(_mm256_srl_epi16(x, shiftDown)));
                }
                const auto total = [](__m256i a) {
                    __m128i t = _mm_add_epi32(_mm256_castsi256_si128(a), _mm256_extracti128_si256(a, 1));
                    t = _mm_add_epi32(t, _mm_shuffle_epi32(t, 0x4E));
                    t = _mm_add_epi32(t, _mm_shuffle_epi32(t, 0xB1));
                    return static_cast<uint64_t>(static_cast<uint32_t>(_mm_cvtsi128_si32(t)));
                };
                sum = total(aSum);
                fsAt = total(aAt);
                fsUp = total(aUp);
                fsDown = total(aDown);
                summed = true;
            }
        }
#endif
        if (!summed) {
            for (size_t j = 0; j < thisBs; ++j) {
                const uint64_t x = static_cast<uint64_t>(v[j]);
                sum += x;
                fsAt += x >> k0;
                fsUp += x >> kUp;
                fsDown += x >> kDown;
            }
        }

        auto fsLength = [&](unsigned k) {
            if (k == k0) return fsAt;
            if (k == kUp) return fsUp;
            if (k == kDown) return fsDown;
            uint64_t fs = 0;
            for (size_t j = 0; j < thisBs; ++j) {
                fs += static_cast<uint64_t>(v[j] >> k);
            }
            return fs;
        };

        uint64_t lenMin = UINT64_MAX;
        unsigned k = k0;
        unsigned kBest = k;
        bool noTurn = (k == 0);  // do not reverse the direction
        bool up = true;
        for (;;) {
            const uint64_t fs = fsLength(k);
            const uint64_t len = fs + thisBs * (k + 1);
            if (len < lenMin) {
                if (lenMin < UINT64_MAX) {
                    noTurn = true;
                }
                lenMin = len;
                kBest = k;
                if (up) {
                    if (fs < thisBs || k >= kMax) {
                        if (noTurn) {
                            break;
                        }
                        k = splitK_ - 1;
                        up = false;
                        noTurn = true;
                    }
                    else {
                        ++k;
                    }
                }
                else {
                    if (fs >= thisBs || k == 0) {
                        break;
                    }
                    --k;
                }
            }
            else {
                if (noTurn) {
                    break;
                }
                k = splitK_ - 1;
                up = false;
                noTurn = true;
            }
        }
        splitK_ = static_cast<uint8_t>(kBest);

        uint64_t fixedOverhead = ssId_.length;
        if constexpr (RType == ReferenceType::Ref) {
            fixedOverhead += bpv_;
        }

        return Result{ static_cast<uint8_t>(kBest), fixedOverhead + lenMin, sum };
    }

    // Split-sample k state (see selectBestMethod). The parallel encoder sets it at chunk starts.
    uint8_t splitK() const { return splitK_; }
    void splitK(uint8_t k) { splitK_ = k; }

    EncoderBase(uint8_t bpv, uint8_t block_size, uint16_t rsiSize, CodeOptionSet codeOptionSet) :
        bpv_((validateCcsdsParameters(bpv, block_size, rsiSize, 8 * sizeof(DeltaType)), bpv)),
        blockSizeV_(block_size),
        rsiSizeB_(rsiSize),
        rsiSizeV_(rsiSize * block_size),
        codeOptionSet_(codeOptionSet),
        zbId_{ bpv_, codeOptionSet },
        ncId_{ bpv_, codeOptionSet },
        ssId_{ bpv_, codeOptionSet },
        seId_{ bpv_, codeOptionSet },
        limits_{ bpv_, codeOptionSet }
    {
        for (uint8_t k = limits_.kMin_; limits_.available_ && k <= limits_.kMax_; ++k) {
            ssIdStore.emplace_back(bpv_, k, codeOptionSet_);
        }
    }
    ~EncoderBase() {}


public:
    const uint8_t bpv_;
    const uint8_t blockSizeV_;
    static constexpr size_t segmentSizeB_ = 64;
    const size_t segmentSizeV_ = segmentSizeB_ * blockSizeV_;
    const size_t rsiSizeB_;
    const size_t rsiSizeV_;

    CodeOptionSet codeOptionSet_;

    const ZeroBlockCodeOption zbId_;
    const NoCompressionCodeOption ncId_;
    const SplitSampleIdLength ssId_;
    const SecondExtensionCodeOption seId_;


    const SplitSampleLimits limits_;
    const size_t kMin = limits_.kMin_;
    const size_t kMax = limits_.kMax_;

    std::vector<SplitSampleCodeOption> ssIdStore;

private:
    uint8_t splitK_ = 0;
};

}  // namespace encoder

}  // namespace spaec::detail
