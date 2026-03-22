#pragma once

#include <vector>
#include <bit>
#include <assert.h>

#include "Utils.h"
#include "CodeOption.h"
#include "Preprocessor.h"
#include "Span.h"


namespace encoder {

template <typename ValueType, typename DeltaType>
class EncoderBase {
    public:
        // Estimate the optimal GPO2 code parameter k using the block mean,
        // then compute exact cost for kEst only.
        // Based on Kiely, "Selecting the Golomb Parameter in Rice Coding" (2004).
        // Paper Eq. 11 guarantees kEst is within the optimal range.
        template <ReferenceType RType>
        auto selectBestMethod(Span<DeltaType> values) {
            constexpr size_t startIndex = (RType == ReferenceType::Ref) ? 1 : 0;
            const size_t nEncoded = blockSizeV_ - startIndex;

            // Single pass: compute block sum and shifted sum for kEst
            uint64_t sum = 0;
            for (size_t j = startIndex; j < blockSizeV_; ++j) {
                sum += values[j];
            }

            // Estimate optimal k (Eq. 10): k̂ = max{0, ⌊log2(µ + 49/128)⌋}
            size_t kEst = 0;
            {
                uint64_t num = 128 * sum + 49 * static_cast<uint64_t>(nEncoded);
                uint64_t den = 128 * static_cast<uint64_t>(nEncoded);
                if (num >= den) {
                    kEst = std::bit_width(num / den) - 1;
                }
            }
            if (kEst < kMin) kEst = kMin;
            if (kEst > kMax) kEst = kMax;

            // Compute exact size for kEst only
            uint64_t sumMSBs = 0;
            for (size_t j = startIndex; j < blockSizeV_; ++j) {
                sumMSBs += (values[j] >> kEst);
            }

            size_t fixedOverhead;
            if constexpr (RType == ReferenceType::Ref) {
                fixedOverhead = ssId_.length + bpv_;
            } else {
                fixedOverhead = ssId_.length;
            }
            size_t bestSize = fixedOverhead + (sumMSBs + nEncoded) + kEst * nEncoded;

            struct Result { uint8_t k; size_t size; uint64_t blockSum; };
            return Result{static_cast<uint8_t>(kEst), bestSize, sum};
        }

        template <ReferenceType RType>
        size_t computeSize(Span<DeltaType> values, uint8_t k) const {
            size_t sumMSBs = 0;
            constexpr size_t startIndex = (RType == ReferenceType::Ref) ? 1 : 0;
            const size_t nEncoded = blockSizeV_ - startIndex;
            for (size_t j = startIndex; j < blockSizeV_; ++j) {
                sumMSBs += (values[j] >> k);
            }

            if constexpr (RType == ReferenceType::Ref) {
                return ssId_.length + bpv_ + (sumMSBs + nEncoded) + k * nEncoded;
            } else {
                return ssId_.length + (sumMSBs + nEncoded) + k * nEncoded;
            }
        }

        EncoderBase(uint8_t bpv, uint8_t block_size, uint16_t rsiSize, CodeOptionSet codeOptionSet) :
            bpv_(bpv),
            blockSizeV_(block_size),
            rsiSizeB_(rsiSize),
            rsiSizeV_(rsiSize * block_size),
            codeOptionSet_(codeOptionSet),
            zbId_{bpv_, codeOptionSet},
            ncId_{bpv_, codeOptionSet},
            ssId_{bpv_, codeOptionSet},
            seId_{bpv_, codeOptionSet},
            limits_{bpv_, codeOptionSet}
        {
            for (uint8_t k = limits_.kMin_; k <= limits_.kMax_; ++k) {
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

        std::vector<SplitSampleCodeOption<DeltaType>> ssIdStore;
};

}  // namespace encoder
