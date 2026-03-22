#include <iostream>
#include <vector>
#include <assert.h>
#include <random>

#include "BitBuffer.h"
#include "encoder/EncoderSeq.h"

using ValueType = uint16_t;
constexpr size_t BPV = 16;

void testRefEncodeDecode(Span<ValueType> block)
{

    constexpr size_t blockSize = 8;
    constexpr size_t rsiSize = 128;

    std::vector<uint8_t> vec(30); // Extra space for overhead
    BitBuffer bitBuffer(vec); // Extra space for overhead

    {
        // SplitSampleEncoder<ValueType, BPV, 5, ReferenceType::Ref, CodeOptionSet::Basic, blockSize> ss{};
        Encoder<ValueType> encoder{BPV, blockSize, rsiSize, CodeOptionSet::Basic};
        encoder.ssEncode<ReferenceType::Ref>(5, block, bitBuffer);
    }
    // dump buffer
    const auto& buf = bitBuffer.getBuffer();
    std::cout << "Encoded buffer size: " << buf.size() << " bytes" << std::endl;
    for (size_t i = 0; i < buf.size(); ++i) {
        std::cout << std::bitset<8>(buf[i]) << " ";
    }
    std::cout << std::endl;

    bitBuffer.resetBitPos();

    auto id = bitBuffer.readBits<uint8_t>(4) - 1;
    std::cout << "Read ID: " << static_cast<int>(id) << std::endl;

    assert(id >= 1 && id <= 29 && "Incorrect code ID");

    ValueType ref = bitBuffer.readBits<ValueType>(BPV);
    assert(ref == block[0] && "Incorrect reference value");

    std::vector<ValueType> decodedBlock(block.size(), 0);
    {
        SplitSampleDecoder<ValueType, BPV, ReferenceType::Ref, CodeOptionSet::Basic, blockSize> sd;
        sd.decode(bitBuffer, id, Span(decodedBlock));
    }

    decodedBlock[0] = ref; // Set the reference value

    // Compare
    for (size_t i = 0; i < block.size(); ++i) {
        std::cout << "Index " << i << ": original " << block[i] << ", decoded " << decodedBlock[i] << std::endl;
    }

    // Verify
    assert(block.size() == decodedBlock.size());
    for (size_t i = 0; i < block.size(); ++i) {
        assert(block[i] == decodedBlock[i]);
    }
    std::cout << "Test passed for block of size " << block.size() << std::endl;
}



void testNorefEncodeDecode(Span<ValueType> block)
{

    constexpr size_t blockSize = 8;
    constexpr size_t rsiSize = 128;

    std::vector<uint8_t> vec(30); // Extra space for overhead
    BitBuffer bitBuffer(vec); // Extra space for overhead

    {
        // SplitSampleEncoder<ValueType, BPV, 5, ReferenceType::NoRef, CodeOptionSet::Basic, blockSize> ss{};
        // ss.encode(block, bitBuffer);
        Encoder<ValueType> encoder{BPV, blockSize, rsiSize, CodeOptionSet::Basic};
        encoder.ssEncode<ReferenceType::NoRef>(5, block, bitBuffer);
    }
    // dump buffer
    const auto& buf = bitBuffer.getBuffer();
    std::cout << "Encoded buffer size: " << buf.size() << " bytes" << std::endl;
    for (size_t i = 0; i < buf.size(); ++i) {
        std::cout << std::bitset<8>(buf[i]) << " ";
    }
    std::cout << std::endl;

    bitBuffer.resetBitPos();

    auto id = bitBuffer.readBits<uint8_t>(5);
    // assert(id == 0b00001 && "Incorrect code ID");

    std::vector<ValueType> decodedBlock(block.size(), 0);
    {
        SplitSampleDecoder<ValueType, BPV, ReferenceType::Ref, CodeOptionSet::Basic, blockSize> sd;
        sd.decode(bitBuffer, id, Span(decodedBlock));
    }

    // Verify
    assert(block.size() == decodedBlock.size());
    for (size_t i = 0; i < block.size(); ++i) {
        assert(block[i] == decodedBlock[i]);
    }
    std::cout << "Test passed for block of size " << block.size() << std::endl;
}


int main(int argc, char* argv[])
{
    {
        std::vector<ValueType> vec = {100, 2, 3, 1, 2, 3, 1, 2};
        testRefEncodeDecode(vec);
    }
    // {
    //     std::vector<ValueType> vec = {1, 2, 3, 1, 2, 3, 1, 2};
    //     testNorefEncodeDecode(vec);
    // }
    return 0;
}
