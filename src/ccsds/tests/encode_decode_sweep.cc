#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <numeric>
#include <random>
#include <string>
#include <vector>

#include "encoder/EncoderSeq.h"
#include "decoder/DecoderSeq.h"

static int testCount = 0;
static int passCount = 0;
static int failCount = 0;

#define CHECK_EQ(a, b, msg)                                                 \
    do {                                                                    \
        if ((a) != (b)) {                                                   \
            std::cerr << "FAIL: " << msg << " (expected " << (a)            \
                      << ", got " << (b) << ")" << std::endl;               \
            failCount++;                                                   \
            return false;                                                   \
        }                                                                   \
    } while (0)

// Returns true if encode→decode round-trip matches the input.
template <typename ValueType>
bool roundTrip(uint8_t bpv, uint8_t block_size, uint16_t rsi,
                CodeOptionSet cos, const std::vector<ValueType>& data,
                const std::string& label) {
    // Encode
    std::vector<uint8_t> encoded(data.size() * sizeof(ValueType) * 2 + 1024);
    encoder::EncoderSeq<ValueType, ValueType> encoder(bpv, block_size, rsi, cos);
    size_t encBytes = encoder.encode(
        Span<ValueType>(const_cast<ValueType*>(data.data()), data.size()),
        Span<uint8_t>(encoded));

    // Decode
    std::vector<ValueType> decoded(data.size(), 0);
    DecoderSeq<ValueType, ValueType> decoder(bpv, block_size, rsi, cos);
    decoder.decode(Span<uint8_t>(encoded.data(), encBytes),
                   Span<ValueType>(decoded));

    // Verify
    for (size_t i = 0; i < data.size(); ++i) {
        CHECK_EQ(data[i], decoded[i],
                 label + " sample[" + std::to_string(i) + "]");
    }
    return true;
}

// Generate a small deterministic dataset that exercises various code paths.
template <typename ValueType>
std::vector<ValueType> makeData(size_t n, uint8_t bpv, unsigned seed) {
    std::vector<ValueType> v(n);
    std::mt19937 gen(seed);
    ValueType mask = (bpv >= sizeof(ValueType) * 8)
                         ? static_cast<ValueType>(~ValueType(0))
                         : static_cast<ValueType>((ValueType(1) << bpv) - 1);

    // Mix of patterns: ramp, zeros, random
    for (size_t i = 0; i < n; ++i) {
        if (i < n / 3) {
            v[i] = static_cast<ValueType>(i) & mask;           // ramp
        } else if (i < 2 * n / 3) {
            v[i] = 0;                                           // zeros
        } else {
            v[i] = static_cast<ValueType>(gen()) & mask;       // random
        }
    }
    return v;
}

template <typename ValueType>
void sweep(const std::string& typeName) {
    constexpr uint8_t maxBpv = sizeof(ValueType) * 8;

    const uint8_t block_sizes[] = {8, 16, 32, 64};
    const CodeOptionSet cosValues[] = {CodeOptionSet::Basic,
                                         CodeOptionSet::Restricted};
    const char* cosNames[] = {"Basic", "Restricted"};

    // Per spec: RSI is 1..4096 blocks.  We test a representative set.
    const uint16_t rsiValues[] = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 4096};

    for (uint8_t bpv = 1; bpv <= maxBpv; ++bpv) {
        for (size_t ci = 0; ci < 2; ++ci) {
            CodeOptionSet cos = cosValues[ci];

            // Restricted set disallows bpv 1-2
            if (cos == CodeOptionSet::Restricted && bpv <= 2) continue;

            for (uint8_t bs : block_sizes) {
                for (uint16_t rsi : rsiValues) {
                    // Use a small data size: 2 RSIs worth of samples
                    // so we cover full + partial RSI paths.
                    size_t nSamples =
                        static_cast<size_t>(rsi) * bs * 2 + bs / 2;

                    auto data =
                        makeData<ValueType>(nSamples, bpv, bpv * 1000 + bs);

                    std::string label =
                        typeName + " bpv=" + std::to_string(bpv) +
                        " bs=" + std::to_string(bs) +
                        " rsi=" + std::to_string(rsi) +
                        " cos=" + cosNames[ci];

                    testCount++;
                    if (roundTrip<ValueType>(bpv, bs, rsi, cos, data,
                                              label)) {
                        passCount++;
                    } else {
                        std::cerr << "  FAILED: " << label << std::endl;
                    }
                }
            }
        }
    }
}

int main() {
    std::cout << "=== Encode/Decode Sweep Test ===" << std::endl;

    std::cout << "\n--- uint8_t ---" << std::endl;
    sweep<uint8_t>("uint8");

    std::cout << "\n--- uint16_t ---" << std::endl;
    sweep<uint16_t>("uint16");

    std::cout << "\n--- Summary ---" << std::endl;
    std::cout << passCount << "/" << testCount << " passed";
    if (failCount > 0) {
        std::cout << ", " << failCount << " FAILED";
    }
    std::cout << std::endl;

    return (failCount > 0) ? 1 : 0;
}
