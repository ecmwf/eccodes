#include <iostream>
#include <vector>
#include <assert.h>
#include <random>

#include "BitBuffer.h"
#include "encoder/EncoderPar.h"

using ValueType = uint16_t;
constexpr size_t BPV = 16;

void testRefEncodeDecode(Span<ValueType> block)
{

    constexpr size_t blockSize = 8;
    constexpr size_t rsiSize = 128;

    std::vector<uint8_t> vec(30); // Extra space for overhead
    BitBuffer bitBuffer(vec); // Extra space for overhead

    {
        // NoCompressionEncoder<ValueType, BPV, ReferenceType::Ref, CodeOptionSet::Basic, blockSize> se{};
        // se.encode(bitBuffer);
        // auto seIres = se.prepare(block);
        encoder::EncoderParallel<ValueType> encoder{BPV, blockSize, rsiSize, CodeOptionSet::Basic};
        auto seIres = encoder.ncPrepare<ReferenceType::Ref>(block);
        seIres.offset = 0;
        size_t sizeWritten = seIres.write(bitBuffer);
        std::cout << sizeWritten << " bits written to buffer." << std::endl;
        std::cout << "Expected size: " << seIres.size << " bits." << std::endl;
        assert(seIres.size == sizeWritten && "Bits written do not match expected size.");
    }
    // dump buffer
    const auto& buf = bitBuffer.getBuffer();
    std::cout << "Encoded buffer size: " << buf.size() << " bytes" << std::endl;
    for (size_t i = 0; i < buf.size(); ++i) {
        std::cout << std::bitset<8>(buf[i]) << " ";
    }
    std::cout << std::endl;

    bitBuffer.resetBitPos();

    auto id = bitBuffer.readBits<uint8_t>(4);
    std::cout << "Read code ID: " << std::bitset<8>(id) << std::endl;
    assert(id == 0b1111 && "Incorrect code ID");

    ValueType ref = bitBuffer.readBits<ValueType>(BPV);
    std::cerr << "Read reference value: " << ref << " " << "Expected: " << block[0] << std::endl;
    assert(ref == block[0] && "Incorrect reference value");

    std::vector<ValueType> decodedBlock(block.size(), 0);
    {
        NoCompressionDecoder<ValueType, BPV, ReferenceType::Ref, CodeOptionSet::Basic, blockSize> sd;
        sd.decode(bitBuffer, Span(decodedBlock));
    }

    decodedBlock[0] = ref; // Set the reference value
                           //
    // Verify
    assert(block.size() == decodedBlock.size());
    for (size_t i = 0; i < block.size(); ++i) {
        assert(block[i] == decodedBlock[i]);
    }
    std::cout << "Test passed for block of size " << block.size() << std::endl;
}


void testRefEncodeDecode2blocks(Span<ValueType> block)
{

    constexpr size_t blockSize = 8;
    constexpr size_t rsiSize = 128;

    std::vector<uint8_t> vec(40); // Extra space for overhead
    BitBuffer bitBuffer(vec); // Extra space for overhead

    {
        // NoCompressionEncoder<ValueType, BPV, ReferenceType::Ref, CodeOptionSet::Basic, blockSize> se{};
        // se.encode(bitBuffer);
        // auto ncIres01 = se.prepare(block);
        // auto ncIres02 = se.prepare(block);
        encoder::EncoderParallel<ValueType> encoder{BPV, blockSize, rsiSize, CodeOptionSet::Basic};
        auto ncIres01 = encoder.ncPrepare<ReferenceType::Ref>(block);
        auto ncIres02 = encoder.ncPrepare<ReferenceType::Ref>(block);

        ncIres01.offset = 0;
        ncIres02.offset = ncIres01.size;

        size_t sizeWritten_01 = ncIres01.write(bitBuffer);
        size_t sizeWritten_02 = ncIres02.write(bitBuffer);

        std::cout << sizeWritten_01 << " bits written to buffer." << std::endl;
        std::cout << sizeWritten_02 - ncIres02.offset << " bits written to buffer." << std::endl;
        std::cout << "Expected size: " << ncIres01.size << " bits." << std::endl;
        std::cout << "Expected size: " << ncIres02.size << " bits." << std::endl;
        assert(ncIres01.size == sizeWritten_01 && "Bits written do not match expected size.");
        assert(ncIres02.size == sizeWritten_02 - ncIres02.offset && "Bits written do not match expected size.");
    }
    // dump buffer
    const auto& buf = bitBuffer.getBuffer();
    std::cout << "Encoded buffer size: " << buf.size() << " bytes" << std::endl;
    for (size_t i = 0; i < buf.size(); ++i) {
        std::cout << std::bitset<8>(buf[i]) << " ";
    }
    std::cout << std::endl;

    bitBuffer.resetBitPos();

    auto id = bitBuffer.readBits<uint8_t>(4);
    std::cout << "Read code ID: " << std::bitset<8>(id) << std::endl;
    assert(id == 0b1111 && "Incorrect code ID");

    ValueType ref = bitBuffer.readBits<ValueType>(BPV);
    std::cerr << "Read reference value: " << ref << " " << "Expected: " << block[0] << std::endl;
    assert(ref == block[0] && "Incorrect reference value");

    std::vector<ValueType> decodedBlock(block.size(), 0);
    {
        NoCompressionDecoder<ValueType, BPV, ReferenceType::Ref, CodeOptionSet::Basic, blockSize> sd;
        sd.decode(bitBuffer, Span(decodedBlock));
    }

    decodedBlock[0] = ref; // Set the reference value
                           //
    // Verify
    assert(block.size() == decodedBlock.size());
    for (size_t i = 0; i < block.size(); ++i) {
        assert(block[i] == decodedBlock[i]);
    }
    std::cout << "Test passed for block of size " << block.size() << std::endl;
}




// void testNorefEncodeDecode(Span<ValueType> block)
// {
//
//     constexpr size_t blockSize = 8;
//     constexpr size_t rsiSize = 128;
//
//     std::vector<uint8_t> vec(30); // Extra space for overhead
//     BitBuffer bitBuffer(vec); // Extra space for overhead
//
//     {
//         SecondExtensionEncoder<ValueType, BPV, ReferenceType::NoRef, CodeOptionSet::Basic, blockSize> se(block);
//         se.encode(bitBuffer);
//     }
//     // dump buffer
//     const auto& buf = bitBuffer.getBuffer();
//     std::cout << "Encoded buffer size: " << buf.size() << " bytes" << std::endl;
//     for (size_t i = 0; i < buf.size(); ++i) {
//         std::cout << std::bitset<8>(buf[i]) << " ";
//     }
//     std::cout << std::endl;
//
//     bitBuffer.resetBitPos();
//
//     auto id = bitBuffer.readBits<uint8_t>(5);
//     std::cout << "Read code ID: " << std::bitset<5>(id) << std::endl;
//     assert(id == 0b00001 && "Incorrect code ID");
//
//     std::vector<ValueType> decodedBlock(block.size(), 0);
//     {
//         SecondExtensionDecoder<ValueType, BPV, blockSize> sd;
//         sd.decode(bitBuffer, Span(decodedBlock));
//     }
//
//     // Verify
//     assert(block.size() == decodedBlock.size());
//     for (size_t i = 0; i < block.size(); ++i) {
//         assert(block[i] == decodedBlock[i]);
//     }
//     std::cout << "Test passed for block of size " << block.size() << std::endl;
// }


int main(int argc, char* argv[])
{
    {
        std::vector<ValueType> vec = {100, 2, 3, 1, 2, 3, 1, 2};
        testRefEncodeDecode(vec);
    }
    {
        std::vector<ValueType> vec = {100, 2, 3, 1, 2, 3, 1, 2};
        testRefEncodeDecode2blocks(vec);
    }
    // {
    //     std::vector<ValueType> vec = {1, 2, 3, 1, 2, 3, 1, 2};
    //     testNorefEncodeDecode(vec);
    // }
    return 0;
}
