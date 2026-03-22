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
// template <typename ValueType, size_t BPV, ReferenceType RType, CodeOptionSet COS, size_t BSize>
        // static SplitSampleSelector<ValueType, BPV, ReferenceType::Ref, CodeOptionSet::Basic, blockSize> ssSelector{};
        encoder::EncoderParallel<ValueType> encoder{BPV, blockSize, rsiSize, CodeOptionSet::Basic};
        auto [ssK, ssSize] = encoder.selectBestMethod<ReferenceType::Ref>(block);
        // static SplitSampleStore<ValueType, BPV, ReferenceType::Ref, CodeOptionSet::Basic, blockSize> ssStore{};
        // auto ssVariant = ssStore.select(ssK);
        // auto ssIres = std::visit([&](auto&& ssEnc) {
            // return ssEnc.prepare(block, ssSize); }, ssVariant);
        auto ssIres = encoder.ssPrepare<ReferenceType::Ref>(block, ssSize, ssK);

        ssIres.offset = 0;
        std::cout << ssIres.size << " bits encoded." << std::endl;

        auto bitsWritten = ssIres.write(bitBuffer);
        std::cout << bitsWritten << " bits written to buffer." << std::endl;
        std::cout << "Expected size: " << ssIres.size << " bits." << std::endl;
        assert(bitsWritten == ssIres.size && "Bits written do not match expected size.");
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
// template <typename ValueType, size_t BPV, ReferenceType RType, CodeOptionSet COS, size_t BSize>
        // static SplitSampleSelector<ValueType, BPV, ReferenceType::NoRef, CodeOptionSet::Basic, blockSize> ssSelector{};
        // auto [ssK, ssSize] = ssSelector.selectBestMethod(block);
        // static SplitSampleStore<ValueType, BPV, ReferenceType::NoRef, CodeOptionSet::Basic, blockSize> ssStore{};
        // auto ssVariant = ssStore.select(ssK);
        // auto ssIres = std::visit([&](auto&& ssEnc) {
        //     return ssEnc.prepare(block, ssSize); }, ssVariant);

        encoder::EncoderParallel<ValueType> encoder{BPV, blockSize, rsiSize, CodeOptionSet::Basic};
        auto [ssK, ssSize] = encoder.selectBestMethod<ReferenceType::NoRef>(block);
        auto ssIres = encoder.ssPrepare<ReferenceType::NoRef>(block, ssSize, ssK);

        ssIres.offset = 0;
        std::cout << "Selected k: " << static_cast<int>(ssK) << std::endl;
        std::cout << ssIres.size << " bits encoded." << std::endl;

        auto bitsWritten = ssIres.write(bitBuffer);
        std::cout << bitsWritten << " bits written to buffer." << std::endl;
        std::cout << "Expected size: " << ssIres.size << " bits." << std::endl;
        assert(bitsWritten == ssIres.size && "Bits written do not match expected size.");
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


    std::vector<ValueType> decodedBlock(block.size(), 0);
    {
        SplitSampleDecoder<ValueType, BPV, ReferenceType::NoRef, CodeOptionSet::Basic, blockSize> sd;
        sd.decode(bitBuffer, id, Span(decodedBlock));
    }

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


int main(int argc, char* argv[])
{
    {
        std::vector<ValueType> vec = {100, 2, 3, 1, 2, 3, 1, 2};
        testRefEncodeDecode(vec);
    }
    {
        std::vector<ValueType> vec = {0, 2, 3, 1, 2, 3, 1, 2};
        testNorefEncodeDecode(vec);
    }
    return 0;
}
