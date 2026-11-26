/*
 * SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "spaec/detail/Isa.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <limits>
#include <memory>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>
#include "spaec/detail/CodeOption.h"
#include "spaec/detail/Span.h"
#include "spaec/detail/Utils.h"
#include "spaec/detail/encoder/EncoderSeq.h"
#include "spaec/detail/encoder/EncoderPar.h"
#include "spaec/detail/decoder/DecoderSeq.h"
#include "spaec/detail/decoder/DecoderPar.h"

namespace spaec::detail
{
inline namespace SPAEC_ISA
{

template <typename T>
T grib_power(int s, int n)
{
    T divisor = 1.0;
    if (s == 0) return static_cast<T>(1.0);
    if (s == 1) return static_cast<T>(n);
    while (s < 0) {
        divisor /= n;
        s++;
    }
    while (s > 0) {
        divisor *= n;
        s--;
    }
    return divisor;
}


namespace encoder
{

template <typename ValueType>
class SpAec
{
    // The values are scaled with powers of 2 and 10 (an integer type would truncate 10^-D to 0)
    static_assert(std::is_floating_point<ValueType>::value, "SpAec requires a floating-point value type");

public:
    // Mapping between the values and the coded integers (public for the unit tests)
    template <typename MappedType>
    class SimpleMapper
    {
    public:
        SimpleMapper(double referenceValue, int binaryScaleFactor, int decimalScaleFactor) :
            referenceValue_(referenceValue),
            decimal_{ grib_power<double>(decimalScaleFactor, 10) },
            binary_{ grib_power<double>(-binaryScaleFactor, 2) }
        {
        }

        MappedType operator()(ValueType value) const
        {
            return static_cast<MappedType>(mapped(value));
        }

        // The value before the conversion to MappedType (to check its range)
        double mapped(ValueType value) const
        {
            return (static_cast<double>(value) * decimal_ - referenceValue_) * binary_ + 0.5;
        }

    private:
        const double referenceValue_;
        // The mapping computes in double for every value type (like the unmapper of the decoder):
        // the reference value and the factors are doubles and the value is converted before it is scaled, so the coder of
        // float values gives the samples of the coder of the converted doubles. Mapping in float
        // rounds the intermediates (the difference of two floats near a large reference value
        // needs more than 24 bits) and gives other samples than a double compression.
        const double decimal_;
        const double binary_;
    };


    template <typename MappedType>
    class SimpleUnmapper
    {
    public:
        // Like legacy ecCodes: the powers are computed in ValueType, the scaling in double, and
        // the result is rounded to ValueType once. Scaling in float would lose the low bits of
        // samples with more than 24 bits and cancel catastrophically near zero.
        SimpleUnmapper(double referenceValue, int binaryScaleFactor, int decimalScaleFactor) :
            referenceValue_(referenceValue),
            bscale_{ grib_power<ValueType>(binaryScaleFactor, 2) },
            dscale_{ grib_power<ValueType>(-decimalScaleFactor, 10) }
        {
        }

        ValueType operator()(MappedType mapped) const
        {
            return static_cast<ValueType>((static_cast<double>(mapped) * bscale_ + referenceValue_) * dscale_);
        }

    private:
        const double referenceValue_;
        const double bscale_;
        const double dscale_;
    };


public:
    // prediction: the preprocessing of the coded integers (CCSDS 121.0-B-3, 4); they are unsigned
    SpAec(uint8_t bpv, uint8_t blockSize, uint16_t rsiSize, CodeOptionSet codeOptionSet, double referenceValue, int binaryScaleFactor = 0, int decimalScaleFactor = 0, uint8_t nThreads = 1,
          Prediction prediction = Prediction::UnitDelay, SplitSelector selector = SplitSelector::Search) :
        bpv_(bpv),
        blockSize_(blockSize),
        rsiSize_(rsiSize),
        nThreads_(nThreads),
        referenceValue_(referenceValue),
        binaryScaleFactor_(binaryScaleFactor),
        decimalScaleFactor_(decimalScaleFactor),
        codeOptionSet_(effectiveCodeOptionSet(codeOptionSet, bpv)),
        preprocessing_{ prediction, false },
        selector_(selector)
    {
        validatePreprocessing(preprocessing_);
        validateCcsdsParameters(bpv_, blockSize_, rsiSize_);
        if (nThreads_ == 0) {
            throw std::invalid_argument("CCSDS: the number of threads must be at least 1");
        }
        // GRIB stores the scale factors in 16 bits (sign and magnitude)
        if (binaryScaleFactor_ < -maxScaleFactor || binaryScaleFactor_ > maxScaleFactor) {
            throw std::invalid_argument("CCSDS: binary scale factor " + std::to_string(binaryScaleFactor_) + " is out of range (must be between " + std::to_string(-maxScaleFactor) + " and " + std::to_string(maxScaleFactor) + ")");
        }
        if (decimalScaleFactor_ < -maxScaleFactor || decimalScaleFactor_ > maxScaleFactor) {
            throw std::invalid_argument("CCSDS: decimal scale factor " + std::to_string(decimalScaleFactor_) + " is out of range (must be between " + std::to_string(-maxScaleFactor) + " and " + std::to_string(maxScaleFactor) + ")");
        }
        if (!std::isfinite(static_cast<double>(referenceValue_))) {
            throw std::invalid_argument("CCSDS: the reference value must be a finite number");
        }
    }

    // The code option set used for bpv bits per value: the restricted set for 1 to 4 bits, rejected
    // for 5 to 8 bits, the basic set for more bits (CCSDS 121.0-B-3, Table 5-1: the same IDs)
    static CodeOptionSet effectiveCodeOptionSet(CodeOptionSet codeOptionSet, uint8_t bpv)
    {
        if (codeOptionSet != CodeOptionSet::Restricted) {
            return codeOptionSet;
        }
        if (bpv > 4 && bpv <= 8) {
            throw std::invalid_argument("CCSDS: the restricted set of code options does not exist for " + std::to_string(bpv) +
                                        " bits per value (only for 1 to 4 bits, and the basic set above 8 bits)");
        }
        return bpv <= 4 ? CodeOptionSet::Restricted : CodeOptionSet::Basic;
    }

    // Largest possible size in bytes of the encoded data of nValues values (every block
    // uncompressed), including the 8 bytes the encoder may touch after the end of the data.
    // The target of encode() must be at least this large.
    static size_t maxEncodedSize(size_t nValues, uint8_t bpv, uint8_t blockSize)
    {
        validateCcsdsParameters(bpv, blockSize, 1);
        const size_t nBlocks = nValues / blockSize + (nValues % blockSize != 0);
        // The basic set has the longest IDs: the bound also holds for the restricted set
        const size_t idLength = CodeOptionBase{ bpv, CodeOptionSet::Basic }.maxIdLength - 1u;
        const size_t bitsPerBlock = idLength + static_cast<size_t>(blockSize) * bpv;
        // ceil(nBlocks * bitsPerBlock / 8) + 8 without overflow: with nBlocks = 8 * a + b, it is
        // a * bitsPerBlock + ceil(b * bitsPerBlock / 8) + 8, where only the first product can overflow
        const size_t a = nBlocks / 8;
        const size_t tail = ((nBlocks % 8) * bitsPerBlock + 7) / 8 + 8;
        if (a > (std::numeric_limits<size_t>::max() - tail) / bitsPerBlock) {
            throw std::length_error("CCSDS encoding: the encoded size of " + std::to_string(nValues) + " values does not fit in size_t");
        }
        return a * bitsPerBlock + tail;
    }

    size_t maxEncodedSize(size_t nValues) const
    {
        return maxEncodedSize(nValues, bpv_, blockSize_);
    }
    ~SpAec() {}

    size_t encode(const Span<ValueType> data, Span<uint8_t> target)
    {
        checkDecodable();
        if (bpv_ <= 8) {
            return encodeImpl<uint8_t>(data, target);
        }
        else if (bpv_ <= 16) {
            return encodeImpl<uint16_t>(data, target);
        }
        else {  // the constructor has checked that bpv is 1..32
            return encodeImpl<uint32_t>(data, target);
        }
    }

    void decode(Span<uint8_t> source, Span<ValueType> target)
    {
        if (bpv_ <= 8) {
            decodeImpl<uint8_t>(source, target);
        }
        else if (bpv_ <= 16) {
            decodeImpl<uint16_t>(source, target);
        }
        else {  // the constructor has checked that bpv is 1..32
            decodeImpl<uint32_t>(source, target);
        }
    }

private:
    template <typename DeltaType>
    size_t encodeImpl(const Span<ValueType> data, Span<uint8_t> target)
    {
        const size_t needed = maxEncodedSize(data.size());
        if (target.size() < needed) {
            throw std::invalid_argument("CCSDS encoding: the output buffer is too small (" + std::to_string(target.size()) + " bytes; " + std::to_string(needed) + " bytes are needed for " + std::to_string(data.size()) + " values in the worst case)");
        }
        // The encoder ORs bits into the output: it must start zeroed
        std::memset(target.data(), 0, needed);

        if (selector_ == SplitSelector::Estimate) {
            return encodeWith<DeltaType, SplitSelector::Estimate>(data, target);
        }
        return encodeWith<DeltaType, SplitSelector::Search>(data, target);
    }

    // encodeImpl with the split-sample selector Selector (a template parameter of the encoders)
    template <typename DeltaType, SplitSelector Selector>
    size_t encodeWith(const Span<ValueType> data, Span<uint8_t> target)
    {
        // The parallel encoder produces the same bytes as the sequential one (see EncoderPar)
        if (nThreads_ > 1) {
            std::unique_ptr<DeltaType[]> mapped(new DeltaType[data.size()]);  // not value-initialised
            mapValues(data.data(), data.size(), mapped.get(), 0);
            EncoderPar<DeltaType, DeltaType, Identity<DeltaType>, Selector> encoder{ bpv_, blockSize_, rsiSize_, codeOptionSet_, nThreads_, preprocessing_ };
            return encoder.encode(Span<DeltaType>(mapped.get(), data.size()), target);
        }

        // Sequential: the values are mapped RSI by RSI into a buffer of one RSI, which stays in the
        // cache, instead of into an array of the whole field. Same stream as EncoderSeq::encode:
        // the split-sample state starts at 0 and the RSIs are encoded in order.
        EncoderSeq<DeltaType, DeltaType, Identity<DeltaType>, Selector> encoder{ bpv_, blockSize_, rsiSize_, codeOptionSet_, preprocessing_ };
        const size_t rsiValues = static_cast<size_t>(rsiSize_) * blockSize_;
        std::unique_ptr<DeltaType[]> mapped(new DeltaType[std::min(rsiValues, data.size())]);
        BitBuffer buffer(target);
        encoder.splitK(0);
        for (size_t offset = 0; offset < data.size(); offset += rsiValues) {
            const size_t count = std::min(rsiValues, data.size() - offset);
            mapValues(data.data() + offset, count, mapped.get(), offset);
            encoder.encodeRsi(Span<DeltaType>(mapped.get(), count), buffer);
        }
        // An empty stream is one zero byte (the flushed, empty last byte), as in EncoderSeq::encode
        return std::max<size_t>((buffer.bitPos() + 7) / 8, 1);
    }

    // Maps count values to the coded integers; throws for a value outside [0, 2^bpv) after
    // scaling (first index + offset). No dependency between iterations (vectorizable): the range
    // check is a reduction and out-of-range values (never converted: undefined behaviour) are
    // replaced by 0.
    template <typename DeltaType>
    void mapValues(const ValueType* in, size_t count, DeltaType* out, size_t offset) const
    {
        SimpleMapper<DeltaType> mapper{ referenceValue_, binaryScaleFactor_, decimalScaleFactor_ };
        const double maxMapped = std::ldexp(1.0, bpv_);  // values must map to [0, 2^bpv)
        bool inRange = true;
        for (size_t i = 0; i < count; ++i) {
            const double m = mapper.mapped(in[i]);
            const bool ok = (m >= 0.0) & (m < maxMapped);  // false for NaN
            inRange &= ok;
            out[i] = static_cast<DeltaType>(ok ? m : 0.0);
        }
        if (!inRange) {
            for (size_t i = 0; i < count; ++i) {
                const double m = mapper.mapped(in[i]);
                if (!(m >= 0.0 && m < maxMapped)) {
                    throwValueOutOfRange(offset + i, static_cast<double>(in[i]));
                }
            }
        }
    }

    // The smallest and the largest sample must decode to finite values of ValueType, else the
    // encoder would accept values that the decoder returns as infinity or NaN (as the decoder
    // computes them: SimpleUnmapper). Checked by encode only: data are decoded as they are.
    void checkDecodable() const
    {
        const double bscale = grib_power<ValueType>(binaryScaleFactor_, 2);
        const double dscale = grib_power<ValueType>(-decimalScaleFactor_, 10);
        const double reference = static_cast<double>(referenceValue_);
        const double limit = static_cast<double>(std::numeric_limits<ValueType>::max());
        const double smallest = (0.0 * bscale + reference) * dscale;  // NaN if 2^E overflows
        const double largest = ((std::ldexp(1.0, bpv_) - 1.0) * bscale + reference) * dscale;
        if (!(std::fabs(smallest) <= limit && std::fabs(largest) <= limit)) {  // false for NaN
            throw std::invalid_argument("CCSDS encoding: the samples cannot be decoded to finite values with this scaling (" + std::to_string(bpv_) +
                                        " bits per value, reference value " + std::to_string(reference) + ", binary scale factor " +
                                        std::to_string(binaryScaleFactor_) + ", decimal scale factor " + std::to_string(decimalScaleFactor_) + ")");
        }
    }

    [[noreturn]] __attribute__((cold, noinline)) void throwValueOutOfRange(size_t index, double value) const
    {
        throw std::invalid_argument("CCSDS encoding: value " + std::to_string(value) + " at index " + std::to_string(index) + " cannot be packed with " + std::to_string(bpv_) + " bits per value (reference value " + std::to_string(static_cast<double>(referenceValue_)) + ", binary scale factor " + std::to_string(binaryScaleFactor_) + ", decimal scale factor " + std::to_string(decimalScaleFactor_) + ")");
    }

    template <typename DeltaType>
    void decodeImpl(Span<uint8_t> source, Span<ValueType> target)
    {
        // The decoders copy the source into a padded buffer and check all reads (see BitBuffer)
        SimpleUnmapper<DeltaType> unmapper{ referenceValue_, binaryScaleFactor_, decimalScaleFactor_ };

        if (nThreads_ > 1) {
            DecoderPar<DeltaType, DeltaType> decoder{ bpv_, blockSize_, rsiSize_, codeOptionSet_, nThreads_, preprocessing_ };
            decoder.decodeParallel(source, target, unmapper);
        }
        else {
            DecoderSeq<DeltaType, DeltaType> decoder{ bpv_, blockSize_, rsiSize_, codeOptionSet_, preprocessing_ };
            ValueType* out = target.data();
            decoder.decodeSpans(source, target.size(),
                                [&](const DeltaType* values, size_t count, size_t offset) {
                                    ValueType* __restrict__ o = out + offset;
                                    for (size_t j = 0; j < count; ++j) {
                                        o[j] = unmapper(values[j]);
                                    }
                                });
        }
    }

private:
    static constexpr int maxScaleFactor = 32767;

    uint8_t bpv_;
    uint8_t blockSize_;
    uint16_t rsiSize_;
    uint8_t nThreads_ = 1;
    double referenceValue_;  // a double for every value type (not rounded for float values)
    int binaryScaleFactor_;
    int decimalScaleFactor_;
    CodeOptionSet codeOptionSet_;
    Preprocessing preprocessing_;
    SplitSelector selector_;
};

}  // namespace encoder

}  // namespace SPAEC_ISA
}  // namespace spaec::detail
