#pragma once

#include <vector>
#include <cstdint>
#include <algorithm>


template <typename ValueType, uint8_t BPV>
class ArrayCodec {
    static_assert(std::is_floating_point<ValueType>::value, "SimplePacking only supports floating point types");
    static_assert(BPV >= 1 && BPV <= 32, "SimplePacking only supports BPV between 1 and 32");

    using CodedValueType = typename std::conditional<
        (BPV <= 8), uint8_t,
    typename std::conditional<
        (BPV <= 16), uint16_t,
    typename std::conditional<
        (BPV <= 32), uint32_t,
    uint64_t
        >::type
        >::type
        >::type;

    public:
        using Values = std::vector<ValueType>;
        using CodedValues = std::vector<CodedValueType>;

        CodedValues pack(const Values& values, int decimalScaleFactor, int binaryScaleFactor, ValueType referenceValue, ValueType min, ValueType max);
        Values unpack(const CodedValues& codedValues, int decimalScaleFactor, int binaryScaleFactor, ValueType referenceValue, size_t nVals);
};


template <typename ValueType, uint8_t BPV>
typename ArrayCodec<ValueType, BPV>::CodedValues ArrayCodec<ValueType, BPV>::pack(const Values& values, int decimalScaleFactor, int binaryScaleFactor, ValueType referenceValue, ValueType min, ValueType max)
{
    const ValueType decimal = grib_power<ValueType>(decimalScaleFactor, 10);
    const ValueType divisor = grib_power<ValueType>(-binaryScaleFactor, 2);
    const size_t buflen  = (((BPV * values.size()) + 7) / 8) * sizeof(unsigned char);

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
typename ArrayCodec<ValueType, BPV>::Values ArrayCodec<ValueType, BPV>::unpack(const CodedValues& codedValues, int decimalScaleFactor, int binaryScaleFactor, ValueType referenceValue, size_t nVals)
{
    const ValueType s = grib_power<ValueType>(binaryScaleFactor, 2);
    const ValueType d = grib_power<ValueType>(-decimalScaleFactor, 10);

    Values values(codedValues.size());

    auto decode = [&](auto val) {
        return ((static_cast<ValueType>(val) * s) + referenceValue) * d;
    };

    std::transform(codedValues.begin(), codedValues.end(), values.begin(),
        [&](auto cv) {
            return decode(cv);
        });

    return values;
}
