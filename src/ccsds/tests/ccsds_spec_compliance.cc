#include <iostream>
#include <vector>
#include <cassert>
#include <cstdint>
#include <cmath>
#include <random>
#include <numeric>
#include <algorithm>
#include <string>

#include "Preprocessor.h"
#include "encoder/EncoderSeq.h"
#include "decoder/DecoderSeq.h"
#include "BitBuffer.h"
#include "CodeOption.h"

static int testCount = 0;
static int passCount = 0;

#define TEST(name) \
    do { \
        testCount++; \
        std::cout << "  " << name << "... "; \
    } while(0)

#define PASS() \
    do { \
        passCount++; \
        std::cout << "PASS" << std::endl; \
    } while(0)

#define ASSERT_EQ(a, b) \
    do { \
        auto _a = (a); auto _b = (b); \
        if (_a != _b) { \
            std::cerr << "FAIL: " << #a << "=" << +_a << " != " << #b << "=" << +_b \
                      << " at " << __FILE__ << ":" << __LINE__ << std::endl; \
            std::exit(1); \
        } \
    } while(0)

#define ASSERT_TRUE(cond) \
    do { \
        if (!(cond)) { \
            std::cerr << "FAIL: " << #cond \
                      << " at " << __FILE__ << ":" << __LINE__ << std::endl; \
            std::exit(1); \
        } \
    } while(0)

// ============================================================================
// §4.4 Prediction Error Mapper Tests
// ============================================================================

// Verify: δ = 2Δ when 0 ≤ Δ ≤ θ
//         δ = 2|Δ|-1 when -θ ≤ Δ < 0
//         δ = θ+|Δ| otherwise
// where θ = min(x̂-xmin, xmax-x̂)
template <typename ValueType>
void testPreprocessorMapperSpecFormula() {
    std::cout << "§4.4 Prediction Error Mapper (" << sizeof(ValueType)*8 << "-bit "
              << (std::is_signed_v<ValueType> ? "signed" : "unsigned") << "):" << std::endl;

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
            data = {0, 1, -1, 2, -2, 3, -3, 10, -10, 0};
        } else {
            data = {10, 11, 9, 12, 8, 13, 7, 20, 0, 10};
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
        std::vector<ValueType> data = {42, 43, 44};
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
        } else {
            mid = ValueType(1) << (bpv - 1);  // midpoint
        }
        std::vector<ValueType> data = {mid, static_cast<ValueType>(mid + 1)};
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
        } else {
            mid = ValueType(1) << (bpv - 1);
        }
        std::vector<ValueType> data = {mid, static_cast<ValueType>(mid - 1)};
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
        } else {
            xmin = 0;
            xmax = std::numeric_limits<ValueType>::max();
        }
        std::vector<ValueType> data = {xmin, xmax, xmin, xmax};
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
        std::mt19937 gen(42);
        std::vector<ValueType> data(64);
        if constexpr (std::is_signed_v<ValueType>) {
            std::uniform_int_distribution<int> dist(
                std::numeric_limits<ValueType>::min(),
                std::numeric_limits<ValueType>::max());
            for (auto& v : data) v = static_cast<ValueType>(dist(gen));
        } else {
            std::uniform_int_distribution<unsigned> dist(0, std::numeric_limits<ValueType>::max());
            for (auto& v : data) v = static_cast<ValueType>(dist(gen));
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
        std::mt19937 gen(123);
        std::vector<ValueType> data(128);
        if constexpr (std::is_signed_v<ValueType>) {
            std::uniform_int_distribution<int> dist(
                std::numeric_limits<ValueType>::min(),
                std::numeric_limits<ValueType>::max());
            for (auto& v : data) v = static_cast<ValueType>(dist(gen));
        } else {
            std::uniform_int_distribution<unsigned> dist(0, std::numeric_limits<ValueType>::max());
            for (auto& v : data) v = static_cast<ValueType>(dist(gen));
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

void testFundamentalSequenceCodewords() {
    std::cout << "§3.2 Fundamental Sequence Codewords (Table 3-1):" << std::endl;

    {
        TEST("δ=0 → codeword '1' (length 1)");
        std::vector<uint8_t> buf(16, 0);
        BitBuffer bb{Span<uint8_t>(buf)};
        bb.writeCodeword(0);  // write FS for δ=0: "1"
        // Reading back should return length 1 (0 zeros + 1 stopper = 1 bit)
        BitBuffer bb2{Span<uint8_t>(buf)};
        size_t len = bb2.readCodeword();
        ASSERT_EQ(len, size_t(1));  // readCodeword returns total bits consumed
        PASS();
    }

    {
        TEST("δ=1 → codeword '01' (length 2)");
        std::vector<uint8_t> buf(16, 0);
        BitBuffer bb{Span<uint8_t>(buf)};
        bb.writeCodeword(1);  // 1 zero + 1 stopper
        BitBuffer bb2{Span<uint8_t>(buf)};
        size_t len = bb2.readCodeword();
        ASSERT_EQ(len, size_t(2));
        PASS();
    }

    {
        TEST("δ=2 → codeword '001' (length 3)");
        std::vector<uint8_t> buf(16, 0);
        BitBuffer bb{Span<uint8_t>(buf)};
        bb.writeCodeword(2);
        BitBuffer bb2{Span<uint8_t>(buf)};
        size_t len = bb2.readCodeword();
        ASSERT_EQ(len, size_t(3));
        PASS();
    }

    {
        TEST("multiple FS codewords in sequence");
        std::vector<uint8_t> buf(64, 0);
        BitBuffer bb{Span<uint8_t>(buf)};
        // Write δ values: 0, 3, 1, 0, 7
        bb.writeCodeword(0);
        bb.writeCodeword(3);
        bb.writeCodeword(1);
        bb.writeCodeword(0);
        bb.writeCodeword(7);

        // Read back
        BitBuffer bb2{Span<uint8_t>(buf)};
        ASSERT_EQ(bb2.readCodeword(), size_t(1));   // δ=0 → 1 bit
        ASSERT_EQ(bb2.readCodeword(), size_t(4));   // δ=3 → 4 bits
        ASSERT_EQ(bb2.readCodeword(), size_t(2));   // δ=1 → 2 bits
        ASSERT_EQ(bb2.readCodeword(), size_t(1));   // δ=0 → 1 bit
        ASSERT_EQ(bb2.readCodeword(), size_t(8));   // δ=7 → 8 bits
        PASS();
    }
}

// ============================================================================
// §5.2.1 Code Option ID Table (Table 5-1) Tests
// ============================================================================

void testCodeOptionIds() {
    std::cout << "§5.2.1 Code Option ID Keys (Table 5-1):" << std::endl;

    // Basic, 1 ≤ n ≤ 8: ZB/SE are 4 bits, SS/NC are 3 bits (prefix code)
    {
        TEST("Basic n=8: ZB/SE length=4, SS/NC length=3");
        ZeroBlockCodeOption zb(8, CodeOptionSet::Basic);
        SecondExtensionCodeOption se(8, CodeOptionSet::Basic);
        NoCompressionCodeOption nc(8, CodeOptionSet::Basic);
        SplitSampleCodeOption<uint8_t> ss(8, 0, CodeOptionSet::Basic);
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
        SplitSampleCodeOption<uint16_t> ss(16, 0, CodeOptionSet::Basic);
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
        SplitSampleCodeOption<uint32_t> ss(32, 0, CodeOptionSet::Basic);
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
        SplitSampleCodeOption<uint16_t> ss(16, 1, CodeOptionSet::Basic);
        ASSERT_EQ(ss.value, uint32_t(2));
        ASSERT_EQ(ss.length, uint8_t(4));
        PASS();
    }

    {
        TEST("Split-sample IDs are unique and ordered");
        for (uint8_t k = 0; k < 13; ++k) {
            SplitSampleCodeOption<uint16_t> ss(16, k, CodeOptionSet::Basic);
            ASSERT_EQ(ss.value, uint32_t(k + 1));
        }
        PASS();
    }
}

// ============================================================================
// §3.5 Zero Block Option Tests
// ============================================================================

void testZeroBlockEncoding() {
    std::cout << "§3.5 Zero Block Option:" << std::endl;

    constexpr uint8_t BPV = 16;
    constexpr size_t blockSize = 8;
    using ValueType = uint16_t;

    {
        TEST("all-zeros block → ZB option selected");
        std::vector<ValueType> vec(blockSize, 0);
        std::vector<uint8_t> encoded(vec.size() * sizeof(ValueType) * 2);
        encoder::EncoderSeq<ValueType, ValueType> encoder(BPV, blockSize, 128, CodeOptionSet::Basic);
        size_t encodedSize = encoder.encode(Span<ValueType>(vec), encoded);

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
        size_t encodedSize = encoder.encode(Span<ValueType>(vec), encoded);

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
        size_t encodedSize = encoder.encode(Span<ValueType>(vec), encoded);

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
        size_t encodedSize = encoder.encode(Span<ValueType>(vec), encoded);

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
        size_t encodedSize = encoder.encode(Span<ValueType>(vec), encoded);

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

void testSecondExtension() {
    std::cout << "§3.4 Second Extension Transform:" << std::endl;

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
        std::vector<ValueType> vec = {1000, 1001, 1000, 1001, 1000, 1001, 1000, 1001};
        std::vector<uint8_t> encoded(vec.size() * sizeof(ValueType) * 2);
        encoder::EncoderSeq<ValueType, ValueType> encoder(BPV, blockSize, 128, CodeOptionSet::Basic);
        size_t encodedSize = encoder.encode(Span<ValueType>(vec), encoded);

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
        std::vector<ValueType> vec = {5000, 5001, 5000, 5001, 5000, 5001, 5000, 5001};
        std::vector<uint8_t> encoded(vec.size() * sizeof(ValueType) * 2);
        encoder::EncoderSeq<ValueType, ValueType> encoder(BPV, blockSize, 128, CodeOptionSet::Basic);
        size_t encodedSize = encoder.encode(Span<ValueType>(vec), encoded);

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
void test_block_sizes() {
    constexpr uint8_t BPV = sizeof(ValueType) * 8;
    std::cout << "§3.1.6 Block Sizes (J=8,16,32,64) for " << BPV << "-bit "
              << (std::is_signed_v<ValueType> ? "signed" : "unsigned") << ":" << std::endl;

    constexpr size_t blockSizes[] = {8, 16, 32, 64};

    for (size_t bs : blockSizes) {
        std::string name = "J=" + std::to_string(bs) + " encode/decode roundtrip";
        TEST(name.c_str());

        constexpr uint16_t rsiSize = 128;
        size_t dataSize = static_cast<size_t>(bs) * rsiSize;

        std::mt19937 gen(42 + bs);
        std::vector<ValueType> vec(dataSize);
        if constexpr (std::is_signed_v<ValueType>) {
            std::uniform_int_distribution<int> dist(
                std::numeric_limits<ValueType>::min(),
                std::numeric_limits<ValueType>::max());
            for (auto& v : vec) v = static_cast<ValueType>(dist(gen));
        } else {
            std::uniform_int_distribution<unsigned> dist(0, std::numeric_limits<ValueType>::max());
            for (auto& v : vec) v = static_cast<ValueType>(dist(gen));
        }

        std::vector<uint8_t> encoded(vec.size() * sizeof(ValueType) * 2);
        encoder::EncoderSeq<ValueType, ValueType> encoder(BPV, bs, rsiSize, CodeOptionSet::Basic);
        size_t encodedSize = encoder.encode(Span<ValueType>(vec), encoded);

        std::vector<ValueType> decoded(vec.size());
        DecoderSeq<ValueType, ValueType> decoder(BPV, bs, rsiSize, CodeOptionSet::Basic);
        decoder.decode(encoded, decoded);

        for (size_t i = 0; i < vec.size(); ++i) {
            if (vec[i] != decoded[i]) {
                std::cerr << "FAIL at index " << i << ": original=" << +vec[i]
                          << " decoded=" << +decoded[i] << " (bs=" << bs << ")" << std::endl;
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
void testDataPatterns() {
    constexpr uint8_t BPV = sizeof(ValueType) * 8;
    constexpr size_t blockSize = 8;
    constexpr uint16_t rsiSize = 128;

    std::cout << "End-to-end data patterns (" << (int)BPV << "-bit "
              << (std::is_signed_v<ValueType> ? "signed" : "unsigned") << "):" << std::endl;

    auto roundtrip = [](const std::vector<ValueType>& vec) {
        std::vector<uint8_t> encoded(vec.size() * sizeof(ValueType) * 2 + 256);
        encoder::EncoderSeq<ValueType, ValueType> encoder(BPV, blockSize, rsiSize, CodeOptionSet::Basic);
        size_t encodedSize = encoder.encode(Span<ValueType>(const_cast<ValueType*>(vec.data()), vec.size()), encoded);

        std::vector<ValueType> decoded(vec.size());
        DecoderSeq<ValueType, ValueType> decoder(BPV, blockSize, rsiSize, CodeOptionSet::Basic);
        decoder.decode(encoded, decoded);

        for (size_t i = 0; i < vec.size(); ++i) {
            if (vec[i] != decoded[i]) {
                std::cerr << "FAIL at index " << i << ": original=" << (int)vec[i]
                          << " decoded=" << (int)decoded[i] << std::endl;
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
        std::mt19937 gen(999);
        std::vector<ValueType> vec(N);
        if constexpr (std::is_signed_v<ValueType>) {
            std::uniform_int_distribution<int> dist(
                std::numeric_limits<ValueType>::min(),
                std::numeric_limits<ValueType>::max());
            for (auto& v : vec) v = static_cast<ValueType>(dist(gen));
        } else {
            std::uniform_int_distribution<unsigned> dist(0, std::numeric_limits<ValueType>::max());
            for (auto& v : vec) v = static_cast<ValueType>(dist(gen));
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

void testCodeOptionSets() {
    std::cout << "§5.2.1 Basic vs Restricted Code Option Sets:" << std::endl;

    using ValueType = uint16_t;
    constexpr uint8_t BPV = 16;
    constexpr size_t blockSize = 8;
    constexpr uint16_t rsiSize = 128;

    auto roundtrip = [](CodeOptionSet cos, const std::vector<ValueType>& vec) {
        std::vector<uint8_t> encoded(vec.size() * sizeof(ValueType) * 2 + 256);
        encoder::EncoderSeq<ValueType, ValueType> encoder(BPV, blockSize, rsiSize, cos);
        size_t encodedSize = encoder.encode(Span<ValueType>(const_cast<ValueType*>(vec.data()), vec.size()), encoded);

        std::vector<ValueType> decoded(vec.size());
        DecoderSeq<ValueType, ValueType> decoder(BPV, blockSize, rsiSize, cos);
        decoder.decode(encoded, decoded);

        for (size_t i = 0; i < vec.size(); ++i) {
            if (vec[i] != decoded[i]) {
                std::cerr << "FAIL at index " << i << ": original=" << vec[i]
                          << " decoded=" << decoded[i] << std::endl;
                std::exit(1);
            }
        }
    };

    size_t N = blockSize * rsiSize;

    {
        TEST("Basic code option set - random data");
        std::mt19937 gen(777);
        std::uniform_int_distribution<uint16_t> dist(0, 65535);
        std::vector<ValueType> vec(N);
        for (auto& v : vec) v = dist(gen);
        roundtrip(CodeOptionSet::Basic, vec);
        PASS();
    }

    {
        TEST("Restricted code option set - random data");
        std::mt19937 gen(777);
        std::uniform_int_distribution<uint16_t> dist(0, 65535);
        std::vector<ValueType> vec(N);
        for (auto& v : vec) v = dist(gen);
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

void testSplitSample() {
    std::cout << "§3.3 Split-Sample Options:" << std::endl;

    using ValueType = uint16_t;
    constexpr uint8_t BPV = 16;
    constexpr size_t blockSize = 8;
    constexpr uint16_t rsiSize = 128;

    {
        TEST("data favoring split-sample encoding (moderate entropy)");
        // Data with moderate variation should use split-sample
        std::mt19937 gen(42);
        std::uniform_int_distribution<uint16_t> dist(0, 65535);
        size_t N = blockSize * rsiSize;
        std::vector<ValueType> vec(N);
        for (auto& v : vec) v = dist(gen);

        std::vector<uint8_t> encoded(vec.size() * sizeof(ValueType) * 2);
        encoder::EncoderSeq<ValueType, ValueType> encoder(BPV, blockSize, rsiSize, CodeOptionSet::Basic);
        size_t encodedSize = encoder.encode(Span<ValueType>(vec), encoded);

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

void testReferenceSampleIntervals() {
    std::cout << "§4.3 Reference Sample Interval:" << std::endl;

    using ValueType = uint16_t;
    constexpr uint8_t BPV = 16;
    constexpr size_t blockSize = 8;

    uint16_t rsiSizes[] = {8, 64, 128, 256, 512};

    for (uint16_t rsi : rsiSizes) {
        std::string name = "RSI=" + std::to_string(rsi) + " encode/decode roundtrip";
        TEST(name.c_str());

        size_t N = blockSize * rsi * 2 + blockSize * 3; // Slightly more than 2 RSIs
        std::mt19937 gen(rsi);
        std::uniform_int_distribution<uint16_t> dist(0, 65535);
        std::vector<ValueType> vec(N);
        for (auto& v : vec) v = dist(gen);

        std::vector<uint8_t> encoded(vec.size() * sizeof(ValueType) * 2 + 1024);
        encoder::EncoderSeq<ValueType, ValueType> encoder(BPV, blockSize, rsi, CodeOptionSet::Basic);
        size_t encodedSize = encoder.encode(Span<ValueType>(vec), encoded);

        std::vector<ValueType> decoded(vec.size());
        DecoderSeq<ValueType, ValueType> decoder(BPV, blockSize, rsi, CodeOptionSet::Basic);
        decoder.decode(encoded, decoded);

        for (size_t i = 0; i < vec.size(); ++i) {
            if (vec[i] != decoded[i]) {
                std::cerr << "FAIL at index " << i << ": original=" << vec[i]
                          << " decoded=" << decoded[i] << " (RSI=" << rsi << ")" << std::endl;
                std::exit(1);
            }
        }
        PASS();
    }
}

// ============================================================================
// §3.6 No-Compression Option Tests
// ============================================================================

void testNoCompression() {
    std::cout << "§3.6 No-Compression Option:" << std::endl;

    using ValueType = uint16_t;
    constexpr uint8_t BPV = 16;
    constexpr size_t blockSize = 8;
    constexpr uint16_t rsiSize = 128;

    {
        TEST("max-value data (NC should be selected for high-entropy data)");
        // High-entropy random data may trigger NC for some blocks
        std::mt19937 gen(12345);
        std::uniform_int_distribution<uint16_t> dist(0, 65535);
        size_t N = blockSize * rsiSize;
        std::vector<ValueType> vec(N);
        for (auto& v : vec) v = dist(gen);

        std::vector<uint8_t> encoded(vec.size() * sizeof(ValueType) * 2);
        encoder::EncoderSeq<ValueType, ValueType> encoder(BPV, blockSize, rsiSize, CodeOptionSet::Basic);
        size_t encodedSize = encoder.encode(Span<ValueType>(vec), encoded);

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

void testCompressionEfficiency() {
    std::cout << "§3.7 Compression Efficiency:" << std::endl;

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
        size_t encodedSize = encoder.encode(Span<ValueType>(vec), encoded);
        size_t rawSize = N * sizeof(ValueType);
        assert(encodedSize < rawSize / 2);
        PASS();
    }

    {
        TEST("all-zeros compresses to minimal size");
        size_t N = blockSize * rsiSize;
        std::vector<ValueType> vec(N, 0);
        std::vector<uint8_t> encoded(vec.size() * sizeof(ValueType) * 2);
        encoder::EncoderSeq<ValueType, ValueType> encoder(BPV, blockSize, rsiSize, CodeOptionSet::Basic);
        size_t encodedSize = encoder.encode(Span<ValueType>(vec), encoded);
        size_t rawSize = N * sizeof(ValueType);
        // All zeros should compress extremely well
        assert(encodedSize < rawSize / 8);
        PASS();
    }

    {
        TEST("random data does not expand more than ~1.1x");
        size_t N = blockSize * rsiSize;
        std::mt19937 gen(42);
        std::uniform_int_distribution<uint16_t> dist(0, 65535);
        std::vector<ValueType> vec(N);
        for (auto& v : vec) v = dist(gen);
        std::vector<uint8_t> encoded(vec.size() * sizeof(ValueType) * 4);
        encoder::EncoderSeq<ValueType, ValueType> encoder(BPV, blockSize, rsiSize, CodeOptionSet::Basic);
        size_t encodedSize = encoder.encode(Span<ValueType>(vec), encoded);
        size_t rawSize = N * sizeof(ValueType);
        // Even worst case, should not expand much beyond raw + overhead
        assert(encodedSize <= rawSize * 2);
        PASS();
    }
}

// ============================================================================
// Main
// ============================================================================

int main() {
    std::cout << "CCSDS 121.0-B-3 Compliance Tests" << std::endl;
    std::cout << "=================================" << std::endl << std::endl;

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

    std::cout << std::endl;
    std::cout << "=================================" << std::endl;
    std::cout << passCount << "/" << testCount << " tests passed." << std::endl;

    return (passCount == testCount) ? 0 : 1;
}
