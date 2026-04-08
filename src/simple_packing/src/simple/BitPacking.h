#pragma once

#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <array>
#include <bitset>
#include <iostream>
#include <cstring>
#include <iostream>

#include <Utils.h>

#include "U128.h"

// Byte swap for 128 bit unsigned integer
// inline __uint128_t bswap128(__uint128_t x) {
//     uint64_t high = static_cast<uint64_t>(x >> 64);
//     uint64_t low = static_cast<uint64_t>(x & 0xFFFFFFFFFFFFFFFFULL);
//     high = bswap64(high);
//     low = bswap64(low);
//     return (static_cast<__uint128_t>(low) << 64) | static_cast<__uint128_t>(high);
// }

inline UInt128 bswap128(UInt128 x) {
    uint64_t high = static_cast<uint64_t>(x >> 64);
    uint64_t low = static_cast<uint64_t>(x & 0xFFFFFFFFFFFFFFFFULL);
    high = bswap64(x.hi);
    low = bswap64(x.lo);
    return (static_cast<UInt128>(low) << 64) | static_cast<UInt128>(high);
}


// using U128 = __uint128_t;

// using U128 = UInt128;

template <uint8_t BPV>
union UInt {
    public:
    using ValueType =
        std::conditional_t<(BPV <= 8),  std::uint8_t,
        std::conditional_t<(BPV <= 16), std::uint16_t,
        std::conditional_t<(BPV <= 32), std::uint32_t,
        std::conditional_t<(BPV <= 64), std::uint64_t,
        std::conditional_t<(BPV <= 128), UInt128,
        void>>>>>;

    using Bytes = std::array<uint8_t, sizeof(ValueType)>;
    // using Bytes = uint8_t[sizeof(ValueType)];

    ValueType value = 0;
    Bytes bytes;

    uint8_t size() const noexcept {return sizeof(ValueType); }
};


enum class Optimization {
    None,
    Block
};

template <typename T> 
T swap_bytes(const T& value) {
#if defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    // On big-endian, host order matches GRIB wire format (big-endian).
    return value;
#else
    constexpr size_t NBytes = sizeof(T);

    if constexpr (NBytes == 2) {
        return bswap16(value);
    }
    else if constexpr (NBytes == 4) {
        return bswap32(value);
    }
    else if constexpr (NBytes == 8) {
        return bswap64(value);
    }
    else if constexpr (NBytes == 16) {
        return bswap128(value);
    }
    else {
        return value;
    }
#endif
}

// Map a sequential big-endian byte index (0 = most significant) to the
// corresponding index into the bytes array of a union/struct in memory.
// On LE: bytes[0] is LSByte, so MSByte is at [size-1].
// On BE: bytes[0] is MSByte, so MSByte is at [0].
inline constexpr uint8_t be_byte(uint8_t i, uint8_t size) {
#if defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    return i;
#else
    return size - 1 - i;
#endif
}




template <std::size_t BPV, Optimization Method = Optimization::None>
class BitPacking {
    static_assert(BPV >= 1 && BPV <= 64, "BPV must be 1..64");

public:
    using ValueType = typename UInt<BPV>::ValueType;

    __attribute__((always_inline)) inline void encode_element(uint8_t* ptr, uint8_t bitOffset, const ValueType value) const noexcept {
        using UIntPlus = UInt<BPV + 8>;

        const size_t nBytesToRead = (bitOffset + BPV + 7) / 8;
        constexpr uint8_t lshift_partial = sizeof(typename UIntPlus::ValueType) * 8 - BPV;
        const uint8_t lshift = lshift_partial - static_cast<uint8_t>(bitOffset);

        UIntPlus coded;
        coded.value = value << lshift;

        UIntPlus mask;
        mask.value = (static_cast<typename UIntPlus::ValueType>(1) << BPV) - 1;
        mask.value <<= lshift;
        mask.value = ~mask.value;

        UIntPlus existing;
        for (uint8_t i = 0; i < nBytesToRead; ++i) {
            existing.bytes[be_byte(i, existing.size())] = ptr[i];
        }

        coded.value |= (existing.value & mask.value);

        for (uint8_t i = 0; i < nBytesToRead; ++i) {
            ptr[i] = coded.bytes[be_byte(i, coded.size())];
        }
    }

    __attribute__((always_inline)) inline ValueType decode_element(const uint8_t* ptr, uint8_t bitOffset) const noexcept {
        using UIntPlus = UInt<BPV + 8>;

        const size_t nBytesToRead = (bitOffset + BPV + 7) / 8;
        constexpr uint8_t rshift_partial = sizeof(typename UIntPlus::ValueType) * 8 - BPV;
        const uint8_t rshift = rshift_partial - static_cast<uint8_t>(bitOffset);

        UIntPlus coded;
        for (uint8_t i = 0; i < nBytesToRead; ++i) {
            coded.bytes[be_byte(i, coded.size())] = ptr[i];
        }

        coded.value = coded.value >> rshift;

        constexpr typename UIntPlus::ValueType mask = (static_cast<typename UIntPlus::ValueType>(1) << BPV) - 1;

        return static_cast<ValueType>(coded.value & mask);
    }

    __attribute__((always_inline)) inline void encode(uint8_t* ptr, uint8_t bitOffset, const ValueType value) const noexcept
    {
        if constexpr (BPV == 8 || BPV == 16 || BPV == 32 || BPV == 64) {
            UInt<BPV> coded;
            const uint8_t nBytesToRead = sizeof(typename UInt<BPV>::ValueType);
            coded.value = swap_bytes<typename UInt<BPV>::ValueType>(value);
            for (uint8_t i = 0; i < nBytesToRead; ++i) {
                ptr[i] = coded.bytes[i];
            }
        }
        else {
            using UIntPlus = UInt<BPV + 8>;
            UIntPlus coded;

            constexpr uint8_t lshift_partial = sizeof(typename UIntPlus::ValueType) * 8 - BPV;
            constexpr auto nBytes = sizeof(typename UIntPlus::ValueType);

            coded.value = value;
            const uint8_t lshift = lshift_partial - static_cast<uint8_t>(bitOffset);

            coded.value <<= lshift;
            coded.bytes[be_byte(0, coded.size())] |= ptr[0];

            if constexpr (Method == Optimization::Block) {
                *reinterpret_cast<typename UIntPlus::ValueType*>(ptr) = swap_bytes<typename UIntPlus::ValueType>(coded.value);
            } else {
                const size_t nBytesToRead = (bitOffset + BPV + 7) / 8;
                for (uint8_t i = 0; i < nBytesToRead; ++i) {
                    ptr[i] = coded.bytes[be_byte(i, coded.size())];
                }
            }
        }
    }

    __attribute__((always_inline)) inline ValueType decode(const uint8_t* ptr, uint8_t bitOffset) const noexcept
    {

        if constexpr (BPV == 16 || BPV == 32 || BPV == 64 || BPV == 128) {
            UInt<BPV> coded;

            constexpr size_t nBytesToRead = sizeof(typename UInt<BPV>::ValueType);

            if constexpr (Method == Optimization::Block) {
                coded.value = swap_bytes<typename UInt<BPV>::ValueType>(*reinterpret_cast<const typename UInt<BPV>::ValueType*>(ptr));
            }
            else {
                for (uint8_t i = 0; i < nBytesToRead; ++i) {
                    coded.bytes[i] = ptr[i];
                }
                coded.value = swap_bytes<typename UInt<BPV>::ValueType>(coded.value);
            }
            return coded.value;
        }
        else {
            using UIntPlus = UInt<BPV + 8>;
            UIntPlus coded;

            if constexpr (Method == Optimization::Block) {
                coded.value = swap_bytes<typename UIntPlus::ValueType>(*reinterpret_cast<const typename UIntPlus::ValueType*>(ptr));
            } else {
                const uint8_t nBytesToRead = (bitOffset + BPV + 7) / 8;
                for (uint8_t i = 0; i < nBytesToRead; ++i) {
                    coded.bytes[i] = ptr[i];
                }
                coded.value = swap_bytes<typename UIntPlus::ValueType>(coded.value);
            }

            constexpr uint8_t rshift_partial = sizeof(typename UIntPlus::ValueType) * 8 - BPV;
            const uint8_t rshift = rshift_partial - static_cast<uint8_t>(bitOffset);

            constexpr typename UIntPlus::ValueType mask = (static_cast<typename UIntPlus::ValueType>(1) << BPV) - 1;

            return static_cast<ValueType>(coded.value >> rshift & mask);
        }
    }
};

