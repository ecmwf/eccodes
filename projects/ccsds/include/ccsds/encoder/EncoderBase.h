#pragma once

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <vector>

#include "ccsds/Utils.h"
#include "ccsds/CodeOption.h"
#include "ccsds/Span.h"


namespace encoder
{

template <typename ValueType, typename DeltaType>
class EncoderBase
{
public:
    // Selects the split-sample parameter k exactly like libaec (assess_splitting_option), so that
    // the encoded streams are identical. The size of a split-sample block as a function of k has a
    // single minimum (Kiely, "Selecting the Golomb Parameter in Rice Coding", 2004). The search
    // starts at the k of the previous split-sample assessment (splitK_, carried across blocks and
    // RSIs, 0 at the start of a stream) and moves up, then down, while the size strictly decreases.
    // When two k give the same size, the result therefore depends on the previous k, like in libaec.
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

        auto fsLength = [&](unsigned k) {
            uint64_t fs = 0;
            for (size_t j = 0; j < thisBs; ++j) {
                fs += static_cast<uint64_t>(v[j] >> k);
            }
            return fs;
        };

        uint64_t sum = 0;
        uint64_t lenMin = UINT64_MAX;
        unsigned k = splitK_;
        unsigned kBest = k;
        bool noTurn = (k == 0);  // do not reverse the direction
        bool up = true;
        for (;;) {
            const uint64_t fs = fsLength(k);
            if (k == 0) {
                sum = fs;
            }
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

        if (kBest != 0) {
            sum = 0;
            for (size_t j = 0; j < thisBs; ++j) {
                sum += v[j];
            }
        }

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
