/*
 * SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <type_traits>

#include "simple_packing/detail/Compat.h"

#include "simple_packing/detail/Utils.h"

#include "simple_packing/detail/U128.h"

namespace sp::detail
{

// Byte swap for 128 bit unsigned integer
inline UInt128 bswap128(UInt128 x)
{
    uint64_t high = static_cast<uint64_t>(x >> 64);
    uint64_t low = static_cast<uint64_t>(x & 0xFFFFFFFFFFFFFFFFULL);
    high = bswap64(x.hi);
    low = bswap64(x.lo);
    return (static_cast<UInt128>(low) << 64) | static_cast<UInt128>(high);
}


// The unsigned integer type of BPV bits (UInt128 for 65..128 bits). A type only: values are
// loaded and stored with memcpy (load_be/store_be), never read through another union member.
template <uint8_t BPV>
struct UInt
{
    using ValueType =
        std::conditional_t<(BPV <= 8), std::uint8_t,
                           std::conditional_t<(BPV <= 16), std::uint16_t,
                                              std::conditional_t<(BPV <= 32), std::uint32_t,
                                                                 std::conditional_t<(BPV <= 64), std::uint64_t,
                                                                                    std::conditional_t<(BPV <= 128), UInt128,
                                                                                                       void>>>>>;
};


enum class Optimization
{
    None,
    Block
};

template <typename T>
T swap_bytes(const T& value)
{
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

// Unaligned load/store of a word (the coded data has no alignment); compiles to a plain move
template <typename T>
SIMPLE_PACKING_ALWAYS_INLINE T load_word(const uint8_t* ptr) noexcept
{
    T value;
    std::memcpy(&value, ptr, sizeof(T));
    return value;
}

template <typename T>
SIMPLE_PACKING_ALWAYS_INLINE void store_word(uint8_t* ptr, const T& value) noexcept
{
    std::memcpy(ptr, &value, sizeof(T));
}


// Big-endian (most significant byte first) load of the first n <= sizeof(W) bytes of a word W; the
// other bytes are 0. The bytes are copied and converted with swap_bytes (128-bit words as two 64-bit
// halves): correct on every platform, no type punning.
template <typename W>
SIMPLE_PACKING_ALWAYS_INLINE W load_be(const uint8_t* ptr, size_t n) noexcept
{
    uint8_t b[sizeof(W)] = {};
    std::memcpy(b, ptr, n);
    if constexpr (std::is_same_v<W, UInt128>) {
        return UInt128(swap_bytes<uint64_t>(load_word<uint64_t>(b)), swap_bytes<uint64_t>(load_word<uint64_t>(b + 8)));
    }
    else {
        return swap_bytes<W>(load_word<W>(b));
    }
}

// Big-endian store of the first n <= sizeof(W) bytes of a word W
template <typename W>
SIMPLE_PACKING_ALWAYS_INLINE void store_be(uint8_t* ptr, const W& value, size_t n) noexcept
{
    uint8_t b[sizeof(W)];
    if constexpr (std::is_same_v<W, UInt128>) {
        store_word(b, swap_bytes<uint64_t>(value.hi));
        store_word(b + 8, swap_bytes<uint64_t>(value.lo));
    }
    else {
        store_word(b, swap_bytes<W>(value));
    }
    std::memcpy(ptr, b, n);
}


// Packing of one value of BPV bits at a bit offset (0..7) of ptr, most significant bit first.
// Optimization::Block reads and writes a whole word (sizeof(Word) bytes) instead of only the bytes
// of the value: the caller must provide them.
template <std::size_t BPV, Optimization Method = Optimization::None>
class BitPacking
{
    static_assert(BPV >= 1 && BPV <= 64, "BPV must be 1..64");

public:
    using ValueType = typename UInt<BPV>::ValueType;

private:
    // A word that holds a value at any bit offset
    using Word = typename UInt<BPV + 8>::ValueType;
    static constexpr uint32_t wordBits = 8 * sizeof(Word);

    static Word mask() noexcept { return static_cast<Word>((Word(1) << BPV) - Word(1)); }
    static uint32_t shift(uint8_t bitOffset) noexcept { return wordBits - BPV - bitOffset; }
    static size_t nBytes(uint8_t bitOffset) noexcept { return (bitOffset + BPV + 7) / 8; }

public:
    // Writes the value and keeps all other bits
    SIMPLE_PACKING_ALWAYS_INLINE void encode_element(uint8_t* ptr, uint8_t bitOffset, const ValueType value) const noexcept
    {
        const size_t n = nBytes(bitOffset);
        const Word fieldMask = static_cast<Word>(mask() << shift(bitOffset));
        const Word existing = load_be<Word>(ptr, n);
        const Word coded = static_cast<Word>((static_cast<Word>(static_cast<Word>(value) << shift(bitOffset)) & fieldMask) | (existing & static_cast<Word>(~fieldMask)));
        store_be(ptr, coded, n);
    }

    SIMPLE_PACKING_ALWAYS_INLINE ValueType decode_element(const uint8_t* ptr, uint8_t bitOffset) const noexcept
    {
        const Word coded = load_be<Word>(ptr, nBytes(bitOffset));
        return static_cast<ValueType>((coded >> shift(bitOffset)) & mask());
    }

    // Writes the value; keeps the bits before it in the first byte, the bits after it must be 0
    // (Block: the rest of the word is overwritten with 0)
    SIMPLE_PACKING_ALWAYS_INLINE void encode(uint8_t* ptr, uint8_t bitOffset, const ValueType value) const noexcept
    {
        if constexpr (BPV == 8 || BPV == 16 || BPV == 32 || BPV == 64) {
            store_word(ptr, swap_bytes<ValueType>(value));  // byte-aligned: bitOffset is 0
        }
        else {
            Word coded = static_cast<Word>(static_cast<Word>(value) << shift(bitOffset));
            coded = static_cast<Word>(coded | static_cast<Word>(Word(ptr[0]) << (wordBits - 8)));
            if constexpr (Method == Optimization::Block) {
                store_be(ptr, coded, sizeof(Word));
            }
            else {
                store_be(ptr, coded, nBytes(bitOffset));
            }
        }
    }

    SIMPLE_PACKING_ALWAYS_INLINE ValueType decode(const uint8_t* ptr, uint8_t bitOffset) const noexcept
    {
        if constexpr (BPV == 16 || BPV == 32 || BPV == 64) {
            return swap_bytes<ValueType>(load_word<ValueType>(ptr));  // byte-aligned: bitOffset is 0
        }
        else {
            const Word coded = load_be<Word>(ptr, Method == Optimization::Block ? sizeof(Word) : nBytes(bitOffset));
            return static_cast<ValueType>((coded >> shift(bitOffset)) & mask());
        }
    }
};

}  // namespace sp::detail
