#include <iostream>
#include <vector>
#include <assert.h>
#include <fstream>
#include <chrono>
// #include <random>
// #include <bitset>

#include "encoder/EncoderSeq.h"
#include "decoder/DecoderSeq.h"

#include <libaec.h>


int main(int argc, char* argv[])
{
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file>" << std::endl;
        return 1;
    }
    std::string filename{argv[1]};

    using ValueType = uint16_t;
    constexpr size_t BPV = 16;
    constexpr size_t blockSize = 8;
    constexpr size_t rsiSize = 128;
    size_t nValues = 0;

    std::vector<ValueType> vec;
    std::fstream file(filename, std::ios::in | std::ios::binary);
    // std::fstream file("/home/joobog/git/ccsds/data/ccsds2/ccsds_uint16.bin", std::ios::in | std::ios::binary);
    // std::fstream file("/home/joobog/git/ccsds/data/hres/uint16.bin", std::ios::in | std::ios::binary);
    // std::fstream file("/home/joobog/git/ccsds/data/ccsds2/ccsds_uint16.bin", std::ios::in | std::ios::binary);

    if (!file) {
        std::cerr << "Unable to open file" << filename << std::endl;
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
    // // nValues = 8 * 64;
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
        encoder::EncoderSeq<ValueType, ValueType> encoder((uint8_t) BPV, (uint8_t) blockSize, (uint16_t) rsiSize, CodeOptionSet::Basic);
        auto start = std::chrono::high_resolution_clock::now();
        encodedSize = encoder.encode(data, encoded);
        // encoded = encoder.encode_no_ref(data);
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - start;
        std::cout << "Throughput (encode): " << static_cast<double>(nValues) * sizeof(ValueType) / elapsed.count() / 1024 / 1024 << " MiB/s" << std::endl;
        //std::cout << "Compressibity: " << encoder.compressibility() << std::endl;

        //write to file
        std::ofstream outFile("encodedOutput.bin", std::ios::out | std::ios::binary);
        outFile.write(reinterpret_cast<const char*>(encoded.data()), encodedSize);
        outFile.close();
    }

    // dump encoded
    // std::cout << "Encoded size: " << encoded.size() << " bytes." << std::endl;
    // for (size_t i = 0; i < std::min(encoded.size(), size_t(64)); ++i) {
    //     std::cout << std::bitset<8>(encoded[i]) << " ";
    // }
    // std::cout << std::dec << std::endl;


    std::vector<ValueType> aecOutputBuffer(vec.size() * sizeof(ValueType) * 2);
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
            std::cout << "libaec Throughput: " << static_cast<double>(nValues) * sizeof(ValueType) / elapsed.count() / 1024 / 1024 << " MiB/s" << std::endl;
        }
        aec_decode_end(&strm);
    }


    // Decode
    std::vector<ValueType> decoded(vec.size());
    {
        // read file into a vector
        // std::ifstream inFile("encodedOutput.bin", std::ios::in | std::ios::binary);
        // if (!inFile) {
        //     std::cerr << "Unable to open file";
        //     return 1;
        // }
        // std::vector<uint8_t> encodedData((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>());
        // inFile.close();

        DecoderSeq<ValueType, ValueType> decoder(BPV, blockSize, rsiSize, CodeOptionSet::Basic);

        auto start = std::chrono::high_resolution_clock::now();
        decoder.decode(encoded, decoded);
        // decodedData = decoder.decode_no_ref(nValues, encoded);
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - start;
        std::cout << "Throughput (decode): " << static_cast<double>(nValues) * sizeof(ValueType) / elapsed.count() / 1024 / 1024 << " MiB/s" << std::endl;
    }

    size_t nValsPrint = std::min(vec.size(), size_t(10));

    // print decoded values
    for (size_t i = 0; i < nValsPrint; ++i) {
        std::cout << vec[i] << " " << aecOutputBuffer[i] << " " << decoded[i] << std::endl;
    }

    // Verify
    assert(decoded.size() == vec.size());
    for (size_t i = 0; i < vec.size(); ++i) {
        if (decoded[i] != vec[i]) {
            std::cerr << "Mismatch at index " << i << ": original " << vec[i] << ", decoded " << decoded[i] << std::endl;
            return 1;
        }
    }
    std::cout << "Decoded data matches original data." << std::endl;


    // control with libaec (encode + decode)


    std::vector<uint8_t> controlEncodedBuffer(vec.size() * sizeof(ValueType) * 2);
    size_t totalOut = 0;
    {
        struct aec_stream strm;
        strm.block_size = blockSize;
        strm.bits_per_sample = BPV;
        strm.rsi = rsiSize;
        strm.flags = AEC_DATA_PREPROCESS;
        strm.avail_in = vec.size() * sizeof(ValueType);
        strm.next_in = reinterpret_cast<uint8_t*>(vec.data());
        strm.avail_out = controlEncodedBuffer.size();
        strm.next_out = controlEncodedBuffer.data();

        aec_encode_init(&strm);

        auto start = std::chrono::high_resolution_clock::now();
        int ret = aec_encode(&strm, AEC_FLUSH);
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - start;
        std::cout << "libaec encode Throughput: " << static_cast<double>(nValues) * sizeof(ValueType) / elapsed.count() / 1024 / 1024 << " MiB/s" << std::endl;

        if (ret != AEC_OK) {
            std::cerr << "libaec compression failed with error code: " << ret << std::endl;
        }
        totalOut = strm.total_out;
        aec_encode_end(&strm);
    }

    std::vector<ValueType> controlDecodedBuffer(vec.size());
    {
        struct aec_stream strm;
        strm.block_size = blockSize;
        strm.bits_per_sample = BPV;
        strm.rsi = rsiSize;
        strm.flags = AEC_DATA_PREPROCESS;

        strm.avail_in = totalOut;
        strm.next_in = controlEncodedBuffer.data();

        strm.avail_out = controlDecodedBuffer.size() * sizeof(ValueType); 
        strm.next_out = reinterpret_cast<uint8_t*>(controlDecodedBuffer.data());


        // write to file
        std::ofstream outFile("libaec_encoded_output.bin", std::ios::out | std::ios::binary);
        outFile.write(reinterpret_cast<const char*>(controlEncodedBuffer.data()), totalOut);
        outFile.close();

        aec_decode_init(&strm);

        auto start = std::chrono::high_resolution_clock::now();
        int ret = aec_decode(&strm, AEC_FLUSH);
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - start;
        std::cout << "libaec decode Throughput: " << static_cast<double>(nValues) * sizeof(ValueType) / elapsed.count() / 1024 / 1024 << " MiB/s" << std::endl;

        if (ret != AEC_OK) {
            std::cerr << "libaec decompression failed with error code: " << ret << std::endl;
        }
        aec_decode_end(&strm);
    }

    {
        // print control decoded values
        std::cout << "Control decoded values (libaec):" << std::endl;
        for (size_t i = 0; i < nValsPrint; ++i) {
            std::cout << vec[i] << " " << controlDecodedBuffer[i] << std::endl;
        }

        // compare decoded data from libaec and custom decoder
        for (size_t i = 0; i < nValsPrint; ++i) {
            if (controlDecodedBuffer[i] != decoded[i]) {
                std::cerr << "Mismatch in decoded data at index " << i << ": libaec " << controlDecodedBuffer[i] << ", custom " << decoded[i] << std::endl;
                return 1;
            }
        }

        for (size_t i = 0; i < vec.size(); ++i) {
            if (controlDecodedBuffer[i] != decoded[i]) {
                std::cerr << "Mismatch in decoded data at index " << i << ": libaec " << controlDecodedBuffer[i] << ", custom " << decoded[i] << " original " << vec[i] << std::endl;
                return 1;
            }
        }
        std::cout << "Custom decoder output matches libaec decoder output." << std::endl;
    }

    return 0;
}
