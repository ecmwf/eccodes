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

#include "spaec/SpAec.h"

using namespace spaec;

// Wide enough for the exact size of any size_t count
#if defined(__SIZEOF_INT128__)
__extension__ using Wide = unsigned __int128;  // __extension__: no -Wpedantic warning
#else
using Wide = uint64_t;  // 32-bit size_t
static_assert(sizeof(size_t) <= 4, "no integer type wide enough for the test");
#endif

int main()
{
    int failures = 0;
    const size_t maxSize = std::numeric_limits<size_t>::max();
    const size_t counts[] = { 0, 1, 1000, 133920257, size_t{ 1 } << 31, maxSize / 64, maxSize / 8, maxSize / 2, maxSize - 1, maxSize };
    for (unsigned bpv : { 1u, 8u, 16u, 24u, 32u }) {
        for (unsigned blockSize : { 8u, 16u, 32u, 64u }) {
            for (size_t n : counts) {
                // Exact size: (nBlocks * (idLength + blockSize * bpv) + 7) / 8 + 8
                const unsigned idLength = CodeOptionBase{ static_cast<uint8_t>(bpv), CodeOptionSet::Basic }.maxIdLength - 1u;
                const Wide nBlocks = (static_cast<Wide>(n) + blockSize - 1) / blockSize;
                const Wide exact = (nBlocks * (idLength + blockSize * bpv) + 7) / 8 + 8;
                const std::string name = "n=" + std::to_string(n) + " bpv=" + std::to_string(bpv) + " blockSize=" + std::to_string(blockSize);
                try {
                    const size_t size = encoder::SpAec<double>::maxEncodedSize(n, static_cast<uint8_t>(bpv), static_cast<uint8_t>(blockSize));
                    if (size != exact) {
                        std::cerr << name << ": " << size << " bytes, but " << (exact > maxSize ? "the size does not fit in size_t" : "a different size is needed") << '\n';
                        ++failures;
                    }
                }
                catch (const std::length_error&) {
                    if (exact <= maxSize) {
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
