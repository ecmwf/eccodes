#pragma once

#include <array>
#include <memory>
#include <utility>

#include <codec/Binary.h>
#include <SimplePacking.h>



template <typename T>
class SimplePackingBinary : public SimplePacking<T> {
    static_assert(std::is_floating_point<T>::value, "SimplePacking only supports floating point types");

    public:
        using ValueType = T;
        using Values = std::vector<ValueType>;
        using Buffer = std::vector<unsigned char>;

        SimplePackingBinary(std::unique_ptr<Optimizer> optimizer) : SimplePacking<T>(std::move(optimizer)) {}
        ~SimplePackingBinary() = default;

        std::pair<Parameters, Buffer> pack(const Values& values);
        Values unpack(const Parameters& params, const Buffer& buffer, size_t nVals);


    private:
        template <typename ValueType, std::size_t... Is>
            static /*constexpr*/ auto make_binary_codecs_impl(std::index_sequence<Is...>) {
                using Base = BinaryInterface<ValueType>;
                return std::array<std::unique_ptr<Base>, sizeof...(Is)>{
                    std::unique_ptr<Base>{ std::make_unique<Binary<ValueType, Is + 1>>() }...
                };
            }

    public:
        template <typename ValueType, std::size_t N>
            static /*constexpr*/ auto make_binary_codecs() {
                return make_binary_codecs_impl<ValueType>(std::make_index_sequence<N>{});
            }

    private:

        static constexpr uint8_t maxNBits = 64;
        inline static const std::array<std::unique_ptr<BinaryInterface<ValueType>>, maxNBits> codecs_ = make_binary_codecs<ValueType, maxNBits>();
};

template <typename ValueType>
std::pair<Parameters, typename SimplePackingBinary<ValueType>::Buffer> SimplePackingBinary<ValueType>::pack(const Values& values)
{
    auto [params, min, max] = this->getParameters(values);
    if (params.isConstantField() || params.bitsPerValue() == 0) {
        return {params, Buffer{}};
    }
    auto d = params.decimalScaleFactor();
    auto b = params.binaryScaleFactor();
    auto r = params.referenceValue();
    return {params, codecs_[params.bitsPerValue() - 1]->pack(values, d, b, r, min, max)};
}


template <typename ValueType>
typename SimplePackingBinary<ValueType>::Values SimplePackingBinary<ValueType>::unpack(const Parameters& params, const Buffer& buf, size_t nVals)
{
    auto d = params.decimalScaleFactor();
    auto b = params.binaryScaleFactor();
    auto r = params.referenceValue();
    return codecs_[params.bitsPerValue() - 1]->unpack(buf, d, b, r, nVals);
}
