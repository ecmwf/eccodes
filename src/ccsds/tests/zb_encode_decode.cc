#include <iostream>
#include <vector>
#include <assert.h>
#include <random>
#include <bitset>

#include "encoder/EncoderSeq.h"
#include "decoder/DecoderSeq.h"

using ValueType = uint16_t;
constexpr size_t BPV = 16;
constexpr CodeOptionSet COS = CodeOptionSet::Basic;

void testEncodeDecode(std::vector<ValueType>& vec)
{

    constexpr size_t blockSize = 8;
    constexpr size_t rsiSize = 128;

    size_t encodedSize = 0;
    std::vector<uint8_t> encoded(vec.size() * sizeof(ValueType) * 2);
    {
        Span<ValueType> data(vec);
        encoder::EncoderSeq<ValueType, ValueType> encoder(BPV, blockSize, rsiSize, COS);
        encodedSize = encoder.encode(data, encoded);
    }

    // print encoded data
    std::cout << "Encoded data (" << encodedSize << ") (" << encoded.size() << " bytes): ";
    for (size_t i = 0; i < encodedSize; ++i) {
        std::cout << std::bitset<8>(encoded[i]) << " ";
    }
    std::cout << std::endl;

    std::vector<ValueType> decoded(vec.size());
    {
        DecoderSeq<ValueType, ValueType> decoder(BPV, blockSize, rsiSize, COS);
        decoder.decode(encoded, decoded);
    }

    // Verify
    assert(decoded.size() == vec.size());
    for (size_t i = 0; i < vec.size(); ++i) {
        std::cout << "Index " << i << ": original " << vec[i] << ", decoded " << decoded[i] << std::endl;
        if (decoded[i] != vec[i]) {
            std::cerr << "Mismatch at index " << i << ": original " << vec[i] << ", decoded " << decoded[i] << std::endl;
            // dump encoded data
            for (size_t j = 0; j < encoded.size(); ++j) {
                std::cout << std::bitset<8>(encoded[j]) << " ";
            }
            std::cout << std::endl;
            throw std::runtime_error("Encoded/decoded data mismatch.");
        }
    }
    std::cout << "Decoded data matches original data." << std::endl;
}

int main(int argc, char* argv[])
{
    {
        std::vector<ValueType> vec{0, 0, 0, 0, 0, 0, 0, 0};
        testEncodeDecode(vec);
    }

    {
        std::vector<ValueType> vec{0, 0, 0, 0, 0, 0, 0, 0, 0};
        testEncodeDecode(vec);
    }

    {
        std::vector<ValueType> vec(8 * 128, 0xFFFF); // full segment
        testEncodeDecode(vec);
    }

    {
        std::vector<ValueType> vec(8 * 64 + 1, 0xFFFF);
        testEncodeDecode(vec);
    }
    return 0;
}
