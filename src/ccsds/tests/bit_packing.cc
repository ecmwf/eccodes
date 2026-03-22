#include <cassert>
#include <iostream>
#include <bitset>

#include "BitBuffer.h"


void testWriteReadBits() {
    std::vector<uint8_t> data(10, 0);
    BitBuffer bitBuffer(data);

    bitBuffer.writeBits<uint16_t>(0b1100110011001100, 16);
    auto bitPos = bitBuffer.bitPos();
    assert(bitPos == 16);
    bitBuffer.resetBitPos();
    auto result = bitBuffer.readBits<uint16_t>(16);
    assert(result == 0b1100110011001100);
}

void test235() {
    std::vector<uint8_t> data(10, 0);
    BitBuffer bitBuffer(data);

    bitBuffer.writeBits<uint16_t>(0b1111, 4);
    bitBuffer.writeBits<uint16_t>(235, 16);
    bitBuffer.resetBitPos();
    auto nai = bitBuffer.readBits<uint16_t>(4);
    auto result = bitBuffer.readBits<uint16_t>(16);
    assert(result == 235);
}


void testReadWriteCodeword() {
    std::vector<uint8_t> data(10, 0);
    BitBuffer bitBuffer(data);

    bitBuffer.writeCodeword(5); // 5 zeros + 1 stopper = 6 bits
    std::cout << "Bit position after writing codeword 5: " << bitBuffer.bitPos() << std::endl;
    assert(bitBuffer.bitPos() == 6);
    bitBuffer.writeBits<uint8_t>(0b11, 2);
    std::cout << "Bit position after writing 2 bits: " << bitBuffer.bitPos() << std::endl;
    assert(bitBuffer.bitPos() == 8);

    std::cout << "Data written to buffer: ";
    const Span<uint8_t>& buf = bitBuffer.getBuffer();
    for (auto b : buf) {
        std::cout << std::bitset<8>(b) << " ";
    }
    std::cout << std::endl;

    bitBuffer.resetBitPos();
    auto result = bitBuffer.readCodeword();
    std::cout << "Codeword read: " << result << std::endl;
    assert(result == 6);
}

void testReadWriteCodeword2() {
    std::vector<uint8_t> data(10, 0);
    BitBuffer bitBuffer(data);

    bitBuffer.writeBits<uint16_t>(0b0000'0000'0000'0001, 16);
    assert(bitBuffer.bitPos() == 16);

    std::cout << "Data written to buffer: ";
    const Span<uint8_t>& buf = bitBuffer.getBuffer();
    for (auto b : buf) {
        std::cout << std::bitset<8>(b) << " ";
    }
    std::cout << std::endl;

    bitBuffer.resetBitPos();
    auto result = bitBuffer.readCodeword();
    std::cout << "Codeword read: " << result << std::endl;
    assert(result == 16);
}

void testReadWriteCodeword3() {
    std::vector<uint8_t> data(10, 0);
    BitBuffer bitBuffer(data);

    bitBuffer.writeBits<uint16_t>(0b0000'0000'0000'0001, 16); // 11
    assert(bitBuffer.bitPos() == 16);

    std::cout << "Data written to buffer: ";
    const Span<uint8_t>& buf = bitBuffer.getBuffer();
    for (auto b : buf) {
        std::cout << std::bitset<8>(b) << " ";
    }
    std::cout << std::endl;

    bitBuffer.resetBitPos();
    bitBuffer.skipBits(5);
    auto result = bitBuffer.readCodeword();
    std::cout << "Codeword read: " << result << std::endl;
    assert(result == 11);
}

int main() {
    std::vector<uint8_t> data(1000, 0);
    BitBuffer bitBuffer(data);

    testReadWriteCodeword3();
    testReadWriteCodeword2();
    testReadWriteCodeword();
    testWriteReadBits();
    test235();

    return 0;
}
