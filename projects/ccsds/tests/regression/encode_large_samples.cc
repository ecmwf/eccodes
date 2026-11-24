// Encoding blocks of large samples (here: alternating 0 and the largest value) must choose the
// code option from the exact sizes. On 32-bit platforms the option sizes were computed in size_t:
// the sum of the second-extension codewords (about 2^63 for 31-bit samples) wrapped around, the
// second extension was chosen, and its codewords (2^30 zero bits) were written far beyond the
// output buffer (segmentation fault).
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

#include "ccsds/SpAec.h"

template <typename T>
static int check(unsigned bpv, unsigned blockSize, unsigned rsi, size_t n)
{
    const uint64_t maxValue = (uint64_t{ 1 } << bpv) - 1;
    std::vector<T> values(n);
    for (size_t i = 0; i < n; ++i) {
        values[i] = static_cast<T>((i % 2) ? maxValue : 0);
    }

    const std::string name = "bpv=" + std::to_string(bpv) + " blockSize=" + std::to_string(blockSize) + " rsi=" + std::to_string(rsi) +
                             " n=" + std::to_string(n);
    try {
        encoder::EncoderSeq<T, T> encoder(static_cast<uint8_t>(bpv), static_cast<uint8_t>(blockSize), static_cast<uint16_t>(rsi), CodeOptionSet::Basic);
        const size_t maxSize = encoder::SpAec<double>::maxEncodedSize(n, static_cast<uint8_t>(bpv), static_cast<uint8_t>(blockSize));
        std::vector<uint8_t> encoded(maxSize, 0);
        const size_t length = encoder.encode(Span<T>(values), Span<uint8_t>(encoded));
        // Alternating extremes do not compress: about bpv bits per value
        if (length > maxSize || length > (n * bpv + 7) / 8 + (n / blockSize + 1) * 2 + 16) {
            std::cerr << name << ": encoded size " << length << " (at most " << maxSize << ")\n";
            return 1;
        }

        DecoderSeq<T, T> decoder(static_cast<uint8_t>(bpv), static_cast<uint8_t>(blockSize), static_cast<uint16_t>(rsi), CodeOptionSet::Basic);
        std::vector<T> decoded(n);
        decoder.decode(Span<uint8_t>(encoded.data(), length), Span<T>(decoded));
        if (decoded != values) {
            std::cerr << name << ": decoded values differ\n";
            return 1;
        }
    }
    catch (const std::exception& e) {
        std::cerr << name << ": " << e.what() << '\n';
        return 1;
    }
    return 0;
}

int main()
{
    int failures = 0;
    for (unsigned bpv : { 24u, 25u, 28u, 31u, 32u }) {
        for (unsigned blockSize : { 8u, 16u, 32u, 64u }) {
            for (unsigned rsi : { 1u, 64u, 255u }) {
                failures += check<uint32_t>(bpv, blockSize, rsi, 5435);
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
