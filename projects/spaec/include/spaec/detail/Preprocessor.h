/*
 * SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <type_traits>

#include "spaec/detail/Compat.h"
#include "spaec/detail/Span.h"

namespace spaec::detail
{

template <typename MappedType>
struct Identity
{
    MappedType operator()(const MappedType& value) const
    {
        return static_cast<MappedType>(value);
    }
};


template <typename ValueType, typename DeltaType>
class Preprocessor
{
public:
    explicit Preprocessor(uint8_t bpv) :
        xMin_(std::is_signed<DeltaType>::value ? static_cast<ValueTypePlus>(-std::pow(2, bpv - 1)) : static_cast<ValueTypePlus>(0)),
        xMax_(std::is_signed<DeltaType>::value ? static_cast<ValueTypePlus>(std::pow(2, bpv - 1) - 1) : static_cast<ValueTypePlus>(std::pow(2, bpv) - 1)) {}

    using ValueTypePlus =
        std::conditional_t<(sizeof(DeltaType) <= 1), int16_t,
                           std::conditional_t<(sizeof(DeltaType) <= 2), int32_t,
                                              std::conditional_t<(sizeof(DeltaType) <= 4), int64_t,
                                                                 void>>>;

    const DeltaType xMin_;
    const DeltaType xMax_;
};


template <typename ValueType, typename DeltaType = ValueType, typename EncodeFunc = Identity<DeltaType>>
class PredictivePreprocessingEncodeFast : public Preprocessor<ValueType, DeltaType>
{
public:
    explicit PredictivePreprocessingEncodeFast(uint8_t bpv) : Preprocessor<ValueType, DeltaType>(bpv) {}

    void encode(Span<ValueType> data, Span<DeltaType> mapped)
    {
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
    explicit PredictivePreprocessingDecodeFast(uint8_t bpv) : Preprocessor<ValueType, DeltaType>(bpv) {}

    // The mapped value m of a sample is the zigzag code of its difference to the previous sample
    // whenever m <= 2 * theta, theta = min(previous - xMin, xMax - previous) (the common case);
    // otherwise the difference depends on which limit is closer. decode() handles chunks of
    // values first assuming the common case for all of them (a vectorizable zigzag decoding and
    // a prefix sum without branches), then checks that assumption (vectorizable) and decodes a
    // chunk where it does not hold again with the exact rule.
    void decode(Span<DeltaType> mapped_span, ValueType reference, Span<ValueType> data_span)
    {
        const DeltaType* __restrict__ mapped = mapped_span.data();
        ValueType* __restrict__ data = data_span.data();
        const size_t n = mapped_span.size();
        if (n == 0) {
            return;
        }
        data[0] = reference;

        if constexpr (std::is_same_v<DecodeFunc, Identity<DeltaType>> && std::is_same_v<ValueType, DeltaType>) {
            const ValueTypePlus xMin = this->xMin_;
            const ValueTypePlus xMax = this->xMax_;
            constexpr size_t chunk = 64;
            ValueTypePlus diff[chunk];
            bool speculate = true;  // false after a chunk with uncommon values (likely more follow)
            for (size_t start = 1; start < n; start += chunk) {
                const size_t count = std::min(chunk, n - start);
                if (!speculate) {
                    speculate = decodeExact(mapped, data, start, start + count);
                    continue;
                }
                const DeltaType* m = mapped + start;
                ValueType* x = data + start;
                for (size_t j = 0; j < count; ++j) {
                    const ValueTypePlus mj = m[j];
                    diff[j] = (mj >> 1) ^ -(mj & 1);
                }
                ValueTypePlus prev = x[-1];
                for (size_t j = 0; j < count; ++j) {
                    prev += diff[j];
                    x[j] = static_cast<ValueType>(prev);
                }
                // The first value that does not follow the common case makes all later
                // values of the chunk wrong: it is enough to find whether there is one
                bool common = true;
                for (size_t j = 0; j < count; ++j) {
                    const ValueTypePlus p = x[static_cast<ptrdiff_t>(j) - 1];
                    const ValueTypePlus theta = std::min(p - xMin, xMax - p);
                    common &= static_cast<ValueTypePlus>(m[j]) <= 2 * theta;
                }
                if (SPAEC_UNLIKELY(!common)) {
                    speculate = decodeExact(mapped, data, start, start + count);
                }
            }
        }
        else {
            decodeExact(mapped, data, 1, n);
        }
    }

private:
    // Decodes data[from, to) with the exact rule, given data[from - 1]. Returns true if all
    // values followed the common case.
    bool decodeExact(const DeltaType* __restrict__ mapped, ValueType* __restrict__ data, size_t from, size_t to)
    {
        bool common = true;
        const DeltaType xMin = this->xMin_;
        const DeltaType xMax = this->xMax_;

        for (size_t i = from; i < to; ++i) {
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
                common = false;
                ValueTypePlus mag = static_cast<ValueTypePlus>(m - theta);
                ValueTypePlus left_smaller = static_cast<ValueTypePlus>(
                    (predicted - xMin) < (xMax - predicted));
                decoded = (mag ^ -(1 - left_smaller)) + (1 - left_smaller);
            }
            data[i] = decodeFunc_(static_cast<DeltaType>(
                static_cast<ValueTypePlus>(predicted) + decoded));
        }
        return common;
    }

public:
    // Fused decode: applies preprocessor + output function in one pass
    // avoiding the intermediate output array write/read
    template <typename OutputFunc>
    void decodeFused(const DeltaType* __restrict__ mapped, size_t n,
                     ValueType reference, size_t offset, OutputFunc&& outputFunc)
    {
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

}  // namespace spaec::detail
