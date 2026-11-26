// SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// Unit tests of the codecs Binary<T, BPV> and ArrayCodec<T, BPV> derived from regression tests
// (those go through SimplePackingBinary):
// - pack_value_range: packTo rejects values that cannot be coded (below the reference value,
//   2^BPV or more, NaN, infinity) with std::runtime_error, in the block path and in the tail
// - pack_fp_contraction: packTo rounds every operation of ((v * 10^D) - R) * 2^-E + 0.5 separately
// - zero_values: packing and unpacking 0 values does not touch the buffers (the number of blocks
//   minus one wrapped around)
// - unpack_float_precision: unpacking into float scales in double (powers in float) and rounds once
#include <cmath>
#include <cstdint>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "simple_packing/detail/codec/Array.h"
#include "simple_packing/detail/codec/Binary.h"

#include "../fp_diagnostics.h"

using namespace sp::detail;

static int32_t failures = 0;

static void check(bool ok, const std::string& what)
{
    if (!ok) {
        std::cerr << "FAIL: " << what << '\n';
        ++failures;
    }
}

template <typename T, uint8_t BPV>
static bool packThrows(const std::vector<T>& values, double reference)
{
    Binary<T, BPV> codec;
    std::vector<uint8_t> out((values.size() * BPV + 7) / 8 + 16, 0);
    try {
        codec.packTo(values.data(), values.size(), 0, 0, static_cast<T>(reference), out.data());
    }
    catch (const std::runtime_error&) {
        return true;
    }
    return false;
}

template <typename T, uint8_t BPV>
static void valueRange()
{
    const T nan = std::numeric_limits<T>::quiet_NaN();
    const T inf = std::numeric_limits<T>::infinity();
    const double limit = std::ldexp(1.0, BPV);  // reference 10, no scaling: values in [10, 10 + 2^BPV)
    const T tooLarge = static_cast<T>(10 + limit + 1);
    for (size_t n : { size_t{ 5 }, size_t{ 17 }, size_t{ 600 } }) {  // tail only, block and tail, two chunks
        for (size_t pos : { size_t{ 3 }, n - 1 }) {
            for (T bad : { T(9), T(-1), tooLarge, nan, inf, -inf }) {
                std::vector<T> values(n, T(100));
                values[pos] = bad;
                check(packThrows<T, BPV>(values, 10), "BPV " + std::to_string(BPV) + ": value " + std::to_string(bad) + " at " + std::to_string(pos) + " of " +
                                                          std::to_string(n) + " must be rejected");
            }
            std::vector<T> good(n, T(10));
            good[pos] = static_cast<T>(10 + limit - 1);
            check(!packThrows<T, BPV>(good, 10), "BPV " + std::to_string(BPV) + ": the largest value must be accepted");
        }
    }
}

static void rounding()
{
    struct Case
    {
        double value;
        int32_t decimalScale;
        int32_t binaryScale;
        double reference;
        uint32_t coded;
    };
    // Values at a rounding boundary (a fused multiply-add codes them one lower)
    const Case cases[] = { { 13136.093945312499, 1, -8, 99646, 8119025 },
                           { 1263.81806640625, 2, -8, 69140, 14653903 },
                           { 6890.2935546874996, 1, -8, 65279, 927728 },
                           { 1025.2067187499999, 2, -5, 54860, 1525142 },
                           { 1211.4638085937499, 2, -8, 87227, 8683362 } };
    for (const auto& c : cases) {
        Binary<double, 24> codec;
        const std::vector<double> values(8, c.value);
        std::vector<uint8_t> out(3 * 8 + 16, 0);
        codec.packTo(values.data(), values.size(), c.decimalScale, c.binaryScale, c.reference, out.data());
        const uint32_t coded = (uint32_t{ out[0] } << 16) | (uint32_t{ out[1] } << 8) | out[2];
        check(coded == c.coded, "pack " + std::to_string(c.value) + ": " + std::to_string(coded) + " instead of " + std::to_string(c.coded));
    }
}

template <typename T, size_t... B>
static void zeroValues(std::index_sequence<B...>)
{
    // 0 values must not access the buffers (Binary: 1..64 bits). Real one-element buffers, not
    // nullptr: with nullptr the compiler may assume that the loops do not run
    std::vector<T> values(1, T(1));
    std::vector<uint8_t> bytes(1, 0xAB);
    ((Binary<T, B + 1>().packTo(values.data(), 0, 0, 0, T(1), bytes.data()), Binary<T, B + 1>().unpackTo(bytes.data(), 0, 0, 1.0, 0, values.data())), ...);
    check(values[0] == T(1) && bytes[0] == 0xAB, "packing or unpacking 0 values changed the buffers");
}

template <typename T, size_t... B>
static void zeroValuesArray(std::index_sequence<B...>)
{
    // ArrayCodec: 1..32 bits
    ((check(ArrayCodec<T, B + 1>().pack({}, 0, 0, T(1), T(0), T(0)).empty(), "ArrayCodec::pack of 0 values"), ...));
    ((check(ArrayCodec<T, B + 1>().unpack({}, 0, 0, 1.0, 0).empty(), "ArrayCodec::unpack of 0 values"), ...));
}

// Packs the values MSB first with bpv bits each
static std::vector<uint8_t> packBits(const std::vector<uint64_t>& values, uint32_t bpv)
{
    std::vector<uint8_t> out((values.size() * bpv + 7) / 8 + 16, 0);
    size_t bit = 0;
    for (uint64_t v : values) {
        for (int32_t b = static_cast<int32_t>(bpv) - 1; b >= 0; --b, ++bit) {
            if ((v >> b) & 1u) {
                out[bit / 8] |= static_cast<uint8_t>(0x80u >> (bit % 8));
            }
        }
    }
    return out;
}

template <uint8_t BPV>
static void unpackFloat(double reference, int32_t binaryScale, int32_t decimalScale)
{
    const uint64_t mask = (uint64_t{ 1 } << BPV) - 1;
    std::vector<uint64_t> coded = { 0, 1, mask, ((uint64_t{ 1 } << (BPV - 1)) + 12345) & mask, 777777 & mask, 3, mask - 1, 42, 99 };
    const std::vector<uint8_t> bytes = packBits(coded, BPV);
    // Like the legacy ecCodes code: powers in float, scaling in double, one rounding to float
    const double s = sp_power<float>(binaryScale, 2);
    const double d = sp_power<float>(-decimalScale, 10);
    std::vector<float> out(coded.size());
    Binary<float, BPV>().unpackTo(bytes.data(), decimalScale, binaryScale, reference, coded.size(), out.data());
    for (size_t i = 0; i < coded.size(); ++i) {
        const float expected = static_cast<float>((static_cast<double>(coded[i]) * s + reference) * d);
        check(out[i] == expected, "Binary<float, " + std::to_string(BPV) + ">::unpackTo of " + std::to_string(coded[i]) + " (R=" + std::to_string(reference) + "): " +
                                      std::to_string(out[i]) + " instead of " + std::to_string(expected));
    }
    std::vector<typename ArrayCodec<float, BPV>::CodedValues::value_type> codedValues(coded.begin(), coded.end());
    const std::vector<float> array = ArrayCodec<float, BPV>().unpack(codedValues, decimalScale, binaryScale, reference, coded.size());
    for (size_t i = 0; i < coded.size(); ++i) {
        const float expected = static_cast<float>((static_cast<double>(coded[i]) * s + reference) * d);
        check(array[i] == expected, "ArrayCodec<float, " + std::to_string(BPV) + ">::unpack of " + std::to_string(coded[i]));
    }
}

int main()
{
    valueRange<double, 8>();
    valueRange<float, 8>();
    valueRange<double, 40>();
    rounding();
    zeroValues<double>(std::make_index_sequence<64>{});
    zeroValues<float>(std::make_index_sequence<64>{});
    zeroValuesArray<double>(std::make_index_sequence<32>{});
    zeroValuesArray<float>(std::make_index_sequence<32>{});
    // The cases of the regression test (ecCodes messages whose float values were wrong)
    unpackFloat<30>(-152.5, -22, 0);
    unpackFloat<32>(-9.9997, -27, 0);
    unpackFloat<26>(-4646.09, -13, 0);
    unpackFloat<32>(-3.0e6, -8, 3);
    unpackFloat<25>(12.5, -20, 1);
    unpackFloat<16>(-100.0, -4, 2);
    if (failures != 0) {
        fp_diagnostics::explain();
        std::cerr << failures << " checks failed\n";
        return 1;
    }
    std::cout << "OK\n";
    return 0;
}
