#pragma once

#include <cstddef>
#include <limits>
#include <utility>


size_t nSplits(size_t totalSize, size_t splitSize);

#if defined(_MSC_VER)
  #include <stdlib.h>
  #define bswap16(x) _byteswap_ushort(x)
  #define bswap32(x) _byteswap_ulong(x)
  #define bswap64(x) _byteswap_uint64(x)
#elif defined(__GNUC__) || defined(__clang__)
#if defined(__APPLE__)
  #include <libkern/OSByteOrder.h>
  #define bswap16(x) OSSwapInt16(x)
  #define bswap32(x) OSSwapInt32(x)
  #define bswap64(x) OSSwapInt64(x)
#else
  #include <byteswap.h>
  #define bswap16(x) __builtin_bswap16(x)
  #define bswap32(x) __builtin_bswap32(x)
  #define bswap64(x) __builtin_bswap64(x)
#endif

#else
// Fallback (portable but slower)
static inline uint16_t bswap16(uint16_t x) {
    return ((x >> 8) & 0x00FF) |
        ((x << 8) & 0xFF00);
}
static inline uint32_t bswap32(uint32_t x) {
    return ((x >> 24) & 0x000000FF) |
        ((x >> 8)  & 0x0000FF00) |
        ((x << 8)  & 0x00FF0000) |
        ((x << 24) & 0xFF000000);
}
static inline uint64_t bswap64(uint64_t x) {
    return ((x >> 56) & 0x00000000000000FFULL) |
        ((x >> 40) & 0x000000000000FF00ULL) |
        ((x >> 24) & 0x0000000000FF0000ULL) |
        ((x >> 8)  & 0x00000000FF000000ULL) |
        ((x << 8)  & 0x000000FF00000000ULL) |
        ((x << 24) & 0x0000FF0000000000ULL) |
        ((x << 40) & 0x00FF000000000000ULL) |
        ((x << 56) & 0xFF00000000000000ULL);
}
#endif
