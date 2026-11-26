// SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// BitPacking must store and load values most significant bit first on every platform and with
// every compiler. It wrote bytes into one member of a union and read another (type punning):
// icpx removed the byte stores (decoded values were 0), and on big-endian platforms the 128-bit
// path (57 to 64 bits per value) read the bytes in the wrong order.
// Each operation is compared with a bit-by-bit reference for all widths and bit offsets.
#include <cstdint>
#include <iostream>
#include <utility>
#include <vector>

#include "simple_packing/detail/BitPacking.h"

using namespace sp::detail;

static uint64_t next(uint64_t& s)  // splitmix64
{
    uint64_t z = (s += 0x9E3779B97F4A7C15ull);
    z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ull;
    z = (z ^ (z >> 27)) * 0x94D049BB133111EBull;
    return z ^ (z >> 31);
}

static uint64_t readBits(const std::vector<uint8_t>& buf, size_t bit, uint32_t n)
{
    uint64_t v = 0;
    for (uint32_t i = 0; i < n; ++i, ++bit) {
        v = (v << 1) | ((buf[bit / 8] >> (7 - bit % 8)) & 1u);
    }
    return v;
}

static int32_t failures = 0;

static void fail(const char* what, uint32_t bpv, uint32_t offset, uint64_t expected, uint64_t actual)
{
    if (failures++ < 20) {
        std::cerr << what << " bpv=" << bpv << " offset=" << offset << ": expected " << expected << ", got " << actual << '\n';
    }
}

template <std::size_t BPV, Optimization Method>
static void check(uint64_t& rng)
{
    using Packing = BitPacking<BPV, Method>;
    using V = typename Packing::ValueType;
    const uint64_t mask = BPV == 64 ? ~uint64_t{ 0 } : (uint64_t{ 1 } << BPV) - 1;
    Packing packing;
    for (uint32_t offset = 0; offset < 8; ++offset) {
        // Byte-aligned widths are only written at byte boundaries
        if ((BPV % 8 == 0) && offset != 0) {
            continue;
        }
        for (int32_t round = 0; round < 50; ++round) {
            const uint64_t value = round == 0 ? mask : round == 1 ? 0
                                                                  : next(rng) & mask;

            // decode / decode_element: arbitrary bytes, 32 bytes of slack for the block codec
            std::vector<uint8_t> buf(48);
            for (auto& b : buf)
                b = static_cast<uint8_t>(next(rng));
            const uint64_t field = readBits(buf, offset, BPV);
            if (static_cast<uint64_t>(packing.decode(buf.data(), static_cast<uint8_t>(offset))) != field) {
                fail(Method == Optimization::Block ? "decode (block)" : "decode", BPV, offset, field, static_cast<uint64_t>(packing.decode(buf.data(), static_cast<uint8_t>(offset))));
            }
            if (static_cast<uint64_t>(packing.decode_element(buf.data(), static_cast<uint8_t>(offset))) != field) {
                fail("decode_element", BPV, offset, field, static_cast<uint64_t>(packing.decode_element(buf.data(), static_cast<uint8_t>(offset))));
            }

            // encode: the bits before the offset are kept, the following bytes are zero
            std::vector<uint8_t> out(48, 0);
            const uint8_t prefix = static_cast<uint8_t>(next(rng)) & static_cast<uint8_t>(0xFF00u >> offset);
            out[0] = prefix;
            packing.encode(out.data(), static_cast<uint8_t>(offset), static_cast<V>(value));
            if (readBits(out, offset, BPV) != value) {
                fail(Method == Optimization::Block ? "encode (block)" : "encode", BPV, offset, value, readBits(out, offset, BPV));
            }
            if ((out[0] & static_cast<uint8_t>(0xFF00u >> offset)) != prefix) {
                fail("encode: bits before the value", BPV, offset, prefix, out[0]);
            }

            // encode_element: all other bits are kept
            std::vector<uint8_t> elem = buf;
            packing.encode_element(elem.data(), static_cast<uint8_t>(offset), static_cast<V>(value));
            if (readBits(elem, offset, BPV) != value) {
                fail("encode_element", BPV, offset, value, readBits(elem, offset, BPV));
            }
            for (size_t bit = 0; bit < elem.size() * 8; ++bit) {
                if ((bit < offset || bit >= offset + BPV) && readBits(elem, bit, 1) != readBits(buf, bit, 1)) {
                    fail("encode_element: other bits", BPV, offset, readBits(buf, bit, 1), readBits(elem, bit, 1));
                    break;
                }
            }
        }
    }
}

template <std::size_t... B>
static void checkAll(uint64_t& rng, std::index_sequence<B...>)
{
    (check<B + 1, Optimization::None>(rng), ...);
    (check<B + 1, Optimization::Block>(rng), ...);
}

int main()
{
    uint64_t rng = 12345;
    checkAll(rng, std::make_index_sequence<64>{});
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }
    std::cout << "OK\n";
    return 0;
}
