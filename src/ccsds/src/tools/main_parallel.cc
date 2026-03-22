#include <iostream>
#include <vector>
#include <assert.h>
#include <fstream>
#include <chrono>

#include "encoder/EncoderPar.h"
#include "decoder/DecoderPar.h"

#include <libaec.h>

using ValueType = uint16_t;

int paecLibaec(size_t BPV, size_t blockSize, size_t rsiSize)
{
    // constexpr size_t BPV = 16;
    // // constexpr BlockSize blockSize = 8;
    // constexpr BlockSize blockSize = 32;
    // // constexpr size_t rsiSize = 128;
    // constexpr size_t rsiSize = 128;
    size_t nValues = 0;

    std::vector<ValueType> vec;
    // std::fstream file("/home/joobog/git/ccsds/data/ccsds2/ccsds_uint16.bin", std::ios::in | std::ios::binary);
    std::fstream file("/home/joobog/git/ccsds/data/hres/uint16.bin", std::ios::in | std::ios::binary);
    if (!file) {
        std::cerr << "Unable to open file";
        return 1;
    }
    ValueType value;
    while (file.read(reinterpret_cast<char*>(&value), sizeof(value))) {
        vec.push_back(value);
    }
    file.close();
    // vec.resize(512);
    nValues = vec.size();


// 87320838:
// 97'023'193:

    // // random data
    // std::vector<ValueType> vec;
    // std::mt19937 rng(42); // Fixed seed for reproducibility
    // std::uniform_int_distribution<ValueType> dist(0, (1 << BPV) - 1);
    // // nValues = 8 * 128;
    // nValues = 100'000'000;
    // vec.reserve(nValues);
    // for (size_t i = 0; i < nValues; ++i) {
    //     vec.push_back(dist(rng));
    //     // vec.push_back(124);
    // }
    // // std::cout << "Read " << vec.size() << " values from file." << std::endl;

    std::vector<uint8_t> encoded(vec.size() * sizeof(ValueType) * 2);
    size_t encodedSize = 0;
    {
        Span<ValueType> data(vec);

        encoder::EncoderPar<ValueType> encoder(BPV, blockSize, rsiSize, CodeOptionSet::Basic);
        auto start = std::chrono::high_resolution_clock::now();
        encodedSize = encoder.encodeParallel(data, encoded);
        // encodedSize = encoder.encodeParallelSeq(data, encoded);
        // encoded = encoder.encode_no_ref(data);
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - start;
        std::cout << "Throughput (encode): " << static_cast<double>(nValues) * sizeof(ValueType) / elapsed.count() / 1024 / 1024 << " MiB/s" << std::endl;

        //write to file
        std::ofstream outFile("encodedOutput.bin", std::ios::out | std::ios::binary);
        outFile.write(reinterpret_cast<const char*>(encoded.data()), encodedSize);
        outFile.close();
    }

    std::vector<ValueType> aecOutputBuffer(vec.size() * 2);

    {// aec encode
        struct aec_stream strm;
        strm.block_size = blockSize;
        strm.bits_per_sample = BPV;
        strm.rsi = rsiSize;
        strm.flags = AEC_DATA_PREPROCESS;
        strm.avail_in = nValues * sizeof(ValueType);
        strm.next_in = reinterpret_cast<uint8_t*>(vec.data());
        strm.avail_out = encoded.size();
        strm.next_out = encoded.data();

        aec_encode_init(&strm);

        auto start = std::chrono::high_resolution_clock::now();
        int ret = aec_encode(&strm, AEC_FLUSH);
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - start;
        std::cout << "libaec Throughput (encode): " << static_cast<double>(nValues) * sizeof(ValueType) / elapsed.count() / 1024 / 1024 << " MiB/s" << std::endl;
        if (ret != AEC_OK) {
            std::cerr << "libaec compression failed with error code: " << ret << std::endl;
        } else {
            std::cout << "libaec Encoded size: " << strm.total_out << " bytes." << std::endl;
        }
        aec_encode_end(&strm);
    }


    {
        // Decode using libaec for comparison
        struct aec_stream strm;
        strm.block_size = blockSize;
        strm.bits_per_sample = BPV;
        strm.rsi = rsiSize;
        strm.flags = AEC_DATA_PREPROCESS;
        strm.avail_in = encodedSize;
        strm.next_in = const_cast<uint8_t*>(encoded.data());
        strm.avail_out = aecOutputBuffer.size() * sizeof(ValueType);
        strm.next_out = reinterpret_cast<uint8_t*>(aecOutputBuffer.data());

        // std::cerr << "Total in: " << strm.total_in << ", Total out: " << strm.total_out << std::endl;
        // std::cerr << "Available in: " << strm.avail_in << ", Available out: " << strm.avail_out << std::endl;
        // std::cerr << "Next in ptr: " << static_cast<const void*>(strm.next_in) << ", Next out ptr: " << static_cast<void*>(strm.next_out) << std::endl;
        //
        aec_decode_init(&strm);

        auto start = std::chrono::high_resolution_clock::now();
        int ret = aec_decode(&strm, AEC_FLUSH);
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - start;

        if (ret != AEC_OK) {
            std::cerr << "libaec decompression failed with error code: " << ret << std::endl;
            std::cerr << "Total in: " << strm.total_in << ", Total out: " << strm.total_out << std::endl;
            std::cerr << "Available in: " << strm.avail_in << ", Available out: " << strm.avail_out << std::endl;
            std::cerr << "Next in ptr: " << static_cast<const void*>(strm.next_in) << ", Next out ptr: " << static_cast<void*>(strm.next_out) << std::endl;
        } else {
            std::cout << "libaec Throughput (decode): " << static_cast<double>(nValues) * sizeof(ValueType) / elapsed.count() / 1024 / 1024 << " MiB/s" << std::endl;
        }
        aec_decode_end(&strm);
    }


    // print first 20 entries
    // for (size_t i = 0; i < 40; ++i) {
    //     std::cout << "Original[" << i << "] = " << vec[i] << ", libaec Decoded[" << i << "] = " << aecOutputBuffer[i] << std::endl;
    // }

// compare aecOutputBuffer with vec
    for (size_t i = 0; i < vec.size(); ++i) {
        if (aecOutputBuffer[i] != vec[i]) {
            std::cerr << "Mismatch at index " << i << ": original " << vec[i] << ", libaec decoded " << aecOutputBuffer[i] << std::endl;
            return 1;
        }
    }
    std::cout << "libaec Decoded data matches original data." << std::endl;


    //
    // // Decode
    // std::vector<ValueType> decoded(vec.size());
    // {
    //     // read file into a vector
    //     // std::ifstream inFile("encodedOutput.bin", std::ios::in | std::ios::binary);
    //     // if (!inFile) {
    //     //     std::cerr << "Unable to open file";
    //     //     return 1;
    //     // }
    //     // std::vector<uint8_t> encodedData((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>());
    //     // inFile.close();
    //
    //     Decoder<ValueType, BPV, CodeOptionSet::Basic, blockSize> decoder(rsiSize);
    //
    //     auto start = std::chrono::high_resolution_clock::now();
    //     decoder.decode(encoded, decoded);
    //     // decodedData = decoder.decode_no_ref(nValues, encoded);
    //     auto end = std::chrono::high_resolution_clock::now();
    //     std::chrono::duration<double> elapsed = end - start;
    //     std::cout << "Throughput (decode): " << static_cast<double>(nValues) * sizeof(ValueType) / elapsed.count() / 1024 / 1024 << " MiB/s" << std::endl;
    // }
    //
    // size_t nValsPrint = std::min(vec.size(), size_t(10));
    //
    // // print decoded values
    // for (size_t i = 0; i < nValsPrint; ++i) {
    //     std::cout << vec[i] << " " << decoded[i] << std::endl;
    // }
    //
    // // Verify
    // assert(decoded.size() == vec.size());
    // for (size_t i = 0; i < vec.size(); ++i) {
    //     if (decoded[i] != vec[i]) {
    //         std::cerr << "Mismatch at index " << i << ": original " << vec[i] << ", decoded " << decoded[i] << std::endl;
    //         return 1;
    //     }
    // }
    // std::cout << "Decoded data matches original data." << std::endl;
    return 0;
}


int libaecPaec(size_t BPV, size_t blockSize, size_t rsiSize)
{
    // using ValueType = uint16_t;
    // constexpr size_t BPV = 16;
    // constexpr BlockSize blockSize = 8;
    // constexpr size_t rsiSize = 128;
    size_t nValues = 0;

    std::vector<ValueType> vec;
    // std::fstream file("/home/joobog/git/ccsds/data/ccsds2/ccsds_uint16.bin", std::ios::in | std::ios::binary);
    std::fstream file("/home/joobog/git/ccsds/data/hres/uint16.bin", std::ios::in | std::ios::binary);
    if (!file) {
        std::cerr << "Unable to open file";
        return 1;
    }
    ValueType value;
    while (file.read(reinterpret_cast<char*>(&value), sizeof(value))) {
        vec.push_back(value);
    }
    file.close();
    // vec.resize(512);
    nValues = vec.size();




    // std::vector<ValueType> vec;
    // std::mt19937 rng(42); // Fixed seed for reproducibility
    // std::uniform_int_distribution<ValueType> dist(0, (1 << BPV) - 1);
    // // nValues = 8 * 128;
    // // nValues = 100'000'000;
    // nValues = 1024*1024*64;
    // // nValues = 1024*1024;
    // vec.reserve(nValues);
    // for (size_t i = 0; i < nValues; ++i) {
    //     vec.push_back(dist(rng));
    //     // vec.push_back(124);
    // }
    // // std::cout << "Read " << vec.size() << " values from file." << std::endl;


    std::vector<uint8_t> encoded(vec.size() * sizeof(ValueType) * 2);
    std::vector<size_t> libaecOffsets;

    size_t totalSize = 0;
    {
        struct aec_stream strm;
        strm.block_size = blockSize;
        strm.bits_per_sample = BPV;
        strm.rsi = rsiSize;
        strm.flags = AEC_DATA_PREPROCESS;
        strm.avail_in = vec.size() * sizeof(ValueType);
        strm.next_in = reinterpret_cast<uint8_t*>(vec.data());
        strm.avail_out = encoded.size();
        strm.next_out = encoded.data();

        aec_encode_init(&strm);
        /* Enable RSI offsets */
        if (aec_encode_enable_offsets(&strm) != AEC_OK)
            return 1;

        auto start = std::chrono::high_resolution_clock::now();
        int ret = aec_encode(&strm, AEC_FLUSH);
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - start;
        std::cout << "libaec Throughput (encode): " << static_cast<double>(nValues) * sizeof(ValueType) / elapsed.count() / 1024 / 1024 << " MiB/s" << std::endl;
        if (ret != AEC_OK) {
            std::cerr << "libaec compression failed with error code: " << ret << std::endl;
            return 1;
        }

        size_t countOffsets = 0;
        size_t* offsets = nullptr;

        /* Count RSI offsets */
            if (aec_encode_count_offsets(&strm, &countOffsets) != AEC_OK)
                return 1;
        offsets = (size_t*) malloc(countOffsets * sizeof(*offsets));
        /* Get RSI offsets */
        if (aec_encode_get_offsets(&strm, offsets, countOffsets) != AEC_OK)
            return 1;

        libaecOffsets.assign(offsets, offsets + countOffsets);
        free(offsets);

        totalSize = strm.total_out;
        aec_encode_end(&strm);

        std::cout << "libaec Encoded size: " << strm.total_out << " bytes." << std::endl;
    }


    std::vector<size_t> paecOffsets;
    {
        // Decode using paec for comparison
        DecoderPar<ValueType, ValueType> decoder(BPV, blockSize, rsiSize, CodeOptionSet::Basic);
        // Decoder<ValueType, BPV, CodeOptionSet::Basic, blockSize> decoder(rsiSize);
        std::vector<ValueType> paecOutputBuffer(vec.size());
        auto start = std::chrono::high_resolution_clock::now();
        paecOffsets = decoder.decodeRsiOffsets(Span<uint8_t>(encoded.data(), totalSize), paecOutputBuffer.size());
        // decoder.decode(Span<uint8_t>(encoded.data(), totalSize), Span<ValueType>(paecOutputBuffer));
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - start;
        std::cout << "paec Throughput (decode): " << static_cast<double>(nValues) * sizeof(ValueType) / elapsed.count() / 1024 / 1024 << " MiB/s" << std::endl;
    }

    // // print offsets
    // std::cout << "libaec RSI offsets: size = " << libaecOffsets.size() << std::endl;
    // std::cout << "paec RSI offsets: size = " << paecOffsets.size() << std::endl;
    // std::cout << "expected RSI offsets: size = " << ((vec.size() + blockSize - 1) / blockSize + rsiSize - 1) / rsiSize << std::endl;
    // for (size_t i = 0; i < libaecOffsets.size(); ++i) {
    //     std::cout << libaecOffsets[i] << " " << paecOffsets[i] << std::endl;
    // }
    //
    // std::cout << std::endl;
    // // compare libaecOffsets with paecOffsets
    // if (libaecOffsets.size() != paecOffsets.size()) {
    //     std::cerr << "Mismatch in number of RSI offsets: libaec " << libaecOffsets.size() << ", paec " << paecOffsets.size() << std::endl;
    //     return 1;
    // }
    // for (size_t i = 0; i < libaecOffsets.size(); ++i) {
    //     if (libaecOffsets[i] != paecOffsets[i]) {
    //         std::cerr << "Mismatch at RSI offset index " << i << ": libaec " << libaecOffsets[i] << ", paec " << paecOffsets[i] << std::endl;
    //         return 1;
    //     }
    // }
    // std::cout << "RSI offsets from libaec and paec match." << std::endl;


    std::vector<ValueType> aecOutputBuffer(vec.size() * 2);
    {        // Decode using libaec for comparison
        struct aec_stream strm;
        strm.block_size = blockSize;
        strm.bits_per_sample = BPV;
        strm.rsi = rsiSize;
        strm.flags = AEC_DATA_PREPROCESS;
        strm.avail_in = totalSize;
        strm.next_in = const_cast<uint8_t*>(encoded.data());
        strm.avail_out = aecOutputBuffer.size() * sizeof(ValueType);
        strm.next_out = reinterpret_cast<uint8_t*>(aecOutputBuffer.data());

        aec_decode_init(&strm);

        auto start = std::chrono::high_resolution_clock::now();
        int ret = aec_decode(&strm, AEC_FLUSH);
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - start;
        std::cout << "libaec Throughput (decode): " << static_cast<double>(nValues) * sizeof(ValueType) / elapsed.count() / 1024 / 1024 << " MiB/s" << std::endl;
        if (ret != AEC_OK) {
            std::cerr << "libaec decompression failed with error code: " << ret << std::endl;
            return 1;
        }
        aec_decode_end(&strm);
    }


    // compare aecOutputBuffer with vec
    for (size_t i = 0; i < vec.size(); ++i) {
        if (aecOutputBuffer[i] != vec[i]) {
            std::cerr << "Mismatch at index " << i << ": original " << vec[i] << ", libaec decoded " << aecOutputBuffer[i] << std::endl;
            return 1;
        }
    }
    std::cout << "libaec Decoded data matches original data." << std::endl;

    return 0;
}



int main(int argc, char* argv[])
{
    constexpr size_t BPV = 16;
    // constexpr BlockSize blockSize = 32;
    // constexpr BlockSize blockSize = 16;
    constexpr size_t blockSize = 64;
    constexpr size_t rsiSize = 128;

    std::cout << "Running paecLibaec test..." << std::endl;
    std::cout << "BPV: " << BPV << ", BlockSize: " << static_cast<size_t>(blockSize) << ", RSI Size: " << rsiSize << std::endl;

    paecLibaec(BPV, blockSize, rsiSize);
    libaecPaec(BPV, blockSize, rsiSize);
    return 0;
}
