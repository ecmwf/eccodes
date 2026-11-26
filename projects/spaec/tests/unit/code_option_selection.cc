// SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// Unit test of the code option the encoder chooses, adapted from libaec's
// tests/check_code_options.c (preprocessed, unsigned samples) and tests/check_long_fs.c. For 8, 16,
// 24 and 32 bits per value, every block size and every RSI size up to 3072 bytes of samples, values
// crafted for one option must be coded with that option in the first block (its ID at the start
// of the stream) and must decode to the same values:
// - a constant field: zero block (ID of all zeros, one bit longer than the other IDs)
// - 2^(k-1) - 1, 0, 2^(k+1) - 1, 0, ...: split sample with k (ID k + 1), and k = 0 (fundamental
//   sequence, ID 1) for 2, 0, 0, 0, ...
// - alternating 2^n - 1 and 0: no compression (ID of all ones)
// - four values 2^n - 2, four values 2^n - 1, ...: second extension (ID 1, one bit longer)
// and a block of 32 zeros and 32 values of 65000 with 16 bits per value (long fundamental
// sequences) must decode to the same values.

#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

#include "spaec/detail/SpAec.h"
#include "spaec/detail/decoder/DecoderSeq.h"
#include "spaec/detail/encoder/EncoderSeq.h"

using namespace spaec::detail;

static int32_t failures = 0;

static void check(bool ok, const std::string& what)
{
    if (!ok) {
        if (failures < 20) std::cerr << "FAIL: " << what << '\n';
        ++failures;
    }
}

static std::vector<uint8_t> roundTrip(const std::vector<uint32_t>& values, uint8_t bpv, uint8_t blockSize, uint16_t rsi, const std::string& what)
{
    encoder::EncoderSeq<uint32_t, uint32_t> enc(bpv, blockSize, rsi, CodeOptionSet::Basic);
    std::vector<uint8_t> stream(encoder::SpAec<double>::maxEncodedSize(values.size(), bpv, blockSize), 0);
    stream.resize(enc.encode(Span<uint32_t>(const_cast<uint32_t*>(values.data()), values.size()), Span<uint8_t>(stream)));

    DecoderSeq<uint32_t, uint32_t> decoder(bpv, blockSize, rsi, CodeOptionSet::Basic);
    std::vector<uint32_t> decoded(values.size());
    decoder.decode(Span<uint8_t>(stream), Span<uint32_t>(decoded));
    check(decoded == values, what + ": round trip");
    return stream;
}

// Encodes the pattern for every block and RSI size and checks the ID that starts the stream
static void checkOption(const std::vector<uint32_t>& pattern, uint8_t bpv, size_t bytesPerSample, uint32_t id, int32_t idLength, const std::string& option)
{
    const size_t bufferBytes = 1024 * 3;
    std::vector<uint32_t> values(bufferBytes / bytesPerSample);
    for (size_t i = 0; i < values.size(); ++i)
        values[i] = pattern[i % pattern.size()];

    for (uint8_t blockSize = 8; blockSize <= 64; blockSize *= 2) {
        const size_t maxRsi = std::min<size_t>(bufferBytes / (blockSize * bytesPerSample), 4096);
        for (size_t rsi = 1; rsi <= maxRsi; ++rsi) {
            const std::string what = option + " bpv=" + std::to_string(bpv) + " bs=" + std::to_string(blockSize) + " rsi=" + std::to_string(rsi);
            const auto stream = roundTrip(values, bpv, blockSize, static_cast<uint16_t>(rsi), what);
            const uint32_t got = stream[0] >> (8 - idLength);
            check(got == id, what + ": ID " + std::to_string(got) + ", expected " + std::to_string(id));
        }
    }
}

int main()
{
    for (uint8_t bpv = 8; bpv <= 32; bpv += 8) {
        // libaec: 1 byte up to 8 bits, 2 up to 16, 3 for 24 bits (AEC_DATA_3BYTE), else 4
        const size_t bytesPerSample = bpv == 32 ? 4 : bpv / 8;
        const int32_t idLength = bpv > 16 ? 5 : bpv > 8 ? 4
                                                        : 3;
        const uint32_t xmax = static_cast<uint32_t>((uint64_t{ 1 } << bpv) - 1);

        checkOption({ static_cast<uint32_t>(0x5555555555555555ull & xmax) }, bpv, bytesPerSample, 0, idLength + 1, "zero block");
        checkOption({ xmax - 1, xmax - 1, xmax - 1, xmax - 1, xmax, xmax, xmax, xmax }, bpv, bytesPerSample, 1, idLength + 1, "second extension");
        checkOption({ xmax, 0 }, bpv, bytesPerSample, (1u << idLength) - 1, idLength, "no compression");
        checkOption({ 2, 0, 0, 0 }, bpv, bytesPerSample, 1, idLength, "fundamental sequence");
        for (uint32_t k = 1; k < bpv - 2u; ++k) {
            const uint32_t low = static_cast<uint32_t>((uint64_t{ 1 } << (k - 1)) - 1), high = static_cast<uint32_t>((uint64_t{ 1 } << (k + 1)) - 1);
            checkOption({ low, 0, high, 0 }, bpv, bytesPerSample, k + 1, idLength, "split sample k=" + std::to_string(k));
        }
    }

    // libaec check_long_fs: 16 bits per value, block size 64, RSI 1
    {
        std::vector<uint32_t> values(128, 0);
        for (size_t i = 32; i < 64; ++i)
            values[i] = 65000;
        roundTrip(values, 16, 64, 1, "long fundamental sequences");
    }

    if (failures) {
        std::cerr << failures << " failure(s)\n";
        return 1;
    }
    std::cout << "all code options chosen as expected\n";
    return 0;
}
