#pragma once

#include <bit>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <string>
#include "ccsds/Span.h"

constexpr bool isLittleEndian = (std::endian::native == std::endian::little);

// Conversion between the native byte order and the byte order of the bit stream (big-endian, most
// significant bit first); the same function converts in both directions
inline uint16_t bigEndian16(uint16_t v)
{
    if constexpr (isLittleEndian) {
        return __builtin_bswap16(v);
    }
    return v;
}
inline uint32_t bigEndian32(uint32_t v)
{
    if constexpr (isLittleEndian) {
        return __builtin_bswap32(v);
    }
    return v;
}
inline uint64_t bigEndian64(uint64_t v)
{
    if constexpr (isLittleEndian) {
        return __builtin_bswap64(v);
    }
    return v;
}

class BitBuffer
{
private:
    // Safe unaligned load/store to avoid strict aliasing UB
    template <typename T>
    static T loadUnaligned(const uint8_t* ptr)
    {
        T val;
        std::memcpy(&val, ptr, sizeof(T));
        return val;
    }
    template <typename T>
    static void storeUnaligned(uint8_t* ptr, T val)
    {
        T old;
        std::memcpy(&old, ptr, sizeof(T));
        old |= val;
        std::memcpy(ptr, &old, sizeof(T));
    }

public:
    // Readers may load up to 8 bytes at the current position and decode a whole block before the
    // position is checked again. A decoder therefore reads from a copy of the input followed by
    // this many zero bytes (see DecoderSeq::makeBuffer) and checks the position against the end
    // of the data at every block start and in every unbounded scan.
    static constexpr size_t guardBytes = 512;

    BitBuffer(const BitBuffer& buffer, size_t bitPos = 0) : buffer_{ buffer.buffer_ }, currentBitPos_{ bitPos }, limitBits_{ buffer.limitBits_ } {}
    BitBuffer(Span<uint8_t> data, size_t bitPos = 0) : buffer_{ data }, currentBitPos_{ bitPos }, limitBits_{ data.size() * 8 } {}
    // data must contain at least guardBytes bytes after the first limitBits bits
    BitBuffer(Span<uint8_t> data, size_t bitPos, size_t limitBits) : buffer_{ data }, currentBitPos_{ bitPos }, limitBits_{ limitBits } {}

    [[noreturn]] __attribute__((cold, noinline)) static void throwOverrun(size_t pos, size_t limitBits)
    {
        throw std::runtime_error("CCSDS decoding failed: the compressed data is truncated or corrupt (needed data beyond its end: bit " + std::to_string(pos) + " of " + std::to_string(limitBits) + ")");
    }

    [[noreturn]] __attribute__((cold, noinline)) static void throwWriteOverrun(uint64_t pos, size_t limitBits)
    {
        throw std::length_error("CCSDS encoding failed: the output buffer is too small (bit " + std::to_string(pos) + " of " + std::to_string(limitBits) + ")");
    }

    // Throws if the read position is beyond the end of the compressed data
    void checkPos(size_t pos) const
    {
        if (pos > limitBits_) [[unlikely]] {
            throwOverrun(pos, limitBits_);
        }
    }
    void checkPos() const { checkPos(currentBitPos_); }

    uint16_t peekBits(size_t nBits, size_t offset) const
    {
        using TPlus = uint16_t;

        constexpr TPlus ones = ~static_cast<TPlus>(0);
        size_t bytePos = offset / 8;
        size_t bitOffset = offset % 8;

        TPlus data = bigEndian16(loadUnaligned<TPlus>(&buffer_[bytePos]));
        data >>= (sizeof(TPlus) * 8 - nBits - bitOffset);

        TPlus mask = ones >> (sizeof(TPlus) * 8 - nBits);
        return data & mask;
    }

    uint16_t peekBits(size_t nBits) const
    {
        return peekBits(nBits, currentBitPos_);
    }

    void skipBits(size_t nBits) const
    {
        currentBitPos_ += nBits;
    }


    void skipCodewords(size_t nCodewords) const
    {
        while (nCodewords > 0) {
            size_t bytepos = currentBitPos_ / 8;
            size_t bitpos = currentBitPos_ % 8;

            uint64_t acc = bigEndian64(loadUnaligned<uint64_t>(&buffer_[bytepos]));
            acc <<= bitpos;

            while (acc != 0 && nCodewords > 0) {
                // Find the next 1-bit (codeword terminator)
                size_t lz = std::countl_zero(acc);
                size_t cwBits = lz + 1;  // zeros + terminating 1
                currentBitPos_ += cwBits;
                nCodewords--;
                size_t consumed = bitpos + lz + 1;
                if (consumed >= 64) break;
                acc <<= cwBits;
                bitpos += cwBits;
            }

            if (nCodewords > 0 && acc == 0) {
                // All remaining bits in this 64-bit window are zero;
                // advance past them and load next window.
                size_t remaining = 64 - bitpos;
                currentBitPos_ += remaining;
                checkPos();
            }
        }
    }


    size_t readCodeword(size_t pos) const
    {
        size_t bytepos = pos / 8;
        size_t bitpos = pos % 8;

        uint64_t acc = bigEndian64(loadUnaligned<uint64_t>(&buffer_[bytepos]));
        acc <<= bitpos;

        if (acc != 0) {
            return std::countl_zero(acc) + 1;
        }

        // Codeword spans more than 56 bits: fall back to byte scanning.
        // The data after limitBits_ is zero, so an unterminated codeword ends here.
        size_t zeros = 64 - bitpos;
        bytepos += 8;
        while (buffer_[bytepos] == 0) {
            zeros += 8;
            bytepos++;
            checkPos(bytepos * 8);
        }
        zeros += std::countl_zero(static_cast<unsigned>(buffer_[bytepos])) - 24;
        return zeros + 1;
    }

    size_t readCodeword() const
    {
        size_t result = readCodeword(currentBitPos_);
        currentBitPos_ += result;
        return result;
    }

    void writeCodeword(size_t length)
    {
        const size_t pos = currentBitPos_;
        size_t bitOffset = (pos + length) % 8;
        uint8_t mask = 0b1000'0000 >> bitOffset;
        buffer_[(pos + length) / 8] |= mask;
        currentBitPos_ += length + 1;
    }

    // Writes count unary codewords like writeCodeword(data[i] >> k), with the bits collected in a
    // 64-bit register: one memory access per 64 bits instead of one read-modify-write per
    // codeword (and no reload of the position, which byte stores may alias). Like writeCodeword,
    // it only sets the terminating one bits: the buffer must be zero after the current position.
    template <typename T>
        requires(std::is_integral<T>::value)
    void writeCodewords(const T* data, size_t count, unsigned k = 0)
    {
        uint8_t* out = buffer_.data();
        size_t base = currentBitPos_ & ~size_t(7);  // bit position of the register's first bit
        size_t fill = currentBitPos_ - base;        // bits of the register in use
        uint64_t acc = 0;
        constexpr uint64_t top = uint64_t(1) << 63;
        for (size_t i = 0; i < count; ++i) {
            const uint64_t zeros = static_cast<uint64_t>(data[i]) >> k;  // 64 bits also where size_t has 32
            if (fill + zeros >= 64) [[unlikely]] {
                orBigEndian64(out + base / 8, acc);
                const uint64_t onePos = base + fill + zeros;  // the terminating one bit
                if (zeros >= limitBits_ || onePos >= limitBits_) {
                    // Never write beyond the buffer, whatever the codeword lengths
                    throwWriteOverrun(onePos, limitBits_);
                }
                base = static_cast<size_t>(onePos) & ~size_t(7);
                fill = static_cast<size_t>(onePos) - base;
                acc = 0;
            }
            else {
                fill += zeros;
            }
            acc |= top >> fill;
            ++fill;
        }
        orBigEndian64(out + base / 8, acc);
        currentBitPos_ = base + fill;
    }

    template <typename T>
        requires(std::is_integral<T>::value)
    void writeBits(T value, size_t nBits)
    {
        using TPlus =
            typename std::conditional_t<sizeof(T) <= 1, uint16_t,
                                        typename std::conditional_t<sizeof(T) <= 2, uint32_t,
                                                                    typename std::conditional_t<sizeof(T) <= 4, uint64_t,
                                                                                                void>>>;

        TPlus valuePlus = static_cast<TPlus>(value);

        assert(nBits <= sizeof(T) * 8 && "nBits exceeds size of T");

        // Only the low nBits bits of the value are written, like in libaec: values with more
        // bits (possible in the C API for bits per sample below the sample type width) must not
        // leak into the following bits of the stream
        if (nBits < sizeof(T) * 8) {
            valuePlus &= static_cast<TPlus>((TPlus{ 1 } << nBits) - 1);
        }

        constexpr uint8_t nbitsInTplus = sizeof(TPlus) * 8;
        const auto shift = currentBitPos_ % 8;
        valuePlus = valuePlus << (nbitsInTplus - nBits - shift);

        if constexpr (sizeof(TPlus) == 2) {
            valuePlus = bigEndian16(valuePlus);
        }
        else if constexpr (sizeof(TPlus) == 4) {
            valuePlus = bigEndian32(valuePlus);
        }
        else if constexpr (sizeof(TPlus) == 8) {
            valuePlus = bigEndian64(valuePlus);
        }
        storeUnaligned<TPlus>(&buffer_[currentBitPos_ / 8], valuePlus);
        currentBitPos_ += nBits;
    }

    // Batch write: packs k-bit values from an array using 64-bit accumulator.
    // Reduces memory operations by ~8x compared to per-value writeBits.
    template <typename T>
        requires(std::is_integral<T>::value)
    void writeBitsBatch(const T* data, size_t count, uint8_t k)
    {
        if (k == 0 || count == 0) return;

        const uint64_t mask = (static_cast<uint64_t>(1) << k) - 1;
        const size_t valsPerChunk = 56 / k;

        size_t idx = 0;
        while (idx < count) {
            size_t chunkSize = std::min(valsPerChunk, count - idx);
            size_t totalBits = chunkSize * k;

            uint64_t acc = 0;
            for (size_t j = 0; j < chunkSize; ++j) {
                acc = (acc << k) | (static_cast<uint64_t>(data[idx + j]) & mask);
            }

            size_t bytePos = currentBitPos_ >> 3;
            size_t bitOffset = currentBitPos_ & 7;
            acc <<= (64 - totalBits - bitOffset);
            acc = bigEndian64(acc);

            uint64_t old;
            std::memcpy(&old, &buffer_[bytePos], 8);
            old |= acc;
            std::memcpy(&buffer_[bytePos], &old, 8);

            currentBitPos_ += totalBits;
            idx += chunkSize;
        }
    }

    // Position-based batch write for parallel encoding (no currentBitPos_ dependency).
    template <typename T>
        requires(std::is_integral<T>::value)
    void writeBitsBatch(const T* data, size_t count, uint8_t k, size_t pos)
    {
        if (k == 0 || count == 0) return;

        const uint64_t mask = (static_cast<uint64_t>(1) << k) - 1;
        const size_t valsPerChunk = 56 / k;

        size_t idx = 0;
        while (idx < count) {
            size_t chunkSize = std::min(valsPerChunk, count - idx);
            size_t totalBits = chunkSize * k;

            uint64_t acc = 0;
            for (size_t j = 0; j < chunkSize; ++j) {
                acc = (acc << k) | (static_cast<uint64_t>(data[idx + j]) & mask);
            }

            size_t bytePos = pos >> 3;
            size_t bitOffset = pos & 7;
            acc <<= (64 - totalBits - bitOffset);
            acc = bigEndian64(acc);

            uint64_t old;
            std::memcpy(&old, &buffer_[bytePos], 8);
            old |= acc;
            std::memcpy(&buffer_[bytePos], &old, 8);

            pos += totalBits;
            idx += chunkSize;
        }
    }


    template <typename IdType>
        requires(std::is_integral<IdType>::value && std::is_unsigned<IdType>::value)
    void writeId(IdType idShifted, uint8_t idLength)
    {
        idShifted >>= currentBitPos_ % 8;
        storeUnaligned<IdType>(&buffer_[currentBitPos_ / 8], bigEndian16(idShifted));
        currentBitPos_ += idLength;
    }


    template <typename T>
        requires(std::is_integral<T>::value)
    T readBits(const size_t nBits, size_t bitPos) const
    {
        using TPlus =
            typename std::conditional_t<sizeof(T) <= 1, uint16_t,
                                        typename std::conditional_t<sizeof(T) <= 2, uint32_t,
                                                                    typename std::conditional_t<sizeof(T) <= 4, uint64_t,
                                                                                                void>>>;

        assert(nBits <= sizeof(T) * 8 && "nBits exceeds size of T");

        const size_t bytePos = bitPos >> 3;
        const size_t bitOffset = bitPos % 8;

        TPlus valuePlus = loadUnaligned<TPlus>(&buffer_[bytePos]);
        if constexpr (sizeof(TPlus) == 2) {
            valuePlus = bigEndian16(valuePlus);
        }
        else if constexpr (sizeof(TPlus) == 4) {
            valuePlus = bigEndian32(valuePlus);
        }
        else if constexpr (sizeof(TPlus) == 8) {
            valuePlus = bigEndian64(valuePlus);
        }

        valuePlus >>= sizeof(TPlus) * 8 - nBits - bitOffset;
        TPlus mask = static_cast<TPlus>(~TPlus(0)) >> (sizeof(TPlus) * 8 - nBits);
        TPlus value = valuePlus & mask;
        return static_cast<T>(value);
    }

    template <typename T>
        requires(std::is_integral<T>::value)
    T readBits(const size_t nBits) const
    {
        size_t oldBitPos = currentBitPos_;
        currentBitPos_ += nBits;
        return readBits<T>(nBits, oldBitPos);
    }

    const Span<uint8_t>& getBuffer() const
    {
        return buffer_;
    }

    size_t bitPos() const
    {
        return currentBitPos_;
    }

    void bitPos(size_t newPos) const
    {
        currentBitPos_ = newPos;
    }

    void resetBitPos()
    {
        currentBitPos_ = 0;
    }

private:
    // ORs the 64 bits of value (first bit = most significant) into the 8 bytes at ptr
    static void orBigEndian64(uint8_t* ptr, uint64_t value)
    {
        value = bigEndian64(value);
        uint64_t old;
        std::memcpy(&old, ptr, 8);
        old |= value;
        std::memcpy(ptr, &old, 8);
    }

    Span<uint8_t> buffer_;
    mutable size_t currentBitPos_ = 0;
    size_t limitBits_ = 0;
};
