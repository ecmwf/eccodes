#pragma once

#include <BitPacking.h>

#include <vector>
#include <cstdint>
#include <algorithm>
#include <cassert>
#include <iostream>


template <typename ValueType>
class BinaryInterface {
    static_assert(std::is_floating_point<ValueType>::value, "SimplePacking only supports floating point types");

    public:
        using Values = std::vector<ValueType>;
        using Buffer = std::vector<unsigned char>;

        virtual ~BinaryInterface() = default;

        virtual Buffer pack(const Values& values, int decimalScaleFactor, int binaryScaleFactor, ValueType referenceValue, ValueType min, ValueType max) = 0;
        virtual Values unpack(const Buffer& codedBuf, int decimalScaleFactor, int binaryScaleFactor, ValueType referenceValue, size_t nVals) = 0;
};

template <typename ValueType, uint8_t BPV>
class Binary : public BinaryInterface<ValueType> {
    static_assert(std::is_floating_point<ValueType>::value, "SimplePacking only supports floating point types");
    static_assert(BPV >= 1 && BPV <= 64, "SimplePacking only supports BPV between 1 and 64");

    public:
        using Values = std::vector<ValueType>;
        using Buffer = std::vector<unsigned char>;

        Buffer pack(const Values& values, int decimalScaleFactor, int binaryScaleFactor, ValueType referenceValue, ValueType min, ValueType max) override;
        Values unpack(const Buffer& codedBuf, int decimalScaleFactor, int binaryScaleFactor, ValueType referenceValue, size_t nVals) override;
};


template <typename ValueType, uint8_t BPV>
typename Binary<ValueType, BPV>::Buffer Binary<ValueType, BPV>::pack(const Values& values, int decimalScaleFactor, int binaryScaleFactor, ValueType referenceValue, ValueType min, ValueType max)
{
    const ValueType decimal = sp_power<ValueType>(decimalScaleFactor, 10);
    const ValueType divisor = sp_power<ValueType>(-binaryScaleFactor, 2);
    const size_t buflen  = (((BPV * values.size()) + 7) / 8) * sizeof(unsigned char);

    auto encode = [&](auto val) {
        return ((val * decimal) - referenceValue) * divisor + 0.5;
    };

    size_t nVals = values.size();
    Buffer codedBuf(buflen);

    size_t nBlocks = (nVals + 7) / 8;

    BitPacking<BPV, Optimization::Block> bvf;
    size_t nval = 0, nbit = 0;
    for (; nval < (nBlocks - 1) * 8; nbit += BPV, ++nval) {
        bvf.encode(&codedBuf[nbit / 8], nbit % 8, encode(values[nval]));
    }

    BitPacking<BPV> bv;
    for (; nval < nVals; nbit += BPV, ++nval) {
        bv.encode(&codedBuf[nbit / 8], nbit % 8, encode(values[nval]));
    }

    codedBuf.resize(buflen);
    return codedBuf;
}


template <typename ValueType, uint8_t BPV>
typename Binary<ValueType, BPV>::Values Binary<ValueType, BPV>::unpack(const Buffer& codedBuf, int decimalScaleFactor, int binaryScaleFactor, ValueType referenceValue, size_t nVals)
{
    const ValueType s = sp_power<ValueType>(binaryScaleFactor, 2);
    const ValueType d = sp_power<ValueType>(-decimalScaleFactor, 10);

    Values values(nVals);

    size_t nBlocks = (nVals + 7) / 8;

    auto decode = [&](const ValueType& val) {
        return (static_cast<ValueType>(val) * s + referenceValue) * d;
    };

    BitPacking<BPV, Optimization::Block> bvf;
    size_t nval = 0, nbit = 0;
    for (; nval < (nBlocks - 1) * 8; nbit += BPV, ++nval) {
        values[nval] = decode(bvf.decode(&codedBuf[nbit / 8], nbit % 8));
    }

    BitPacking<BPV> bv;
    for (; nval < nVals; nbit += BPV, ++nval) {
        values[nval] = decode(bv.decode(&codedBuf[nbit / 8], nbit % 8));
    }

    return values;
}
