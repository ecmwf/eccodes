#pragma once

#include <cstdint>
#include <cmath>
#include <cstring>
#include <stdexcept>
#include <string>
#include "ccsds/encoder/EncoderSeq.h"
#include "ccsds/encoder/EncoderPar.h"
#include "ccsds/decoder/DecoderSeq.h"
#include "ccsds/decoder/DecoderPar.h"
#include "ccsds/Stats.h"

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
    template <typename MappedType>
    class SimpleMapper
    {
    public:
        SimpleMapper(ValueType referenceValue, int binaryScaleFactor, int decimalScaleFactor) :
            referenceValue_(referenceValue),
            binary_{ grib_power<ValueType>(-binaryScaleFactor, 2) },
            decimal_{ grib_power<ValueType>(decimalScaleFactor, 10) }
        {
        }

        // template <typename, typename>
        MappedType operator()(ValueType value) const
        {
            ValueType mapped = ((value * decimal_) - referenceValue_) * binary_ + 0.5;
            return static_cast<MappedType>(mapped);
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
        SimpleUnmapper(ValueType referenceValue, int binaryScaleFactor, int decimalScaleFactor) :
            referenceValue_(referenceValue),
            bscale_{ grib_power<ValueType>(binaryScaleFactor, 2) },
            dscale_{ grib_power<ValueType>(-decimalScaleFactor, 10) }
        {
        }

        ValueType operator()(MappedType mapped) const
        {
            return (static_cast<ValueType>(mapped) * bscale_ + referenceValue_) * dscale_;
        }

    private:
        const ValueType referenceValue_;
        const ValueType bscale_;
        const ValueType dscale_;
    };


public:
    SpAec(uint8_t bpv, uint8_t blockSize, uint16_t rsiSize, CodeOptionSet codeOptionSet, ValueType referenceValue, int binaryScaleFactor = 0, int decimalScaleFactor = 0, uint8_t nThreads = 1) :
        bpv_(bpv),
        blockSize_(blockSize),
        rsiSize_(rsiSize),
        referenceValue_(referenceValue),
        binaryScaleFactor_(binaryScaleFactor),
        decimalScaleFactor_(decimalScaleFactor),
        codeOptionSet_(codeOptionSet),
        nThreads_(nThreads)
    {
        validateCcsdsParameters(bpv_, blockSize_, rsiSize_);
        if (codeOptionSet_ != CodeOptionSet::Basic) {
            throw std::invalid_argument("CCSDS: unsupported code option set (only CodeOptionSet::Basic is supported)");
        }
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

    // Largest possible size in bytes of the encoded data of nValues values (every block
    // uncompressed), including the 8 bytes the encoder may touch after the end of the data.
    // The target of encode() must be at least this large.
    static size_t maxEncodedSize(size_t nValues, uint8_t bpv, uint8_t blockSize)
    {
        validateCcsdsParameters(bpv, blockSize, 1);
        const size_t nBlocks = (nValues + blockSize - 1) / blockSize;
        const size_t idLength = CodeOptionBase{ bpv, CodeOptionSet::Basic }.maxIdLength - 1u;
        const size_t bitsPerBlock = idLength + static_cast<size_t>(blockSize) * bpv;
        return (nBlocks * bitsPerBlock + 7) / 8 + 8;
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
        else if (bpv_ <= 32) {
            return encodeImpl<uint32_t>(data, target);
        }
        else {
            throw std::invalid_argument("Unsupported bpv for encoding. Supported bpv: 1-32.");
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
        else if (bpv_ <= 32) {
            decodeImpl<uint32_t>(source, target);
        }
        else {
            throw std::invalid_argument("Unsupported bpv for decoding. Supported bpv: 1-32.");
        }
    }

    SpAecStats getStats() const
    {
        return stats_;
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
        std::vector<DeltaType> mapped(data.size());
        for (size_t i = 0; i < data.size(); ++i) {
            mapped[i] = mapper(data[i]);
        }

        if (nThreads_ > 1) {
            EncoderPar<DeltaType, DeltaType> encoder{ bpv_, blockSize_, rsiSize_, CodeOptionSet::Basic, nThreads_ };
            return encoder.encodeParallel(Span<DeltaType>(mapped), target);
        }
        else {
            EncoderSeq<DeltaType, DeltaType> encoder{ bpv_, blockSize_, rsiSize_, CodeOptionSet::Basic };
            return encoder.encode(Span<DeltaType>(mapped), target);
        }
    }

    template <typename DeltaType>
    void decodeImpl(Span<uint8_t> source, Span<ValueType> target)
    {
        // The decoders copy the source into a padded buffer and check all reads (see BitBuffer)
        SimpleUnmapper<DeltaType> unmapper{ referenceValue_, binaryScaleFactor_, decimalScaleFactor_ };

        if (nThreads_ > 1) {
            DecoderPar<DeltaType, DeltaType> decoder{ bpv_, blockSize_, rsiSize_, CodeOptionSet::Basic, nThreads_ };
            decoder.decodeParallel(source, target, unmapper);
        }
        else {
            DecoderSeq<DeltaType, DeltaType> decoder{ bpv_, blockSize_, rsiSize_, CodeOptionSet::Basic };
            decoder.decodeFused(source, target.size(),
                                [&](DeltaType value, size_t idx) {
                                    target[idx] = unmapper(value);
                                });
            stats_ = decoder.getStats();
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

    SpAecStats stats_;
};

}  // namespace encoder
