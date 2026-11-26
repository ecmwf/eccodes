// SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// Exhaustive unit test of the codec Binary<T, BPV> for every BPV (1..64) and every length up to 20
// and around the group (8 values) and chunk (512 values) boundaries of packTo/unpackTo:
// - packTo writes exactly the bytes of a bit-by-bit reference packer (most significant bit first)
//   and nothing after them; unpackTo restores the values
// - values: 0, the largest coded value, values from 2^31 on (32 bits: the unsigned conversion
//   sp_to_uint32), noise
// - the vector interface pack/unpack, float for BPV <= 23
// The coded values are exact: integers below 2^52, or multiples of 2^(BPV - 52) above, so that
// value + 0.5 rounds to the value.
#include <cstdint>
#include <iostream>
#include <random>
#include <string>
#include <utility>
#include <vector>

#include "simple_packing/detail/codec/Binary.h"

using namespace sp::detail;

static int32_t failures = 0;

static void check(bool ok, const std::string& what)
{
    if (!ok && failures++ < 30) {
        std::cerr << "FAIL: " << what << '\n';
    }
}

static std::vector<uint8_t> referencePack(const std::vector<uint64_t>& coded, uint32_t bpv)
{
    std::vector<uint8_t> out((coded.size() * bpv + 7) / 8, 0);
    size_t bit = 0;
    for (uint64_t c : coded) {
        for (int32_t b = static_cast<int32_t>(bpv) - 1; b >= 0; --b, ++bit) {
            if ((c >> b) & 1u) out[bit / 8] |= static_cast<uint8_t>(0x80u >> (bit % 8));
        }
    }
    return out;
}

static const size_t lengths[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 63, 64, 65, 255, 256, 257,
                                  511, 512, 513, 519, 520, 521, 1023, 1024, 1025, 1535, 1536, 1537 };

template <typename T, uint32_t BPV>
static void codec(std::mt19937_64& rng)
{
    const uint32_t exactBits = BPV < 52 ? BPV : 52;
    const uint32_t shift = BPV - exactBits;
    const uint64_t maxExact = ((uint64_t{ 1 } << exactBits) - 1) << shift;
    const std::string name = std::string(sizeof(T) == 4 ? "float" : "double") + " BPV " + std::to_string(BPV);
    Binary<T, BPV> binary;
    for (size_t n : lengths) {
        std::vector<uint64_t> coded(n);
        for (size_t i = 0; i < n; ++i) {
            switch (i % 5) {
                case 0:
                    coded[i] = 0;
                    break;
                case 1:
                    coded[i] = maxExact;
                    break;
                case 2:
                    coded[i] = BPV >= 32 && BPV <= 52 ? (uint64_t{ 1 } << 31) + (rng() % 1000) : (rng() & (maxExact >> 1)) & ~((uint64_t{ 1 } << shift) - 1);
                    break;
                default:
                    coded[i] = (rng() & ((uint64_t{ 1 } << exactBits) - 1)) << shift;
            }
            coded[i] &= maxExact;
        }
        std::vector<T> values(n);
        for (size_t i = 0; i < n; ++i)
            values[i] = static_cast<T>(coded[i]);  // reference 0, no scaling

        const std::vector<uint8_t> expected = referencePack(coded, BPV);
        std::vector<uint8_t> out(expected.size() + 16, 0xAB);
        binary.packTo(values.data(), n, 0, 0, T(0), out.data());
        bool same = std::equal(expected.begin(), expected.end(), out.begin());
        bool after = true;
        for (size_t i = expected.size(); i < out.size(); ++i)
            after = after && out[i] == 0xAB;
        check(same, name + ", " + std::to_string(n) + " values: packed bytes differ from the reference");
        check(after, name + ", " + std::to_string(n) + " values: bytes after the packed data were written");

        std::vector<uint8_t> in = expected;
        in.resize(expected.size() + 16, 0);  // the block decoder may read whole words
        std::vector<T> decoded(n + 1, T(-7));
        binary.unpackTo(in.data(), 0, 0, 0.0, n, decoded.data());
        decoded.resize(n + 1);
        check(std::equal(values.begin(), values.end(), decoded.begin()) && decoded[n] == T(-7), name + ", " + std::to_string(n) + " values: unpacked values differ");

        if (n == 9 || n == 513) {  // the vector interface
            const auto bytes = binary.pack(values, 0, 0, T(0), T(0), T(0));
            check(bytes == expected, name + ": pack() differs from packTo()");
            check(binary.unpack(bytes, 0, 0, 0.0, n) == values, name + ": unpack() differs");
        }
    }
}

template <typename T, size_t... B>
static void allBpv(std::mt19937_64& rng, std::index_sequence<B...>)
{
    (codec<T, B + 1>(rng), ...);
}

int main()
{
    std::mt19937_64 rng(11);
    allBpv<double>(rng, std::make_index_sequence<64>{});
    allBpv<float>(rng, std::make_index_sequence<23>{});
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }
    std::cout << "OK\n";
    return 0;
}
