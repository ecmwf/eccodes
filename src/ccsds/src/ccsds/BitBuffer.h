#pragma once

#include <cstdint>
#include <cstddef>
#include <cstring>
#include <cassert>
#include <byteswap.h>

#include "Span.h"

constexpr bool isLittleEndian = (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__);

class BitBuffer {
    private:
        // Safe unaligned load/store to avoid strict aliasing UB
        template <typename T>
        static T loadUnaligned(const uint8_t* ptr) {
            T val;
            std::memcpy(&val, ptr, sizeof(T));
            return val;
        }
        template <typename T>
        static void storeUnaligned(uint8_t* ptr, T val) {
            T old;
            std::memcpy(&old, ptr, sizeof(T));
            old |= val;
            std::memcpy(ptr, &old, sizeof(T));
        }

    public:
        BitBuffer(const BitBuffer& buffer, size_t bitPos = 0) : buffer_{buffer.buffer_}, currentBitPos_{bitPos} {}
        BitBuffer(Span<uint8_t> data, size_t bitPos = 0) : buffer_{data}, currentBitPos_{bitPos} {}

        uint16_t peekBits(size_t nBits, size_t offset) const {
            using TPlus = uint16_t;

            constexpr TPlus ones = ~static_cast<TPlus>(0);
            size_t bytePos = offset / 8;
            size_t bitOffset = offset % 8;

            TPlus data = bswap_16(loadUnaligned<TPlus>(&buffer_[bytePos]));
            data >>= (sizeof(TPlus) * 8 - nBits - bitOffset);

            TPlus mask = ones >> (sizeof(TPlus) * 8 - nBits);
            return data & mask;
        }

        uint16_t peekBits(size_t nBits) const {
            return peekBits(nBits, currentBitPos_);
        }

        void skipBits(size_t nBits) const {
            currentBitPos_ += nBits;
        }


        void skipCodewords(size_t nCodewords) const {
            while (nCodewords > 0) {
                size_t bytepos = currentBitPos_ / 8;
                size_t bitpos = currentBitPos_ % 8;

                uint64_t acc = bswap_64(loadUnaligned<uint64_t>(&buffer_[bytepos]));
                acc <<= bitpos;

                while (acc != 0 && nCodewords > 0) {
                    // Find the next 1-bit (codeword terminator)
                    size_t lz = static_cast<size_t>(__builtin_clzll(acc));
                    size_t cwBits = lz + 1; // zeros + terminating 1
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
                }
            }
        }


        size_t readCodeword(size_t pos) const {
            size_t bytepos = pos / 8;
            size_t bitpos = pos % 8;

            uint64_t acc = bswap_64(loadUnaligned<uint64_t>(&buffer_[bytepos]));
            acc <<= bitpos;

            if (acc != 0) {
                return static_cast<size_t>(__builtin_clzll(acc)) + 1;
            }

            // Codeword spans more than 56 bits — fall back to byte scanning
            size_t zeros = 64 - bitpos;
            bytepos += 8;
            while (buffer_[bytepos] == 0) {
                zeros += 8;
                bytepos++;
                assert(bytepos <= buffer_.size() && "Codeword exceeds buffer size");
            }
            zeros += static_cast<size_t>(__builtin_clz(static_cast<unsigned>(buffer_[bytepos]))) - 24;
            return zeros + 1;
        }

        size_t readCodeword() const {
            size_t result = readCodeword(currentBitPos_);
            currentBitPos_ += result;
            return result;
        }

        void writeCodeword(size_t length, size_t pos) {
            // size_t bytepos = pos / 8;
            // size_t bitpos = pos % 8;

            size_t bitOffset = (pos + length) % 8;
            uint8_t mask = 0b1000'0000 >> bitOffset;
            buffer_[(pos + length) / 8] |= mask;
        }

        void writeCodeword(size_t length) {
            writeCodeword(length, currentBitPos_);
            currentBitPos_ += length + 1;
        }

        template<typename T> requires(std::is_integral<T>::value)
            void writeBits(T value, size_t nBits, size_t pos)
            {
                using TPlus =
                    typename std::conditional_t <sizeof(T) <= 1, uint16_t,
                             typename std::conditional_t <sizeof(T) <= 2, uint32_t,
                             typename std::conditional_t <sizeof(T) <= 4, uint64_t,
                             void>>>;

                TPlus valuePlus = static_cast<TPlus>(value);

                assert(nBits <= sizeof(T) * 8 && "nBits exceeds size of T");

                constexpr uint8_t nbitsInTplus = sizeof(TPlus) * 8;
                constexpr uint8_t nbytesInTplus = sizeof(TPlus);
                const auto shift = pos % 8;
                valuePlus = valuePlus << (nbitsInTplus - nBits - shift);

                if constexpr (sizeof(TPlus) == 2) {
                    valuePlus = bswap_16(valuePlus);
                } else if constexpr (sizeof(TPlus) == 4) {
                    valuePlus = bswap_32(valuePlus);
                } else if constexpr (sizeof(TPlus) == 8) {
                    valuePlus = bswap_64(valuePlus);
                }
                storeUnaligned<TPlus>(&buffer_[pos / 8], valuePlus);
            }

        template<typename T> requires(std::is_integral<T>::value)
            void writeBits(T value, size_t nBits) {
                writeBits(value, nBits, currentBitPos_);
                currentBitPos_ += nBits;
            }

        // Batch write: packs k-bit values from an array using 64-bit accumulator.
        // Reduces memory operations by ~8x compared to per-value writeBits.
        template<typename T> requires(std::is_integral<T>::value)
        void writeBitsBatch(const T* data, size_t count, uint8_t k) {
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
                acc = __builtin_bswap64(acc);

                uint64_t old;
                std::memcpy(&old, &buffer_[bytePos], 8);
                old |= acc;
                std::memcpy(&buffer_[bytePos], &old, 8);

                currentBitPos_ += totalBits;
                idx += chunkSize;
            }
        }

        // Position-based batch write for parallel encoding (no currentBitPos_ dependency).
        template<typename T> requires(std::is_integral<T>::value)
        void writeBitsBatch(const T* data, size_t count, uint8_t k, size_t pos) {
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
                acc = __builtin_bswap64(acc);

                uint64_t old;
                std::memcpy(&old, &buffer_[bytePos], 8);
                old |= acc;
                std::memcpy(&buffer_[bytePos], &old, 8);

                pos += totalBits;
                idx += chunkSize;
            }
        }


        template <typename IdType> requires(std::is_integral<IdType>::value && std::is_unsigned<IdType>::value)
            void writeId(IdType idShifted, uint8_t idLength, size_t pos) {
                idShifted >>= pos % 8;
                storeUnaligned<IdType>(&buffer_[pos / 8], bswap_16(idShifted));
            }

        template <typename IdType> requires(std::is_integral<IdType>::value && std::is_unsigned<IdType>::value)
            void writeId(IdType idShifted, uint8_t idLength) {
                writeId(idShifted, idLength, currentBitPos_);
                currentBitPos_ += idLength;
            }


        template <typename T> requires(std::is_integral<T>::value)
            T readBits(const size_t nBits, size_t bitPos) const {
                using TPlus =
                    typename std::conditional_t <sizeof(T) <= 1, uint16_t,
                             typename std::conditional_t <sizeof(T) <= 2, uint32_t,
                             typename std::conditional_t <sizeof(T) <= 4, uint64_t,
                             void>>>;

                assert(nBits <= sizeof(T) * 8 && "nBits exceeds size of T");

                const size_t bytePos = bitPos >> 3;
                const size_t bitOffset = bitPos % 8;

                TPlus valuePlus = loadUnaligned<TPlus>(&buffer_[bytePos]);
                if constexpr (sizeof(TPlus) == 2) {
                    valuePlus = bswap_16(valuePlus);
                } else if constexpr (sizeof(TPlus) == 4) {
                    valuePlus = bswap_32(valuePlus);
                } else if constexpr (sizeof(TPlus) == 8) {
                    valuePlus = bswap_64(valuePlus);
                }

                valuePlus >>= sizeof(TPlus) * 8 - nBits - bitOffset;
                TPlus mask = static_cast<TPlus>(~TPlus(0)) >> (sizeof(TPlus) * 8 - nBits);
                TPlus value = valuePlus & mask;
                return static_cast<T>(value);
            }

        template <typename T> requires(std::is_integral<T>::value)
            T readBits(const size_t nBits) const {
                size_t oldBitPos = currentBitPos_;
                currentBitPos_ += nBits;
                return readBits<T>(nBits, oldBitPos);
            }

        const Span<uint8_t>& getBuffer() const {
            return buffer_;
        }

        size_t bitPos() const {
            return currentBitPos_;
        }

        void bitPos(size_t newPos) const {
            currentBitPos_ = newPos;
        }

        void resetBitPos() {
            currentBitPos_ = 0;
        }

    private:
        Span<uint8_t> buffer_;
        mutable size_t currentBitPos_ = 0;
};
