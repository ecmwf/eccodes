#pragma once

#include <SimplePacking.h>
#include <codec/Array.h>

template <typename ValueType>
class SimplePackingArray : public SimplePacking<ValueType> {
    static_assert(std::is_floating_point<ValueType>::value, "SimplePacking only supports floating point types");

    public:
        using Values = std::vector<ValueType>;
        using CodedValues = std::variant<
            std::vector<uint8_t>,
            std::vector<uint16_t>,
            std::vector<uint32_t>
        >;

        SimplePackingArray(std::unique_ptr<Optimizer> optimizer) : SimplePacking<ValueType>(std::move(optimizer)) {}
        ~SimplePackingArray() = default;

        std::pair<Parameters, CodedValues> pack(const Values& values);
        Values unpack(const Parameters& params, const CodedValues& buffer, size_t nVals);

    public:
        inline static ArrayCodec<ValueType, 8> codec8;
        inline static ArrayCodec<ValueType, 16> codec16;
        inline static ArrayCodec<ValueType, 32> codec32;
};


template <typename ValueType>
std::pair<Parameters, typename SimplePackingArray<ValueType>::CodedValues> SimplePackingArray<ValueType>::pack(const Values& values)
{
    auto [params, min, max] = this->getParameters(values);

    auto bitsPerValue = params.bitsPerValue();
    if (bitsPerValue > 1 && bitsPerValue <= 8) {
        return {params, codec8.pack(values, params.decimalScaleFactor(), params.binaryScaleFactor(), params.referenceValue(), min, max)};
    }
    else if (bitsPerValue > 8 && bitsPerValue <= 16) {
        return {params, codec16.pack(values, params.decimalScaleFactor(), params.binaryScaleFactor(), params.referenceValue(), min, max)};
    }
    else if (bitsPerValue > 16 && bitsPerValue <= 32) {
        return {params, codec32.pack(values, params.decimalScaleFactor(), params.binaryScaleFactor(), params.referenceValue(), min, max)};
    }
    else {
        throw std::runtime_error("Unsupported bitsPerValue for Array output format: " + std::to_string(bitsPerValue));
    }
}

template <typename ValueType>
typename SimplePackingArray<ValueType>::Values SimplePackingArray<ValueType>::unpack(const Parameters& params, const CodedValues& buffer, size_t nVals)
{
    auto bitsPerValue = params.bitsPerValue();
    if (bitsPerValue > 1 && bitsPerValue <= 8) {
        return codec8.unpack(std::get<std::vector<uint8_t>>(buffer), params.decimalScaleFactor(), params.binaryScaleFactor(), params.referenceValue(), nVals);
    }
    else if (bitsPerValue > 8 && bitsPerValue <= 16) {
        return codec16.unpack(std::get<std::vector<uint16_t>>(buffer), params.decimalScaleFactor(), params.binaryScaleFactor(), params.referenceValue(), nVals);
    }
    else if (bitsPerValue > 16 && bitsPerValue <= 32) {
        return codec32.unpack(std::get<std::vector<uint32_t>>(buffer), params.decimalScaleFactor(), params.binaryScaleFactor(), params.referenceValue(), nVals);
    }
    else {
        throw std::runtime_error("Unsupported bitsPerValue for Array output format: " + std::to_string(bitsPerValue));
    }
}
