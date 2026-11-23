#include <bitset>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <vector>

#include <exception>
#include <cstddef>
#include <cassert>
#include "ccsds/CodeOption.h"
#include "ccsds/Span.h"
#include "ccsds/encoder/EncoderSeq.h"
#include "ccsds/decoder/DecoderSeq.h"

using ValueType = uint16_t;
constexpr uint8_t BPV = 16;
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
    std::cout << '\n';

    std::vector<ValueType> decoded(vec.size());
    {
        DecoderSeq<ValueType, ValueType> decoder(BPV, blockSize, rsiSize, COS);
        decoder.decode(encoded, decoded);
    }

    // Verify
    assert(decoded.size() == vec.size());
    for (size_t i = 0; i < vec.size(); ++i) {
        std::cout << "Index " << i << ": original " << vec[i] << ", decoded " << decoded[i] << '\n';
        if (decoded[i] != vec[i]) {
            std::cerr << "Mismatch at index " << i << ": original " << vec[i] << ", decoded " << decoded[i] << '\n';
            // dump encoded data
            for (size_t j = 0; j < encoded.size(); ++j) {
                std::cout << std::bitset<8>(encoded[j]) << " ";
            }
            std::cout << '\n';
            throw std::runtime_error("Encoded/decoded data mismatch.");
        }
    }
    std::cout << "Decoded data matches original data." << '\n';
}

int main()
{
    try {
        {
            std::vector<ValueType> vec{ 0, 0, 0, 0, 0, 0, 0, 0 };
            testEncodeDecode(vec);
        }

        {
            std::vector<ValueType> vec{ 0, 0, 0, 0, 0, 0, 0, 0, 0 };
            testEncodeDecode(vec);
        }

        {
            std::vector<ValueType> vec(8 * 128, 0xFFFF);  // full segment
            testEncodeDecode(vec);
        }

        {
            std::vector<ValueType> vec(8 * 64 + 1, 0xFFFF);
            testEncodeDecode(vec);
        }
    }
    catch (const std::exception& e) {
        std::cerr << "FAIL: " << e.what() << '\n';
        return 1;
    }
    return 0;
}
