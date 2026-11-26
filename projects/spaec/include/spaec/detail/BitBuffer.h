/*
 * SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <string>
#if defined(__AVX2__)
    #include <immintrin.h>
#endif

#include "spaec/detail/Compat.h"
#include "spaec/detail/Span.h"

// GCC without AVX2: the scalar writeSplitSample is inlined into the encoder, as GCC did before the
// encoders were instantiated for two split-sample selectors (since then GCC's limit on the growth
// of a translation unit keeps it from inlining). The other compilers and AVX2 builds decide as
// before (an inline keyword would change the decisions of Clang and icx).
#if defined(__GNUC__) && !defined(__clang__) && !defined(__AVX2__)
    #define SPAEC_SCALAR_INLINE __attribute__((always_inline)) inline
#else
    #define SPAEC_SCALAR_INLINE
#endif

namespace spaec::detail
{

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
        if (SPAEC_UNLIKELY(pos > limitBits_)) {
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
                size_t lz = countlZero(acc);
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
            return countlZero(acc) + 1;
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
        zeros += countlZero(static_cast<unsigned>(buffer_[bytepos])) - 24;
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
    void writeCodewords(const T* data, size_t count, unsigned k = 0)
    {
        static_assert(std::is_integral<T>::value, "integral type required");
        uint8_t* out = buffer_.data();
        size_t base = currentBitPos_ & ~size_t(7);  // bit position of the register's first bit
        size_t fill = currentBitPos_ - base;        // bits of the register in use
        uint64_t acc = 0;
        constexpr uint64_t top = uint64_t(1) << 63;
        for (size_t i = 0; i < count; ++i) {
            const uint64_t zeros = static_cast<uint64_t>(data[i]) >> k;  // 64 bits also where size_t has 32
            if (SPAEC_UNLIKELY(fill + zeros >= 64)) {
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

    // Writes the coded data of a split-sample block in one pass over the samples (CCSDS 120.0-G-4,
    // 5.5: J iterations instead of 2 J): the FS codewords of data[i] >> k at the current position
    // and, fsBits bits further (fsBits: the length of the codewords), the k low bits of every
    // sample. Both parts are ORed into the buffer, which must be zero after the current position.
    // Instance: a separate instance per caller (the split-sample selector of the encoder), so that
    // each has one caller, as before the second selector: GCC inlines functions called once
    template <typename T, int Instance = 0>
    SPAEC_SCALAR_INLINE void writeSplitSample(const T* data, size_t count, unsigned k, uint64_t fsBits)
    {
        static_assert(std::is_integral<T>::value, "integral type required");
        if (k == 0) {
            writeCodewords(data, count, 0);
            return;
        }
        const uint64_t end = currentBitPos_ + fsBits + uint64_t{ count } * k;
        if (fsBits > limitBits_ || end > limitBits_) {
            throwWriteOverrun(end, limitBits_);  // never write beyond the buffer
        }

#if defined(__AVX2__)
        if constexpr (sizeof(T) == 2) {
            // 16 bytes are stored at the start of the block and 8 at its end: not at the end of the buffer
            const uint64_t limitBytes = limitBits_ / 8;
            if (SPAEC_LIKELY(count % 16 == 0 && k <= 13 && (currentBitPos_ & 7) + fsBits <= 128 && currentBitPos_ / 8 + 16 <= limitBytes &&
                             end / 8 + 8 <= limitBytes)) {
                writeSplitSampleAvx2<Instance>(reinterpret_cast<const uint16_t*>(data), count, k, static_cast<size_t>(fsBits));
                return;
            }
        }
#endif

        uint8_t* out = buffer_.data();
        size_t base = currentBitPos_ & ~size_t(7);  // FS part: bit position of the register's first bit
        size_t fill = currentBitPos_ - base;        // bits of the register in use
        uint64_t acc = 0;
        constexpr uint64_t top = uint64_t(1) << 63;
        size_t splitPos = currentBitPos_ + static_cast<size_t>(fsBits);  // split part
        const uint64_t mask = (uint64_t{ 1 } << k) - 1;
        const size_t perChunk = 56 / k;

        for (size_t idx = 0; idx < count; idx += perChunk) {
            const size_t chunk = std::min(perChunk, count - idx);
            uint64_t split = 0;
            for (size_t j = 0; j < chunk; ++j) {
                const uint64_t v = static_cast<uint64_t>(data[idx + j]);
                const uint64_t zeros = v >> k;
                if (SPAEC_UNLIKELY(fill + zeros >= 64)) {
                    orBigEndian64(out + base / 8, acc);
                    const uint64_t onePos = base + fill + zeros;
                    base = static_cast<size_t>(onePos) & ~size_t(7);
                    fill = static_cast<size_t>(onePos) - base;
                    acc = 0;
                }
                else {
                    fill += zeros;
                }
                acc |= top >> fill;
                ++fill;
                split = (split << k) | (v & mask);
            }
            const size_t bits = chunk * k;
            orBigEndian64(out + splitPos / 8, split << (64 - bits - splitPos % 8));
            splitPos += bits;
        }
        orBigEndian64(out + base / 8, acc);
        assert(base + fill == currentBitPos_ + fsBits && "fsBits is not the length of the codewords");
        currentBitPos_ = splitPos;
    }

#if defined(__AVX2__)
    // writeSplitSample for 16-bit samples with AVX2: count is a multiple of 16, 1 <= k <= 13, and the
    // FS codewords end within 128 bits of the byte of the current position (fsBits is their exact
    // length). 16 samples per step:
    // - FS codewords: the positions of their terminating one bits are a prefix sum of their
    //   lengths; the bits are set in two 64-bit words with variable shifts (a shift count of 64
    //   or more gives 0: a position selects its word without a comparison)
    // - split bits: a multiply-add joins two k-bit fields, a shift two such pairs: 4 samples per
    //   64-bit word, appended to the stream through a carry register with plain stores
    template <int Instance>
    void writeSplitSampleAvx2(const uint16_t* data, size_t count, unsigned k, size_t fsBits)
    {
        uint8_t* out = buffer_.data();
        const size_t pos = currentBitPos_;
        uint8_t* base = out + pos / 8;
        const __m128i shiftK = _mm_cvtsi32_si128(static_cast<int>(k));
        const __m128i shift2K = _mm_cvtsi32_si128(static_cast<int>(2 * k));
        const __m256i one = _mm256_set1_epi16(1);
        const __m256i top = _mm256_set1_epi64x(static_cast<long long>(uint64_t{ 1 } << 63));
        const __m256i c64 = _mm256_set1_epi64x(64);
        const __m256i mask = _mm256_set1_epi16(static_cast<short>((1u << k) - 1));
        const __m256i mul = _mm256_set1_epi32(static_cast<int>((1u << 16) | (1u << k)));  // (2^k, 1) per pair
        const __m256i low32 = _mm256_set1_epi64x(0xFFFFFFFFll);
        __m256i w0 = _mm256_setzero_si256();
        __m256i w1 = _mm256_setzero_si256();
        unsigned next = static_cast<unsigned>(pos & 7);  // position of the next codeword in the two words
        alignas(32) uint64_t lows[16];                   // count <= 64: 4 samples each
        for (size_t i = 0; i < count; i += 16) {
            const __m256i x = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(data + i));

            const __m256i pairs = _mm256_madd_epi16(_mm256_and_si256(x, mask), mul);
            const __m256i quads = _mm256_or_si256(_mm256_sll_epi64(_mm256_and_si256(pairs, low32), shift2K), _mm256_srli_epi64(pairs, 32));
            _mm256_store_si256(reinterpret_cast<__m256i*>(lows + i / 4), quads);

            __m256i p = _mm256_add_epi16(_mm256_srl_epi16(x, shiftK), one);  // lengths
            p = _mm256_add_epi16(p, _mm256_slli_si256(p, 2));
            p = _mm256_add_epi16(p, _mm256_slli_si256(p, 4));
            p = _mm256_add_epi16(p, _mm256_slli_si256(p, 8));  // prefix sums in each half
            __m256i lower = _mm256_permute2x128_si256(p, p, 0x08);
            lower = _mm256_shuffle_epi32(_mm256_shufflehi_epi16(lower, 0xFF), 0xFF);  // the sum of the lower half
            p = _mm256_add_epi16(_mm256_add_epi16(p, lower), _mm256_set1_epi16(static_cast<short>(next - 1)));
            next = static_cast<unsigned>(_mm256_extract_epi16(p, 15)) + 1;

            const __m128i pl = _mm256_castsi256_si128(p);
            const __m128i ph = _mm256_extracti128_si256(p, 1);
            const __m256i e0 = _mm256_cvtepu16_epi64(pl);
            const __m256i e1 = _mm256_cvtepu16_epi64(_mm_srli_si128(pl, 8));
            const __m256i e2 = _mm256_cvtepu16_epi64(ph);
            const __m256i e3 = _mm256_cvtepu16_epi64(_mm_srli_si128(ph, 8));
            w0 = _mm256_or_si256(w0, _mm256_or_si256(_mm256_or_si256(_mm256_srlv_epi64(top, e0), _mm256_srlv_epi64(top, e1)),
                                                     _mm256_or_si256(_mm256_srlv_epi64(top, e2), _mm256_srlv_epi64(top, e3))));
            w1 = _mm256_or_si256(w1, _mm256_or_si256(_mm256_or_si256(_mm256_srlv_epi64(top, _mm256_sub_epi64(e0, c64)),
                                                                     _mm256_srlv_epi64(top, _mm256_sub_epi64(e1, c64))),
                                                     _mm256_or_si256(_mm256_srlv_epi64(top, _mm256_sub_epi64(e2, c64)),
                                                                     _mm256_srlv_epi64(top, _mm256_sub_epi64(e3, c64)))));
        }
        assert(next == (pos & 7) + fsBits && "fsBits is not the length of the codewords");
        const __m128i a0 = _mm_or_si128(_mm256_castsi256_si128(w0), _mm256_extracti128_si256(w0, 1));
        const __m128i a1 = _mm_or_si128(_mm256_castsi256_si128(w1), _mm256_extracti128_si256(w1, 1));
        const uint64_t f0 = static_cast<uint64_t>(_mm_cvtsi128_si64(a0)) | static_cast<uint64_t>(_mm_extract_epi64(a0, 1));
        const uint64_t f1 = static_cast<uint64_t>(_mm_cvtsi128_si64(a1)) | static_cast<uint64_t>(_mm_extract_epi64(a1, 1));
        // The bits before the block (in its first byte) are kept; the buffer is zero after the block
        storeBigEndian64(base, f0 | (static_cast<uint64_t>(*base) << 56));
        storeBigEndian64(base + 8, f1);

        const size_t splitPos = pos + fsBits;
        uint8_t* q = out + splitPos / 8;
        unsigned fill = static_cast<unsigned>(splitPos & 7);
        uint64_t acc = static_cast<uint64_t>(*q) << 56;  // the last bits of the codewords
        const unsigned bits = 4 * k;
        for (size_t i = 0; i < count / 4; ++i) {
            acc |= lows[i] << (64 - bits - fill);
            storeBigEndian64(q, acc);
            const unsigned bytes = (fill + bits) >> 3;
            q += bytes;
            acc = (acc << (bytes * 4)) << (bytes * 4);  // by up to 64 bits
            fill = (fill + bits) & 7;
        }
        storeBigEndian64(q, acc);
        currentBitPos_ = splitPos + count * k;
    }
#endif

    template <typename T>
    void writeBits(T value, size_t nBits)
    {
        static_assert(std::is_integral<T>::value, "integral type required");
        using TPlus =
            typename std::conditional_t<sizeof(T) <= 1, uint16_t,
                                        typename std::conditional_t<sizeof(T) <= 2, uint32_t,
                                                                    typename std::conditional_t<sizeof(T) <= 4, uint64_t,
                                                                                                void>>>;

        TPlus valuePlus = static_cast<TPlus>(value);

        assert(nBits <= sizeof(T) * 8 && "nBits exceeds size of T");

        // Only the low nBits bits of the value are written: values with more bits (bits per
        // sample below the sample type width) must not leak into the following bits of the stream
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


    template <typename IdType>
    void writeId(IdType idShifted, uint8_t idLength)
    {
        static_assert(std::is_integral<IdType>::value && std::is_unsigned<IdType>::value, "unsigned integral type required");
        idShifted >>= currentBitPos_ % 8;
        storeUnaligned<IdType>(&buffer_[currentBitPos_ / 8], bigEndian16(idShifted));
        currentBitPos_ += idLength;
    }


    template <typename T>
    T readBits(const size_t nBits, size_t bitPos) const
    {
        static_assert(std::is_integral<T>::value, "integral type required");
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
    T readBits(const size_t nBits) const
    {
        static_assert(std::is_integral<T>::value, "integral type required");
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
    static void storeBigEndian64(uint8_t* ptr, uint64_t value)
    {
        value = bigEndian64(value);
        std::memcpy(ptr, &value, 8);
    }

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

}  // namespace spaec::detail
