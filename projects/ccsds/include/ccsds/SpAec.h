#pragma once

#include <cmath>
#include <cstdint>
#include <cstring>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>
#include "ccsds/CodeOption.h"
#include "ccsds/Span.h"
#include "ccsds/Utils.h"
#include "ccsds/encoder/EncoderSeq.h"
#include "ccsds/encoder/EncoderPar.h"
#include "ccsds/decoder/DecoderSeq.h"
#include "ccsds/decoder/DecoderPar.h"

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
public:
    // Mapping between the values and the coded integers (public for the unit tests)
    template <typename MappedType>
    class SimpleMapper
    {
    public:
        SimpleMapper(ValueType referenceValue, int binaryScaleFactor, int decimalScaleFactor) :
            referenceValue_(referenceValue),
            decimal_{ grib_power<ValueType>(decimalScaleFactor, 10) },
            binary_{ grib_power<ValueType>(-binaryScaleFactor, 2) }
        {
        }

        MappedType operator()(ValueType value) const
        {
            ValueType mapped = ((value * decimal_) - referenceValue_) * binary_ + 0.5;
            return static_cast<MappedType>(mapped);
        }

        // The value before the conversion to MappedType (to check its range)
        double mapped(ValueType value) const
        {
            return static_cast<double>(((value * decimal_) - referenceValue_) * binary_ + 0.5);
        }

    private:
        const ValueType referenceValue_;
        const ValueType decimal_;
        const ValueType binary_;
    };


    template <typename MappedType>
    class SimpleUnmapper
    {
    public:
        // Like ecCodes with libaec: the powers are computed in ValueType, the scaling in double, and
        // the result is rounded to ValueType once. Scaling in float would lose the low bits of
        // samples with more than 24 bits and cancel catastrophically near zero.
        SimpleUnmapper(ValueType referenceValue, int binaryScaleFactor, int decimalScaleFactor) :
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
    SpAec(uint8_t bpv, uint8_t blockSize, uint16_t rsiSize, CodeOptionSet codeOptionSet, ValueType referenceValue, int binaryScaleFactor = 0, int decimalScaleFactor = 0, uint8_t nThreads = 1) :
        bpv_(bpv),
        blockSize_(blockSize),
        rsiSize_(rsiSize),
        nThreads_(nThreads),
        referenceValue_(referenceValue),
        binaryScaleFactor_(binaryScaleFactor),
        decimalScaleFactor_(decimalScaleFactor),
        codeOptionSet_(effectiveCodeOptionSet(codeOptionSet, bpv))
    {
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

    // The code option set used for bpv bits per value, like libaec (and the C API): the restricted
    // set for 1 to 4 bits, rejected for 5 to 8 bits, the basic set for more bits (CCSDS 121.0-B-3,
    // Table 5-1: the same IDs)
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

        SimpleMapper<DeltaType> mapper{ referenceValue_, binaryScaleFactor_, decimalScaleFactor_ };
        const double maxMapped = std::ldexp(1.0, bpv_);  // values must map to [0, 2^bpv)
        std::vector<DeltaType> mapped(data.size());
        // No dependency between iterations (vectorizable): the range check is a reduction and
        // out-of-range values (never converted: undefined behaviour) are replaced by 0
        bool inRange = true;
        const ValueType* in = data.data();
        DeltaType* out = mapped.data();
        for (size_t i = 0; i < data.size(); ++i) {
            const double m = mapper.mapped(in[i]);
            const bool ok = (m >= 0.0) & (m < maxMapped);  // false for NaN
            inRange &= ok;
            out[i] = static_cast<DeltaType>(ok ? m : 0.0);
        }
        if (!inRange) {
            for (size_t i = 0; i < data.size(); ++i) {
                const double m = mapper.mapped(data[i]);
                if (!(m >= 0.0 && m < maxMapped)) {
                    throwValueOutOfRange(i, static_cast<double>(data[i]));
                }
            }
        }

        // The parallel encoder produces the same bytes as the sequential one (see EncoderPar)
        if (nThreads_ > 1) {
            EncoderPar<DeltaType, DeltaType> encoder{ bpv_, blockSize_, rsiSize_, codeOptionSet_, nThreads_ };
            return encoder.encode(Span<DeltaType>(mapped), target);
        }
        EncoderSeq<DeltaType, DeltaType> encoder{ bpv_, blockSize_, rsiSize_, codeOptionSet_ };
        return encoder.encode(Span<DeltaType>(mapped), target);
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
            DecoderPar<DeltaType, DeltaType> decoder{ bpv_, blockSize_, rsiSize_, codeOptionSet_, nThreads_ };
            decoder.decodeParallel(source, target, unmapper);
        }
        else {
            DecoderSeq<DeltaType, DeltaType> decoder{ bpv_, blockSize_, rsiSize_, codeOptionSet_ };
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
    ValueType referenceValue_;
    int binaryScaleFactor_;
    int decimalScaleFactor_;
    CodeOptionSet codeOptionSet_;
};

}  // namespace encoder
