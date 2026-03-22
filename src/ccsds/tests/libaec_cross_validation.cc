#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include <libaec.h>

#include "encoder/EncoderSeq.h"
#include "decoder/DecoderSeq.h"

static int testCount = 0;
static int passCount = 0;
static int failCount = 0;

static unsigned int libaec_flags() {
    unsigned int flags = AEC_DATA_PREPROCESS;
    if constexpr (!isLittleEndian) {
        flags |= AEC_DATA_MSB;
    }
    return flags;
}

// Encode with libaec, returns encoded byte count.  Returns 0 on error.
template <typename ValueType>
size_t libaec_encode(const std::vector<ValueType>& src, std::vector<uint8_t>& dst,
                     uint8_t bpv, uint8_t block_size, uint16_t rsi,
                     bool restricted) {
    struct aec_stream strm;
    std::memset(&strm, 0, sizeof(strm));
    strm.bits_per_sample = bpv;
    strm.block_size = block_size;
    strm.rsi = rsi;
    strm.flags = libaec_flags();
    if (restricted) strm.flags |= AEC_RESTRICTED;
    strm.next_in = reinterpret_cast<const unsigned char*>(src.data());
    strm.avail_in = src.size() * sizeof(ValueType);
    strm.next_out = dst.data();
    strm.avail_out = dst.size();

    if (aec_encode_init(&strm) != AEC_OK) return 0;
    int ret = aec_encode(&strm, AEC_FLUSH);
    size_t total = strm.total_out;
    aec_encode_end(&strm);
    return (ret == AEC_OK) ? total : 0;
}

// Decode with libaec, returns true on success.
template <typename ValueType>
bool libaec_decode(const uint8_t* enc, size_t encBytes,
                   std::vector<ValueType>& dst,
                   uint8_t bpv, uint8_t block_size, uint16_t rsi,
                   bool restricted) {
    struct aec_stream strm;
    std::memset(&strm, 0, sizeof(strm));
    strm.bits_per_sample = bpv;
    strm.block_size = block_size;
    strm.rsi = rsi;
    strm.flags = libaec_flags();
    if (restricted) strm.flags |= AEC_RESTRICTED;
    strm.next_in = enc;
    strm.avail_in = encBytes;
    strm.next_out = reinterpret_cast<unsigned char*>(dst.data());
    strm.avail_out = dst.size() * sizeof(ValueType);

    if (aec_decode_init(&strm) != AEC_OK) return false;
    int ret = aec_decode(&strm, AEC_FLUSH);
    aec_decode_end(&strm);
    return ret == AEC_OK;
}

// PAEC encode → libaec decode
template <typename ValueType>
bool test_paec_to_libaec(uint8_t bpv, uint8_t block_size, uint16_t rsi,
                         CodeOptionSet cos, const std::vector<ValueType>& data,
                         const std::string& label) {
    bool restricted = (cos == CodeOptionSet::Restricted);

    // Encode with PAEC
    std::vector<uint8_t> encoded(data.size() * sizeof(ValueType) * 2 + 1024);
    encoder::EncoderSeq<ValueType, ValueType> encoder(bpv, block_size, rsi, cos);
    size_t encBytes = encoder.encode(
        Span<ValueType>(const_cast<ValueType*>(data.data()), data.size()),
        Span<uint8_t>(encoded));

    // Decode with libaec
    std::vector<ValueType> decoded(data.size(), 0);
    if (!libaec_decode<ValueType>(encoded.data(), encBytes, decoded,
                                   bpv, block_size, rsi, restricted)) {
        std::cerr << "FAIL (libaec decode error): " << label << std::endl;
        failCount++;
        return false;
    }

    for (size_t i = 0; i < data.size(); ++i) {
        if (data[i] != decoded[i]) {
            std::cerr << "FAIL: paec→libaec " << label
                      << " sample[" << i << "] expected " << +data[i]
                      << " got " << +decoded[i] << std::endl;
            failCount++;
            return false;
        }
    }
    return true;
}

// libaec encode → PAEC decode
template <typename ValueType>
bool test_libaec_to_paec(uint8_t bpv, uint8_t block_size, uint16_t rsi,
                         CodeOptionSet cos, const std::vector<ValueType>& data,
                         const std::string& label) {
    bool restricted = (cos == CodeOptionSet::Restricted);

    // Encode with libaec
    std::vector<uint8_t> encoded(data.size() * sizeof(ValueType) * 2 + 1024);
    size_t encBytes = libaec_encode<ValueType>(data, encoded, bpv, block_size,
                                                 rsi, restricted);
    if (encBytes == 0) {
        std::cerr << "FAIL (libaec encode error): " << label << std::endl;
        failCount++;
        return false;
    }

    // Decode with PAEC
    std::vector<ValueType> decoded(data.size(), 0);
    DecoderSeq<ValueType, ValueType> decoder(bpv, block_size, rsi, cos);
    decoder.decode(Span<uint8_t>(encoded.data(), encBytes),
                   Span<ValueType>(decoded));

    for (size_t i = 0; i < data.size(); ++i) {
        if (data[i] != decoded[i]) {
            std::cerr << "FAIL: libaec→paec " << label
                      << " sample[" << i << "] expected " << +data[i]
                      << " got " << +decoded[i] << std::endl;
            failCount++;
            return false;
        }
    }
    return true;
}

template <typename ValueType>
std::vector<ValueType> makeData(size_t n, uint8_t bpv, unsigned seed) {
    std::vector<ValueType> v(n);
    std::mt19937 gen(seed);
    ValueType mask = (bpv >= sizeof(ValueType) * 8)
                         ? static_cast<ValueType>(~ValueType(0))
                         : static_cast<ValueType>((ValueType(1) << bpv) - 1);

    for (size_t i = 0; i < n; ++i) {
        if (i < n / 3) {
            v[i] = static_cast<ValueType>(i) & mask;
        } else if (i < 2 * n / 3) {
            v[i] = 0;
        } else {
            v[i] = static_cast<ValueType>(gen()) & mask;
        }
    }
    return v;
}

// libaec determines sample byte width from bpv:
//   bpv 1-8  → 1 byte per sample (use uint8_t)
//   bpv 9-16 → 2 bytes per sample (use uint16_t)
// Also, libaec rejects Restricted mode for bpv 5-8.
template <typename ValueType>
void sweep(const std::string& typeName, uint8_t bpvMin, uint8_t bpvMax) {
    const uint8_t block_sizes[] = {8, 16, 32, 64};
    const CodeOptionSet cosValues[] = {CodeOptionSet::Basic,
                                         CodeOptionSet::Restricted};
    const char* cosNames[] = {"Basic", "Restricted"};
    const uint16_t rsiValues[] = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 4096};

    for (uint8_t bpv = bpvMin; bpv <= bpvMax; ++bpv) {
        for (size_t ci = 0; ci < 2; ++ci) {
            CodeOptionSet cos = cosValues[ci];
            // Our library disallows Restricted for bpv 1-2
            if (cos == CodeOptionSet::Restricted && bpv <= 2) continue;
            // libaec rejects Restricted for bpv 5-8
            if (cos == CodeOptionSet::Restricted && bpv >= 5 && bpv <= 8) continue;

            for (uint8_t bs : block_sizes) {
                for (uint16_t rsi : rsiValues) {
                    size_t nSamples =
                        static_cast<size_t>(rsi) * bs * 2 + bs / 2;
                    auto data =
                        makeData<ValueType>(nSamples, bpv, bpv * 1000 + bs);

                    std::string label =
                        typeName + " bpv=" + std::to_string(bpv) +
                        " bs=" + std::to_string(bs) +
                        " rsi=" + std::to_string(rsi) +
                        " cos=" + cosNames[ci];

                    // PAEC encode → libaec decode
                    testCount++;
                    if (test_paec_to_libaec<ValueType>(bpv, bs, rsi, cos, data,
                                                        label)) {
                        passCount++;
                    }

                    // libaec encode → PAEC decode
                    testCount++;
                    if (test_libaec_to_paec<ValueType>(bpv, bs, rsi, cos, data,
                                                        label)) {
                        passCount++;
                    }
                }
            }
        }
    }
}

int main() {
    std::cout << "=== libaec Cross-Validation Sweep ===" << std::endl;

    std::cout << "\n--- uint8_t (bpv 1-8) ---" << std::endl;
    sweep<uint8_t>("uint8", 1, 8);

    std::cout << "\n--- uint16_t (bpv 9-16) ---" << std::endl;
    sweep<uint16_t>("uint16", 9, 16);

    std::cout << "\n--- Summary ---" << std::endl;
    std::cout << passCount << "/" << testCount << " passed";
    if (failCount > 0) {
        std::cout << ", " << failCount << " FAILED";
    }
    std::cout << std::endl;

    return (failCount > 0) ? 1 : 0;
}
