/*
 * SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <cstddef>
#include <cstdint>
#include <type_traits>

#if defined(__AVX2__) || defined(__SSE2__)
    #include <immintrin.h>
#endif

namespace spaec::detail::encoder
{

// blockShiftedSum<J>(v, k, bits): the sum of the J samples v[0..J) shifted right by k, the sums of
// the split-sample selector SplitSelector::Estimate (k = 0: the sum of the block). J is 8, 16, 32
// or 64 (a constant: no loop prologue or epilogue); the samples have at most bits bits.
//
// SIMD (AVX2, else SSE2 on x86-64; other targets: a loop the compilers vectorise):
// - 8-bit samples: _mm_sad_epu8 sums 8 bytes into a 64-bit lane (one instruction per 16 or 32
//   samples); the shift of the bytes is a 16-bit shift and a mask
// - 16-bit samples: AVX2 unpacks into 32-bit lanes. SSE2: if bits - k + log2(J / 8) <= 15, the
//   vectors are added in 16-bit lanes and one _mm_madd_epi16 (by 1) sums pairs into 32-bit lanes;
//   else madd on samples biased by -2^15 (xor 0x8000; madd multiplies signed numbers), the bias
//   added back once (J * 2^15). (Measured on Zen 3: with AVX2 both madd variants are slower than
//   unpacking, with SSE2 faster.)
// - 32-bit samples: 32-bit lanes if they cannot overflow (bits - k + log2(J / lanes) <= 32), else
//   the low and the high 16 bits of each sample in separate 32-bit lanes (at most 64 * (2^16 - 1))

namespace sums
{

#if defined(__AVX2__)
inline int64_t total32(__m256i a)  // the sum of 8 signed 32-bit lanes
{
    __m128i t = _mm_add_epi32(_mm256_castsi256_si128(a), _mm256_extracti128_si256(a, 1));
    t = _mm_add_epi32(t, _mm_shuffle_epi32(t, 0x4E));
    t = _mm_add_epi32(t, _mm_shuffle_epi32(t, 0xB1));
    return _mm_cvtsi128_si32(t);
}
inline uint64_t total64(__m256i a)  // the sum of 4 64-bit lanes
{
    const __m128i t = _mm_add_epi64(_mm256_castsi256_si128(a), _mm256_extracti128_si256(a, 1));
    return static_cast<uint64_t>(_mm_cvtsi128_si64(_mm_add_epi64(t, _mm_unpackhi_epi64(t, t))));
}
inline uint64_t totalU32(__m256i a)  // the sum of 8 unsigned 32-bit lanes, in 64 bits
{
    const __m256i zero = _mm256_setzero_si256();
    return total64(_mm256_add_epi64(_mm256_unpacklo_epi32(a, zero), _mm256_unpackhi_epi32(a, zero)));
}
inline __m256i widen16(__m256i x)  // 16 unsigned 16-bit lanes added pairwise into 8 32-bit lanes
{
    const __m256i zero = _mm256_setzero_si256();
    return _mm256_add_epi32(_mm256_unpacklo_epi16(x, zero), _mm256_unpackhi_epi16(x, zero));
}
#endif

#if defined(__SSE2__)
inline int64_t total32(__m128i t)  // the sum of 4 signed 32-bit lanes
{
    t = _mm_add_epi32(t, _mm_shuffle_epi32(t, 0x4E));
    t = _mm_add_epi32(t, _mm_shuffle_epi32(t, 0xB1));
    return _mm_cvtsi128_si32(t);
}
inline uint64_t total64(__m128i t)  // the sum of 2 64-bit lanes
{
    return static_cast<uint64_t>(_mm_cvtsi128_si64(_mm_add_epi64(t, _mm_unpackhi_epi64(t, t))));
}
inline uint64_t totalU32(__m128i a)  // the sum of 4 unsigned 32-bit lanes, in 64 bits
{
    const __m128i zero = _mm_setzero_si128();
    return total64(_mm_add_epi64(_mm_unpacklo_epi32(a, zero), _mm_unpackhi_epi32(a, zero)));
}
#endif

}  // namespace sums

// log2(n) of a power of two
constexpr unsigned log2Of(size_t n)
{
    return n <= 1 ? 0 : 1 + log2Of(n / 2);
}

// The variants per instruction set: hasAvx2Sum / hasSse2Sum tell which sample types and block sizes
// they handle (declared without definition where the instruction set is missing: never used)
template <size_t J, typename T>
constexpr bool hasAvx2Sum = (sizeof(T) == 1 && J % 32 == 0) || (sizeof(T) == 2 && J % 16 == 0) || sizeof(T) == 4;
template <size_t J, typename T>
constexpr bool hasSse2Sum = true;

#if defined(__AVX2__)
template <size_t J, typename T>
__attribute__((always_inline)) inline uint64_t avx2Sum(const T* v, unsigned k, [[maybe_unused]] unsigned bits)
{
    if constexpr (sizeof(T) == 1 && J % 32 == 0) {
        const __m256i zero = _mm256_setzero_si256();
        const __m256i mask = _mm256_set1_epi8(static_cast<char>(0xFF >> k));
        const __m128i shift = _mm_cvtsi32_si128(static_cast<int>(k));
        __m256i acc = zero;
        for (size_t j = 0; j < J; j += 32) {
            const __m256i x = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(v + j));
            acc = _mm256_add_epi64(acc, _mm256_sad_epu8(_mm256_and_si256(_mm256_srl_epi16(x, shift), mask), zero));
        }
        return sums::total64(acc);
    }
    else if constexpr (sizeof(T) == 2 && J % 16 == 0) {
        const __m128i shift = _mm_cvtsi32_si128(static_cast<int>(k));
        __m256i acc = _mm256_setzero_si256();
        for (size_t j = 0; j < J; j += 16) {
            acc = _mm256_add_epi32(acc, sums::widen16(_mm256_srl_epi16(_mm256_loadu_si256(reinterpret_cast<const __m256i*>(v + j)), shift)));
        }
        return static_cast<uint32_t>(sums::total32(acc));  // < 2^32: zero-extended (fewer instructions)
    }
    else if constexpr (sizeof(T) == 4) {
        const __m128i shift = _mm_cvtsi32_si128(static_cast<int>(k));
        if (bits - k + log2Of(J / 8) <= 32) {  // 8 lanes of J / 8 samples
            __m256i acc = _mm256_setzero_si256();
            for (size_t j = 0; j < J; j += 8) {
                acc = _mm256_add_epi32(acc, _mm256_srl_epi32(_mm256_loadu_si256(reinterpret_cast<const __m256i*>(v + j)), shift));
            }
            return sums::totalU32(acc);
        }
        const __m256i low16 = _mm256_set1_epi32(0xFFFF);
        __m256i low = _mm256_setzero_si256(), high = _mm256_setzero_si256();
        for (size_t j = 0; j < J; j += 8) {
            const __m256i x = _mm256_srl_epi32(_mm256_loadu_si256(reinterpret_cast<const __m256i*>(v + j)), shift);
            low = _mm256_add_epi32(low, _mm256_and_si256(x, low16));
            high = _mm256_add_epi32(high, _mm256_srli_epi32(x, 16));
        }
        return sums::totalU32(low) + (sums::totalU32(high) << 16);
    }
    else {
        static_assert(J == 0, "no variant for this sample type and block size");
        return 0;
    }
}
#else
template <size_t J, typename T>
uint64_t avx2Sum(const T* v, unsigned k, unsigned bits);
#endif

#if defined(__SSE2__)
template <size_t J, typename T>
__attribute__((always_inline)) inline uint64_t sse2Sum(const T* v, unsigned k, [[maybe_unused]] unsigned bits)
{
    if constexpr (sizeof(T) == 1 && J % 16 == 0) {
        const __m128i zero = _mm_setzero_si128();
        const __m128i mask = _mm_set1_epi8(static_cast<char>(0xFF >> k));
        const __m128i shift = _mm_cvtsi32_si128(static_cast<int>(k));
        __m128i acc = zero;
        for (size_t j = 0; j < J; j += 16) {
            const __m128i x = _mm_loadu_si128(reinterpret_cast<const __m128i*>(v + j));
            acc = _mm_add_epi64(acc, _mm_sad_epu8(_mm_and_si128(_mm_srl_epi16(x, shift), mask), zero));
        }
        return sums::total64(acc);
    }
    else if constexpr (sizeof(T) == 1) {
        const __m128i x = _mm_loadl_epi64(reinterpret_cast<const __m128i*>(v));
        const __m128i y = _mm_and_si128(_mm_srl_epi16(x, _mm_cvtsi32_si128(static_cast<int>(k))), _mm_set1_epi8(static_cast<char>(0xFF >> k)));
        return static_cast<uint64_t>(_mm_cvtsi128_si64(_mm_sad_epu8(y, _mm_setzero_si128())));
    }
    else if constexpr (sizeof(T) == 2) {
        const __m128i ones = _mm_set1_epi16(1);
        const __m128i shift = _mm_cvtsi32_si128(static_cast<int>(k));
        if (bits - k + log2Of(J / 8) <= 15) {
            // The J / 8 vectors added in 16-bit lanes stay below 2^15: one madd into 32-bit lanes
            __m128i acc = _mm_setzero_si128();
            for (size_t j = 0; j < J; j += 8) {
                acc = _mm_add_epi16(acc, _mm_srl_epi16(_mm_loadu_si128(reinterpret_cast<const __m128i*>(v + j)), shift));
            }
            return static_cast<uint32_t>(sums::total32(_mm_madd_epi16(acc, ones)));
        }
        const __m128i bias = _mm_set1_epi16(static_cast<short>(0x8000));
        __m128i acc = _mm_setzero_si128();
        for (size_t j = 0; j < J; j += 8) {
            const __m128i x = _mm_srl_epi16(_mm_loadu_si128(reinterpret_cast<const __m128i*>(v + j)), shift);
            acc = _mm_add_epi32(acc, _mm_madd_epi16(_mm_xor_si128(x, bias), ones));
        }
        return static_cast<uint32_t>(sums::total32(acc) + static_cast<int64_t>(J) * 32768);
    }
    else if constexpr (sizeof(T) == 4) {
        const __m128i shift = _mm_cvtsi32_si128(static_cast<int>(k));
        if (bits - k + log2Of(J / 4) <= 32) {  // 4 lanes of J / 4 samples
            __m128i acc = _mm_setzero_si128();
            for (size_t j = 0; j < J; j += 4) {
                acc = _mm_add_epi32(acc, _mm_srl_epi32(_mm_loadu_si128(reinterpret_cast<const __m128i*>(v + j)), shift));
            }
            return sums::totalU32(acc);
        }
        const __m128i low16 = _mm_set1_epi32(0xFFFF);
        __m128i low = _mm_setzero_si128(), high = _mm_setzero_si128();
        for (size_t j = 0; j < J; j += 4) {
            const __m128i x = _mm_srl_epi32(_mm_loadu_si128(reinterpret_cast<const __m128i*>(v + j)), shift);
            low = _mm_add_epi32(low, _mm_and_si128(x, low16));
            high = _mm_add_epi32(high, _mm_srli_epi32(x, 16));
        }
        return sums::totalU32(low) + (sums::totalU32(high) << 16);
    }
    else {
        static_assert(J == 0, "no variant for this sample type and block size");
        return 0;
    }
}
#else
template <size_t J, typename T>
uint64_t sse2Sum(const T* v, unsigned k, unsigned bits);
#endif

// Always inlined: a few instructions per instance, called twice per block (GCC would call it)
template <size_t J, typename T>
__attribute__((always_inline)) inline uint64_t blockShiftedSum(const T* v, unsigned k, [[maybe_unused]] unsigned bits)
{
    static_assert(std::is_unsigned<T>::value && sizeof(T) <= 4, "samples of 8, 16 or 32 bits");
    static_assert(J == 8 || J == 16 || J == 32 || J == 64, "block size 8, 16, 32 or 64");
#if defined(__AVX2__)
    constexpr bool avx2 = hasAvx2Sum<J, T>;
#else
    constexpr bool avx2 = false;
#endif
#if defined(__SSE2__)
    constexpr bool sse2 = hasSse2Sum<J, T>;
#else
    constexpr bool sse2 = false;
#endif
    if constexpr (avx2) {
        return avx2Sum<J>(v, k, bits);
    }
    else if constexpr (sse2) {
        return sse2Sum<J>(v, k, bits);
    }
    else {
        using Sum = std::conditional_t<(sizeof(T) <= 2), uint32_t, uint64_t>;
        Sum s = 0;
        for (size_t j = 0; j < J; ++j) {
            s += static_cast<Sum>(v[j] >> k);
        }
        return s;
    }
}

}  // namespace spaec::detail::encoder
