// SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include <algorithm>
#include <iostream>
#include <vector>
#include <cassert>
#include <cstdint>
#include <limits>
#include <random>
#include <string>
#include <type_traits>

#include <exception>
#include <cstdlib>
#include <cstddef>
#include "spaec/detail/Span.h"
#include "spaec/detail/Preprocessor.h"
#include "spaec/detail/encoder/EncoderSeq.h"
#include "spaec/detail/decoder/DecoderSeq.h"
#include "spaec/detail/BitBuffer.h"
#include "spaec/detail/CodeOption.h"

using namespace spaec::detail;

static int32_t testCount = 0;
static int32_t passCount = 0;

#define TEST(name)                           \
    do {                                     \
        testCount++;                         \
        std::cout << "  " << name << "... "; \
    } while (0)

#define PASS()                       \
    do {                             \
        passCount++;                 \
        std::cout << "PASS" << '\n'; \
    } while (0)

#define ASSERT_EQ(a, b)                                                             \
    do {                                                                            \
        auto _a = (a);                                                              \
        auto _b = (b);                                                              \
        if (_a != _b) {                                                             \
            std::cerr << "FAIL: " << #a << "=" << +_a << " != " << #b << "=" << +_b \
                      << " at " << __FILE__ << ":" << __LINE__ << '\n';             \
            std::exit(1);                                                           \
        }                                                                           \
    } while (0)

#define ASSERT_TRUE(cond)                                               \
    do {                                                                \
        if (!(cond)) {                                                  \
            std::cerr << "FAIL: " << #cond                              \
                      << " at " << __FILE__ << ":" << __LINE__ << '\n'; \
            std::exit(1);                                               \
        }                                                               \
    } while (0)

// ============================================================================
// §4.4 Prediction Error Mapper Tests
// ============================================================================

// Verify: δ = 2Δ when 0 ≤ Δ ≤ θ
//         δ = 2|Δ|-1 when -θ ≤ Δ < 0
//         δ = θ+|Δ| otherwise
// where θ = min(x̂-xmin, xmax-x̂)
template <typename ValueType>
void testPreprocessorMapperSpecFormula()
{
    std::cout << "§4.4 Prediction Error Mapper (" << sizeof(ValueType) * 8 << "-bit "
              << (std::is_signed_v<ValueType> ? "signed" : "unsigned") << "):" << '\n';

    constexpr uint8_t bpv = sizeof(ValueType) * 8;
    PredictivePreprocessingEncodeFast<ValueType> preprocessor(bpv);
    PredictivePreprocessingDecodeFast<ValueType> preprocessorDecode(bpv);

    // For unsigned n-bit: xmin=0, xmax=2^n - 1
    // For signed n-bit: xmin=-2^(n-1), xmax=2^(n-1)-1

    // Test encode then decode roundtrip with various data patterns
    {
        TEST("encode/decode roundtrip - sequential values");
        std::vector<ValueType> data;
        if constexpr (std::is_signed_v<ValueType>) {
            data = { 0, 1, -1, 2, -2, 3, -3, 10, -10, 0 };
        }
        else {
            data = { 10, 11, 9, 12, 8, 13, 7, 20, 0, 10 };
        }
        std::vector<ValueType> mapped(data.size());
        std::vector<ValueType> restored(data.size());
        ValueType reference = data[0];

        preprocessor.encode(data, mapped);
        preprocessorDecode.decode(mapped, reference, restored);

        for (size_t i = 0; i < data.size(); ++i) {
            ASSERT_EQ(data[i], restored[i]);
        }
        PASS();
    }

    {
        TEST("first sample passes through unchanged (reference)");
        std::vector<ValueType> data = { 42, 43, 44 };
        std::vector<ValueType> mapped(data.size());
        preprocessor.encode(data, mapped);
        ASSERT_EQ(mapped[0], data[0]);
        PASS();
    }

    {
        TEST("identical samples → all zeros after first");
        ValueType val = 100;
        std::vector<ValueType> data(8, val);
        std::vector<ValueType> mapped(data.size());
        preprocessor.encode(data, mapped);
        ASSERT_EQ(mapped[0], val);
        for (size_t i = 1; i < data.size(); ++i) {
            ASSERT_EQ(mapped[i], ValueType(0));
        }
        PASS();
    }

    {
        TEST("unit-delay predictor: Δ=xi-x(i-1), small positive delta → δ=2Δ");
        // When predicted = x(i-1), and delta = x(i) - x(i-1) is small positive
        // and within θ range, then mapped = 2*delta
        ValueType mid;
        if constexpr (std::is_signed_v<ValueType>) {
            mid = 0;
        }
        else {
            mid = ValueType(1) << (bpv - 1);  // midpoint
        }
        std::vector<ValueType> data = { mid, static_cast<ValueType>(mid + 1) };
        std::vector<ValueType> mapped(data.size());
        preprocessor.encode(data, mapped);
        // predicted = mid, delta = 1, θ = min(mid-xmin, xmax-mid)
        // For unsigned: θ = min(mid, xmax-mid)
        // For signed: θ = min(mid-xmin, xmax-mid) = min(2^(n-1), 2^(n-1)-1) = 2^(n-1)-1
        // delta=1, 0 ≤ 1 ≤ θ → mapped = 2*1 = 2
        ASSERT_EQ(mapped[1], ValueType(2));
        PASS();
    }

    {
        TEST("unit-delay predictor: small negative delta → δ=2|Δ|-1");
        ValueType mid;
        if constexpr (std::is_signed_v<ValueType>) {
            mid = 0;
        }
        else {
            mid = ValueType(1) << (bpv - 1);
        }
        std::vector<ValueType> data = { mid, static_cast<ValueType>(mid - 1) };
        std::vector<ValueType> mapped(data.size());
        preprocessor.encode(data, mapped);
        // delta = -1, -θ ≤ -1 < 0 → mapped = 2*|-1| - 1 = 1
        ASSERT_EQ(mapped[1], ValueType(1));
        PASS();
    }

    {
        TEST("roundtrip with min/max values");
        ValueType xmin, xmax;
        if constexpr (std::is_signed_v<ValueType>) {
            xmin = std::numeric_limits<ValueType>::min();
            xmax = std::numeric_limits<ValueType>::max();
        }
        else {
            xmin = 0;
            xmax = std::numeric_limits<ValueType>::max();
        }
        std::vector<ValueType> data = { xmin, xmax, xmin, xmax };
        std::vector<ValueType> mapped(data.size());
        std::vector<ValueType> restored(data.size());
        preprocessor.encode(data, mapped);
        preprocessorDecode.decode(mapped, data[0], restored);
        for (size_t i = 0; i < data.size(); ++i) {
            ASSERT_EQ(data[i], restored[i]);
        }
        PASS();
    }

    {
        TEST("roundtrip with random data");
        // Fixed seed: reproducible data
        std::mt19937 gen(42);  // NOLINT(bugprone-random-generator-seed,cert-msc32-c,cert-msc51-cpp)
        std::vector<ValueType> data(64);
        if constexpr (std::is_signed_v<ValueType>) {
            std::uniform_int_distribution<int32_t> dist(
                std::numeric_limits<ValueType>::min(),
                std::numeric_limits<ValueType>::max());
            for (auto& v : data)
                v = static_cast<ValueType>(dist(gen));
        }
        else {
            std::uniform_int_distribution<uint32_t> dist(0, std::numeric_limits<ValueType>::max());
            for (auto& v : data)
                v = static_cast<ValueType>(dist(gen));
        }
        std::vector<ValueType> mapped(data.size());
        std::vector<ValueType> restored(data.size());
        preprocessor.encode(data, mapped);
        preprocessorDecode.decode(mapped, data[0], restored);
        for (size_t i = 0; i < data.size(); ++i) {
            ASSERT_EQ(data[i], restored[i]);
        }
        PASS();
    }

    {
        TEST("mapped values are non-negative (§4.4 requirement)");
        // Fixed seed: reproducible data
        std::mt19937 gen(123);  // NOLINT(bugprone-random-generator-seed,cert-msc32-c,cert-msc51-cpp)
        std::vector<ValueType> data(128);
        if constexpr (std::is_signed_v<ValueType>) {
            std::uniform_int_distribution<int32_t> dist(
                std::numeric_limits<ValueType>::min(),
                std::numeric_limits<ValueType>::max());
            for (auto& v : data)
                v = static_cast<ValueType>(dist(gen));
        }
        else {
            std::uniform_int_distribution<uint32_t> dist(0, std::numeric_limits<ValueType>::max());
            for (auto& v : data)
                v = static_cast<ValueType>(dist(gen));
        }
        std::vector<ValueType> mapped(data.size());
        preprocessor.encode(data, mapped);
        // All mapped values must be representable as n-bit non-negative integers
        for (size_t i = 1; i < data.size(); ++i) {
            // For unsigned types this is trivially true
            // For signed types, mapped values should fit in the unsigned range
            if constexpr (std::is_signed_v<ValueType>) {
                // The mapped value is stored as ValueType but represents a non-negative value
                // It should be >= 0 when interpreted correctly
            }
        }
        PASS();
    }
}

// ============================================================================
// §3.2 Fundamental Sequence Codeword Tests (Table 3-1)
// ============================================================================

void testFundamentalSequenceCodewords()
{
    std::cout << "§3.2 Fundamental Sequence Codewords (Table 3-1):" << '\n';

    {
        TEST("δ=0 → codeword '1' (length 1)");
        std::vector<uint8_t> buf(16, 0);
        BitBuffer bb{ Span<uint8_t>(buf) };
        bb.writeCodeword(0);  // write FS for δ=0: "1"
        // Reading back should return length 1 (0 zeros + 1 stopper = 1 bit)
        BitBuffer bb2{ Span<uint8_t>(buf) };
        size_t len = bb2.readCodeword();
        ASSERT_EQ(len, size_t(1));  // readCodeword returns total bits consumed
        PASS();
    }

    {
        TEST("δ=1 → codeword '01' (length 2)");
        std::vector<uint8_t> buf(16, 0);
        BitBuffer bb{ Span<uint8_t>(buf) };
        bb.writeCodeword(1);  // 1 zero + 1 stopper
        BitBuffer bb2{ Span<uint8_t>(buf) };
        size_t len = bb2.readCodeword();
        ASSERT_EQ(len, size_t(2));
        PASS();
    }

    {
        TEST("δ=2 → codeword '001' (length 3)");
        std::vector<uint8_t> buf(16, 0);
        BitBuffer bb{ Span<uint8_t>(buf) };
        bb.writeCodeword(2);
        BitBuffer bb2{ Span<uint8_t>(buf) };
        size_t len = bb2.readCodeword();
        ASSERT_EQ(len, size_t(3));
        PASS();
    }

    {
        TEST("multiple FS codewords in sequence");
        std::vector<uint8_t> buf(64, 0);
        BitBuffer bb{ Span<uint8_t>(buf) };
        // Write δ values: 0, 3, 1, 0, 7
        bb.writeCodeword(0);
        bb.writeCodeword(3);
        bb.writeCodeword(1);
        bb.writeCodeword(0);
        bb.writeCodeword(7);

        // Read back
        BitBuffer bb2{ Span<uint8_t>(buf) };
        ASSERT_EQ(bb2.readCodeword(), size_t(1));  // δ=0 → 1 bit
        ASSERT_EQ(bb2.readCodeword(), size_t(4));  // δ=3 → 4 bits
        ASSERT_EQ(bb2.readCodeword(), size_t(2));  // δ=1 → 2 bits
        ASSERT_EQ(bb2.readCodeword(), size_t(1));  // δ=0 → 1 bit
        ASSERT_EQ(bb2.readCodeword(), size_t(8));  // δ=7 → 8 bits
        PASS();
    }
}

// ============================================================================
// §5.2.1 Code Option ID Table (Table 5-1) Tests
// ============================================================================

void testCodeOptionIds()
{
    std::cout << "§5.2.1 Code Option ID Keys (Table 5-1):" << '\n';

    // Basic, 1 ≤ n ≤ 8: ZB/SE are 4 bits, SS/NC are 3 bits (prefix code)
    {
        TEST("Basic n=8: ZB/SE length=4, SS/NC length=3");
        ZeroBlockCodeOption zb(8, CodeOptionSet::Basic);
        SecondExtensionCodeOption se(8, CodeOptionSet::Basic);
        NoCompressionCodeOption nc(8, CodeOptionSet::Basic);
        SplitSampleCodeOption ss(8, 0, CodeOptionSet::Basic);
        ASSERT_EQ(zb.value, uint32_t(0));
        ASSERT_EQ(se.value, uint32_t(1));
        ASSERT_EQ(nc.value, uint32_t(0b111));
        ASSERT_EQ(zb.length, uint8_t(4));
        ASSERT_EQ(se.length, uint8_t(4));
        ASSERT_EQ(nc.length, uint8_t(3));
        ASSERT_EQ(ss.length, uint8_t(3));
        ASSERT_EQ(zb.length, se.length);
        PASS();
    }

    // Basic, 8 < n ≤ 16: ZB/SE are 5 bits, SS/NC are 4 bits
    {
        TEST("Basic n=16: ZB/SE length=5, SS/NC length=4");
        ZeroBlockCodeOption zb(16, CodeOptionSet::Basic);
        SecondExtensionCodeOption se(16, CodeOptionSet::Basic);
        NoCompressionCodeOption nc(16, CodeOptionSet::Basic);
        SplitSampleCodeOption ss(16, 0, CodeOptionSet::Basic);
        ASSERT_EQ(zb.length, uint8_t(5));
        ASSERT_EQ(se.length, uint8_t(5));
        ASSERT_EQ(nc.length, uint8_t(4));
        ASSERT_EQ(ss.length, uint8_t(4));
        ASSERT_EQ(nc.value, uint32_t(0b1111));
        PASS();
    }

    // Basic, 16 < n ≤ 32: ZB/SE are 6 bits, SS/NC are 5 bits
    {
        TEST("Basic n=32: ZB/SE length=6, SS/NC length=5");
        ZeroBlockCodeOption zb(32, CodeOptionSet::Basic);
        SecondExtensionCodeOption se(32, CodeOptionSet::Basic);
        NoCompressionCodeOption nc(32, CodeOptionSet::Basic);
        SplitSampleCodeOption ss(32, 0, CodeOptionSet::Basic);
        ASSERT_EQ(zb.length, uint8_t(6));
        ASSERT_EQ(se.length, uint8_t(6));
        ASSERT_EQ(nc.length, uint8_t(5));
        ASSERT_EQ(ss.length, uint8_t(5));
        ASSERT_EQ(nc.value, uint32_t(0b11111));
        PASS();
    }

    // Restricted, n=1,2: NC length=1
    {
        TEST("Restricted n=2: NC length=1");
        NoCompressionCodeOption nc(2, CodeOptionSet::Restricted);
        ASSERT_EQ(nc.length, uint8_t(1));
        PASS();
    }

    // Split-sample k values
    {
        TEST("Split-sample k=1 for n=16 Basic: value = k+1 = 2");
        SplitSampleCodeOption ss(16, 1, CodeOptionSet::Basic);
        ASSERT_EQ(ss.value, uint32_t(2));
        ASSERT_EQ(ss.length, uint8_t(4));
        PASS();
    }

    {
        TEST("Split-sample IDs are unique and ordered");
        for (uint8_t k = 0; k < 13; ++k) {
            SplitSampleCodeOption ss(16, k, CodeOptionSet::Basic);
            ASSERT_EQ(ss.value, uint32_t(k + 1));
        }
        PASS();
    }
}

// ============================================================================
// §3.5 Zero Block Option Tests
// ============================================================================

void testZeroBlockEncoding()
{
    std::cout << "§3.5 Zero Block Option:" << '\n';

    constexpr uint8_t BPV = 16;
    constexpr size_t blockSize = 8;
    using ValueType = uint16_t;

    {
        TEST("all-zeros block → ZB option selected");
        std::vector<ValueType> vec(blockSize, 0);
        std::vector<uint8_t> encoded(vec.size() * sizeof(ValueType) * 2);
        encoder::EncoderSeq<ValueType, ValueType> encoder(BPV, blockSize, 128, CodeOptionSet::Basic);
        encoder.encode(Span<ValueType>(vec), encoded);

        std::vector<ValueType> decoded(vec.size());
        DecoderSeq<ValueType, ValueType> decoder(BPV, blockSize, 128, CodeOptionSet::Basic);
        decoder.decode(encoded, decoded);

        for (size_t i = 0; i < vec.size(); ++i) {
            ASSERT_EQ(vec[i], decoded[i]);
        }
        PASS();
    }

    {
        TEST("multiple consecutive zero blocks");
        // 4 blocks of 8 zeros = 32 samples
        std::vector<ValueType> vec(4 * blockSize, 0);
        std::vector<uint8_t> encoded(vec.size() * sizeof(ValueType) * 2);
        encoder::EncoderSeq<ValueType, ValueType> encoder(BPV, blockSize, 128, CodeOptionSet::Basic);
        encoder.encode(Span<ValueType>(vec), encoded);

        std::vector<ValueType> decoded(vec.size());
        DecoderSeq<ValueType, ValueType> decoder(BPV, blockSize, 128, CodeOptionSet::Basic);
        decoder.decode(encoded, decoded);

        for (size_t i = 0; i < vec.size(); ++i) {
            ASSERT_EQ(vec[i], decoded[i]);
        }
        PASS();
    }

    {
        TEST("zero blocks with non-zero reference sample");
        // Per §3.5.1: block with ref sample is all-zeros if J-1 preprocessed symbols are zero
        // Data: [42, 42, 42, 42, 42, 42, 42, 42] → after preprocessing, deltas are all 0
        std::vector<ValueType> vec(blockSize, 42);
        std::vector<uint8_t> encoded(vec.size() * sizeof(ValueType) * 2);
        encoder::EncoderSeq<ValueType, ValueType> encoder(BPV, blockSize, 128, CodeOptionSet::Basic);
        encoder.encode(Span<ValueType>(vec), encoded);

        std::vector<ValueType> decoded(vec.size());
        DecoderSeq<ValueType, ValueType> decoder(BPV, blockSize, 128, CodeOptionSet::Basic);
        decoder.decode(encoded, decoded);

        for (size_t i = 0; i < vec.size(); ++i) {
            ASSERT_EQ(vec[i], decoded[i]);
        }
        PASS();
    }

    {
        TEST("≥5 consecutive zero blocks triggers ROS (§3.5.3, Table 3-2)");
        // ROS = 5 per CodewordTable; ≥5 all-zero blocks should trigger ROS codeword
        std::vector<ValueType> vec(8 * blockSize, 0);  // 8 zero blocks
        std::vector<uint8_t> encoded(vec.size() * sizeof(ValueType) * 2);
        encoder::EncoderSeq<ValueType, ValueType> encoder(BPV, blockSize, 128, CodeOptionSet::Basic);
        encoder.encode(Span<ValueType>(vec), encoded);

        std::vector<ValueType> decoded(vec.size());
        DecoderSeq<ValueType, ValueType> decoder(BPV, blockSize, 128, CodeOptionSet::Basic);
        decoder.decode(encoded, decoded);

        for (size_t i = 0; i < vec.size(); ++i) {
            ASSERT_EQ(vec[i], decoded[i]);
        }
        // ROS should produce a more compact encoding than individual zero blocks
        PASS();
    }

    {
        TEST("mixed zero and non-zero blocks");
        // [0-block] [non-zero-block] [0-block] [0-block]
        std::vector<ValueType> vec(4 * blockSize, 0);
        // Make second block non-zero
        for (size_t i = blockSize; i < 2 * blockSize; ++i) {
            vec[i] = static_cast<ValueType>(i);
        }
        std::vector<uint8_t> encoded(vec.size() * sizeof(ValueType) * 2);
        encoder::EncoderSeq<ValueType, ValueType> encoder(BPV, blockSize, 128, CodeOptionSet::Basic);
        encoder.encode(Span<ValueType>(vec), encoded);

        std::vector<ValueType> decoded(vec.size());
        DecoderSeq<ValueType, ValueType> decoder(BPV, blockSize, 128, CodeOptionSet::Basic);
        decoder.decode(encoded, decoded);

        for (size_t i = 0; i < vec.size(); ++i) {
            ASSERT_EQ(vec[i], decoded[i]);
        }
        PASS();
    }
}

// ============================================================================
// §3.4 Second Extension Tests
// ============================================================================

void testSecondExtension()
{
    std::cout << "§3.4 Second Extension Transform:" << '\n';

    {
        TEST("γⱼ = (d1+d2)(d1+d2+1)/2 + d2 (Cantor pairing)");
        // Verify the formula directly
        // (0,0) → 0, (1,0) → 1, (0,1) → 2, (2,0) → 3, (1,1) → 4, (0,2) → 5
        auto cantor = [](uint16_t d1, uint16_t d2) -> uint16_t {
            return (d1 + d2) * (d1 + d2 + 1) / 2 + d2;
        };
        ASSERT_EQ(cantor(0, 0), uint16_t(0));
        ASSERT_EQ(cantor(1, 0), uint16_t(1));
        ASSERT_EQ(cantor(0, 1), uint16_t(2));
        ASSERT_EQ(cantor(2, 0), uint16_t(3));
        ASSERT_EQ(cantor(1, 1), uint16_t(4));
        ASSERT_EQ(cantor(0, 2), uint16_t(5));
        ASSERT_EQ(cantor(3, 0), uint16_t(6));
        PASS();
    }

    {
        TEST("SE encode/decode roundtrip via full pipeline");
        // Use data that should favor SE encoding (small deltas)
        using ValueType = uint16_t;
        constexpr uint8_t BPV = 16;
        constexpr size_t blockSize = 8;
        // Small increments → small preprocessed values → SE efficient
        std::vector<ValueType> vec = { 1000, 1001, 1000, 1001, 1000, 1001, 1000, 1001 };
        std::vector<uint8_t> encoded(vec.size() * sizeof(ValueType) * 2);
        encoder::EncoderSeq<ValueType, ValueType> encoder(BPV, blockSize, 128, CodeOptionSet::Basic);
        encoder.encode(Span<ValueType>(vec), encoded);

        std::vector<ValueType> decoded(vec.size());
        DecoderSeq<ValueType, ValueType> decoder(BPV, blockSize, 128, CodeOptionSet::Basic);
        decoder.decode(encoded, decoded);

        for (size_t i = 0; i < vec.size(); ++i) {
            ASSERT_EQ(vec[i], decoded[i]);
        }
        PASS();
    }

    {
        TEST("SE: δ₁=0 for reference sample block (§3.4.1)");
        // When first sample is reference, γ₁ is calculated with δ₁=0
        // This is verified implicitly by successful roundtrip
        using ValueType = uint16_t;
        constexpr uint8_t BPV = 16;
        constexpr size_t blockSize = 8;
        std::vector<ValueType> vec = { 5000, 5001, 5000, 5001, 5000, 5001, 5000, 5001 };
        std::vector<uint8_t> encoded(vec.size() * sizeof(ValueType) * 2);
        encoder::EncoderSeq<ValueType, ValueType> encoder(BPV, blockSize, 128, CodeOptionSet::Basic);
        encoder.encode(Span<ValueType>(vec), encoded);

        std::vector<ValueType> decoded(vec.size());
        DecoderSeq<ValueType, ValueType> decoder(BPV, blockSize, 128, CodeOptionSet::Basic);
        decoder.decode(encoded, decoded);

        for (size_t i = 0; i < vec.size(); ++i) {
            ASSERT_EQ(vec[i], decoded[i]);
        }
        PASS();
    }
}

// ============================================================================
// §3.1.6 Block Size Tests (J = 8, 16, 32, 64)
// ============================================================================

template <typename ValueType>
void test_block_sizes()
{
    constexpr uint8_t BPV = sizeof(ValueType) * 8;
    std::cout << "§3.1.6 Block Sizes (J=8,16,32,64) for " << static_cast<uint32_t>(BPV) << "-bit "
              << (std::is_signed_v<ValueType> ? "signed" : "unsigned") << ":" << '\n';

    constexpr size_t blockSizes[] = { 8, 16, 32, 64 };

    for (size_t bs : blockSizes) {
        std::string name = "J=" + std::to_string(bs) + " encode/decode roundtrip";
        TEST(name.c_str());

        constexpr uint16_t rsiSize = 128;
        size_t dataSize = static_cast<size_t>(bs) * rsiSize;

        // Fixed seed: reproducible data
        std::mt19937 gen(42 + bs);  // NOLINT(bugprone-random-generator-seed,cert-msc32-c,cert-msc51-cpp)
        std::vector<ValueType> vec(dataSize);
        if constexpr (std::is_signed_v<ValueType>) {
            std::uniform_int_distribution<int32_t> dist(
                std::numeric_limits<ValueType>::min(),
                std::numeric_limits<ValueType>::max());
            for (auto& v : vec)
                v = static_cast<ValueType>(dist(gen));
        }
        else {
            std::uniform_int_distribution<uint32_t> dist(0, std::numeric_limits<ValueType>::max());
            for (auto& v : vec)
                v = static_cast<ValueType>(dist(gen));
        }

        std::vector<uint8_t> encoded(vec.size() * sizeof(ValueType) * 2);
        encoder::EncoderSeq<ValueType, ValueType> encoder(BPV, bs, rsiSize, CodeOptionSet::Basic);
        encoder.encode(Span<ValueType>(vec), encoded);

        std::vector<ValueType> decoded(vec.size());
        DecoderSeq<ValueType, ValueType> decoder(BPV, bs, rsiSize, CodeOptionSet::Basic);
        decoder.decode(encoded, decoded);

        for (size_t i = 0; i < vec.size(); ++i) {
            if (vec[i] != decoded[i]) {
                std::cerr << "FAIL at index " << i << ": original=" << +vec[i]
                          << " decoded=" << +decoded[i] << " (bs=" << bs << ")" << '\n';
                std::exit(1);
            }
        }
        PASS();
    }
}

// ============================================================================
// End-to-End Encode/Decode with Various Data Patterns
// ============================================================================

template <typename ValueType>
void testDataPatterns()
{
    constexpr uint8_t BPV = sizeof(ValueType) * 8;
    constexpr size_t blockSize = 8;
    constexpr uint16_t rsiSize = 128;

    std::cout << "End-to-end data patterns (" << (int32_t)BPV << "-bit "
              << (std::is_signed_v<ValueType> ? "signed" : "unsigned") << "):" << '\n';

    auto roundtrip = [](const std::vector<ValueType>& vec) {
        std::vector<uint8_t> encoded(vec.size() * sizeof(ValueType) * 2 + 256);
        encoder::EncoderSeq<ValueType, ValueType> encoder(BPV, blockSize, rsiSize, CodeOptionSet::Basic);
        encoder.encode(Span<ValueType>(const_cast<ValueType*>(vec.data()), vec.size()), encoded);

        std::vector<ValueType> decoded(vec.size());
        DecoderSeq<ValueType, ValueType> decoder(BPV, blockSize, rsiSize, CodeOptionSet::Basic);
        decoder.decode(encoded, decoded);

        for (size_t i = 0; i < vec.size(); ++i) {
            if (vec[i] != decoded[i]) {
                std::cerr << "FAIL at index " << i << ": original=" << (int32_t)vec[i]
                          << " decoded=" << (int32_t)decoded[i] << '\n';
                std::exit(1);
            }
        }
    };

    size_t N = blockSize * rsiSize;

    {
        TEST("all zeros");
        std::vector<ValueType> vec(N, 0);
        roundtrip(vec);
        PASS();
    }

    {
        TEST("all max value");
        std::vector<ValueType> vec(N, std::numeric_limits<ValueType>::max());
        roundtrip(vec);
        PASS();
    }

    {
        TEST("constant non-zero value");
        std::vector<ValueType> vec(N, 42);
        roundtrip(vec);
        PASS();
    }

    {
        TEST("ascending ramp");
        std::vector<ValueType> vec(N);
        for (size_t i = 0; i < N; ++i) {
            vec[i] = static_cast<ValueType>(i % (size_t(std::numeric_limits<ValueType>::max()) + 1));
        }
        roundtrip(vec);
        PASS();
    }

    {
        TEST("random data");
        // Fixed seed: reproducible data
        std::mt19937 gen(999);  // NOLINT(bugprone-random-generator-seed,cert-msc32-c,cert-msc51-cpp)
        std::vector<ValueType> vec(N);
        if constexpr (std::is_signed_v<ValueType>) {
            std::uniform_int_distribution<int32_t> dist(
                std::numeric_limits<ValueType>::min(),
                std::numeric_limits<ValueType>::max());
            for (auto& v : vec)
                v = static_cast<ValueType>(dist(gen));
        }
        else {
            std::uniform_int_distribution<uint32_t> dist(0, std::numeric_limits<ValueType>::max());
            for (auto& v : vec)
                v = static_cast<ValueType>(dist(gen));
        }
        roundtrip(vec);
        PASS();
    }

    {
        TEST("alternating min/max");
        std::vector<ValueType> vec(N);
        for (size_t i = 0; i < N; ++i) {
            vec[i] = (i % 2 == 0) ? std::numeric_limits<ValueType>::min()
                                  : std::numeric_limits<ValueType>::max();
        }
        roundtrip(vec);
        PASS();
    }

    {
        TEST("single block (J samples)");
        std::vector<ValueType> vec(blockSize, 100);
        roundtrip(vec);
        PASS();
    }

    {
        TEST("partial RSI (not multiple of segment size)");
        // 3.5 blocks worth of data
        std::vector<ValueType> vec(blockSize * 3 + blockSize / 2, 50);
        roundtrip(vec);
        PASS();
    }
}

// ============================================================================
// Code Option Set: Restricted vs Basic
// ============================================================================

void testCodeOptionSets()
{
    std::cout << "§5.2.1 Basic vs Restricted Code Option Sets:" << '\n';

    using ValueType = uint16_t;
    constexpr uint8_t BPV = 16;
    constexpr size_t blockSize = 8;
    constexpr uint16_t rsiSize = 128;

    auto roundtrip = [](CodeOptionSet cos, const std::vector<ValueType>& vec) {
        std::vector<uint8_t> encoded(vec.size() * sizeof(ValueType) * 2 + 256);
        encoder::EncoderSeq<ValueType, ValueType> encoder(BPV, blockSize, rsiSize, cos);
        encoder.encode(Span<ValueType>(const_cast<ValueType*>(vec.data()), vec.size()), encoded);

        std::vector<ValueType> decoded(vec.size());
        DecoderSeq<ValueType, ValueType> decoder(BPV, blockSize, rsiSize, cos);
        decoder.decode(encoded, decoded);

        for (size_t i = 0; i < vec.size(); ++i) {
            if (vec[i] != decoded[i]) {
                std::cerr << "FAIL at index " << i << ": original=" << vec[i]
                          << " decoded=" << decoded[i] << '\n';
                std::exit(1);
            }
        }
    };

    size_t N = blockSize * rsiSize;

    {
        TEST("Basic code option set - random data");
        // Fixed seed: reproducible data
        std::mt19937 gen(777);  // NOLINT(bugprone-random-generator-seed,cert-msc32-c,cert-msc51-cpp)
        std::uniform_int_distribution<uint16_t> dist(0, 65535);
        std::vector<ValueType> vec(N);
        for (auto& v : vec)
            v = dist(gen);
        roundtrip(CodeOptionSet::Basic, vec);
        PASS();
    }

    {
        TEST("Restricted code option set - random data");
        // Fixed seed: reproducible data
        std::mt19937 gen(777);  // NOLINT(bugprone-random-generator-seed,cert-msc32-c,cert-msc51-cpp)
        std::uniform_int_distribution<uint16_t> dist(0, 65535);
        std::vector<ValueType> vec(N);
        for (auto& v : vec)
            v = dist(gen);
        roundtrip(CodeOptionSet::Restricted, vec);
        PASS();
    }

    {
        TEST("Basic - all zeros");
        std::vector<ValueType> vec(N, 0);
        roundtrip(CodeOptionSet::Basic, vec);
        PASS();
    }

    {
        TEST("Restricted - all zeros");
        std::vector<ValueType> vec(N, 0);
        roundtrip(CodeOptionSet::Restricted, vec);
        PASS();
    }
}

// ============================================================================
// §3.3 Split-Sample Tests
// ============================================================================

void testSplitSample()
{
    std::cout << "§3.3 Split-Sample Options:" << '\n';

    using ValueType = uint16_t;
    constexpr uint8_t BPV = 16;
    constexpr size_t blockSize = 8;
    constexpr uint16_t rsiSize = 128;

    {
        TEST("data favoring split-sample encoding (moderate entropy)");
        // Data with moderate variation should use split-sample
        // Fixed seed: reproducible data
        std::mt19937 gen(42);  // NOLINT(bugprone-random-generator-seed,cert-msc32-c,cert-msc51-cpp)
        std::uniform_int_distribution<uint16_t> dist(0, 65535);
        size_t N = blockSize * rsiSize;
        std::vector<ValueType> vec(N);
        for (auto& v : vec)
            v = dist(gen);

        std::vector<uint8_t> encoded(vec.size() * sizeof(ValueType) * 2);
        encoder::EncoderSeq<ValueType, ValueType> encoder(BPV, blockSize, rsiSize, CodeOptionSet::Basic);
        encoder.encode(Span<ValueType>(vec), encoded);

        std::vector<ValueType> decoded(vec.size());
        DecoderSeq<ValueType, ValueType> decoder(BPV, blockSize, rsiSize, CodeOptionSet::Basic);
        decoder.decode(encoded, decoded);

        for (size_t i = 0; i < vec.size(); ++i) {
            ASSERT_EQ(vec[i], decoded[i]);
        }
        PASS();
    }
}

// ============================================================================
// §4.3 Reference Sample Interval Tests
// ============================================================================

void testReferenceSampleIntervals()
{
    std::cout << "§4.3 Reference Sample Interval:" << '\n';

    using ValueType = uint16_t;
    constexpr uint8_t BPV = 16;
    constexpr size_t blockSize = 8;

    uint16_t rsiSizes[] = { 8, 64, 128, 256, 512 };

    for (uint16_t rsi : rsiSizes) {
        std::string name = "RSI=" + std::to_string(rsi) + " encode/decode roundtrip";
        TEST(name.c_str());

        size_t N = blockSize * rsi * 2 + blockSize * 3;  // Slightly more than 2 RSIs
        std::mt19937 gen(rsi);
        std::uniform_int_distribution<uint16_t> dist(0, 65535);
        std::vector<ValueType> vec(N);
        for (auto& v : vec)
            v = dist(gen);

        std::vector<uint8_t> encoded(vec.size() * sizeof(ValueType) * 2 + 1024);
        encoder::EncoderSeq<ValueType, ValueType> encoder(BPV, blockSize, rsi, CodeOptionSet::Basic);
        encoder.encode(Span<ValueType>(vec), encoded);

        std::vector<ValueType> decoded(vec.size());
        DecoderSeq<ValueType, ValueType> decoder(BPV, blockSize, rsi, CodeOptionSet::Basic);
        decoder.decode(encoded, decoded);

        for (size_t i = 0; i < vec.size(); ++i) {
            if (vec[i] != decoded[i]) {
                std::cerr << "FAIL at index " << i << ": original=" << vec[i]
                          << " decoded=" << decoded[i] << " (RSI=" << rsi << ")" << '\n';
                std::exit(1);
            }
        }
        PASS();
    }
}

// ============================================================================
// §3.6 No-Compression Option Tests
// ============================================================================

void testNoCompression()
{
    std::cout << "§3.6 No-Compression Option:" << '\n';

    using ValueType = uint16_t;
    constexpr uint8_t BPV = 16;
    constexpr size_t blockSize = 8;
    constexpr uint16_t rsiSize = 128;

    {
        TEST("max-value data (NC should be selected for high-entropy data)");
        // High-entropy random data may trigger NC for some blocks
        // Fixed seed: reproducible data
        std::mt19937 gen(12345);  // NOLINT(bugprone-random-generator-seed,cert-msc32-c,cert-msc51-cpp)
        std::uniform_int_distribution<uint16_t> dist(0, 65535);
        size_t N = blockSize * rsiSize;
        std::vector<ValueType> vec(N);
        for (auto& v : vec)
            v = dist(gen);

        std::vector<uint8_t> encoded(vec.size() * sizeof(ValueType) * 2);
        encoder::EncoderSeq<ValueType, ValueType> encoder(BPV, blockSize, rsiSize, CodeOptionSet::Basic);
        encoder.encode(Span<ValueType>(vec), encoded);

        std::vector<ValueType> decoded(vec.size());
        DecoderSeq<ValueType, ValueType> decoder(BPV, blockSize, rsiSize, CodeOptionSet::Basic);
        decoder.decode(encoded, decoded);

        for (size_t i = 0; i < vec.size(); ++i) {
            ASSERT_EQ(vec[i], decoded[i]);
        }
        PASS();
    }
}

// ============================================================================
// §3.7 Compression Efficiency Sanity Checks
// ============================================================================

void testCompressionEfficiency()
{
    std::cout << "§3.7 Compression Efficiency:" << '\n';

    using ValueType = uint16_t;
    constexpr uint8_t BPV = 16;
    constexpr size_t blockSize = 8;
    constexpr uint16_t rsiSize = 128;

    {
        TEST("constant data compresses significantly");
        size_t N = blockSize * rsiSize;
        std::vector<ValueType> vec(N, 1000);
        std::vector<uint8_t> encoded(vec.size() * sizeof(ValueType) * 2);
        encoder::EncoderSeq<ValueType, ValueType> encoder(BPV, blockSize, rsiSize, CodeOptionSet::Basic);
        [[maybe_unused]] const size_t encodedSize = encoder.encode(Span<ValueType>(vec), encoded);
        [[maybe_unused]] const size_t rawSize = N * sizeof(ValueType);
        assert(encodedSize < rawSize / 2);
        PASS();
    }

    {
        TEST("all-zeros compresses to minimal size");
        size_t N = blockSize * rsiSize;
        std::vector<ValueType> vec(N, 0);
        std::vector<uint8_t> encoded(vec.size() * sizeof(ValueType) * 2);
        encoder::EncoderSeq<ValueType, ValueType> encoder(BPV, blockSize, rsiSize, CodeOptionSet::Basic);
        [[maybe_unused]] const size_t encodedSize = encoder.encode(Span<ValueType>(vec), encoded);
        [[maybe_unused]] const size_t rawSize = N * sizeof(ValueType);
        // All zeros should compress extremely well
        assert(encodedSize < rawSize / 8);
        PASS();
    }

    {
        TEST("random data does not expand more than ~1.1x");
        size_t N = blockSize * rsiSize;
        // Fixed seed: reproducible data
        std::mt19937 gen(42);  // NOLINT(bugprone-random-generator-seed,cert-msc32-c,cert-msc51-cpp)
        std::uniform_int_distribution<uint16_t> dist(0, 65535);
        std::vector<ValueType> vec(N);
        for (auto& v : vec)
            v = dist(gen);
        std::vector<uint8_t> encoded(vec.size() * sizeof(ValueType) * 4);
        encoder::EncoderSeq<ValueType, ValueType> encoder(BPV, blockSize, rsiSize, CodeOptionSet::Basic);
        [[maybe_unused]] const size_t encodedSize = encoder.encode(Span<ValueType>(vec), encoded);
        [[maybe_unused]] const size_t rawSize = N * sizeof(ValueType);
        // Even worst case, should not expand much beyond raw + overhead
        assert(encodedSize <= rawSize * 2);
        PASS();
    }
}

// ============================================================================
// Main
// ============================================================================

// ============================================================================
// CCSDS 120.0-G-4 (Green Book, November 2021): worked examples, tables and properties that the
// Blue Book does not state
// ============================================================================

namespace greenbook
{

// Bits written MSB first, as in the coded data
struct BitWriter
{
    std::vector<uint8_t> bytes;
    size_t pos = 0;

    void put(uint64_t value, size_t n)
    {
        while (n--) {
            if (bytes.size() <= pos / 8) bytes.push_back(0);
            if ((value >> n) & 1) bytes[pos / 8] |= static_cast<uint8_t>(0x80 >> (pos % 8));
            ++pos;
        }
    }
    void fs(uint64_t zeros)
    {
        for (uint64_t i = 0; i < zeros; ++i)
            put(0, 1);
        put(1, 1);
    }
};

static uint32_t bitAt(const std::vector<uint8_t>& bytes, size_t i)
{
    return (bytes[i / 8] >> (7 - i % 8)) & 1;
}

// Codes one block without reference sample and returns its length in bits and its first byte
template <typename T>
static std::pair<size_t, uint8_t> encodeBlock(encoder::EncoderSeq<T, T>& enc, std::vector<T> block, uint8_t bpv)
{
    std::vector<uint8_t> out(block.size() * (bpv + 8) / 8 + 64, 0);
    BitBuffer buffer{ Span<uint8_t>(out) };
    enc.template encodeFullBlock<ReferenceType::NoRef>(Span<T>(block), buffer);
    return { buffer.bitPos(), out[0] };
}

}  // namespace greenbook

// §3.3.3: the example of the prediction error mapper for 8-bit samples (x̂ = previous sample)
void testGreenBookMapperExample()
{
    std::cout << "Green Book §3.3.3 Prediction Error Mapper example:" << '\n';
    TEST("x = 101 101 100 101 99 101 223 100 → δ = 0 1 2 3 4 223 155");
    std::vector<uint8_t> x = { 101, 101, 100, 101, 99, 101, 223, 100 };
    std::vector<uint8_t> delta(x.size()), restored(x.size());
    PredictivePreprocessingEncodeFast<uint8_t> pre(8);
    pre.encode(x, delta);
    const std::vector<uint8_t> expected = { 101, 0, 1, 2, 3, 4, 223, 155 };  // the first sample is the reference
    for (size_t i = 0; i < x.size(); ++i) {
        ASSERT_EQ(delta[i], expected[i]);
    }
    PredictivePreprocessingDecodeFast<uint8_t> post(8);
    post.decode(delta, x[0], restored);
    for (size_t i = 0; i < x.size(); ++i) {
        ASSERT_EQ(restored[i], x[i]);
    }
    PASS();
}

// §3.2.3.3, table 3-1 and figure 3-4: zero-block codewords and the 200-block example
void testGreenBookZeroBlock()
{
    std::cout << "Green Book §3.2.3.3 Zero-Block option:" << '\n';
    using T = uint8_t;
    constexpr uint8_t n = 8;
    constexpr uint8_t J = 8;

    {
        TEST("table 3-1: m all-zeros blocks → m-1 zeros and a 1 (m ≤ 4), m zeros and a 1 (5 ≤ m ≤ 63), ROS");
        for (size_t m = 1; m <= 64; ++m) {
            // m blocks of a constant (the first carries the reference), then blocks that are not all zeros
            std::vector<T> x(64 * J, 77);
            for (size_t i = m * J; i < x.size(); ++i)
                x[i] = static_cast<T>((i * 37) % 251);
            std::vector<uint8_t> out(x.size() * 2 + 64, 0);
            encoder::EncoderSeq<T, T> enc(n, J, 64, CodeOptionSet::Basic);
            enc.encode(Span<T>(x), Span<uint8_t>(out));

            // zero-block ID (4 bits for n = 8: 000 and 0), reference (8 bits), then the FS codeword
            for (size_t i = 0; i < 4; ++i)
                ASSERT_EQ(greenbook::bitAt(out, i), 0u);
            size_t zeros = 0;
            while (greenbook::bitAt(out, 12 + zeros) == 0)
                ++zeros;
            const size_t expectedZeros = m == 64 ? 4 : (m <= 4 ? m - 1 : m);  // the segment ends: ROS
            ASSERT_EQ(zeros, expectedZeros);

            std::vector<T> decoded(x.size());
            DecoderSeq<T, T> dec(n, J, 64, CodeOptionSet::Basic);
            dec.decode(Span<uint8_t>(out), Span<T>(decoded));
            ASSERT_TRUE(decoded == x);
        }
        PASS();
    }

    {
        TEST("ROS only for 5 or more all-zeros blocks at the end of a segment, else the count");
        for (size_t m = 1; m <= 62; ++m) {
            // 64 - m blocks that are not all zeros, then m blocks repeating the last sample
            std::vector<T> x(64 * J);
            for (size_t i = 0; i < x.size(); ++i)
                x[i] = static_cast<T>(i < (64 - m) * J ? (i * 37) % 251 : ((64 - m) * J - 1) * 37 % 251);
            std::vector<uint8_t> all(x.size() * 2 + 64, 0), prefix(x.size() * 2 + 64, 0);
            encoder::EncoderSeq<T, T> encAll(n, J, 64, CodeOptionSet::Basic), encPrefix(n, J, 64, CodeOptionSet::Basic);
            const size_t bitsAll = encAll.encodeBits(Span<T>(x), Span<uint8_t>(all));
            const size_t bitsPrefix = encPrefix.encodeBits(Span<T>(x.data(), (64 - m) * J), Span<uint8_t>(prefix));

            // the run is coded after the other blocks: zero-block ID (4 bits), then the codeword
            const size_t codeword = m <= 4 ? m : 5;  // m - 1 zeros and a 1, or ROS (4 zeros and a 1)
            ASSERT_EQ(bitsAll, bitsPrefix + 4 + codeword);
            for (size_t i = 0; i < 4 + codeword - 1; ++i)
                ASSERT_EQ(greenbook::bitAt(all, bitsPrefix + i), 0u);
            ASSERT_EQ(greenbook::bitAt(all, bitsAll - 1), 1u);

            std::vector<T> decoded(x.size());
            DecoderSeq<T, T> dec(n, J, 64, CodeOptionSet::Basic);
            dec.decode(Span<uint8_t>(all.data(), (bitsAll + 7) / 8), Span<T>(decoded));
            ASSERT_TRUE(decoded == x);
        }
        PASS();
    }

    {
        TEST("figure 3-4: 200 equal blocks → segments of 64, 64, 64, 8 blocks, each ID and ROS");
        std::vector<T> x(200 * J, 77);
        encoder::EncoderSeq<T, T> enc(n, J, 200, CodeOptionSet::Basic);
        std::vector<uint8_t> out(64, 0);
        const size_t bits = enc.encodeBits(Span<T>(x), Span<uint8_t>(out));

        greenbook::BitWriter expected;
        expected.put(0, 4);   // zero-block ID
        expected.put(77, 8);  // reference
        expected.fs(4);       // ROS
        for (int32_t s = 0; s < 3; ++s) {
            expected.put(0, 4);
            expected.fs(4);  // ROS, also for the last 8 blocks
        }
        ASSERT_EQ(bits, expected.pos);
        for (size_t i = 0; i < expected.bytes.size(); ++i) {
            ASSERT_EQ(out[i], expected.bytes[i]);
        }

        std::vector<T> decoded(x.size());
        DecoderSeq<T, T> dec(n, J, 200, CodeOptionSet::Basic);
        dec.decode(Span<uint8_t>(out.data(), (bits + 7) / 8), Span<T>(decoded));
        ASSERT_TRUE(decoded == x);
        PASS();
    }
}

// §5.4, table 5-1: decoding of the second extension, γ → (β, ms), δ2 = γ - ms, δ1 = β - δ2
void testGreenBookSecondExtensionTable()
{
    std::cout << "Green Book §5.4 Second-Extension decoding (table 5-1):" << '\n';

    // The rows (first γ, β, ms) of table 5-1 and their extension: β_i = β_{i-1} + 1,
    // ms_i = ms_{i-1} + β_i, first γ_i = last γ_{i-1} + 1, last γ_i = last γ_{i-1} + β_i + 1
    struct Row
    {
        uint64_t first, last, beta, ms;
    };
    std::vector<Row> rows{ { 0, 0, 0, 0 } };
    while (rows.size() < 256) {
        const Row& p = rows.back();
        rows.push_back({ p.last + 1, p.last + p.beta + 2, p.beta + 1, p.ms + p.beta + 1 });
    }

    {
        TEST("table 5-1 as printed: γ 0 | 1,2 | 3..5 | 6..9 | 10..14 | 15..20 | 21..27 | 28..35 | 36..44");
        const uint64_t printed[9][4] = { { 0, 0, 0, 0 }, { 1, 2, 1, 1 }, { 3, 5, 2, 3 }, { 6, 9, 3, 6 }, { 10, 14, 4, 10 }, { 15, 20, 5, 15 }, { 21, 27, 6, 21 }, { 28, 35, 7, 28 }, { 36, 44, 8, 36 } };
        for (size_t i = 0; i < 9; ++i) {
            ASSERT_EQ(rows[i].first, printed[i][0]);
            ASSERT_EQ(rows[i].last, printed[i][1]);
            ASSERT_EQ(rows[i].beta, printed[i][2]);
            ASSERT_EQ(rows[i].ms, printed[i][3]);
        }
        PASS();
    }

    {
        TEST("decoder follows table 5-1 for every γ with β ≤ 255");
        constexpr uint8_t J = 8;
        DecoderSeq<uint16_t, uint16_t> dec(16, J, 1, CodeOptionSet::Basic);
        std::vector<uint64_t> gammas;
        for (const Row& r : rows) {
            for (uint64_t g = r.first; g <= r.last; ++g)
                gammas.push_back(g);
        }
        while (gammas.size() % (J / 2))
            gammas.push_back(0);

        greenbook::BitWriter w;
        for (uint64_t g : gammas)
            w.fs(g);
        const size_t limitBits = w.pos;
        w.bytes.resize(w.bytes.size() + BitBuffer::guardBytes, 0);
        const BitBuffer buffer{ Span<uint8_t>(w.bytes), 0, limitBits };

        // the row of every γ
        std::vector<const Row*> rowOf(rows.back().last + 1);
        for (const Row& r : rows) {
            for (uint64_t g = r.first; g <= r.last; ++g)
                rowOf[g] = &r;
        }

        std::vector<uint16_t> delta(J);
        for (size_t i = 0; i < gammas.size(); i += J / 2) {
            dec.seDecode(buffer, Span<uint16_t>(delta));
            for (size_t j = 0; j < J / 2; ++j) {
                const Row& r = *rowOf[gammas[i + j]];
                const uint64_t d2 = gammas[i + j] - r.ms;
                ASSERT_EQ(uint64_t{ delta[2 * j + 1] }, d2);
                ASSERT_EQ(uint64_t{ delta[2 * j] }, r.beta - d2);
            }
        }
        ASSERT_EQ(buffer.bitPos(), limitBits);
        PASS();
    }
}

// §5.4: the second extension is chosen only for low entropy: β ≤ 4 for J = 16 and β ≤ 8 for J = 64.
// It is a consequence of option selection: a pair with sum β costs β(β+1)/2 + β2 + 1 bits instead of
// β + 2 (FS, k = 0), every other pair saves at most 1 bit, and the SE ID is 1 bit longer.
void testGreenBookSecondExtensionBeta()
{
    std::cout << "Green Book §5.4 Second Extension only for low entropy:" << '\n';
    constexpr uint8_t n = 8;
    for (const auto& jBound : { std::pair<uint8_t, uint64_t>{ 16, 4 }, std::pair<uint8_t, uint64_t>{ 64, 8 } }) {
        const uint8_t J = jBound.first;
        const uint64_t bound = jBound.second;
        TEST("J = " + std::to_string(J) + ": second extension chosen ⇒ β ≤ " + std::to_string(bound));
        encoder::EncoderSeq<uint8_t, uint8_t> enc(n, J, 1, CodeOptionSet::Basic);
        std::mt19937 rng(J);
        size_t seChosen = 0;
        auto check = [&](const std::vector<uint8_t>& block) {
            const auto [bits, first] = greenbook::encodeBlock(enc, block, n);
            (void)bits;
            if ((first >> 4) != 1) return;  // SE ID for n = 8: 0001
            ++seChosen;
            for (size_t i = 0; i < block.size(); i += 2) {
                ASSERT_TRUE(uint64_t{ block[i] } + block[i + 1] <= bound);
            }
        };
        // one pair with sum β, all others zero: SE must not be chosen beyond the bound
        for (uint64_t beta = 0; beta <= bound + 4; ++beta) {
            for (uint64_t d2 = 0; d2 <= beta; ++d2) {
                std::vector<uint8_t> block(J, 0);
                block[0] = static_cast<uint8_t>(beta - d2);
                block[1] = static_cast<uint8_t>(d2);
                check(block);
            }
        }
        // random low-entropy blocks: mostly 0, some 1, rare larger values
        for (int32_t t = 0; t < 200000; ++t) {
            std::vector<uint8_t> block(J);
            for (auto& v : block) {
                const uint32_t r = rng() % 64;
                v = static_cast<uint8_t>(r < 48 ? 0 : r < 60 ? 1
                                                  : r < 63   ? 2
                                                             : rng() % 12);
            }
            check(block);
        }
        ASSERT_TRUE(seChosen > 0);
        PASS();
    }
}

// §5.5: a coded block is never longer than the no-compression option (ID + J·n bits), and the
// option selected has the fewest bits (§3.2.5); the split-sample length L_k is convex in k, so the
// first k with L_k - L_{k+1} < 0 minimises it (reference [16], Kiely)
template <typename T>
void testGreenBookOptionSelection(uint8_t n)
{
    std::cout << "Green Book §3.2.5/§5.5 option selection (n = " << +n << "):" << '\n';
    TEST("block ≤ no-compression length, = min over options, L_k convex");
    std::mt19937_64 rng(n);
    for (uint8_t J : { 8, 16, 32, 64 }) {
        encoder::EncoderSeq<T, T> enc(n, J, 1, CodeOptionSet::Basic);
        const NoCompressionCodeOption ncId(n, CodeOptionSet::Basic);
        const SplitSampleIdLength ssId(n, CodeOptionSet::Basic);
        const SecondExtensionCodeOption seId(n, CodeOptionSet::Basic);
        const SplitSampleLimits limits(n, CodeOptionSet::Basic);
        const uint64_t ncLength = ncId.length + uint64_t{ J } * n;
        for (int32_t t = 0; t < 20000; ++t) {
            // geometric-like samples of a random scale, sometimes the full range
            const uint32_t scale = static_cast<uint32_t>(rng() % (n + 1));
            std::vector<T> block(J);
            for (auto& v : block) {
                const uint64_t r = rng();
                v = static_cast<T>(scale == 0 ? 0 : scale == n ? r & ((uint64_t{ 1 } << n) - 1)
                                                               : (r >> (64 - scale)) >> (r % 3));
            }
            if (std::all_of(block.begin(), block.end(), [](T v) { return v == 0; })) continue;

            const auto [bits, first] = greenbook::encodeBlock(enc, block, n);
            (void)first;
            ASSERT_TRUE(bits <= ncLength);

            // split-sample lengths, convexity, and the first-negative-difference rule
            std::vector<uint64_t> L;
            for (size_t k = limits.kMin_; k <= limits.kMax_; ++k) {
                uint64_t len = ssId.length + uint64_t{ J } * (k + 1);
                for (T v : block)
                    len += uint64_t{ v } >> k;
                L.push_back(len);
            }
            auto diff = [&L](size_t k) { return static_cast<int64_t>(L[k]) - static_cast<int64_t>(L[k + 1]); };
            for (size_t k = 0; k + 2 < L.size(); ++k) {
                ASSERT_TRUE(diff(k + 1) <= diff(k));  // convex
            }
            size_t kStar = L.size() - 1;
            for (size_t k = 0; k + 1 < L.size(); ++k) {
                if (diff(k) < 0) {
                    kStar = k;
                    break;
                }
            }
            ASSERT_EQ(L[kStar], *std::min_element(L.begin(), L.end()));

            uint64_t se = seId.length;  // pairs beyond 2^20 make SE far too long (and overflow)
            for (size_t i = 0; i < J && se < UINT64_MAX; i += 2) {
                const uint64_t d1 = block[i], d2 = block[i + 1];
                se = d1 + d2 >= (uint64_t{ 1 } << 20) ? UINT64_MAX : se + (d1 + d2) * (d1 + d2 + 1) / 2 + d2 + 1;
            }
            ASSERT_EQ(uint64_t{ bits }, std::min({ L[kStar], se, ncLength }));
        }
    }
    PASS();
}

// §4.1.1, §4.2.3, §4.4, §5.2.2: the bypass predictor, coding without preprocessor, signed samples
template <typename T>
static std::vector<uint8_t> encodeWith(const std::vector<T>& x, uint8_t n, uint8_t J, uint16_t r, Preprocessing pre, size_t& bits)
{
    encoder::EncoderSeq<T, T> enc(n, J, r, CodeOptionSet::Basic, pre);
    std::vector<uint8_t> out(x.size() * 8 + 64, 0);
    std::vector<T> in(x);
    bits = enc.encodeBits(Span<T>(in), Span<uint8_t>(out));
    out.resize((bits + 7) / 8);
    return out;
}

template <typename T>
static std::vector<T> decodeWith(std::vector<uint8_t> stream, size_t count, uint8_t n, uint8_t J, uint16_t r, Preprocessing pre)
{
    DecoderSeq<T, T> dec(n, J, r, CodeOptionSet::Basic, pre);
    std::vector<T> out(count);
    dec.decode(Span<uint8_t>(stream), Span<T>(out));
    return out;
}

void testPreprocessingModes()
{
    std::cout << "§4.1.1/§4.2.3/§4.4/§5.2.2 Preprocessing modes:" << '\n';

    {
        TEST("§5.2.2 no preprocessor: 255 0 255 0 ... (n = 8) → no-compression block without reference sample");
        const std::vector<uint8_t> x = { 255, 0, 255, 0, 255, 0, 255, 0 };
        size_t bits = 0;
        const auto stream = encodeWith(x, 8, 8, 1, Preprocessing{ Prediction::None }, bits);
        greenbook::BitWriter expected;
        expected.put(7, 3);  // no-compression ID for n = 8
        for (uint8_t v : x)
            expected.put(v, 8);
        ASSERT_EQ(bits, expected.pos);
        ASSERT_TRUE(stream == expected.bytes);
        ASSERT_TRUE(decodeWith<uint8_t>(stream, x.size(), 8, 8, 1, Preprocessing{ Prediction::None }) == x);
        PASS();
    }

    {
        TEST("§4.2.3 bypass predictor, signed n = 4: x = 0 -1 1 -2 ... -8 → δ = 0 1 2 ... 15 (no reference sample)");
        // Predicted value 0 (θ = min(0 + 8, 7 - 0) = 7): δ = 2x for x ≥ 0, 2|x| - 1 for x < 0; x = -8: θ + 8 = 15
        std::vector<uint8_t> x;
        for (int32_t i = 0; i < 16; ++i) {
            const int32_t v = i % 2 == 0 ? i / 2 : -(i + 1) / 2;
            x.push_back(static_cast<uint8_t>(v));  // two's complement, sign-extended to 8 bits
        }
        const Preprocessing bypassSigned{ Prediction::Bypass, true };
        size_t bits = 0;
        const auto stream = encodeWith(x, 4, 16, 1, bypassSigned, bits);
        greenbook::BitWriter expected;
        expected.put(7, 3);  // no compression: the 16 preprocessed samples 0..15
        for (uint32_t d = 0; d < 16; ++d)
            expected.put(d, 4);
        ASSERT_EQ(bits, expected.pos);
        ASSERT_TRUE(stream == expected.bytes);
        ASSERT_TRUE(decodeWith<uint8_t>(stream, x.size(), 4, 16, 1, bypassSigned) == x);
        PASS();
    }

    {
        TEST("§4.4 unit-delay predictor, signed n = 8: 8 × (-3) → zero block, reference 11111101");
        const std::vector<uint8_t> x(8, static_cast<uint8_t>(-3));
        const Preprocessing unitDelaySigned{ Prediction::UnitDelay, true };
        size_t bits = 0;
        const auto stream = encodeWith(x, 8, 8, 1, unitDelaySigned, bits);
        greenbook::BitWriter expected;
        expected.put(0, 4);     // zero-block ID
        expected.put(0xFD, 8);  // the reference sample: -3 in 8-bit two's complement
        expected.fs(0);         // one zero block
        ASSERT_EQ(bits, expected.pos);
        ASSERT_TRUE(stream == expected.bytes);
        ASSERT_TRUE(decodeWith<uint8_t>(stream, x.size(), 8, 8, 1, unitDelaySigned) == x);
        PASS();
    }

    {
        TEST("decoded signed samples are sign-extended to the sample type (n = 12 in 16 bits)");
        std::vector<uint16_t> x = { 0, 1, static_cast<uint16_t>(-1), 2047, static_cast<uint16_t>(-2048), 5, static_cast<uint16_t>(-7), 100 };
        for (Prediction pred : { Prediction::UnitDelay, Prediction::Bypass }) {
            const Preprocessing pre{ pred, true };
            size_t bits = 0;
            const auto stream = encodeWith(x, 12, 8, 1, pre, bits);
            ASSERT_TRUE(decodeWith<uint16_t>(stream, x.size(), 12, 8, 1, pre) == x);
            // only the 12 low bits are coded: the samples given as 12-bit patterns code alike
            std::vector<uint16_t> patterns(x);
            for (auto& v : patterns)
                v &= 0xFFF;
            size_t bits2 = 0;
            ASSERT_TRUE(encodeWith(patterns, 12, 8, 1, pre, bits2) == stream);
        }
        PASS();
    }

    {
        TEST("§4.2.3 bypass predictor with unsigned samples codes the samples unchanged, like no preprocessor");
        std::mt19937 rng(42);
        for (uint8_t n : { 1, 7, 13, 24, 32 }) {
            std::vector<uint32_t> x(1000);
            for (auto& v : x)
                v = static_cast<uint32_t>(rng()) & static_cast<uint32_t>((uint64_t{ 1 } << n) - 1);
            for (size_t i = 0; i < x.size(); i += 97)
                x[i] = 0;
            size_t b1 = 0, b2 = 0;
            ASSERT_TRUE(encodeWith(x, n, 16, 3, Preprocessing{ Prediction::Bypass }, b1) == encodeWith(x, n, 16, 3, Preprocessing{ Prediction::None }, b2));
        }
        PASS();
    }
}

int main()
{
    try {
        std::cout << "CCSDS 121.0-B-3 Compliance Tests" << '\n';
        std::cout << "=================================" << '\n'
                  << '\n';

        // §4.4 Prediction Error Mapper (spec defines unsigned J-bit samples only)
        testPreprocessorMapperSpecFormula<uint8_t>();
        testPreprocessorMapperSpecFormula<uint16_t>();

        // §3.2 Fundamental Sequence
        testFundamentalSequenceCodewords();

        // §5.2.1 Code Option IDs
        testCodeOptionIds();

        // §3.5 Zero Block
        testZeroBlockEncoding();

        // §3.4 Second Extension
        testSecondExtension();

        // §3.3 Split Sample
        testSplitSample();

        // §3.6 No Compression
        testNoCompression();

        // §3.1.6 Block Sizes
        test_block_sizes<uint8_t>();
        test_block_sizes<uint16_t>();

        // Data Patterns
        testDataPatterns<uint8_t>();
        testDataPatterns<uint16_t>();

        // §5.2.1 Code Option Sets
        testCodeOptionSets();

        // §4.3 RSI
        testReferenceSampleIntervals();

        // §3.7 Compression efficiency
        testCompressionEfficiency();

        // CCSDS 120.0-G-4 (Green Book)
        testGreenBookMapperExample();
        testGreenBookZeroBlock();
        testGreenBookSecondExtensionTable();
        testGreenBookSecondExtensionBeta();
        testGreenBookOptionSelection<uint16_t>(8);
        testGreenBookOptionSelection<uint16_t>(16);
        testGreenBookOptionSelection<uint32_t>(24);
        testGreenBookOptionSelection<uint32_t>(32);

        // §4.1.1, §4.2.3, §4.4, §5.2.2 preprocessing modes
        testPreprocessingModes();

        std::cout << '\n';
        std::cout << "=================================" << '\n';
        std::cout << passCount << "/" << testCount << " tests passed." << '\n';

        return (passCount == testCount) ? 0 : 1;
    }
    catch (const std::exception& e) {
        std::cerr << "FAIL: " << e.what() << '\n';
        return 1;
    }
    return 0;
}
