#include <iostream>
#include <vector>
#include <assert.h>
#include <fstream>
#include <chrono>
#include <random>
#include <bitset>

#include "SpAec.h"

#include <libaec.h>


int main(int argc, char* argv[])
{

    using ValueType = double;
    constexpr size_t BPV = 16;
    constexpr size_t blockSize = 8;
    constexpr size_t rsiSize = 128;
    size_t nValues = 0;

    // std::vector<ValueType> vec;
    // // std::fstream file("/home/joobog/git/ccsds/data/ccsds2/ccsds_uint16.bin", std::ios::in | std::ios::binary);
    // std::fstream file("/home/joobog/git/ccsds/data/hres/uint16.bin", std::ios::in | std::ios::binary);
    // // std::fstream file("/home/joobog/git/ccsds/data/ccsds2/ccsds_uint16.bin", std::ios::in | std::ios::binary);
    //
    // if (!file) {
    //     std::cerr << "Unable to open file";
    //     return 1;
    // }
    // ValueType value;
    // while (file.read(reinterpret_cast<char*>(&value), sizeof(value))) {
    //     vec.push_back(value);
    // }
    // file.close();
    // // vec.resize(512);
    // nValues = vec.size();

// 87320838:
// 97'023'193:

    // random data
    std::vector<ValueType> vec;
    std::mt19937 rng(42); // Fixed seed for reproducibility
    // std::uniform_int_distribution<ValueType> dist(0, (1 << BPV) - 1);
    std::uniform_real_distribution<ValueType> dist(0.0, 1.0);
    // nValues = 8 * 64;
    nValues = 100'000'000;
    vec.reserve(nValues);
    for (size_t i = 0; i < nValues; ++i) {
        vec.push_back(dist(rng));
        // vec.push_back(124);
    }
    // std::cout << "Read " << vec.size() << " values from file." << std::endl;

    constexpr int binaryScaleFactor = BPV;
    constexpr int decimalScaleFactor = 0;
    constexpr ValueType referenceValue = 0.0;

    std::vector<uint8_t> encoded_vec(vec.size() * sizeof(ValueType) * 2);
    Span<uint8_t> encoded(encoded_vec);
    size_t encodedSize = 0;
    {
        Span<ValueType> data(vec);
        auto start = std::chrono::high_resolution_clock::now();
        encoder::SpAec<ValueType> sp_encoder{(uint8_t) BPV, (uint8_t) blockSize, (uint16_t) rsiSize, CodeOptionSet::Basic, referenceValue, binaryScaleFactor, decimalScaleFactor};
        encodedSize = sp_encoder.encode(data, encoded);
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - start;
        std::cout << "Throughput (encode): " << static_cast<double>(nValues) * sizeof(ValueType) / elapsed.count() / 1024 / 1024 << " MiB/s" << std::endl;
        std::cout << "Encoded size: " << encodedSize << " bytes" << std::endl;
    }

    // Decode
    {
        std::vector<ValueType> decoded_vec(nValues);
        Span<ValueType> decoded(decoded_vec);
        Span<uint8_t> encodedSpan(encoded_vec.data(), encodedSize);

        auto start = std::chrono::high_resolution_clock::now();
        encoder::SpAec<ValueType> sp_decoder{(uint8_t) BPV, (uint8_t) blockSize, (uint16_t) rsiSize, CodeOptionSet::Basic, referenceValue, binaryScaleFactor, decimalScaleFactor};
        sp_decoder.decode(encodedSpan, decoded);
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - start;
        std::cout << "Throughput (decode): " << static_cast<double>(nValues) * sizeof(ValueType) / elapsed.count() / 1024 / 1024 << " MiB/s" << std::endl;

        // Verify (lossy compression: quantization error <= 1/2^BPV)
        const double tolerance = 1.0 / (1 << BPV);
        size_t mismatches = 0;
        double maxError = 0.0;
        for (size_t i = 0; i < nValues; ++i) {
            double err = std::abs(vec[i] - decoded_vec[i]);
            if (err > maxError) maxError = err;
            if (err > tolerance) {
                if (mismatches < 10) {
                    std::cerr << "Mismatch at index " << i << ": original=" << vec[i] << " decoded=" << decoded_vec[i] << " error=" << err << std::endl;
                }
                mismatches++;
            }
        }
        if (mismatches == 0) {
            std::cout << "PASS: All " << nValues << " values match within tolerance " << tolerance << " (max error: " << maxError << ")" << std::endl;
        } else {
            std::cout << "FAIL: " << mismatches << " / " << nValues << " mismatches (max error: " << maxError << ", tolerance: " << tolerance << ")" << std::endl;
        }
    }


    return 0;
}
