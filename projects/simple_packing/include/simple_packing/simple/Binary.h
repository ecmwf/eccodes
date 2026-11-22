#pragma once

#include <array>
#include <memory>
#include <utility>

#include "simple_packing/codec/Binary.h"
#include "simple_packing/SimplePacking.h"


template <typename T>
class SimplePackingBinary : public SimplePacking<T>
{
    static_assert(std::is_floating_point<T>::value, "SimplePacking only supports floating point types");

public:
    using ValueType = T;
    using Values = std::vector<ValueType>;
    using Buffer = std::vector<unsigned char>;

    SimplePackingBinary(std::unique_ptr<Optimizer> optimizer) : SimplePacking<T>(std::move(optimizer)) {}
    ~SimplePackingBinary() = default;

    std::pair<Parameters, Buffer> pack(const Values& values);
    Values unpack(const Parameters& params, const Buffer& buffer, size_t nVals);

    // Zero-copy variants for callers that already own the memory
    // (e.g. the ecCodes accessors), avoiding a full copy of the field.
    //
    // packTo()   writes ceil(bitsPerValue * nVals / 8) bytes into 'out'
    // unpackTo() writes nVals elements into 'out'
    void packTo(const Parameters& params, const ValueType* values, size_t nVals, unsigned char* out);
    void unpackTo(const Parameters& params, const unsigned char* buffer, size_t nVals, ValueType* out);


private:
    template <typename ValueType, std::size_t... Is>
    static /*constexpr*/ auto make_binary_codecs_impl(std::index_sequence<Is...>)
    {
        using Base = BinaryInterface<ValueType>;
        return std::array<std::unique_ptr<Base>, sizeof...(Is)>{
            std::unique_ptr<Base>{ std::make_unique<Binary<ValueType, Is + 1>>() }...
        };
    }

public:
    template <typename ValueType, std::size_t N>
    static /*constexpr*/ auto make_binary_codecs()
    {
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
        return { params, Buffer{} };
    }
    auto d = params.decimalScaleFactor();
    auto b = params.binaryScaleFactor();
    auto r = params.referenceValue();
    return { params, codecs_[params.bitsPerValue() - 1]->pack(values, d, b, r, min, max) };
}


template <typename ValueType>
typename SimplePackingBinary<ValueType>::Values SimplePackingBinary<ValueType>::unpack(const Parameters& params, const Buffer& buf, size_t nVals)
{
    Values values(nVals);
    unpackTo(params, buf.data(), nVals, values.data());
    return values;
}


template <typename ValueType>
void SimplePackingBinary<ValueType>::packTo(const Parameters& params, const ValueType* values, size_t nVals, unsigned char* out)
{
    if (params.isConstantField() || params.bitsPerValue() == 0) {
        return;
    }
    codecs_[params.bitsPerValue() - 1]->packTo(values, nVals,
                                               params.decimalScaleFactor(),
                                               params.binaryScaleFactor(),
                                               params.referenceValue(),
                                               out);
}


template <typename ValueType>
void SimplePackingBinary<ValueType>::unpackTo(const Parameters& params, const unsigned char* buf, size_t nVals, ValueType* out)
{
    codecs_[params.bitsPerValue() - 1]->unpackTo(buf,
                                                 params.decimalScaleFactor(),
                                                 params.binaryScaleFactor(),
                                                 params.referenceValue(),
                                                 nVals, out);
}
