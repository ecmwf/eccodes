#pragma once

#include <cstdint>
#include <cmath>
#include <cstring>
#include "encoder/EncoderSeq.h"
#include "encoder/EncoderPar.h"
#include "decoder/DecoderSeq.h"
#include "decoder/DecoderPar.h"
#include "Stats.h"

template <typename T>
T grib_power(int exponent, int base) {
    return static_cast<T>(std::pow(base, exponent));
}


namespace encoder {

template <typename ValueType>
class SpAec {

    template <typename MappedType>
    class SimpleMapper {
        public:
            SimpleMapper(ValueType referenceValue, int binaryScaleFactor, int decimalScaleFactor) :
                referenceValue_(referenceValue),
                binary_{grib_power<ValueType>(binaryScaleFactor, 2)},
                decimal_{grib_power<ValueType>(-decimalScaleFactor, 10)}
            {}

            // template <typename, typename>
            MappedType operator()(ValueType value) const {
                ValueType mapped = ((value * decimal_) - referenceValue_) * binary_ + 0.5;
                return static_cast<MappedType>(mapped);
            }

        private:
            const ValueType referenceValue_;
            const ValueType decimal_;
            const ValueType binary_;
    };


    template <typename MappedType>
    class SimpleUnmapper {
        public:
            SimpleUnmapper(ValueType referenceValue, int binaryScaleFactor, int decimalScaleFactor) :
                combined_{grib_power<ValueType>(-binaryScaleFactor, 2) * grib_power<ValueType>(decimalScaleFactor, 10)},
                offset_{referenceValue * grib_power<ValueType>(decimalScaleFactor, 10)}
            {}

            ValueType operator()(MappedType mapped) const {
                return static_cast<ValueType>(mapped) * combined_ + offset_;
            }

        private:
            const ValueType combined_;  // binary * decimal
            const ValueType offset_;    // referenceValue * decimal
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
            if (bpv_ == 0 || bpv_ > 32) {
                throw std::invalid_argument("Unsupported bpv. Supported bpv: 1-32.");
            }
            if (blockSize_ == 0) {
                throw std::invalid_argument("Block size must be greater than 0.");
            }
            if (rsiSize_ == 0) {
                throw std::invalid_argument("RSI size must be greater than 0.");
            }
        }
        ~SpAec() {}

        size_t encode(const Span<ValueType> data, Span<uint8_t> target) {
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

        void decode(Span<uint8_t> source, Span<ValueType> target) {
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

        SpAecStats getStats() const {
            return stats_;
        }

    private:
        template <typename DeltaType>
        size_t encodeImpl(const Span<ValueType> data, Span<uint8_t> target) {
            SimpleMapper<DeltaType> mapper{referenceValue_, binaryScaleFactor_, decimalScaleFactor_};
            std::vector<DeltaType> mapped(data.size());
            for (size_t i = 0; i < data.size(); ++i) {
                mapped[i] = mapper(data[i]);
            }

            if (nThreads_ > 1) {
                EncoderPar<DeltaType, DeltaType> encoder{bpv_, blockSize_, rsiSize_, CodeOptionSet::Basic, nThreads_};
                return encoder.encodeParallel(Span<DeltaType>(mapped), target);
            } else {
                EncoderSeq<DeltaType, DeltaType> encoder{bpv_, blockSize_, rsiSize_, CodeOptionSet::Basic};
                return encoder.encode(Span<DeltaType>(mapped), target);
            }
        }

        template <typename DeltaType>
        void decodeImpl(Span<uint8_t> source, Span<ValueType> target) {
            // Pad source buffer to prevent out-of-bounds reads in loadU64
            static constexpr size_t LOAD_PADDING = 8;
            std::vector<uint8_t> paddedSource(source.size() + LOAD_PADDING, 0);
            std::memcpy(paddedSource.data(), source.data(), source.size());
            Span<uint8_t> padded(paddedSource);

            SimpleUnmapper<DeltaType> unmapper{referenceValue_, binaryScaleFactor_, decimalScaleFactor_};

            if (nThreads_ > 1) {
                DecoderPar<DeltaType, DeltaType> decoder{bpv_, blockSize_, rsiSize_, CodeOptionSet::Basic, nThreads_};
                decoder.decodeParallel(padded, target, unmapper);
            } else {
                DecoderSeq<DeltaType, DeltaType> decoder{bpv_, blockSize_, rsiSize_, CodeOptionSet::Basic};
                decoder.decodeFused(padded, target.size(),
                    [&](DeltaType value, size_t idx) {
                        target[idx] = unmapper(value);
                    });
                stats_ = decoder.getStats();
            }
        }

    private:
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
