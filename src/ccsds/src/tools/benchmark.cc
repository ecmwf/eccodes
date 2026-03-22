#include <iostream>
#include <vector>
#include <assert.h>
#include <fstream>
#include <chrono>
#include <random>
#include <bitset>

#include "encoder/EncoderSeq.h"
#include "decoder/DecoderSeq.h"
#include "Span.h"

#include <libaec.h>


using ValueType = uint16_t;
constexpr size_t BPV = 16;
constexpr size_t blockSize = 8;
constexpr size_t rsiSize = 128;
constexpr CodeOptionSet COS = CodeOptionSet::Basic;

class Measurement {
    public:
        double timeS;
};

class BenchmarkResult {
public:
    std::string name;
    Measurement encode;
    Measurement decode;
    size_t decodedSize;
    size_t encodedSize;

    void print(const std::string& label) const {
        //print in a line
        std::cout << name << "(" << label << "), "
                  << "ratio: " << static_cast<double>(decodedSize) / encodedSize << ", "
                  << static_cast<double>(decodedSize) / encode.timeS / 1024 / 1024 << ", "
                  << static_cast<double>(decodedSize) / decode.timeS / 1024 / 1024
                  << std::endl;
    }
};

BenchmarkResult benchmarkPaec(Span<ValueType> vec, size_t nValues)
{
    BenchmarkResult result;
    result.name = "P-AEC";

    std::vector<uint8_t> encoded(vec.size() * sizeof(ValueType) * 2);
    std::vector<ValueType> decoded(nValues);

    {
        // Span<const ValueType> data(vec);

        // Encoder<ValueType, BPV, CodeOptionSet::Basic, blockSize> encoder(rsiSize);
        encoder::EncoderSeq<ValueType, ValueType> encoder{BPV, blockSize, rsiSize, CodeOptionSet::Basic};
        auto start = std::chrono::high_resolution_clock::now();
        size_t encSize = encoder.encode(Span(vec), encoded);
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - start;
        result.encode.timeS = elapsed.count();
        result.encodedSize = encSize;
    }

    {
        DecoderSeq<ValueType, ValueType> decoder(BPV, blockSize, rsiSize, CodeOptionSet::Basic);
        auto start =  std::chrono::high_resolution_clock::now();
        decoder.decode(encoded, decoded);
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - start;
        result.decode.timeS = elapsed.count();
        result.decodedSize = decoded.size() * sizeof(ValueType);
    }

    return result;
}

// BenchmarkResult benchmarkPaecNoref(Span<const ValueType> vec, size_t nValues)
// {
//     BenchmarkResult result;
//     result.name = "P-AEC";
//
//     std::vector<uint8_t> encoded(vec.size() * sizeof(ValueType) * 2);
//     std::vector<ValueType> decoded(nValues);
//     {
//         Span<ValueType> data(vec.data(), vec.size());
//
//         Encoder<ValueType, BPV, CodeOptionSet::Basic, blockSize> encoder(rsiSize);
//         auto start = std::chrono::high_resolution_clock::now();
//         encoder.encode_no_ref(data, encoded);
//         auto end = std::chrono::high_resolution_clock::now();
//         std::chrono::duration<double> elapsed = end - start;
//         result.encode.timeS = elapsed.count();
//         result.encodedSize = encoded.size();
//     }
//
//     {
//         Decoder<ValueType, BPV, CodeOptionSet::Basic, blockSize> decoder(rsiSize);
//         auto start =  std::chrono::high_resolution_clock::now();
//         decoder.decode_no_ref(encoded, decoded);
//         auto end = std::chrono::high_resolution_clock::now();
//         std::chrono::duration<double> elapsed = end - start;
//         result.decode.timeS = elapsed.count();
//         result.decodedSize = decoded.size() * sizeof(ValueType);
//     }
//
//     return result;
// }


BenchmarkResult benchmarkLibaec(Span<ValueType> vec, size_t nValues)
{
    BenchmarkResult result;
    result.name = "libaec";

    std::vector<uint8_t> encoded(vec.size() * sizeof(ValueType) * 2);
    size_t totalOut = 0;
    {
        struct aec_stream strm;
        strm.block_size = blockSize;
        strm.bits_per_sample = BPV;
        strm.rsi = rsiSize;
        strm.flags = AEC_DATA_PREPROCESS;

        strm.avail_in = vec.size() * sizeof(ValueType);
        strm.next_in = reinterpret_cast<const uint8_t*>(vec.data());

        strm.avail_out = encoded.size();
        strm.next_out = encoded.data();

        aec_encode_init(&strm);

        auto start = std::chrono::high_resolution_clock::now();
        int ret = aec_encode(&strm, AEC_FLUSH);
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - start;
        result.encode.timeS = elapsed.count();

        if (ret != AEC_OK) {
            std::cerr << "libaec compression failed with error code: " << ret << std::endl;
        }
        totalOut = strm.total_out;
        aec_encode_end(&strm);
    }

    result.encodedSize = totalOut;

    {
        std::vector<ValueType> decodedBuffer(vec.size());

        struct aec_stream strm;
        strm.block_size = blockSize;
        strm.bits_per_sample = BPV;
        strm.rsi = rsiSize;
        strm.flags = AEC_DATA_PREPROCESS;

        strm.avail_in = totalOut;
        strm.next_in = encoded.data();

        strm.avail_out = decodedBuffer.size() * sizeof(ValueType); 
        strm.next_out = reinterpret_cast<uint8_t*>(decodedBuffer.data());

        aec_decode_init(&strm);

        auto start = std::chrono::high_resolution_clock::now();
        int ret = aec_decode(&strm, AEC_FLUSH);
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - start;
        result.decode.timeS = elapsed.count();

        if (ret != AEC_OK) {
            std::cerr << "libaec decompression failed with error code: " << ret << std::endl;
        }
    }
    result.decodedSize = nValues * sizeof(ValueType);
    return result;
}


std::vector<ValueType> generateRandomData(size_t nValues)
{
    std::vector<ValueType> vec;
    std::mt19937 rng(42); // Fixed seed for reproducibility
    std::uniform_int_distribution<ValueType> dist(0, (1 << BPV) - 1);
    vec.reserve(nValues);
    for (size_t i = 0; i < nValues; ++i) {
        vec.push_back(dist(rng));
    }
    return vec;
}


std::vector<ValueType> readDataFromFile(const std::string& filename)
{
    std::vector<ValueType> vec;
    std::fstream file(filename, std::ios::in | std::ios::binary);
    if (!file) {
        std::cerr << "Unable to open file: " << filename << std::endl;
        return vec;
    }
    ValueType value;
    while (file.read(reinterpret_cast<char*>(&value), sizeof(value))) {
        vec.push_back(value);
    }
    file.close();
    return vec;
}


BenchmarkResult benchmarkPreprocessor(Span<ValueType> vec)
{
    BenchmarkResult result;
    result.name = "Preprocessor";
    ValueType reference = 0;

    // Encoding (preprocessing)
    std::vector<ValueType> preprocessed(vec.size());
    {
        PredictivePreprocessingEncodeFast<ValueType, ValueType> preprocessor(BPV);
        auto start = std::chrono::high_resolution_clock::now();
        preprocessor.encode(vec, preprocessed);
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - start;
        result.encode.timeS = elapsed.count();
    }

    // Decoding (postprocessing)
    {
        PredictivePreprocessingDecodeFast<ValueType, ValueType> postprocessor(BPV);
        std::vector<ValueType> decoded(vec.size());
        auto start = std::chrono::high_resolution_clock::now();
        postprocessor.decode(preprocessed, reference, decoded);
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - start;
        result.decode.timeS = elapsed.count();
    }

    result.decodedSize = vec.size() * sizeof(ValueType);
    result.encodedSize = preprocessed.size() * sizeof(ValueType);

    return result;
}

BenchmarkResult benchmarkSplitSample()
{
    BenchmarkResult result;
    result.name = "SplitSample";
    ValueType reference = 0;

    std::vector<std::vector<ValueType>> vecs
    {
        {1, 1, 1, 1, 1, 1, 1, 1},
        {2, 2, 2, 2, 2, 2, 2, 2},
        {3, 3, 3, 3, 3, 3, 3, 3},
        {4, 4, 4, 4, 4, 4, 4, 4},
        {5, 5, 5, 5, 5, 5, 5, 5},
        {6, 6, 6, 6, 6, 6, 6, 6},
        {7, 7, 7, 7, 7, 7, 7, 7}
    };


    std::vector<double> elapsedEncodedTimes;
    std::vector<double> elapsedDecodedTimes;

    size_t nIterations = 10;

    for (size_t i = 0; i < nIterations; ++i) {
        for (auto& vec : vecs) {
            std::vector<uint8_t> encoded(vec.size() * sizeof(ValueType) * 2);
            BitBuffer buffer(encoded);
            {
                auto start = std::chrono::high_resolution_clock::now();
                // SplitSampleEncoder<ValueType, BPV, 1, ReferenceType::Ref, COS, blockSize> encoder{};
                // encoder.encode(vec, buffer);
                encoder::EncoderSeq<ValueType, ValueType> encoder{BPV, blockSize, rsiSize, CodeOptionSet::Basic};
                encoder.encode(Span<ValueType>(vec), encoded);
                auto end = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double> elapsed = end - start;
                elapsedEncodedTimes.push_back(elapsed.count());
                result.encodedSize += (buffer.bitPos() + 7) / 8;
            }

            std::vector<ValueType> decoded(vec.size());
            {
                buffer.resetBitPos();
                // static SplitSampleDecoder<ValueType, BPV, ReferenceType::Ref, COS, blockSize> decoder{};
                DecoderSeq<ValueType, ValueType> decoder(BPV, blockSize, rsiSize, CodeOptionSet::Basic);

                auto start = std::chrono::high_resolution_clock::now();
                uint8_t id = buffer.readBits<uint8_t>(4) - 1;
                ValueType ref = buffer.readBits<ValueType>(BPV);
                // decoder.decode(buffer, id, Span<ValueType>(decoded));
                decoder.ssDecode<ReferenceType::Ref>(buffer, id, Span<ValueType>(decoded));
                auto end = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double> elapsed = end - start;
                elapsedDecodedTimes.push_back(elapsed.count());
            }
        }
    }

    result.encode.timeS = std::accumulate(elapsedEncodedTimes.begin(), elapsedEncodedTimes.end(), 0.0) / elapsedEncodedTimes.size();
    result.decode.timeS = std::accumulate(elapsedDecodedTimes.begin(), elapsedDecodedTimes.end(), 0.0) / elapsedDecodedTimes.size();

    result.decodedSize = vecs[0].size() * sizeof(ValueType);

    return result;
}

BenchmarkResult benchmarkNocompression()
{
    std::vector<std::vector<ValueType>> randomVecs;
    for (size_t i = 0; i < 10000; ++i) {
        randomVecs.push_back(generateRandomData(8));
    }

    BenchmarkResult result;
    result.name = "NoCompression";

    std::vector<double> elapsedEncodedTimes;
    std::vector<double> elapsedDecodedTimes;
    size_t nIterations = 10;

// template <typename ValueType, uint8_t BPV, ReferenceType RefType, CodeOptionSet COS, BlockSize BSize>
    for (size_t i = 0; i < nIterations; ++i) {
        for (auto& vec : randomVecs) {
            std::vector<uint8_t> encoded(vec.size() * sizeof(ValueType));
            BitBuffer buffer(encoded);
            {
                auto start = std::chrono::high_resolution_clock::now();
                // NoCompressionEncoder<ValueType, BPV, ReferenceType::NoRef, COS, blockSize> encoder{};
                // encoder.encode(vec, buffer);
                encoder::EncoderSeq<ValueType, ValueType> encoder{BPV, blockSize, rsiSize, CodeOptionSet::Basic};
                encoder.encode(Span<ValueType>(vec), encoded);
                auto end = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double> elapsed = end - start;
                elapsedEncodedTimes.push_back(elapsed.count());
                result.encodedSize += (buffer.bitPos() + 7) / 8;
            }

            std::vector<ValueType> decoded(vec.size());
            {
                buffer.resetBitPos();
                // static NoCompressionDecoder<ValueType, BPV, ReferenceType::NoRef, COS, blockSize> decoder{};
                DecoderSeq<ValueType, ValueType> decoder(BPV, blockSize, rsiSize, CodeOptionSet::Basic);

                auto start = std::chrono::high_resolution_clock::now();
                // decoder.decode(buffer, Span<ValueType>(decoded));
                decoder.ncDecode<ReferenceType::NoRef>(buffer, Span<ValueType>(decoded));
                auto end = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double> elapsed = end - start;
                elapsedDecodedTimes.push_back(elapsed.count());
            }
        }
    }

    result.encode.timeS = std::accumulate(elapsedEncodedTimes.begin(), elapsedEncodedTimes.end(), 0.0) / elapsedEncodedTimes.size();
    result.decode.timeS = std::accumulate(elapsedDecodedTimes.begin(), elapsedDecodedTimes.end(), 0.0) / elapsedDecodedTimes.size();

    result.decodedSize = randomVecs[0].size() * sizeof(ValueType);

    return result;
}


BenchmarkResult benchmarkSecondExtension()
{
    BenchmarkResult result;
    result.name = "SecondExtension";

    std::vector<std::vector<ValueType>> vecs
    {
        {1, 2, 1, 2, 1, 2, 1, 2},
        {3, 4, 3, 4, 3, 4, 3, 4},
        {5, 6, 5, 6, 5, 6, 5, 6},
        {7, 8, 7, 8, 7, 8, 7, 8}
    };
    std::vector<double> elapsedEncodedTimes;
    std::vector<double> elapsedDecodedTimes;
    size_t nIterations = 10;

    for (size_t i = 0; i < nIterations; ++i) {
        for (auto& vec : vecs) {
            std::vector<uint8_t> encoded(vec.size() * sizeof(ValueType) * 3);
            BitBuffer buffer(encoded);
            {
                auto start = std::chrono::high_resolution_clock::now();
                // SecondExtensionEncoder<ValueType, BPV, ReferenceType::Ref, COS, blockSize> encoder(vec);
                // encoder.encode(buffer);
                encoder::EncoderSeq<ValueType, ValueType> encoder{BPV, blockSize, rsiSize, CodeOptionSet::Basic};
                encoder.encode(Span<ValueType>(vec), encoded);
                auto end = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double> elapsed = end - start;
                elapsedEncodedTimes.push_back(elapsed.count());
                result.encodedSize += (buffer.bitPos() + 7) / 8;
            }

            std::vector<ValueType> decoded(vec.size());
            {
                buffer.resetBitPos();
                // static SecondExtensionDecoder<ValueType, BPV, blockSize> decoder{};
                DecoderSeq<ValueType, ValueType> decoder(BPV, blockSize, rsiSize, CodeOptionSet::Basic);

                auto start = std::chrono::high_resolution_clock::now();
                // decoder.decode(buffer, Span<ValueType>(decoded));
                decoder.seDecode(buffer, Span<ValueType>(decoded));
                auto end = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double> elapsed = end - start;
                elapsedDecodedTimes.push_back(elapsed.count());
            }
        }
    }

    result.encode.timeS = std::accumulate(elapsedEncodedTimes.begin(), elapsedEncodedTimes.end(), 0.0) / elapsedEncodedTimes.size();
    result.decode.timeS = std::accumulate(elapsedDecodedTimes.begin(), elapsedDecodedTimes.end(), 0.0) / elapsedDecodedTimes.size(); 
    result.decodedSize = vecs[0].size() * sizeof(ValueType);

    return result;
}

//
// BenchmarkResult benchmarkZeroBlock()
// {
//     BenchmarkResult result;
//     result.name = "ZeroBlock";
//
//     std::vector<std::vector<ValueType>> vecs
//     {
//         {0, 0, 0, 0, 0, 0, 0, 0},
//         {0, 0, 0, 0, 0, 0, 0, 0},
//         {0, 0, 0, 0, 0, 0, 0, 0},
//         {0, 0, 0, 0, 0, 0, 0, 0}
//     };
//     std::vector<double> elapsedEncodedTimes;
//     std::vector<double> elapsedDecodedTimes;
//     size_t nIterations = 10;
//
//     for (size_t i = 0; i < nIterations; ++i) {
//         for (auto& vec : vecs) {
//             std::vector<uint8_t> encoded(vec.size() * sizeof(ValueType) * 2);
//             BitBuffer buffer(encoded);
//             {
//                 auto start = std::chrono::high_resolution_clock::now();
//                 ZeroBlockEncoder<ValueType, BPV, ReferenceType::NoRef, COS, blockSize> encoder(vec);
//                 encoder.encode(buffer);
//                 auto end = std::chrono::high_resolution_clock::now();
//                 std::chrono::duration<double> elapsed = end - start;
//                 elapsedEncodedTimes.push_back(elapsed.count());
//                 result.encodedSize += (buffer.bitPos() + 7) / 8;
//             }
//
//             std::vector<ValueType> decoded(vec.size());
//             {
//                 buffer.resetBitPos();
//                 static ZeroBlockDecoder<ValueType, BPV, ReferenceType::NoRef, COS, blockSize> decoder{};
//
//                 auto start = std::chrono::high_resolution_clock::now();
//                 decoder.decode(buffer, Span<ValueType>(decoded.data(), decoded.size()));
//                 auto end = std::chrono::high_resolution_clock::now();
//                 std::chrono::duration<double> elapsed = end - start;
//                 elapsedDecodedTimes.push_back(elapsed.count());
//             }
//         }
//     }
//
//     result.encode.timeS = std::accumulate(elapsedEncodedTimes.begin(), elapsedEncodedTimes.end(), 0.0) / elapsedEncodedTimes.size();
//     result.decode.timeS = std::accumulate(elapsedDecodedTimes.begin(), elapsedDecodedTimes.end(), 0.0) / elapsedDecodedTimes.size(); 
//     result.decodedSize = vecs[0].size() * sizeof(ValueType);   
//     return result;
// }

int main(int argc, char* argv[])
{

    using ValueType = uint16_t;
    constexpr size_t BPV = 16;
    constexpr size_t blockSize = 8;
    constexpr size_t rsiSize = 128;

    auto vecFile = readDataFromFile("/home/joobog/git/ccsds/data/ccsds2/ccsds_uint16.bin");
    auto vecRandom = generateRandomData(100'000'000);

    {
        // std::cout << "Benchmarking P-AEC on file data:" << std::endl;
        auto result = benchmarkPaec(Span<ValueType>(vecFile), vecFile.size());
        result.print("file");
    }

    // {
    //     // std::cout << "Benchmarking P-AEC NoRef on file data:" << std::endl;
    //     auto result = benchmarkPaecNoref(Span<const ValueType>(vecFile), vecFile.size());
    //     result.print("file_noref");
    // }

    {
        // std::cout << "Benchmarking libaec on file data:" << std::endl;
        auto result = benchmarkLibaec(Span<ValueType>(vecFile), vecFile.size());
        result.print("file");
    }

    {
        // std::cout << "Benchmarking P-AEC on random data:" << std::endl;
        auto result = benchmarkPaec(Span<ValueType>(vecRandom), vecRandom.size());
        result.print("random");
    }

    // {
    //     // std::cout << "Benchmarking P-AEC NoRef on random data:" << std::endl;
    //     auto result = benchmarkPaecNoref(Span<const ValueType>(vecRandom), vecRandom.size());
    //     result.print("random_noref");
    // }

    {
        // std::cout << "Benchmarking libaec on random data:" << std::endl;
        auto result = benchmarkLibaec(Span<ValueType>(vecRandom), vecRandom.size());
        result.print("random");
    }

    {
        // std::cout << "Benchmarking Preprocessor on file data:" << std::endl;
        auto result = benchmarkPreprocessor(Span<ValueType>(vecFile));
        result.print("file");
    }

    {
        // std::cout << "Benchmarking Preprocessor on random data:" << std::endl;
        auto result = benchmarkPreprocessor(Span<ValueType>(vecRandom));
        result.print("random");
    }

    {
        // std::cout << "Benchmarking SplitSample on file data:" << std::endl;
        auto result = benchmarkSplitSample();
        result.print("file");
    }

    {
        // std::cout << "Benchmarking NoCompression on file data:" << std::endl;
        auto result = benchmarkNocompression();
        result.print("file");
    }

    {
        // std::cout << "Benchmarking SecondExtension on file data:" << std::endl;
        auto result = benchmarkSecondExtension();
        result.print("file");
    }

    return 0;
}
