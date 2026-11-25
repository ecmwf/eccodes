#pragma once

#include "simple_packing/BitPacking.h"

#include <algorithm>
#include <cstring>
#include <utility>

#include <cstddef>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <stdexcept>
#include <vector>

namespace sp
{


template <typename ValueType>
class BinaryInterface
{
    static_assert(std::is_floating_point<ValueType>::value, "SimplePacking only supports floating point types");

public:
    using Values = std::vector<ValueType>;
    using Buffer = std::vector<unsigned char>;

    virtual ~BinaryInterface() = default;

    virtual Buffer pack(const Values& values, int decimalScaleFactor, int binaryScaleFactor, ValueType referenceValue, ValueType min, ValueType max) = 0;
    virtual Values unpack(const Buffer& codedBuf, int decimalScaleFactor, int binaryScaleFactor, double referenceValue, size_t nVals) = 0;

    // Zero-copy variants: read from / write into caller-owned memory.
    // 'out' must hold at least (BPV * nVals + 7) / 8 bytes (packTo) or
    // nVals elements (unpackTo).
    virtual void packTo(const ValueType* values, size_t nVals, int decimalScaleFactor, int binaryScaleFactor, ValueType referenceValue, unsigned char* out) = 0;
    virtual void unpackTo(const unsigned char* codedBuf, int decimalScaleFactor, int binaryScaleFactor, double referenceValue, size_t nVals, ValueType* out) = 0;
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
    Values unpack(const Buffer& codedBuf, int decimalScaleFactor, int binaryScaleFactor, double referenceValue, size_t nVals) override;
    void packTo(const ValueType* values, size_t nVals, int decimalScaleFactor, int binaryScaleFactor, ValueType referenceValue, unsigned char* out) override;
    void unpackTo(const unsigned char* codedBuf, int decimalScaleFactor, int binaryScaleFactor, double referenceValue, size_t nVals, ValueType* out) override;
};


// Packs count coded values of BPV bits (each < 2^BPV) into out, most significant bit first,
// starting at a byte boundary. The bits are collected in a 64-bit register and stored as whole
// big-endian 32-bit words: no read-modify-write of the output, which would make every value
// wait for the store of the previous one. Values of up to 16 bits are first combined into groups
// of up to 32 bits (independent of each other), which shortens the dependency chain through the
// register. Writes exactly (BPV * count + 7) / 8 bytes; the unused bits of the last byte are 0.
template <uint8_t BPV, typename Coded>
inline void sp_pack_bits(const Coded* coded, size_t count, unsigned char* out)
{
    uint64_t acc = 0;
    unsigned nb = 0;                              // number of pending bits: the low nb bits of acc
    auto push = [&](uint64_t bits, unsigned n) {  // n <= 32 and bits < 2^n
        acc = (acc << n) | bits;
        nb += n;
        if (nb >= 32) {
            nb -= 32;
            store_word(out, swap_bytes<uint32_t>(static_cast<uint32_t>(acc >> nb)));
            out += 4;
        }
    };

    constexpr size_t group = BPV <= 16 ? 32 / BPV : 1;
    size_t j = 0;
    if constexpr (group > 1) {
        for (; j + group <= count; j += group) {
            uint64_t bits = 0;
            for (size_t g = 0; g < group; ++g) {
                bits |= static_cast<uint64_t>(coded[j + g]) << ((group - 1 - g) * BPV);
            }
            push(bits, static_cast<unsigned>(group * BPV));
        }
    }
    for (; j < count; ++j) {
        if constexpr (BPV <= 32) {
            push(coded[j], BPV);
        }
        else {
            push(static_cast<uint64_t>(coded[j]) >> 32, BPV - 32);
            push(static_cast<uint64_t>(coded[j]) & 0xFFFFFFFFu, 32);
        }
    }
    while (nb >= 8) {
        nb -= 8;
        *out++ = static_cast<unsigned char>(acc >> nb);
    }
    if (nb > 0) {
        *out = static_cast<unsigned char>(acc << (8 - nb));
    }
}

// Extracts the 8 values of BPV bits (BPV <= 56) of a group of BPV bytes: each from a big-endian
// 32-bit (BPV <= 25) or 64-bit load at its first byte, at offsets known at compile time (reads
// up to byte (7 * BPV) / 8 + 3 or + 7 of the group)
template <uint8_t BPV, typename Coded, size_t... G>
inline void sp_unpack_group(const unsigned char* p, Coded* out, std::index_sequence<G...>)
{
    // A value of up to 25 bits fits in the 32 bits from its first byte
    using Word = std::conditional_t<(BPV <= 25), uint32_t, uint64_t>;
    constexpr unsigned wordBits = 8 * sizeof(Word);
    ((out[G] = static_cast<Coded>(static_cast<Word>(swap_bytes<Word>(load_word<Word>(p + G * BPV / 8)) << (G * BPV % 8)) >> (wordBits - BPV))), ...);
}

// The bits of a double as an integer and back (std::bit_cast of C++20)
inline uint64_t sp_bits(double v)
{
    uint64_t u;
    std::memcpy(&u, &v, sizeof u);
    return u;
}

inline double sp_double(uint64_t u)
{
    double v;
    std::memcpy(&v, &u, sizeof v);
    return v;
}

// Converts a coded value of up to 32 bits (< 2^32) to an unsigned integer with signed 32-bit
// conversions, which are vectorizable (64-bit ones are not with AVX2): values from 2^31 on are
// first reduced by 2^31 (exactly) and get the bit back afterwards
inline uint32_t sp_to_uint32(double v)
{
    const uint64_t high = std::isgreaterequal(v, 2147483648.0);
    const double reduced = v - sp_double(sp_bits(2147483648.0) & (uint64_t{ 0 } - high));
    return static_cast<uint32_t>(static_cast<int32_t>(reduced)) + static_cast<uint32_t>(high << 31);
}

template <typename ValueType, uint8_t BPV>
void Binary<ValueType, BPV>::packTo(const ValueType* values, size_t nVals, int decimalScaleFactor, int binaryScaleFactor, ValueType referenceValue, unsigned char* out)
{
    const ValueType decimal = sp_power<ValueType>(decimalScaleFactor, 10);
    const ValueType divisor = sp_power<ValueType>(-binaryScaleFactor, 2);

    // Coded values must be in [0, 2^BPV): others (NaN, values below the reference value or beyond the
    // range of BPV bits) are replaced by 0, and reported after the loop
    const double limit = std::ldexp(1.0, BPV);

    bool inRange = true;
    if constexpr (BPV <= 32) {
        // The values are converted to integers chunk by chunk (a vectorizable loop), then packed.
        // Chunks are a multiple of 8 values long: each starts at a byte boundary.
        constexpr size_t chunk = 512;
        uint32_t coded[chunk];
        for (size_t start = 0; start < nVals; start += chunk) {
            const size_t count = std::min(chunk, nVals - start);
            const ValueType* v = values + start;
            int ok = 1;
            for (size_t j = 0; j < count; ++j) {
                const double c = static_cast<double>(((v[j] * decimal) - referenceValue) * divisor + 0.5);
                // Quiet comparisons (false for NaN) and a selection through the bit pattern: no
                // floating-point selection, which GCC does not vectorize (trapping math)
                const int o = std::isgreaterequal(c, 0.0) & std::isless(c, limit);
                ok &= o;
                const double valid = sp_double(sp_bits(c) & (uint64_t{ 0 } - static_cast<uint64_t>(o)));
                if constexpr (BPV <= 31) {
                    coded[j] = static_cast<uint32_t>(static_cast<int32_t>(valid));
                }
                else {
                    coded[j] = sp_to_uint32(valid);
                }
            }
            inRange &= (ok != 0);
            sp_pack_bits<BPV>(coded, count, out + start / 8 * BPV);
        }
    }
    else {
        // The codecs below OR each value into the first byte it uses: start from zero bytes, the
        // output must not depend on the previous content of the buffer
        if (nVals > 0) {
            std::memset(out, 0, (BPV * nVals + 7) / 8);
        }
        auto encode = [&](auto val) {
            const double coded = static_cast<double>(((val * decimal) - referenceValue) * divisor + 0.5);
            const bool ok = (coded >= 0.0) & (coded < limit);
            inRange &= ok;
            return ok ? coded : 0.0;
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

    if (!inRange) {
        for (size_t i = 0; i < nVals; ++i) {
            const double coded = static_cast<double>(((values[i] * decimal) - referenceValue) * divisor + 0.5);
            if (!(coded >= 0.0 && coded < limit)) {
                char message[256];
                std::snprintf(message, sizeof(message),
                              "Simple packing: value %.10g at index %zu cannot be packed with %d bits per value "
                              "(reference value %.10g, binary scale factor %d, decimal scale factor %d)",
                              static_cast<double>(values[i]), i, static_cast<int>(BPV), static_cast<double>(referenceValue), binaryScaleFactor, decimalScaleFactor);
                throw std::runtime_error(message);
            }
        }
    }
}


template <typename ValueType, uint8_t BPV>
void Binary<ValueType, BPV>::unpackTo(const unsigned char* codedBuf, int decimalScaleFactor, int binaryScaleFactor, double referenceValue, size_t nVals, ValueType* out)
{
    // Like the legacy ecCodes code: the powers are computed in ValueType, the scaling in double,
    // and the result is rounded to ValueType once (scaling in float loses the low bits of values
    // with more than 24 bits and cancels catastrophically near zero)
    const double s = sp_power<ValueType>(binaryScaleFactor, 2);
    const double d = sp_power<ValueType>(-decimalScaleFactor, 10);

    auto decode = [&](auto val) {
        return static_cast<ValueType>((static_cast<double>(val) * s + referenceValue) * d);
    };

    if constexpr (BPV == 8 || BPV == 16 || BPV == 32) {
        // Whole bytes: one vectorizable loop (byte swap and conversion)
        using Word = typename UInt<BPV>::ValueType;
        using Signed = std::conditional_t<(BPV <= 16), int32_t, int64_t>;  // exact and vectorizable
        for (size_t i = 0; i < nVals; ++i) {
            const Word w = swap_bytes<Word>(load_word<Word>(&codedBuf[i * sizeof(Word)]));
            out[i] = decode(static_cast<Signed>(w));
        }
    }
    else if constexpr (BPV < 32) {
        // Groups of 8 values (BPV bytes) are extracted at offsets known at compile time (independent
        // of each other), chunk by chunk; the conversion of a chunk is a vectorizable loop. The
        // values of groups whose loads would reach beyond the coded data are read one by one.
        const size_t nBytes = (BPV * nVals + 7) / 8;
        constexpr size_t groupReach = (7 * BPV) / 8 + (BPV <= 25 ? 4 : 8);  // bytes read by a group
        const size_t nFastGroups = nBytes >= groupReach ? std::min(nVals / 8, (nBytes - groupReach) / BPV + 1) : 0;
        constexpr size_t chunk = 512;
        uint32_t coded[chunk];
        BitPacking<BPV> bv;
        for (size_t start = 0; start < nVals; start += chunk) {
            const size_t count = std::min(chunk, nVals - start);
            const size_t fastGroups = start / 8 < nFastGroups ? std::min(count / 8, nFastGroups - start / 8) : 0;
            for (size_t g = 0; g < fastGroups; ++g) {
                sp_unpack_group<BPV>(codedBuf + (start / 8 + g) * BPV, coded + 8 * g, std::make_index_sequence<8>{});
            }
            for (size_t j = 8 * fastGroups; j < count; ++j) {
                const size_t bit = (start + j) * BPV;
                coded[j] = static_cast<uint32_t>(bv.decode(&codedBuf[bit / 8], bit % 8));
            }
            ValueType* o = out + start;
            for (size_t j = 0; j < count; ++j) {
                o[j] = decode(static_cast<int32_t>(coded[j]));
            }
        }
    }
    else {
        // The block codec reads and writes whole words: it handles all values except the last block,
        // whose values are decoded one by one (also when there are fewer than 8 values, or none)
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
typename Binary<ValueType, BPV>::Values Binary<ValueType, BPV>::unpack(const Buffer& codedBuf, int decimalScaleFactor, int binaryScaleFactor, double referenceValue, size_t nVals)
{
    Values values(nVals);
    unpackTo(codedBuf.data(), decimalScaleFactor, binaryScaleFactor, referenceValue, nVals, values.data());
    return values;
}

}  // namespace sp
