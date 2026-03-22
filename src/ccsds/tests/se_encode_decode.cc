#include <iostream>
#include <vector>
#include <assert.h>
#include <random>
#include <bitset>

#include "BitBuffer.h"
#include "encoder/EncoderSeq.h"
#include "decoder/DecoderSeq.h"

using ValueType = uint16_t;
constexpr size_t BPV = 16;

void testRefEncodeDecode(Span<ValueType> block)
{

    constexpr size_t blockSize = 8;
    constexpr size_t rsiSize = 128;

    std::vector<uint8_t> vec(30); // Extra space for overhead
    BitBuffer bitBuffer(vec); // Extra space for overhead

    {
        // SecondExtensionEncoder<ValueType, BPV, ReferenceType::Ref, CodeOptionSet::Basic, blockSize> se{block};
        // se.encode(bitBuffer);
        encoder::EncoderSeq<ValueType, ValueType> seIres{BPV, rsiSize, blockSize, CodeOptionSet::Basic};
        size_t expectedSize = seIres.sePrepare<ReferenceType::Ref>(block);
        seIres.seEncode<ReferenceType::Ref>(bitBuffer);
        std::cout << bitBuffer.bitPos() << " bits written to buffer." << std::endl;
        std::cout << "Expected size: " << expectedSize << " bits." << std::endl;
        assert(expectedSize == bitBuffer.bitPos() && "Bits written do not match expected size.");
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
    std::cout << "Read code ID: " << std::bitset<8>(id) << std::endl;
    assert(id == 0b00001 && "Incorrect code ID");

    ValueType ref = bitBuffer.readBits<ValueType>(BPV);
    std::cerr << "Read reference value: " << ref << " " << "Expected: " << block[0] << std::endl;
    assert(ref == block[0] && "Incorrect reference value");

    std::vector<ValueType> decodedBlock(block.size(), 0);
    {
        DecoderSeq<ValueType, ValueType> sd{BPV, blockSize, rsiSize, CodeOptionSet::Basic};
        sd.seDecode(bitBuffer, Span(decodedBlock));
    }

    decodedBlock[0] = ref; // Set the reference value

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
        encoder::EncoderSeq<ValueType, ValueType> seIres{BPV, rsiSize, blockSize, CodeOptionSet::Basic};
        size_t expectedSize = seIres.sePrepare<ReferenceType::NoRef>(block);
        seIres.seEncode<ReferenceType::NoRef>(bitBuffer);
        std::cout << bitBuffer.bitPos() << " bits written to buffer." << std::endl;
        std::cout << "Expected size: " << expectedSize << " bits." << std::endl;
        assert(expectedSize == bitBuffer.bitPos() && "Bits written do not match expected size.");
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
    std::cout << "Read code ID: " << std::bitset<5>(id) << std::endl;
    assert(id == 0b00001 && "Incorrect code ID");

    std::vector<ValueType> decodedBlock(block.size(), 0);
    {
        DecoderSeq<ValueType, ValueType> sd{BPV, blockSize, rsiSize, CodeOptionSet::Basic};
        sd.seDecode(bitBuffer, Span(decodedBlock));
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
    {
        std::vector<ValueType> vec = {1, 2, 3, 1, 2, 3, 1, 2};
        testNorefEncodeDecode(vec);
    }
    return 0;
}
