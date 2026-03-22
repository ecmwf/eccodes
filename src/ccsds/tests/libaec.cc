#include <iostream>
#include <vector>
#include <assert.h>
#include <random>
#include <libaec.h>

#include "BitBuffer.h"
#include "decoder/DecoderSeq.h"
#include "encoder/EncoderSeq.h"


using ValueType = uint16_t;


void testRefEncodeDecode1(Span<ValueType> source)
{
    // Config
    constexpr size_t BPV = 16;
    constexpr BlockSize blockSize = 8;
    constexpr size_t rsiSize = 128;

    constexpr unsigned int flags = [&]() {
        unsigned int flags = 0;
        if constexpr (isLittleEndian) {
            flags = AEC_DATA_PREPROCESS;
        } else {
            flags = AEC_DATA_PREPROCESS | AEC_DATA_MSB;
        }
        return flags;
    }();

    // Encode with libaec
    std::vector<uint8_t> encoded(source.size() * sizeof(ValueType) * 2);
    struct aec_stream strm;
    {
        size_t totalOut = 0;
        {
            strm.block_size = blockSize;
            strm.bits_per_sample = BPV;
            strm.rsi = rsiSize;
            strm.flags = flags;
            strm.avail_in = source.size() * sizeof(ValueType);
            strm.next_in = reinterpret_cast<uint8_t*>(source.data());
            strm.avail_out = encoded.size();
            strm.next_out = encoded.data();

            aec_encode_init(&strm);

            int ret = aec_encode(&strm, AEC_FLUSH);

            if (ret != AEC_OK) {
                std::cerr << "libaec compression failed with error code: " << ret << std::endl;
            }
            totalOut = strm.total_out;
            aec_encode_end(&strm);
        }
    }


    // Decode with PAEC
    std::vector<ValueType> decoded(source.size());
    Decoder<ValueType, BPV, CodeOptionSet::Basic, blockSize> decoder{128};
    decoder.decode(encoded, decoded);

    // Compare
    for (size_t i = 0; i < source.size(); ++i) {
        if (source[i] != decoded[i]) {
            std::cerr << "Mismatch at index " << i << ": original " << source[i] << ", decoded " << decoded[i] << std::endl;
            throw std::runtime_error("Encoded/decoded data mismatch.");
        }
    }
}

void testRefEncodeDecode2(Span<ValueType> source)
{
    // Config
    constexpr size_t BPV = 16;
    constexpr BlockSize blockSize = 8;
    constexpr size_t rsiSize = 128;

    constexpr unsigned int flags = [&]() {
        unsigned int flags = 0;
        if constexpr (isLittleEndian) {
            flags = AEC_DATA_PREPROCESS;
        } else {
            flags = AEC_DATA_PREPROCESS | AEC_DATA_MSB;
        }
        return flags;
    }();

    // Encode with PAEC
    std::vector<uint8_t> encoded(source.size() * sizeof(ValueType) * 2);
    {
        // Encoder<ValueType, BPV, CodeOptionSet::Basic, blockSize> encoder{128};
        // encoder.encode(source, encoded);
        Encoder<ValueType> encoder{BPV, blockSize, rsiSize, CodeOptionSet::Basic};
        encoder.encode(source, encoded);
    }
    // Decode with libaec

    std::vector<ValueType> decoded(source.size());
    struct aec_stream strm;
    {
        strm.block_size = blockSize;
        strm.bits_per_sample = BPV;
        strm.rsi = rsiSize;
        strm.flags = flags;
        strm.avail_in = encoded.size();
        strm.next_in = encoded.data();
        strm.avail_out = decoded.size() * sizeof(ValueType);
        strm.next_out = reinterpret_cast<uint8_t*>(decoded.data());
        aec_decode_init(&strm);
        int ret = aec_decode(&strm, AEC_FLUSH);
        if (ret != AEC_OK) {
            std::cerr << "libaec decompression failed with error code: " << ret << std::endl;
        }
        aec_decode_end(&strm);
    }
    // Compare
    for (size_t i = 0; i < source.size(); ++i) {
        if (source[i] != decoded[i]) {
            std::cerr << "Mismatch at index " << i << ": original " << source[i] << ", decoded " << decoded[i] << std::endl;
            throw std::runtime_error("Encoded/decoded data mismatch.");
        }
    }
}

int main(int argc, char* argv[])
{
    {
        // std::vector<ValueType> vec = {0, 2, 3, 1, 2, 3, 1, 2};
        std::vector<ValueType> vec = {100, 2, 3, 1, 2, 3, 1, 2, 0, 2, 3, 1, 2, 3, 1, 2};
        testRefEncodeDecode1(vec);
        testRefEncodeDecode2(vec);
    }

    return 0;
}
