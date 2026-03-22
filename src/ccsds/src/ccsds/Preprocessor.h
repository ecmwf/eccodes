#pragma once

#include <cmath>
#include <utility>
#include <type_traits>

#include "Span.h"

template <typename MappedType>
struct Identity
{
    MappedType operator()(const MappedType& value) const {
        return static_cast<MappedType>(value);
    }
};


template <typename ValueType, typename DeltaType>
class Preprocessor {
    public:
        explicit Preprocessor(uint8_t bpv) :
            xMin_(std::is_signed<DeltaType>::value ? static_cast<ValueTypePlus>(-std::pow(2, bpv - 1)) : static_cast<ValueTypePlus>(0)),
            xMax_(std::is_signed<DeltaType>::value ? static_cast<ValueTypePlus>(std::pow(2, bpv - 1) - 1) : static_cast<ValueTypePlus>(std::pow(2, bpv) - 1)) {}

    using ValueTypePlus =
        std::conditional_t< (sizeof(DeltaType) <= 1), int16_t,
        std::conditional_t< (sizeof(DeltaType) <= 2), int32_t,
        std::conditional_t< (sizeof(DeltaType) <= 4), int64_t,
        void>>>;

        const DeltaType xMin_;
        const DeltaType xMax_;
};


template <typename ValueType, typename DeltaType = ValueType, typename EncodeFunc = Identity<DeltaType>>
class PredictivePreprocessingEncodeFast : public Preprocessor<ValueType, DeltaType>
{

    public:
        PredictivePreprocessingEncodeFast() = default;

        template <typename... Args>
        PredictivePreprocessingEncodeFast(uint8_t bpv, Args... args) : Preprocessor<ValueType, DeltaType>(bpv), encodeFunc_(std::forward<Args>(args)...) {}

        void encode(Span<ValueType> data, Span<DeltaType> mapped) {

            for (size_t i = 1; i < data.size(); ++i) {
                DeltaType predicted = encodeFunc_(data[i - 1]);
                ValueTypePlus delta = static_cast<ValueTypePlus>(data[i]) - static_cast<ValueTypePlus>(predicted);
                const DeltaType theta = std::min(predicted - this->xMin_, this->xMax_ - predicted);
                ValueTypePlus absDelta = delta >= 0 ? delta : -delta;
                DeltaType isNeg = static_cast<DeltaType>(delta < 0);
                DeltaType inRange = static_cast<DeltaType>(2 * absDelta) - isNeg;
                DeltaType outer = static_cast<DeltaType>(static_cast<ValueTypePlus>(theta) + absDelta);
                mapped[i] = (absDelta <= static_cast<ValueTypePlus>(theta)) ? inRange : outer;
            }
            mapped[0] = data[0];
        }

    private:
        EncodeFunc encodeFunc_;
        using PreprocessorBase = Preprocessor<ValueType, DeltaType>;
        using ValueTypePlus = typename PreprocessorBase::ValueTypePlus;
};


template <typename ValueType, typename DeltaType = ValueType, typename DecodeFunc = Identity<DeltaType>>
class PredictivePreprocessingDecodeFast : public Preprocessor<ValueType, DeltaType>
{
    public:
        PredictivePreprocessingDecodeFast() = default;
        PredictivePreprocessingDecodeFast(uint8_t bpv) : Preprocessor<ValueType, DeltaType>(bpv) {}

        template <typename... Args>
        PredictivePreprocessingDecodeFast(uint8_t bpv, Args... args) : Preprocessor<ValueType, DeltaType>(bpv), decodeFunc_(std::forward<Args>(args)...) {}

        template <typename... Args>
        void decode(Span<DeltaType> mapped_span, ValueType reference, Span<ValueType> data_span, Args&&... args) {
            const DeltaType* __restrict__ mapped = mapped_span.data();
            ValueType* __restrict__ data = data_span.data();
            const size_t n = mapped_span.size();

            data[0] = reference;

            const DeltaType xMin = this->xMin_;
            const DeltaType xMax = this->xMax_;

            for (size_t i = 1; i < n; ++i) {
                const DeltaType predicted = data[i - 1];
                const DeltaType m = mapped[i];
                const DeltaType theta = std::min(
                    static_cast<DeltaType>(predicted - xMin),
                    static_cast<DeltaType>(xMax - predicted));

                ValueTypePlus decoded;
                if (__builtin_expect(m <= 2 * static_cast<DeltaType>(theta), 1)) {
                    ValueTypePlus half = static_cast<ValueTypePlus>((m + 1) >> 1);
                    ValueTypePlus neg = static_cast<ValueTypePlus>(m & 1);
                    decoded = (half ^ -neg) + neg;
                }
                else {
                    ValueTypePlus mag = static_cast<ValueTypePlus>(m - theta);
                    ValueTypePlus left_smaller = static_cast<ValueTypePlus>(
                        (predicted - xMin) < (xMax - predicted));
                    decoded = (mag ^ -(1 - left_smaller)) + (1 - left_smaller);
                }
                data[i] = decodeFunc_(static_cast<DeltaType>(
                    static_cast<ValueTypePlus>(predicted) + decoded));
            }
        }

        // Fused decode: applies preprocessor + output function in one pass
        // avoiding the intermediate output array write/read
        template <typename OutputFunc>
        void decodeFused(const DeltaType* __restrict__ mapped, size_t n,
                         ValueType reference, size_t offset, OutputFunc&& outputFunc) {
            const DeltaType xMin = this->xMin_;
            const DeltaType xMax = this->xMax_;

            outputFunc(decodeFunc_(reference), offset);
            DeltaType prev = reference;

            for (size_t i = 1; i < n; ++i) {
                const DeltaType m = mapped[i];
                const DeltaType theta = std::min(
                    static_cast<DeltaType>(prev - xMin),
                    static_cast<DeltaType>(xMax - prev));

                ValueTypePlus decoded;
                if (__builtin_expect(m <= 2 * static_cast<DeltaType>(theta), 1)) {
                    ValueTypePlus half = static_cast<ValueTypePlus>((m + 1) >> 1);
                    ValueTypePlus neg = static_cast<ValueTypePlus>(m & 1);
                    decoded = (half ^ -neg) + neg;
                }
                else {
                    ValueTypePlus mag = static_cast<ValueTypePlus>(m - theta);
                    ValueTypePlus left_smaller = static_cast<ValueTypePlus>(
                        (prev - xMin) < (xMax - prev));
                    decoded = (mag ^ -(1 - left_smaller)) + (1 - left_smaller);
                }
                prev = decodeFunc_(static_cast<DeltaType>(
                    static_cast<ValueTypePlus>(prev) + decoded));
                outputFunc(prev, offset + i);
            }
        }

    private:
        DecodeFunc decodeFunc_;
        using PreprocessorBase = Preprocessor<ValueType, DeltaType>;
        using ValueTypePlus = typename PreprocessorBase::ValueTypePlus;
};


