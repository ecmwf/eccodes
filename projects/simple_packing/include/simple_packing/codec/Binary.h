#pragma once

#include "simple_packing/BitPacking.h"

#include <vector>
#include <cstdint>
#include <algorithm>
#include <cassert>
#include <iostream>


template <typename ValueType>
class BinaryInterface
{
    static_assert(std::is_floating_point<ValueType>::value, "SimplePacking only supports floating point types");

public:
    using Values = std::vector<ValueType>;
    using Buffer = std::vector<unsigned char>;

    virtual ~BinaryInterface() = default;

    virtual Buffer pack(const Values& values, int decimalScaleFactor, int binaryScaleFactor, ValueType referenceValue, ValueType min, ValueType max) = 0;
    virtual Values unpack(const Buffer& codedBuf, int decimalScaleFactor, int binaryScaleFactor, ValueType referenceValue, size_t nVals) = 0;

    // Zero-copy variants: read from / write into caller-owned memory.
    // 'out' must hold at least (BPV * nVals + 7) / 8 bytes (packTo) or
    // nVals elements (unpackTo).
    virtual void packTo(const ValueType* values, size_t nVals, int decimalScaleFactor, int binaryScaleFactor, ValueType referenceValue, unsigned char* out) = 0;
    virtual void unpackTo(const unsigned char* codedBuf, int decimalScaleFactor, int binaryScaleFactor, ValueType referenceValue, size_t nVals, ValueType* out) = 0;
};

template <typename ValueType, uint8_t BPV>
class Binary : public BinaryInterface<ValueType>
{
    static_assert(std::is_floating_point<ValueType>::value, "SimplePacking only supports floating point types");
    static_assert(BPV >= 1 && BPV <= 64, "SimplePacking only supports BPV between 1 and 64");

public:
    using Values = std::vector<ValueType>;
    using Buffer = std::vector<unsigned char>;

    Buffer pack(const Values& values, int decimalScaleFactor, int binaryScaleFactor, ValueType referenceValue, ValueType min, ValueType max) override;
    Values unpack(const Buffer& codedBuf, int decimalScaleFactor, int binaryScaleFactor, ValueType referenceValue, size_t nVals) override;
    void packTo(const ValueType* values, size_t nVals, int decimalScaleFactor, int binaryScaleFactor, ValueType referenceValue, unsigned char* out) override;
    void unpackTo(const unsigned char* codedBuf, int decimalScaleFactor, int binaryScaleFactor, ValueType referenceValue, size_t nVals, ValueType* out) override;
};


template <typename ValueType, uint8_t BPV>
void Binary<ValueType, BPV>::packTo(const ValueType* values, size_t nVals, int decimalScaleFactor, int binaryScaleFactor, ValueType referenceValue, unsigned char* out)
{
    const ValueType decimal = sp_power<ValueType>(decimalScaleFactor, 10);
    const ValueType divisor = sp_power<ValueType>(-binaryScaleFactor, 2);

    auto encode = [&](auto val) {
        return ((val * decimal) - referenceValue) * divisor + 0.5;
    };

    // The block codec reads and writes whole words: it handles all values except the last block,
    // whose values are encoded one by one (also when there are fewer than 8 values, or none)
    const size_t nBlockVals = nVals >= 8 ? ((nVals + 7) / 8 - 1) * 8 : 0;

    BitPacking<BPV, Optimization::Block> bvf;
    size_t nval = 0, nbit = 0;
    for (; nval < nBlockVals; nbit += BPV, ++nval) {
        bvf.encode(&out[nbit / 8], nbit % 8, encode(values[nval]));
    }

    BitPacking<BPV> bv;
    for (; nval < nVals; nbit += BPV, ++nval) {
        bv.encode(&out[nbit / 8], nbit % 8, encode(values[nval]));
    }
}


template <typename ValueType, uint8_t BPV>
void Binary<ValueType, BPV>::unpackTo(const unsigned char* codedBuf, int decimalScaleFactor, int binaryScaleFactor, ValueType referenceValue, size_t nVals, ValueType* out)
{
    const ValueType s = sp_power<ValueType>(binaryScaleFactor, 2);
    const ValueType d = sp_power<ValueType>(-decimalScaleFactor, 10);

    auto decode = [&](const ValueType& val) {
        return (static_cast<ValueType>(val) * s + referenceValue) * d;
    };

    // The block codec reads and writes whole words: it handles all values except the last block,
    // whose values are encoded one by one (also when there are fewer than 8 values, or none)
    const size_t nBlockVals = nVals >= 8 ? ((nVals + 7) / 8 - 1) * 8 : 0;

    BitPacking<BPV, Optimization::Block> bvf;
    size_t nval = 0, nbit = 0;
    for (; nval < nBlockVals; nbit += BPV, ++nval) {
        out[nval] = decode(bvf.decode(&codedBuf[nbit / 8], nbit % 8));
    }

    BitPacking<BPV> bv;
    for (; nval < nVals; nbit += BPV, ++nval) {
        out[nval] = decode(bv.decode(&codedBuf[nbit / 8], nbit % 8));
    }
}


template <typename ValueType, uint8_t BPV>
typename Binary<ValueType, BPV>::Buffer Binary<ValueType, BPV>::pack(const Values& values, int decimalScaleFactor, int binaryScaleFactor, ValueType referenceValue, ValueType /*min*/, ValueType /*max*/)
{
    const size_t buflen = (((BPV * values.size()) + 7) / 8) * sizeof(unsigned char);
    Buffer codedBuf(buflen);
    packTo(values.data(), values.size(), decimalScaleFactor, binaryScaleFactor, referenceValue, codedBuf.data());
    return codedBuf;
}


template <typename ValueType, uint8_t BPV>
typename Binary<ValueType, BPV>::Values Binary<ValueType, BPV>::unpack(const Buffer& codedBuf, int decimalScaleFactor, int binaryScaleFactor, ValueType referenceValue, size_t nVals)
{
    Values values(nVals);
    unpackTo(codedBuf.data(), decimalScaleFactor, binaryScaleFactor, referenceValue, nVals, values.data());
    return values;
}
