// SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// maxEncodedSize() must not wrap around: callers allocate the output buffer of the encoder with
// it. The product of the number of blocks and the bits per block overflowed size_t for large
// fields on 32-bit platforms (and for huge counts on 64-bit ones), giving a buffer far too small.
#include <cstdint>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>

#include "spaec/detail/SpAec.h"

using namespace spaec::detail;

// The exact size (nBlocks * bitsPerBlock + 7) / 8 + 8, computed without overflow in 64 bits
// (portable, no 128-bit integers): with nBlocks = 8q + r it is q * bitsPerBlock + (r * bitsPerBlock + 7) / 8 + 8.
// fits = false if the size needs more than 64 bits.
struct ExactSize
{
    bool fits;
    uint64_t value;
};

static ExactSize exactSize(uint64_t n, uint32_t blockSize, uint32_t bitsPerBlock)
{
    const uint64_t nBlocks = n / blockSize + (n % blockSize != 0 ? 1 : 0);
    const uint64_t q = nBlocks / 8, r = nBlocks % 8;
    const uint64_t tail = (r * bitsPerBlock + 7) / 8 + 8;
    if (q != 0 && bitsPerBlock > (UINT64_MAX - tail) / q) {
        return { false, 0 };
    }
    return { true, q * bitsPerBlock + tail };
}

int main()
{
    int32_t failures = 0;
    const size_t maxSize = std::numeric_limits<size_t>::max();
    const size_t counts[] = { 0, 1, 1000, 133920257, size_t{ 1 } << 31, maxSize / 64, maxSize / 8, maxSize / 2, maxSize - 1, maxSize };
    for (uint32_t bpv : { 1u, 8u, 16u, 24u, 32u }) {
        for (uint32_t blockSize : { 8u, 16u, 32u, 64u }) {
            for (size_t n : counts) {
                // Exact size: (nBlocks * (idLength + blockSize * bpv) + 7) / 8 + 8
                const uint32_t idLength = CodeOptionBase{ static_cast<uint8_t>(bpv), CodeOptionSet::Basic }.maxIdLength - 1u;
                const ExactSize exact = exactSize(n, blockSize, idLength + blockSize * bpv);
                const bool exactFits = exact.fits && exact.value <= maxSize;
                const std::string name = "n=" + std::to_string(n) + " bpv=" + std::to_string(bpv) + " blockSize=" + std::to_string(blockSize);
                try {
                    const size_t size = encoder::SpAec<double>::maxEncodedSize(n, static_cast<uint8_t>(bpv), static_cast<uint8_t>(blockSize));
                    if (!exactFits || size != exact.value) {
                        std::cerr << name << ": " << size << " bytes, but " << (!exactFits ? "the size does not fit in size_t" : "a different size is needed") << '\n';
                        ++failures;
                    }
                }
                catch (const std::length_error&) {
                    if (exactFits) {
                        std::cerr << name << ": unexpected exception\n";
                        ++failures;
                    }
                }
            }
        }
    }
    if (failures != 0) {
        std::cerr << failures << " cases failed\n";
        return 1;
    }
    std::cout << "OK\n";
    return 0;
}
