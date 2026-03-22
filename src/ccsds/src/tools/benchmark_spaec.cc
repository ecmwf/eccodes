#include "Message.h"
#include "SpAec.h"
#include "libaec.h"
#include <cstring>

int main(int argc, char* argv[])
{
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <grib_file>" << std::endl;
        return 1;
    }
    std::string filename = argv[1];
    Grib grib(filename);

    for (const auto& message : grib) {
        std::cout << "Message at offset " << message.offset() << " with length " << message.length() << std::endl;
        // print parameters
        auto s5 = message.getSection(5);
        if (!s5) {
            std::cerr << "No Section 5 found" << std::endl;
            continue;
        }
        const auto& d5 = s5->data();

        uint32_t numDataPoints     = readU32BE(&d5[0]);
        float referenceValue       = readFloatBE(&d5[6]);
        int16_t binaryScaleFactor  = readS16BE(&d5[10]);
        int16_t decimalScaleFactor = readS16BE(&d5[12]);
        uint8_t bitsPerValue       = d5[14];
        uint8_t ccsdsFlags         = d5[16];
        uint8_t ccsdsBlockSize     = d5[17];
        uint16_t ccsdsRsi          = readU16BE(&d5[18]);
        
        std::cout << "Parameters:" << std::endl;
        std::cout << " numDataPoints:      " << numDataPoints << std::endl;
        std::cout << " referenceValue:     " << referenceValue << std::endl;
        std::cout << " binaryScaleFactor:  " << binaryScaleFactor << std::endl;
        std::cout << " decimalScaleFactor: " << decimalScaleFactor << std::endl;
        std::cout << " bitsPerValue:       " << (uint32_t)bitsPerValue << std::endl;
        std::cout << " ccsdsFlags:         " << (uint32_t)ccsdsFlags << std::endl;
        std::cout << " ccsdsBlockSize:     " << (uint32_t)ccsdsBlockSize << std::endl;
        std::cout << " ccsdsRsi:           " << ccsdsRsi << std::endl;

        auto s7 = message.getSection(7);
        if (!s7) {
            std::cerr << "No Section 7 found" << std::endl;
            continue;
        }
        const auto& d7 = s7->data();
        size_t encodedSize = s7->length() - 5;
        Span<uint8_t> encodedData(const_cast<uint8_t*>(d7.data()), encodedSize);
        std::vector<double> decodedData(numDataPoints);
        Span<double> decodedSpan(decodedData);
        if (bitsPerValue != 0) {
            constexpr int threadCounts[] = {1, 2, 4, 8};
            for (int nThreads : threadCounts) {
                std::cout << "\n--- nThreads = " << nThreads << " ---" << std::endl;
                {
                    auto start = std::chrono::high_resolution_clock::now();
                    encoder::SpAec<double> spaec(bitsPerValue, ccsdsBlockSize, ccsdsRsi, CodeOptionSet::Basic, static_cast<double>(referenceValue), binaryScaleFactor, decimalScaleFactor, nThreads);
                    spaec.decode(encodedData, decodedSpan);
                    auto end = std::chrono::high_resolution_clock::now();
                    std::chrono::duration<double> elapsed = end - start;
                    std::cout << "Throughput (decode): " << static_cast<double>(numDataPoints) * sizeof(numDataPoints) / elapsed.count() / 1024 / 1024 << " MiB/s" << std::endl;
                }
                {
                    std::vector<uint8_t> encoded_vec(numDataPoints * sizeof(double) * 2);
                    Span<uint8_t> encoded(encoded_vec);
                    Span<double> data(decodedData);
                    encoder::SpAec<double> sp_encoder{bitsPerValue, ccsdsBlockSize, ccsdsRsi, CodeOptionSet::Basic, static_cast<double>(referenceValue), binaryScaleFactor, decimalScaleFactor, static_cast<uint8_t>(nThreads)};
                    // warmup + get encoded size for efficient zeroing
                    size_t encodedSize = sp_encoder.encode(data, encoded);
                    size_t clearSize = encodedSize + 64; // small safety margin
                    constexpr int NITERS = 10;
                    auto start = std::chrono::high_resolution_clock::now();
                    for (int iter = 0; iter < NITERS; ++iter) {
                        std::memset(encoded_vec.data(), 0, clearSize);
                        encodedSize = sp_encoder.encode(data, encoded);
                    }
                    auto end = std::chrono::high_resolution_clock::now();
                    std::chrono::duration<double> elapsed = end - start;
                    std::cout << "Throughput (encode): " << static_cast<double>(numDataPoints) * NITERS * sizeof(numDataPoints) / elapsed.count() / 1024 / 1024 << " MiB/s" << std::endl;
                    std::cout << "Encoded size: " << encodedSize << " bytes" << std::endl;
                }
            }
            // libaec (following eccodes DataCcsdsPacking approach)
            size_t nbytes = (bitsPerValue + 7) / 8;
            if (nbytes == 3) nbytes = 4;

            unsigned int aecFlags = ccsdsFlags;
            aecFlags &= ~AEC_DATA_3BYTE;
            {
                unsigned short endianness_test = 1;
                if (reinterpret_cast<const char*>(&endianness_test)[0] == 0)
                    aecFlags |= AEC_DATA_MSB;
                else
                    aecFlags &= ~AEC_DATA_MSB;
            }

            // libaec decode
            std::vector<double> unpackedValues (numDataPoints);
            std::vector<uint8_t> libaecDecodedBuf(numDataPoints * nbytes);
            {
                struct aec_stream strm;
                strm.bits_per_sample = bitsPerValue;
                strm.block_size      = ccsdsBlockSize;
                strm.rsi             = ccsdsRsi;
                strm.flags           = aecFlags;

                strm.next_in   = encodedData.data();
                strm.avail_in  = encodedSize;
                strm.next_out  = libaecDecodedBuf.data();
                strm.avail_out = libaecDecodedBuf.size();

                auto start = std::chrono::high_resolution_clock::now();
                int ret = aec_buffer_decode(&strm);

                if (ret != AEC_OK) {
                    std::cerr << "libaec decompression failed with error code: " << ret << std::endl;
                }

                // Convert libaec decoded integers to double and verify against SpAec output
                double bscale = std::pow(2.0, binaryScaleFactor);
                double dscale = std::pow(10.0, -decimalScaleFactor);
                double R = static_cast<double>(referenceValue);
                size_t mismatches = 0;
                for (size_t i = 0; i < numDataPoints; ++i) {
                    switch (nbytes) {
                        case 1: unpackedValues[i] = (libaecDecodedBuf[i] * bscale + R) * dscale; break;
                        case 2: unpackedValues[i] = (reinterpret_cast<uint16_t*>(libaecDecodedBuf.data())[i] * bscale + R) * dscale; break;
                        case 4: unpackedValues[i] = (reinterpret_cast<uint32_t*>(libaecDecodedBuf.data())[i] * bscale + R) * dscale; break;
                    }
                }
                auto end = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double> elapsed = end - start;
                std::cout << "Throughput (libaec decode): " << static_cast<double>(numDataPoints) * sizeof(numDataPoints) / elapsed.count() / 1024 / 1024 << " MiB/s" << std::endl;
            }
            // libaec encode
            {
                double d = std::pow(10.0, decimalScaleFactor);
                double divisor = std::pow(2.0, -binaryScaleFactor);
                double R = static_cast<double>(referenceValue);
                std::vector<uint8_t> packed(numDataPoints * nbytes);

                size_t buflen = (nbytes * numDataPoints) * 67 / 64 + 256;
                std::vector<uint8_t> libaecEncoded(buflen);

                constexpr int LIBAEC_NITERS = 10;
                auto start2 = std::chrono::high_resolution_clock::now();
                int ret2 = AEC_OK;
                size_t totalOut = 0;
                for (int iter = 0; iter < LIBAEC_NITERS; ++iter) {
                    // Map doubles to packed integers (same as eccodes pack_double)
                    switch (nbytes) {
                        case 1:
                            for (size_t i = 0; i < numDataPoints; ++i)
                                packed[i] = static_cast<uint8_t>(((unpackedValues[i] * d - R) * divisor) + 0.5);
                            break;
                        case 2:
                            for (size_t i = 0; i < numDataPoints; ++i)
                                reinterpret_cast<uint16_t*>(packed.data())[i] = static_cast<uint16_t>(((unpackedValues[i] * d - R) * divisor) + 0.5);
                            break;
                        case 4:
                            for (size_t i = 0; i < numDataPoints; ++i)
                                reinterpret_cast<uint32_t*>(packed.data())[i] = static_cast<uint32_t>(((unpackedValues[i] * d - R) * divisor) + 0.5);
                            break;
                    }

                    struct aec_stream estrm;
                    estrm.bits_per_sample = bitsPerValue;
                    estrm.block_size      = ccsdsBlockSize;
                    estrm.rsi             = ccsdsRsi;
                    estrm.flags           = aecFlags;

                    estrm.next_in   = packed.data();
                    estrm.avail_in  = packed.size();
                    estrm.next_out  = libaecEncoded.data();
                    estrm.avail_out = libaecEncoded.size();

                    ret2 = aec_buffer_encode(&estrm);
                    totalOut = estrm.total_out;
                }
                auto end2 = std::chrono::high_resolution_clock::now();

                std::chrono::duration<double> elapsed2 = end2 - start2;
                std::cout << "Throughput (libaec encode): " << static_cast<double>(numDataPoints) * LIBAEC_NITERS * sizeof(numDataPoints) / elapsed2.count() / 1024 / 1024 << " MiB/s" << std::endl;
                std::cout << "Encoded size (libaec): " << totalOut << " bytes" << std::endl;

                if (ret2 != AEC_OK) {
                    std::cerr << "libaec compression failed with error code: " << ret2 << std::endl;
                }
            }

        }
        else {
            std::cerr << "Bits per value is 0, skipping" << std::endl;
            continue;
        }

    }

    return 0;
}
