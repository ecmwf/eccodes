#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include "BitBuffer.h"

static int testCount = 0;
static int passCount = 0;
static int failCount = 0;

#define TEST(name) \
    do { testCount++; std::cout << "  " << name << "... "; } while(0)

#define PASS() \
    do { passCount++; std::cout << "PASS" << std::endl; } while(0)

#define FAIL(msg) \
    do { \
        failCount++; \
        std::cerr << "FAIL: " << msg << std::endl; \
        return; \
    } while(0)

#define EXPECT_EQ(a, b, msg) \
    do { \
        if ((a) != (b)) { \
            std::cerr << "FAIL: " << msg << " (expected " << (uint64_t)(a) \
                      << ", got " << (uint64_t)(b) << ")" << std::endl; \
            failCount++; \
            return; \
        } \
    } while(0)

// ===== writeBits / readBits round-trip =====

// For every type and every valid nBits and every bit alignment (0..7),
// write a value and read it back.
template <typename T>
void test_writeBits_readBits_exhaustive() {
    constexpr size_t maxBits = sizeof(T) * 8;
    std::string typeName;
    if constexpr (sizeof(T) == 1) typeName = "uint8_t";
    else if constexpr (sizeof(T) == 2) typeName = "uint16_t";
    else if constexpr (sizeof(T) == 4) typeName = "uint32_t";

    for (size_t nBits = 1; nBits <= maxBits; ++nBits) {
        T mask = (nBits == maxBits) ? static_cast<T>(~T(0))
                                     : static_cast<T>((T(1) << nBits) - 1);
        // Test several values: 0, 1, mask/2, mask-1, mask
        T testValues[] = {0, 1, static_cast<T>(mask / 2),
                           static_cast<T>(mask - 1), mask};

        for (T val : testValues) {
            for (size_t alignment = 0; alignment < 8; ++alignment) {
                std::string label = typeName + " nBits=" +
                    std::to_string(nBits) + " val=" + std::to_string(val) +
                    " align=" + std::to_string(alignment);
                TEST(label);

                // Buffer large enough: alignment + nBits + TPlus trailing bytes
                size_t bufBytes = (alignment + nBits + 7) / 8 + 8;
                std::vector<uint8_t> buf(bufBytes, 0);
                BitBuffer bb{Span<uint8_t>(buf)};

                bb.bitPos(alignment);
                bb.writeBits<T>(val, nBits);
                EXPECT_EQ(alignment + nBits, bb.bitPos(), label + " write pos");

                bb.bitPos(alignment);
                T got = bb.readBits<T>(nBits);
                EXPECT_EQ(val, got, label);
                EXPECT_EQ(alignment + nBits, bb.bitPos(), label + " read pos");

                PASS();
            }
        }
    }
}

// Test that sequential writes don't clobber each other
void testSequentialWrites() {
    TEST("sequential writes uint8");
    {
        std::vector<uint8_t> buf(64, 0);
        BitBuffer bb{Span<uint8_t>(buf)};

        // Write several values back-to-back
        bb.writeBits<uint8_t>(0x07, 3);    // 111
        bb.writeBits<uint8_t>(0x00, 3);    // 000
        bb.writeBits<uint8_t>(0x05, 3);    // 101
        bb.writeBits<uint8_t>(0x03, 4);    // 0011

        // Read them back
        bb.bitPos(0);
        EXPECT_EQ(0x07u, bb.readBits<uint8_t>(3), "seq[0]");
        EXPECT_EQ(0x00u, bb.readBits<uint8_t>(3), "seq[1]");
        EXPECT_EQ(0x05u, bb.readBits<uint8_t>(3), "seq[2]");
        EXPECT_EQ(0x03u, bb.readBits<uint8_t>(4), "seq[3]");
    }
    PASS();

    TEST("sequential writes uint16");
    {
        std::vector<uint8_t> buf(64, 0);
        BitBuffer bb{Span<uint8_t>(buf)};

        bb.writeBits<uint16_t>(0x1234, 16);
        bb.writeBits<uint16_t>(0x00AB, 12);
        bb.writeBits<uint8_t>(0x03, 2);

        bb.bitPos(0);
        EXPECT_EQ(0x1234u, bb.readBits<uint16_t>(16), "seq16[0]");
        EXPECT_EQ(0x00ABu, bb.readBits<uint16_t>(12), "seq16[1]");
        EXPECT_EQ(0x03u, bb.readBits<uint8_t>(2), "seq16[2]");
    }
    PASS();

    TEST("sequential writes mixed types");
    {
        std::vector<uint8_t> buf(64, 0);
        BitBuffer bb{Span<uint8_t>(buf)};

        bb.writeBits<uint8_t>(0xFF, 8);
        bb.writeBits<uint16_t>(0xBEEF, 16);
        bb.writeBits<uint32_t>(0xDEADC0DE, 32);

        bb.bitPos(0);
        EXPECT_EQ(0xFFu, bb.readBits<uint8_t>(8), "mixed[0]");
        EXPECT_EQ(0xBEEFu, bb.readBits<uint16_t>(16), "mixed[1]");
        EXPECT_EQ(0xDEADC0DEu, bb.readBits<uint32_t>(32), "mixed[2]");
    }
    PASS();
}

// ===== writeCodeword / readCodeword =====

void testCodewordRoundtrip() {
    // FS codeword of length L: L zeros followed by a 1
    // writeCodeword(length) writes that; readCodeword() returns length+1
    // (the total number of bits including the 1)
    for (size_t length = 0; length <= 128; ++length) {
        std::string label = "codeword length=" + std::to_string(length);
        TEST(label);

        size_t bufBytes = (length + 1 + 7) / 8 + 16;
        std::vector<uint8_t> buf(bufBytes, 0);
        BitBuffer bb{Span<uint8_t>(buf)};

        bb.writeCodeword(length);
        EXPECT_EQ(length + 1, bb.bitPos(), label + " write pos");

        bb.bitPos(0);
        size_t readLen = bb.readCodeword();
        // readCodeword returns the total bits consumed (zeros + 1)
        EXPECT_EQ(length + 1, readLen, label + " read value");
        EXPECT_EQ(length + 1, bb.bitPos(), label + " read pos");

        PASS();
    }
}

void testCodewordAtAlignment() {
    for (size_t align = 0; align < 8; ++align) {
        for (size_t length = 0; length <= 64; ++length) {
            std::string label = "codeword align=" + std::to_string(align) +
                                " length=" + std::to_string(length);
            TEST(label);

            size_t bufBytes = (align + length + 1 + 7) / 8 + 16;
            std::vector<uint8_t> buf(bufBytes, 0);
            BitBuffer bb{Span<uint8_t>(buf)};

            // Write some filler bits, then the codeword
            if (align > 0) {
                bb.writeBits<uint8_t>(0, align);
            }
            bb.writeCodeword(length);

            bb.bitPos(align);
            size_t readLen = bb.readCodeword();
            EXPECT_EQ(length + 1, readLen, label);

            PASS();
        }
    }
}

void testSequentialCodewords() {
    TEST("sequential codewords");
    {
        std::vector<uint8_t> buf(256, 0);
        BitBuffer bb{Span<uint8_t>(buf)};

        size_t lengths[] = {0, 1, 2, 3, 5, 8, 13, 0, 4, 7};
        for (size_t l : lengths) {
            bb.writeCodeword(l);
        }

        bb.bitPos(0);
        for (size_t l : lengths) {
            size_t got = bb.readCodeword();
            EXPECT_EQ(l + 1, got, "seq cw len=" + std::to_string(l));
        }
    }
    PASS();
}

// ===== peekBits =====

void test_peekBits() {
    TEST("peekBits basic");
    {
        // Write known pattern: 0b11010011 01010101
        std::vector<uint8_t> buf = {0xD3, 0x55, 0x00, 0x00};
        BitBuffer bb{Span<uint8_t>(buf)};

        // Peek from bit 0, 4 bits: 1101 = 0xD
        EXPECT_EQ(0xDu, bb.peekBits(4, 0), "peek[0,4]");
        // Peek from bit 0, 8 bits: 11010011 = 0xD3
        EXPECT_EQ(0xD3u, bb.peekBits(8, 0), "peek[0,8]");
        // Peek from bit 4, 4 bits: 0011 = 0x3
        EXPECT_EQ(0x3u, bb.peekBits(4, 4), "peek[4,4]");
        // Peek from bit 4, 8 bits: 00110101 = 0x35
        EXPECT_EQ(0x35u, bb.peekBits(8, 4), "peek[4,8]");
    }
    PASS();

    TEST("peekBits does not advance position");
    {
        std::vector<uint8_t> buf = {0xAB, 0xCD, 0x00, 0x00};
        BitBuffer bb{Span<uint8_t>(buf)};
        bb.bitPos(0);
        bb.peekBits(8);
        EXPECT_EQ(0u, bb.bitPos(), "peek no advance");
    }
    PASS();
}

// ===== skipBits =====

void test_skipBits() {
    TEST("skipBits");
    {
        std::vector<uint8_t> buf(16, 0);
        BitBuffer bb{Span<uint8_t>(buf)};
        EXPECT_EQ(0u, bb.bitPos(), "initial pos");
        bb.skipBits(5);
        EXPECT_EQ(5u, bb.bitPos(), "after skip 5");
        bb.skipBits(11);
        EXPECT_EQ(16u, bb.bitPos(), "after skip 16");
        bb.skipBits(0);
        EXPECT_EQ(16u, bb.bitPos(), "after skip 0");
    }
    PASS();
}

// ===== skipCodewords =====

void test_skipCodewords() {
    TEST("skipCodewords");
    {
        std::vector<uint8_t> buf(128, 0);
        BitBuffer bb{Span<uint8_t>(buf)};

        // Write 5 codewords: lengths 3, 0, 5, 1, 2
        // CW of length L = L zeros + 1 bit, consuming L+1 bits
        size_t lengths[] = {3, 0, 5, 1, 2};
        for (size_t l : lengths) {
            bb.writeCodeword(l);
        }
        size_t totalBits = 0;
        for (size_t l : lengths) totalBits += l + 1;

        // Skip 3 codewords, verify position
        bb.bitPos(0);
        bb.skipCodewords(3);
        size_t expected = (3+1) + (0+1) + (5+1); // 4 + 1 + 6 = 11
        EXPECT_EQ(expected, bb.bitPos(), "skip 3 codewords");

        // Read the 4th codeword (length=1, consumes 2 bits)
        size_t cw = bb.readCodeword();
        EXPECT_EQ(2u, cw, "4th codeword");
    }
    PASS();
}

// ===== writeId =====

void test_writeId() {
    TEST("writeId round-trip");
    {
        // writeId writes a pre-shifted 16-bit ID value
        // We test by reading the same bits back with peekBits
        std::vector<uint8_t> buf(16, 0);
        BitBuffer bb{Span<uint8_t>(buf)};

        // Simulate what CodeOption does: idShifted = value << (16 - idLength)
        // then writeId shifts right by (pos % 8)
        uint16_t idValue = 0b101; // 3-bit ID
        uint8_t idLength = 3;
        uint16_t idShifted = idValue << (16 - idLength); // 0b101_0000000000000

        bb.writeId(idShifted, idLength);
        EXPECT_EQ(idLength, bb.bitPos(), "writeId pos");

        // Read back
        uint16_t got = bb.peekBits(idLength, 0);
        EXPECT_EQ(idValue, got, "writeId value");
    }
    PASS();

    TEST("writeId at various alignments");
    {
        for (size_t align = 0; align < 8; ++align) {
            std::vector<uint8_t> buf(16, 0);
            BitBuffer bb{Span<uint8_t>(buf)};

            uint16_t idValue = 0b1011;
            uint8_t idLength = 4;
            uint16_t idShifted = idValue << (16 - idLength);

            // Skip to alignment
            if (align > 0) {
                bb.writeBits<uint8_t>(0, align);
            }
            bb.writeId(idShifted, idLength);

            uint16_t got = bb.peekBits(idLength, align);
            std::string label = "writeId align=" + std::to_string(align);
            EXPECT_EQ(idValue, got, label);
        }
    }
    PASS();
}

// ===== Random round-trip stress test =====

template <typename T>
void testRandomRoundtrip() {
    constexpr size_t maxBits = sizeof(T) * 8;
    std::string typeName;
    if constexpr (sizeof(T) == 1) typeName = "uint8_t";
    else if constexpr (sizeof(T) == 2) typeName = "uint16_t";
    else if constexpr (sizeof(T) == 4) typeName = "uint32_t";

    TEST("random round-trip " + typeName);

    std::mt19937 gen(42 + sizeof(T));
    std::vector<uint8_t> buf(8192, 0);
    BitBuffer bb{Span<uint8_t>(buf)};

    struct Entry { T value; size_t nBits; };
    std::vector<Entry> entries;

    size_t pos = 0;
    for (int i = 0; i < 500; ++i) {
        size_t nBits = (gen() % maxBits) + 1;
        T mask = (nBits == maxBits) ? static_cast<T>(~T(0))
                                     : static_cast<T>((T(1) << nBits) - 1);
        T val = static_cast<T>(gen()) & mask;

        if (pos + nBits + sizeof(T) * 2 > buf.size() * 8) break;

        bb.writeBits<T>(val, nBits);
        entries.push_back({val, nBits});
        pos += nBits;
    }

    bb.bitPos(0);
    for (size_t i = 0; i < entries.size(); ++i) {
        T got = bb.readBits<T>(entries[i].nBits);
        EXPECT_EQ(entries[i].value, got,
                  "random " + typeName + " entry " + std::to_string(i));
    }

    PASS();
}

// ===== Interleaved codewords and bits =====

void testInterleavedCodewordBits() {
    TEST("interleaved codewords and bits");
    {
        std::vector<uint8_t> buf(256, 0);
        BitBuffer bb{Span<uint8_t>(buf)};

        // Write: codeword(3), 8-bit value 0xAB, codeword(0), 4-bit value 0x5
        bb.writeCodeword(3);
        bb.writeBits<uint8_t>(0xAB, 8);
        bb.writeCodeword(0);
        bb.writeBits<uint8_t>(0x5, 4);

        bb.bitPos(0);
        size_t cw1 = bb.readCodeword();
        EXPECT_EQ(4u, cw1, "interleaved cw1");
        uint8_t v1 = bb.readBits<uint8_t>(8);
        EXPECT_EQ(0xABu, v1, "interleaved v1");
        size_t cw2 = bb.readCodeword();
        EXPECT_EQ(1u, cw2, "interleaved cw2");
        uint8_t v2 = bb.readBits<uint8_t>(4);
        EXPECT_EQ(0x5u, v2, "interleaved v2");
    }
    PASS();
}

// ===== Bit position tracking =====

void testPositionTracking() {
    TEST("position tracking through mixed operations");
    {
        std::vector<uint8_t> buf(256, 0);
        BitBuffer bb{Span<uint8_t>(buf)};

        bb.writeBits<uint16_t>(0x1234, 16);
        EXPECT_EQ(16u, bb.bitPos(), "pos after 16-bit write");

        bb.writeCodeword(5); // 5 zeros + 1 = 6 bits
        EXPECT_EQ(22u, bb.bitPos(), "pos after codeword(5)");

        bb.writeBits<uint8_t>(0x0F, 4);
        EXPECT_EQ(26u, bb.bitPos(), "pos after 4-bit write");

        bb.resetBitPos();
        EXPECT_EQ(0u, bb.bitPos(), "pos after reset");

        bb.bitPos(10);
        EXPECT_EQ(10u, bb.bitPos(), "pos after set to 10");
    }
    PASS();
}

// ===== Copy constructor =====

void testCopyConstructor() {
    TEST("copy constructor shares buffer");
    {
        std::vector<uint8_t> buf(16, 0);
        BitBuffer bb1{Span<uint8_t>(buf)};
        bb1.writeBits<uint8_t>(0xAB, 8);

        BitBuffer bb2(bb1, 0);
        uint8_t got = bb2.readBits<uint8_t>(8);
        EXPECT_EQ(0xABu, got, "copy read");
    }
    PASS();
}

// ===== Edge case: single bit =====

void testSingleBit() {
    TEST("single bit write/read at every position");
    {
        for (size_t pos = 0; pos < 64; ++pos) {
            for (uint8_t val = 0; val <= 1; ++val) {
                std::vector<uint8_t> buf(16, 0);
                BitBuffer bb{Span<uint8_t>(buf)};
                bb.bitPos(pos);
                bb.writeBits<uint8_t>(val, 1);

                bb.bitPos(pos);
                uint8_t got = bb.readBits<uint8_t>(1);
                std::string label = "bit pos=" + std::to_string(pos) +
                                    " val=" + std::to_string(val);
                EXPECT_EQ(val, got, label);
            }
        }
    }
    PASS();
}

// ===== Long codeword spanning multiple uint64 =====

void testLongCodeword() {
    // Codewords longer than 64 bits need the fallback path in readCodeword
    for (size_t length = 56; length <= 130; ++length) {
        std::string label = "long codeword length=" + std::to_string(length);
        TEST(label);

        size_t bufBytes = (length + 1 + 7) / 8 + 16;
        std::vector<uint8_t> buf(bufBytes, 0);
        BitBuffer bb{Span<uint8_t>(buf)};

        bb.writeCodeword(length);

        bb.bitPos(0);
        size_t got = bb.readCodeword();
        EXPECT_EQ(length + 1, got, label);

        PASS();
    }
}

// ===== Verify buffer bytes for known pattern =====

void testKnownBytePattern() {
    TEST("known byte pattern from writeBits");
    {
        // Write 0xFF at 8 bits starting at bit 0 => byte[0] = 0xFF
        std::vector<uint8_t> buf(16, 0);
        BitBuffer bb{Span<uint8_t>(buf)};
        bb.writeBits<uint8_t>(0xFF, 8);
        EXPECT_EQ(0xFFu, buf[0], "byte[0]");
    }
    PASS();

    TEST("known byte pattern from writeBits offset 4");
    {
        // Write 0xF (4 bits) at bit 4 => byte[0] should have 0x0F
        std::vector<uint8_t> buf(16, 0);
        BitBuffer bb{Span<uint8_t>(buf)};
        bb.bitPos(4);
        bb.writeBits<uint8_t>(0xF, 4);
        EXPECT_EQ(0x0Fu, buf[0], "byte[0] after 4-bit offset write");
    }
    PASS();

    TEST("known byte pattern 16-bit write");
    {
        // Write 0xABCD at bit 0 => byte[0]=0xAB, byte[1]=0xCD
        std::vector<uint8_t> buf(16, 0);
        BitBuffer bb{Span<uint8_t>(buf)};
        bb.writeBits<uint16_t>(0xABCD, 16);
        EXPECT_EQ(0xABu, buf[0], "byte[0]");
        EXPECT_EQ(0xCDu, buf[1], "byte[1]");
    }
    PASS();
}

// ===== peekBits consistency with readBits =====

void testPeekVsRead() {
    TEST("peekBits matches readBits");
    {
        // Write some known data
        std::vector<uint8_t> buf(32, 0);
        BitBuffer bb{Span<uint8_t>(buf)};
        bb.writeBits<uint16_t>(0x1234, 16);
        bb.writeBits<uint8_t>(0xAB, 8);

        for (size_t offset = 0; offset < 16; ++offset) {
            for (size_t nBits = 1; nBits <= 8 && offset + nBits <= 24; ++nBits) {
                uint16_t peeked = bb.peekBits(nBits, offset);
                bb.bitPos(offset);
                uint16_t readVal = bb.readBits<uint16_t>(nBits);
                std::string label = "peek vs read offset=" +
                    std::to_string(offset) + " nBits=" + std::to_string(nBits);
                EXPECT_EQ(peeked, readVal, label);
            }
        }
    }
    PASS();
}

int main() {
    std::cout << "=== BitBuffer Exhaustive Test ===" << std::endl;

    std::cout << "\n--- writeBits/readBits round-trip (uint8_t) ---" << std::endl;
    test_writeBits_readBits_exhaustive<uint8_t>();

    std::cout << "\n--- writeBits/readBits round-trip (uint16_t) ---" << std::endl;
    test_writeBits_readBits_exhaustive<uint16_t>();

    std::cout << "\n--- writeBits/readBits round-trip (uint32_t) ---" << std::endl;
    test_writeBits_readBits_exhaustive<uint32_t>();

    std::cout << "\n--- Sequential writes ---" << std::endl;
    testSequentialWrites();

    std::cout << "\n--- Codeword round-trip ---" << std::endl;
    testCodewordRoundtrip();

    std::cout << "\n--- Codeword at alignment ---" << std::endl;
    testCodewordAtAlignment();

    std::cout << "\n--- Sequential codewords ---" << std::endl;
    testSequentialCodewords();

    std::cout << "\n--- Long codewords ---" << std::endl;
    testLongCodeword();

    std::cout << "\n--- peekBits ---" << std::endl;
    test_peekBits();

    std::cout << "\n--- skipBits ---" << std::endl;
    test_skipBits();

    std::cout << "\n--- skipCodewords ---" << std::endl;
    test_skipCodewords();

    std::cout << "\n--- writeId ---" << std::endl;
    test_writeId();

    std::cout << "\n--- Random round-trip ---" << std::endl;
    testRandomRoundtrip<uint8_t>();
    testRandomRoundtrip<uint16_t>();
    testRandomRoundtrip<uint32_t>();

    std::cout << "\n--- Interleaved codewords + bits ---" << std::endl;
    testInterleavedCodewordBits();

    std::cout << "\n--- Position tracking ---" << std::endl;
    testPositionTracking();

    std::cout << "\n--- Copy constructor ---" << std::endl;
    testCopyConstructor();

    std::cout << "\n--- Single bit ---" << std::endl;
    testSingleBit();

    std::cout << "\n--- Known byte patterns ---" << std::endl;
    testKnownBytePattern();

    std::cout << "\n--- Peek vs Read consistency ---" << std::endl;
    testPeekVsRead();

    std::cout << "\n=== Summary ===" << std::endl;
    std::cout << passCount << "/" << testCount << " passed";
    if (failCount > 0) {
        std::cout << ", " << failCount << " FAILED";
    }
    std::cout << std::endl;

    return (failCount > 0) ? 1 : 0;
}
