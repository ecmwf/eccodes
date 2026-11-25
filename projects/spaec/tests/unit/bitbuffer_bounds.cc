// SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// Unit tests of BitBuffer derived from regression tests:
// - decode_truncated: reads beyond the end of the data (limitBits) must throw, not read on
// - encode_large_samples: writeCodewords must not write beyond the buffer (std::length_error)
// - writeBits writes only the low nBits bits of a wider value
// - encode_byte_order: the stream is most significant bit first: exact bytes for every writer,
//   readers return the values of given bytes (fails on big-endian machines if a writer or reader
//   swaps bytes unconditionally)
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "spaec/detail/BitBuffer.h"
#include "spaec/detail/CodeOption.h"
#include "spaec/detail/Span.h"

using namespace spaec::detail;

static int failures = 0;

static void check(bool ok, const std::string& what)
{
    if (!ok) {
        std::cerr << "FAIL: " << what << '\n';
        ++failures;
    }
}

template <typename F>
static bool throws(F&& f)
{
    try {
        f();
    }
    catch (const std::exception&) {
        return true;
    }
    return false;
}

// A buffer of nBytes data bytes followed by the guard bytes the readers may load
static std::vector<uint8_t> padded(std::vector<uint8_t> data)
{
    data.resize(data.size() + BitBuffer::guardBytes, 0);
    return data;
}

static void readsBeyondTheEnd()
{
    // Two data bytes: 00000000 00000001 -> one codeword of 15 zeros
    std::vector<uint8_t> ok = padded({ 0x00, 0x01 });
    BitBuffer b1(Span<uint8_t>(ok), 0, 16);
    check(b1.readCodeword() == 16, "readCodeword of 15 zeros and a one");

    // Unterminated codeword: only zeros up to the end of the data (the guard bytes are zero too)
    std::vector<uint8_t> zeros = padded({ 0x00, 0x00 });
    BitBuffer b2(Span<uint8_t>(zeros), 0, 16);
    check(throws([&] { b2.readCodeword(); }), "unterminated codeword must throw");

    BitBuffer b3(Span<uint8_t>(zeros), 0, 16);
    check(throws([&] { b3.checkPos(17); }), "checkPos beyond the end must throw");
    check(!throws([&] { b3.checkPos(16); }), "checkPos at the end must not throw");

    BitBuffer b4(Span<uint8_t>(zeros), 0, 16);
    check(throws([&] {
              b4.skipCodewords(1);
              b4.checkPos();
          }),
          "skipping an unterminated codeword must throw");
}

static void writesBeyondTheEnd()
{
    // 16 bytes (128 bits): a codeword of 200 zeros does not fit
    std::vector<uint8_t> out(16, 0);
    BitBuffer b{ Span<uint8_t>(out) };
    const uint64_t lengths[] = { 3, 200 };
    bool thrown = false;
    try {
        b.writeCodewords(lengths, 2);
    }
    catch (const std::length_error&) {
        thrown = true;
    }
    check(thrown, "writeCodewords beyond the buffer must throw std::length_error");

    // A codeword of 2^40 zeros (only representable in 64 bits) must not wrap around
    std::vector<uint8_t> out2(16, 0);
    BitBuffer b2{ Span<uint8_t>(out2) };
    const uint64_t huge[] = { uint64_t{ 1 } << 40 };
    check(throws([&] { b2.writeCodewords(huge, 1); }), "writeCodewords of 2^40 zeros must throw");
}

static void writesOnlyTheLowBits()
{
    std::vector<uint8_t> out(16, 0);
    BitBuffer b{ Span<uint8_t>(out) };
    // The bits above nBits must not reach the bits before the value (at a bit offset > 0)
    b.writeBits<uint8_t>(0x00, 5);       // 00000
    b.writeBits<uint8_t>(0xFF, 3);       // 111 (only the low 3 bits of 0xFF)
    b.writeBits<uint32_t>(0, 4);         // 0000
    b.writeBits<uint32_t>(0xFFFFFF, 4);  // 1111 (only the low 4 bits of a 24-bit value)
    check(out[0] == 0x07, "writeBits(0xFF, 3) at bit 5 must write 111 only (got " + std::to_string(out[0]) + ")");
    check(out[1] == 0x0F, "writeBits(0xFFFFFF, 4) at bit 12 must write 1111 only (got " + std::to_string(out[1]) + ")");
    check(out[2] == 0x00, "no bits beyond the written values");
}

static void byteOrder()
{
    {  // writeBits: 4 bits 0xA, 16 bits 0x1234 -> A1 23 40
        std::vector<uint8_t> out(16, 0);
        BitBuffer b{ Span<uint8_t>(out) };
        b.writeBits<uint8_t>(0xA, 4);
        b.writeBits<uint16_t>(0x1234, 16);
        b.writeBits<uint32_t>(0x89ABCDEF, 32);
        const uint8_t expected[] = { 0xA1, 0x23, 0x48, 0x9A, 0xBC, 0xDE, 0xF0 };
        for (size_t i = 0; i < sizeof(expected); ++i) {
            check(out[i] == expected[i], "writeBits: byte " + std::to_string(i));
        }
    }
    {  // writeCodewords: 0, 1, 2 zeros -> 1 01 001 -> 1010 0100
        std::vector<uint8_t> out(16, 0);
        BitBuffer b{ Span<uint8_t>(out) };
        const uint64_t lengths[] = { 0, 1, 2 };
        b.writeCodewords(lengths, 3);
        check(out[0] == 0xA4 && b.bitPos() == 6, "writeCodewords: byte A4, 6 bits");
    }
    {  // writeId: an ID of 3 bits 101 (as stored in CodeOption: shifted to the top of the type)
        std::vector<uint8_t> out(16, 0);
        BitBuffer b{ Span<uint8_t>(out) };
        b.writeBits<uint8_t>(0x3, 2);  // 11
        b.writeId(static_cast<IdType>(IdType{ 0x5 } << (sizeof(IdType) * 8 - 3)), 3);
        check(out[0] == 0xE8 && b.bitPos() == 5, "writeId: 11 101 -> E8");
    }
    {  // readers
        std::vector<uint8_t> data = padded({ 0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0, 0x20 });
        BitBuffer b(Span<uint8_t>(data), 0, 72);
        check(b.peekBits(8) == 0x12, "peekBits(8) of 12 34");
        check(b.readBits<uint16_t>(16) == 0x1234, "readBits<uint16_t>(16) of 12 34");
        check(b.readBits<uint8_t>(4) == 0x5, "readBits<uint8_t>(4) of 56");
        check(b.readBits<uint32_t>(32) == 0x6789ABCD, "readBits<uint32_t>(32) at bit 20");
        b.bitPos(64);
        check(b.readCodeword() == 3, "readCodeword of 0010 0000 (2 zeros and a one)");
    }
}

int main()
{
    readsBeyondTheEnd();
    writesBeyondTheEnd();
    writesOnlyTheLowBits();
    byteOrder();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }
    std::cout << "OK\n";
    return 0;
}
