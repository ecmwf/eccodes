#include <cstdint>
#include <cmath>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

#include <libaec.h>

#include "SpAec.h"
#include "encoder/EncoderSeq.h"
#include "decoder/DecoderSeq.h"
#include "Message.h"

static int testCount = 0;
static int passCount = 0;
static int failCount = 0;

static unsigned int libaec_flags(uint8_t ccsdsFlags) {
    unsigned int flags = ccsdsFlags;
    flags &= ~AEC_DATA_3BYTE;
    if constexpr (isLittleEndian) {
        flags &= ~AEC_DATA_MSB;
    } else {
        flags |= AEC_DATA_MSB;
    }
    return flags;
}

struct GribParams {
    uint32_t numDataPoints;
    float referenceValue;
    int16_t binaryScaleFactor;
    int16_t decimalScaleFactor;
    uint8_t bitsPerValue;
    uint8_t ccsdsFlags;
    uint8_t ccsdsBlockSize;
    uint16_t ccsdsRsi;
    size_t nbytes;
};

static GribParams extractParams(const Message& message) {
    auto s5 = message.getSection(5);
    const auto& d5 = s5->data();
    GribParams p;
    p.numDataPoints     = readU32BE(&d5[0]);
    p.referenceValue    = readFloatBE(&d5[6]);
    p.binaryScaleFactor = readS16BE(&d5[10]);
    p.decimalScaleFactor= readS16BE(&d5[12]);
    p.bitsPerValue      = d5[14];
    p.ccsdsFlags        = d5[16];
    p.ccsdsBlockSize    = d5[17];
    p.ccsdsRsi          = readU16BE(&d5[18]);
    p.nbytes = (p.bitsPerValue + 7) / 8;
    if (p.nbytes == 3) p.nbytes = 4;
    return p;
}

// Test 1: Decode GRIB data with both SpAec and libaec, compare integers
template <typename IntType>
bool testDecodeCompare(const std::string& label, const GribParams& p,
                       Span<uint8_t> encodedData) {
    testCount++;

    // Decode with SpAec (DecoderSeq at integer level)
    std::vector<IntType> spaecInts(p.numDataPoints);
    DecoderSeq<IntType, IntType> decoder(p.bitsPerValue, p.ccsdsBlockSize,
                                         p.ccsdsRsi, CodeOptionSet::Basic);
    decoder.decode(encodedData, Span<IntType>(spaecInts));

    // Decode with libaec
    unsigned int flags = libaec_flags(p.ccsdsFlags);
    std::vector<uint8_t> libaecBuf(p.numDataPoints * p.nbytes);
    struct aec_stream strm;
    std::memset(&strm, 0, sizeof(strm));
    strm.bits_per_sample = p.bitsPerValue;
    strm.block_size      = p.ccsdsBlockSize;
    strm.rsi             = p.ccsdsRsi;
    strm.flags           = flags;
    strm.next_in         = encodedData.data();
    strm.avail_in        = encodedData.size();
    strm.next_out        = libaecBuf.data();
    strm.avail_out       = libaecBuf.size();

    if (aec_buffer_decode(&strm) != AEC_OK) {
        std::cerr << "FAIL decode-compare " << label << " (libaec decode error)" << std::endl;
        failCount++;
        return false;
    }

    // Compare integers
    const IntType* libaecInts = reinterpret_cast<const IntType*>(libaecBuf.data());
    for (size_t i = 0; i < p.numDataPoints; ++i) {
        if (spaecInts[i] != libaecInts[i]) {
            std::cerr << "FAIL decode-compare " << label
                      << " sample[" << i << "] SpAec=" << +spaecInts[i]
                      << " libaec=" << +libaecInts[i] << std::endl;
            failCount++;
            return false;
        }
    }
    passCount++;
    return true;
}

// Test 2: SpAec encode → libaec decode, compare integers
template <typename IntType>
bool testSpAecEncodeLibaecDecode(const std::string& label, const GribParams& p,
                                 Span<uint8_t> encodedData) {
    testCount++;

    // First decode the original to get integers
    std::vector<IntType> origInts(p.numDataPoints);
    DecoderSeq<IntType, IntType> decoder(p.bitsPerValue, p.ccsdsBlockSize,
                                         p.ccsdsRsi, CodeOptionSet::Basic);
    decoder.decode(encodedData, Span<IntType>(origInts));

    // Re-encode with SpAec
    std::vector<uint8_t> spaecEncoded(p.numDataPoints * sizeof(IntType) * 2 + 1024);
    std::memset(spaecEncoded.data(), 0, spaecEncoded.size());
    encoder::EncoderSeq<IntType, IntType> encoder(p.bitsPerValue, p.ccsdsBlockSize,
                                                   p.ccsdsRsi, CodeOptionSet::Basic);
    size_t encSize = encoder.encode(Span<IntType>(origInts), Span<uint8_t>(spaecEncoded));

    // Decode with libaec
    unsigned int flags = libaec_flags(p.ccsdsFlags);
    std::vector<uint8_t> libaecBuf(p.numDataPoints * p.nbytes);
    struct aec_stream strm;
    std::memset(&strm, 0, sizeof(strm));
    strm.bits_per_sample = p.bitsPerValue;
    strm.block_size      = p.ccsdsBlockSize;
    strm.rsi             = p.ccsdsRsi;
    strm.flags           = flags;
    strm.next_in         = spaecEncoded.data();
    strm.avail_in        = encSize;
    strm.next_out        = libaecBuf.data();
    strm.avail_out       = libaecBuf.size();

    if (aec_buffer_decode(&strm) != AEC_OK) {
        std::cerr << "FAIL SpAec→libaec " << label << " (libaec decode error)" << std::endl;
        failCount++;
        return false;
    }

    // Compare integers
    const IntType* libaecInts = reinterpret_cast<const IntType*>(libaecBuf.data());
    for (size_t i = 0; i < p.numDataPoints; ++i) {
        if (origInts[i] != libaecInts[i]) {
            std::cerr << "FAIL SpAec→libaec " << label
                      << " sample[" << i << "] expected=" << +origInts[i]
                      << " libaec=" << +libaecInts[i] << std::endl;
            failCount++;
            return false;
        }
    }
    passCount++;
    return true;
}

// Test 3: libaec encode → SpAec decode, compare integers
template <typename IntType>
bool testLibaecEncodeSpAecDecode(const std::string& label, const GribParams& p,
                                 Span<uint8_t> encodedData) {
    testCount++;

    // First decode the original to get reference integers
    std::vector<IntType> origInts(p.numDataPoints);
    DecoderSeq<IntType, IntType> decoder(p.bitsPerValue, p.ccsdsBlockSize,
                                         p.ccsdsRsi, CodeOptionSet::Basic);
    decoder.decode(encodedData, Span<IntType>(origInts));

    // Encode those integers with libaec
    unsigned int flags = libaec_flags(p.ccsdsFlags);
    size_t packedSize = p.numDataPoints * p.nbytes;
    size_t buflen = packedSize * 2 + 256;
    std::vector<uint8_t> libaecEncoded(buflen);

    struct aec_stream estrm;
    std::memset(&estrm, 0, sizeof(estrm));
    estrm.bits_per_sample = p.bitsPerValue;
    estrm.block_size      = p.ccsdsBlockSize;
    estrm.rsi             = p.ccsdsRsi;
    estrm.flags           = flags;
    estrm.next_in         = reinterpret_cast<const uint8_t*>(origInts.data());
    estrm.avail_in        = packedSize;
    estrm.next_out        = libaecEncoded.data();
    estrm.avail_out       = libaecEncoded.size();

    if (aec_buffer_encode(&estrm) != AEC_OK) {
        std::cerr << "FAIL libaec→SpAec " << label << " (libaec encode error)" << std::endl;
        failCount++;
        return false;
    }
    size_t encSize = estrm.total_out;

    // Decode with SpAec
    std::vector<IntType> spaecInts(p.numDataPoints);
    DecoderSeq<IntType, IntType> decoder2(p.bitsPerValue, p.ccsdsBlockSize,
                                          p.ccsdsRsi, CodeOptionSet::Basic);
    decoder2.decode(Span<uint8_t>(libaecEncoded.data(), encSize),
                    Span<IntType>(spaecInts));

    // Compare integers
    for (size_t i = 0; i < p.numDataPoints; ++i) {
        if (origInts[i] != spaecInts[i]) {
            std::cerr << "FAIL libaec→SpAec " << label
                      << " sample[" << i << "] expected=" << +origInts[i]
                      << " SpAec=" << +spaecInts[i] << std::endl;
            failCount++;
            return false;
        }
    }
    passCount++;
    return true;
}

// Test 4: SpAec<double> round-trip (full pipeline including mapping)
bool testSpAecDoubleRoundTrip(const std::string& label, const GribParams& p,
                              Span<uint8_t> encodedData) {
    testCount++;

    // Decode original with SpAec<double>
    std::vector<double> decoded1(p.numDataPoints);
    encoder::SpAec<double> dec1(p.bitsPerValue, p.ccsdsBlockSize, p.ccsdsRsi,
                                CodeOptionSet::Basic,
                                static_cast<double>(p.referenceValue),
                                p.binaryScaleFactor, p.decimalScaleFactor);
    dec1.decode(encodedData, Span<double>(decoded1));

    // Re-encode with SpAec<double>
    std::vector<uint8_t> reencoded(p.numDataPoints * sizeof(double) * 2);
    std::memset(reencoded.data(), 0, reencoded.size());
    encoder::SpAec<double> enc(p.bitsPerValue, p.ccsdsBlockSize, p.ccsdsRsi,
                               CodeOptionSet::Basic,
                               static_cast<double>(p.referenceValue),
                               p.binaryScaleFactor, p.decimalScaleFactor);
    size_t encSize = enc.encode(Span<double>(decoded1), Span<uint8_t>(reencoded));

    // Decode with SpAec<double> again
    std::vector<double> decoded2(p.numDataPoints);
    encoder::SpAec<double> dec2(p.bitsPerValue, p.ccsdsBlockSize, p.ccsdsRsi,
                                CodeOptionSet::Basic,
                                static_cast<double>(p.referenceValue),
                                p.binaryScaleFactor, p.decimalScaleFactor);
    dec2.decode(Span<uint8_t>(reencoded.data(), encSize), Span<double>(decoded2));

    for (size_t i = 0; i < p.numDataPoints; ++i) {
        if (decoded1[i] != decoded2[i]) {
            std::cerr << "FAIL SpAec<double> round-trip " << label
                      << " sample[" << i << "] expected=" << decoded1[i]
                      << " got=" << decoded2[i] << std::endl;
            failCount++;
            return false;
        }
    }
    passCount++;
    return true;
}

template <typename IntType>
void runGribTests(const std::string& label, const GribParams& p,
                  Span<uint8_t> encodedData) {
    testDecodeCompare<IntType>(label, p, encodedData);
    testSpAecEncodeLibaecDecode<IntType>(label, p, encodedData);
    testLibaecEncodeSpAecDecode<IntType>(label, p, encodedData);
    testSpAecDoubleRoundTrip(label, p, encodedData);
}

static void processGribFile(const std::string& filepath) {
    std::cout << "--- " << filepath << " ---" << std::endl;
    Grib grib(filepath);
    int msgIdx = 0;

    for (const auto& message : grib) {
        auto s5 = message.getSection(5);
        auto s7 = message.getSection(7);
        if (!s5 || !s7) {
            msgIdx++;
            continue;
        }

        GribParams p = extractParams(message);
        if (p.bitsPerValue == 0) {
            msgIdx++;
            continue;
        }

        const auto& d7 = s7->data();
        size_t encodedSize = s7->length() - 5;
        Span<uint8_t> encodedData(const_cast<uint8_t*>(d7.data()), encodedSize);

        std::string label = filepath + " msg[" + std::to_string(msgIdx) +
                            "] bpv=" + std::to_string(p.bitsPerValue) +
                            " bs=" + std::to_string(p.ccsdsBlockSize) +
                            " rsi=" + std::to_string(p.ccsdsRsi) +
                            " n=" + std::to_string(p.numDataPoints);

        if (p.bitsPerValue <= 8) {
            runGribTests<uint8_t>(label, p, encodedData);
        } else if (p.bitsPerValue <= 16) {
            runGribTests<uint16_t>(label, p, encodedData);
        } else if (p.bitsPerValue <= 32) {
            runGribTests<uint32_t>(label, p, encodedData);
        }

        msgIdx++;
    }
}

int main() {
    std::cout << "=== SpAec GRIB Cross-Validation ===" << std::endl;

    const std::string dataDir = DATA_DIR;
    const std::string files[] = {
        dataDir + "/hres/ccsds.grib",
        dataDir + "/i10fg_228029_ccsds.grib",
        dataDir + "/ust_140215_ccsds.grib",
        dataDir + "/ssrc_210_ccsds.grib",
    };

    for (const auto& f : files) {
        processGribFile(f);
    }

    std::cout << "\n--- Summary ---" << std::endl;
    std::cout << passCount << "/" << testCount << " passed";
    if (failCount > 0) {
        std::cout << ", " << failCount << " FAILED";
    }
    std::cout << std::endl;

    return (failCount > 0) ? 1 : 0;
}
