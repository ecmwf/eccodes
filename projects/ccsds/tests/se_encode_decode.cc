#include <cstddef>
#include <cstdint>
#include <iostream>
#include <vector>
#include <assert.h>
#include <bitset>

#include <exception>
#include "ccsds/Utils.h"
#include "ccsds/CodeOption.h"
#include "ccsds/Span.h"
#include "ccsds/BitBuffer.h"
#include "ccsds/encoder/EncoderSeq.h"
#include "ccsds/decoder/DecoderSeq.h"

using ValueType = uint16_t;
constexpr size_t BPV = 16;

void testRefEncodeDecode(Span<ValueType> block)
{
    constexpr size_t blockSize = 8;
    constexpr size_t rsiSize = 128;

    std::vector<uint8_t> vec(30);  // Extra space for overhead
    BitBuffer bitBuffer(vec);      // Extra space for overhead

    {
        // SecondExtensionEncoder<ValueType, BPV, ReferenceType::Ref, CodeOptionSet::Basic, blockSize> se{block};
        // se.encode(bitBuffer);
        encoder::EncoderSeq<ValueType, ValueType> seIres{ BPV, blockSize, rsiSize, CodeOptionSet::Basic };
        size_t expectedSize = seIres.sePrepare<ReferenceType::Ref>(block);
        seIres.seEncode<ReferenceType::Ref>(bitBuffer);
        std::cout << bitBuffer.bitPos() << " bits written to buffer." << '\n';
        std::cout << "Expected size: " << expectedSize << " bits." << '\n';
        assert(expectedSize == bitBuffer.bitPos() && "Bits written do not match expected size.");
    }
    // dump buffer
    const auto& buf = bitBuffer.getBuffer();
    std::cout << "Encoded buffer size: " << buf.size() << " bytes" << '\n';
    for (size_t i = 0; i < buf.size(); ++i) {
        std::cout << std::bitset<8>(buf[i]) << " ";
    }
    std::cout << '\n';

    bitBuffer.resetBitPos();

    auto id = bitBuffer.readBits<uint8_t>(5);
    std::cout << "Read code ID: " << std::bitset<8>(id) << '\n';
    assert(id == 0b00001 && "Incorrect code ID");

    ValueType ref = bitBuffer.readBits<ValueType>(BPV);
    std::cerr << "Read reference value: " << ref << " " << "Expected: " << block[0] << '\n';
    assert(ref == block[0] && "Incorrect reference value");

    std::vector<ValueType> decodedBlock(block.size(), 0);
    {
        DecoderSeq<ValueType, ValueType> sd{ BPV, blockSize, rsiSize, CodeOptionSet::Basic };
        sd.seDecode(bitBuffer, Span(decodedBlock));
    }

    decodedBlock[0] = ref;  // Set the reference value

    // Verify
    assert(block.size() == decodedBlock.size());
    for (size_t i = 0; i < block.size(); ++i) {
        assert(block[i] == decodedBlock[i]);
    }
    std::cout << "Test passed for block of size " << block.size() << '\n';
}


void testNorefEncodeDecode(Span<ValueType> block)
{
    constexpr size_t blockSize = 8;
    constexpr size_t rsiSize = 128;

    std::vector<uint8_t> vec(30);  // Extra space for overhead
    BitBuffer bitBuffer(vec);      // Extra space for overhead

    {
        encoder::EncoderSeq<ValueType, ValueType> seIres{ BPV, blockSize, rsiSize, CodeOptionSet::Basic };
        size_t expectedSize = seIres.sePrepare<ReferenceType::NoRef>(block);
        seIres.seEncode<ReferenceType::NoRef>(bitBuffer);
        std::cout << bitBuffer.bitPos() << " bits written to buffer." << '\n';
        std::cout << "Expected size: " << expectedSize << " bits." << '\n';
        assert(expectedSize == bitBuffer.bitPos() && "Bits written do not match expected size.");
    }
    // dump buffer
    const auto& buf = bitBuffer.getBuffer();
    std::cout << "Encoded buffer size: " << buf.size() << " bytes" << '\n';
    for (size_t i = 0; i < buf.size(); ++i) {
        std::cout << std::bitset<8>(buf[i]) << " ";
    }
    std::cout << '\n';

    bitBuffer.resetBitPos();

    auto id = bitBuffer.readBits<uint8_t>(5);
    std::cout << "Read code ID: " << std::bitset<5>(id) << '\n';
    assert(id == 0b00001 && "Incorrect code ID");

    std::vector<ValueType> decodedBlock(block.size(), 0);
    {
        DecoderSeq<ValueType, ValueType> sd{ BPV, blockSize, rsiSize, CodeOptionSet::Basic };
        sd.seDecode(bitBuffer, Span(decodedBlock));
    }

    // Verify
    assert(block.size() == decodedBlock.size());
    for (size_t i = 0; i < block.size(); ++i) {
        assert(block[i] == decodedBlock[i]);
    }
    std::cout << "Test passed for block of size " << block.size() << '\n';
}


int main()
{
    try {
        {
            std::vector<ValueType> vec = { 100, 2, 3, 1, 2, 3, 1, 2 };
            testRefEncodeDecode(vec);
        }
        {
            std::vector<ValueType> vec = { 1, 2, 3, 1, 2, 3, 1, 2 };
            testNorefEncodeDecode(vec);
        }
        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "FAIL: " << e.what() << '\n';
        return 1;
    }
    return 0;
}
