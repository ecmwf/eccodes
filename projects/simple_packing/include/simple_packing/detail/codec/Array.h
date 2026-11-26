/*
 * SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <algorithm>
#include <cstdint>
#include <type_traits>
#include <vector>

#include "simple_packing/detail/Utils.h"  // sp_power

namespace sp::detail
{


template <typename ValueType, uint8_t BPV>
class ArrayCodec
{
    static_assert(std::is_floating_point<ValueType>::value, "SimplePacking only supports floating point types");
    static_assert(BPV >= 1 && BPV <= 32, "SimplePacking only supports BPV between 1 and 32");

    using CodedValueType = typename std::conditional<
        (BPV <= 8), uint8_t,
        typename std::conditional<
            (BPV <= 16), uint16_t,
            typename std::conditional<
                (BPV <= 32), uint32_t,
                uint64_t>::type>::type>::type;

public:
    using Values = std::vector<ValueType>;
    using CodedValues = std::vector<CodedValueType>;

    CodedValues pack(const Values& values, int32_t decimalScaleFactor, int32_t binaryScaleFactor, ValueType referenceValue, ValueType /*min*/, ValueType /*max*/);
    Values unpack(const CodedValues& codedValues, int32_t decimalScaleFactor, int32_t binaryScaleFactor, double referenceValue, size_t /*nVals*/);
};


template <typename ValueType, uint8_t BPV>
typename ArrayCodec<ValueType, BPV>::CodedValues ArrayCodec<ValueType, BPV>::pack(const Values& values, int32_t decimalScaleFactor, int32_t binaryScaleFactor, ValueType referenceValue, ValueType /*min*/, ValueType /*max*/)
{
    const ValueType decimal = sp_power<ValueType>(decimalScaleFactor, 10);
    const ValueType divisor = sp_power<ValueType>(-binaryScaleFactor, 2);
    [[maybe_unused]] const size_t buflen = (((BPV * values.size()) + 7) / 8) * sizeof(uint8_t);

    auto encode = [&](auto val) {
        return (((val * decimal) - referenceValue) * divisor) + 0.5;
    };

    CodedValues encoded(values.size());
    std::transform(values.begin(), values.end(), encoded.begin(),
                   [&](auto v) {
                       return encode(v);
                   });
    return encoded;
}


template <typename ValueType, uint8_t BPV>
typename ArrayCodec<ValueType, BPV>::Values ArrayCodec<ValueType, BPV>::unpack(const CodedValues& codedValues, int32_t decimalScaleFactor, int32_t binaryScaleFactor, double referenceValue, size_t /*nVals*/)
{
    // Computed like the legacy ecCodes code, see Binary::unpackTo
    const double s = sp_power<ValueType>(binaryScaleFactor, 2);
    const double d = sp_power<ValueType>(-decimalScaleFactor, 10);

    Values values(codedValues.size());

    auto decode = [&](auto val) {
        return static_cast<ValueType>((static_cast<double>(val) * s + referenceValue) * d);
    };

    std::transform(codedValues.begin(), codedValues.end(), values.begin(),
                   [&](auto cv) {
                       return decode(cv);
                   });

    return values;
}

}  // namespace sp::detail
