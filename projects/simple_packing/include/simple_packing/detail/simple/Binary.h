/*
 * SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <memory>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>

#include "simple_packing/detail/codec/Binary.h"
#include "simple_packing/detail/SimplePacking.h"

namespace sp::detail
{


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
    // 'buffer' holds 'bufferSize' bytes; std::invalid_argument is thrown if that is too short for nVals values
    void unpackTo(const Parameters& params, const unsigned char* buffer, size_t bufferSize, size_t nVals, ValueType* out);


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
    // The codec for the given parameters (bits per value 1..64, GRIB scale factors)
    static BinaryInterface<ValueType>& codecFor(const Parameters& params)
    {
        checkRange("binary scale factor", params.binaryScaleFactor(), -maxScaleFactor, maxScaleFactor);
        checkRange("decimal scale factor", params.decimalScaleFactor(), -maxScaleFactor, maxScaleFactor);
        return codecFor(params.bitsPerValue());
    }

    // The codec for the given bits per value (1..64)
    static BinaryInterface<ValueType>& codecFor(long bitsPerValue)
    {
        if (bitsPerValue < 1 || bitsPerValue > maxNBits) {
            throw std::invalid_argument("Simple packing: unsupported number of bits per value " + std::to_string(bitsPerValue) +
                                        " (must be between 1 and " + std::to_string(maxNBits) + ", or 0 for a constant field)");
        }
        return *codecs_[bitsPerValue - 1];
    }

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
    return { params, codecFor(params).pack(values, d, b, r, min, max) };
}


template <typename ValueType>
typename SimplePackingBinary<ValueType>::Values SimplePackingBinary<ValueType>::unpack(const Parameters& params, const Buffer& buf, size_t nVals)
{
    Values values(nVals);
    unpackTo(params, buf.data(), buf.size(), nVals, values.data());
    return values;
}


template <typename ValueType>
void SimplePackingBinary<ValueType>::packTo(const Parameters& params, const ValueType* values, size_t nVals, unsigned char* out)
{
    if (params.isConstantField() || params.bitsPerValue() == 0) {
        return;
    }
    codecFor(params).packTo(values, nVals, params.decimalScaleFactor(), params.binaryScaleFactor(), params.referenceValue(), out);
}


template <typename ValueType>
void SimplePackingBinary<ValueType>::unpackTo(const Parameters& params, const unsigned char* buf, size_t bufferSize, size_t nVals, ValueType* out)
{
    // Constant field: no packed data
    if (params.isConstantField() || params.bitsPerValue() == 0) {
        std::fill(out, out + nVals, static_cast<ValueType>(params.referenceValue()));
        return;
    }
    BinaryInterface<ValueType>& codec = codecFor(params);
    const size_t bpv = static_cast<size_t>(params.bitsPerValue());
    if (nVals > (std::numeric_limits<size_t>::max() - 7) / bpv || bufferSize < (bpv * nVals + 7) / 8) {
        throw std::invalid_argument("Simple packing: the coded data is too short (" + std::to_string(bufferSize) + " bytes for " + std::to_string(nVals) +
                                    " values with " + std::to_string(bpv) + " bits per value)");
    }
    codec.unpackTo(buf, params.decimalScaleFactor(), params.binaryScaleFactor(), params.referenceValue(), nVals, out);
}

}  // namespace sp::detail
