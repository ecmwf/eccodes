#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <random>
#include <string>
#include <vector>

// Our ccsds compatibility layer
#include "libaec.h"

// We also link against the real libaec for cross-validation.
// Since both define the same symbols, we access real libaec through
// its installed shared library. The real libaec functions are accessed
// via a separate wrapper (see libaec_real_wrapper.cc).
extern "C" {
    int real_aec_encode_init(struct aec_stream *strm);
    int real_aec_encode(struct aec_stream *strm, int flush);
    int real_aec_encode_end(struct aec_stream *strm);
    int real_aec_decode_init(struct aec_stream *strm);
    int real_aec_decode(struct aec_stream *strm, int flush);
    int real_aec_decode_end(struct aec_stream *strm);
}

static int testCount = 0;
static int passCount = 0;
static int failCount = 0;

static void check(bool cond, const std::string &label) {
    testCount++;
    if (cond) {
        passCount++;
    } else {
        failCount++;
        std::cerr << "FAIL: " << label << std::endl;
    }
}

template <typename T>
std::vector<T> makeData(size_t n, uint8_t bpv, unsigned seed) {
    std::vector<T> v(n);
    std::mt19937 gen(seed);
    T mask = (bpv >= sizeof(T) * 8)
                 ? static_cast<T>(~T(0))
                 : static_cast<T>((T(1) << bpv) - 1);
    for (size_t i = 0; i < n; ++i) {
        if (i < n / 3)
            v[i] = static_cast<T>(i) & mask;
        else if (i < 2 * n / 3)
            v[i] = 0;
        else
            v[i] = static_cast<T>(gen()) & mask;
    }
    return v;
}

static unsigned int native_flags() {
    unsigned int flags = AEC_DATA_PREPROCESS;
#if __BYTE_ORDER__ != __ORDER_LITTLE_ENDIAN__
    flags |= AEC_DATA_MSB;
#endif
    return flags;
}

// -------------------------------------------------------------------
// Test 1: ccsds encode → ccsds decode (roundtrip)
// -------------------------------------------------------------------
template <typename T>
void test_roundtrip(uint8_t bpv, uint8_t bs, uint16_t rsi, bool restricted,
                    const std::vector<T> &data, const std::string &label) {
    size_t sample_bytes = sizeof(T);
    unsigned int flags = native_flags();
    if (restricted) flags |= AEC_RESTRICTED;

    // Encode
    std::vector<uint8_t> encoded(data.size() * sample_bytes * 2 + 1024);
    struct aec_stream strm;
    std::memset(&strm, 0, sizeof(strm));
    strm.bits_per_sample = bpv;
    strm.block_size = bs;
    strm.rsi = rsi;
    strm.flags = flags;
    strm.next_in = reinterpret_cast<const unsigned char *>(data.data());
    strm.avail_in = data.size() * sample_bytes;
    strm.next_out = encoded.data();
    strm.avail_out = encoded.size();

    int rc = aec_encode_init(&strm);
    check(rc == AEC_OK, label + " encode_init");
    if (rc != AEC_OK) return;

    rc = aec_encode(&strm, AEC_FLUSH);
    check(rc == AEC_OK, label + " encode");
    size_t enc_bytes = strm.total_out;
    aec_encode_end(&strm);

    // Decode
    std::vector<T> decoded(data.size(), 0);
    std::memset(&strm, 0, sizeof(strm));
    strm.bits_per_sample = bpv;
    strm.block_size = bs;
    strm.rsi = rsi;
    strm.flags = flags;
    strm.next_in = encoded.data();
    strm.avail_in = enc_bytes;
    strm.next_out = reinterpret_cast<unsigned char *>(decoded.data());
    strm.avail_out = decoded.size() * sample_bytes;

    rc = aec_decode_init(&strm);
    check(rc == AEC_OK, label + " decode_init");
    if (rc != AEC_OK) return;

    rc = aec_decode(&strm, AEC_FLUSH);
    check(rc == AEC_OK, label + " decode");
    aec_decode_end(&strm);

    bool match = (data == decoded);
    check(match, label + " roundtrip data match");
}

// -------------------------------------------------------------------
// Test 2: ccsds encode → real libaec decode (cross-validation)
// -------------------------------------------------------------------
template <typename T>
void test_ccsds_to_libaec(uint8_t bpv, uint8_t bs, uint16_t rsi,
                          bool restricted, const std::vector<T> &data,
                          const std::string &label) {
    size_t sample_bytes = sizeof(T);
    unsigned int flags = native_flags();
    if (restricted) flags |= AEC_RESTRICTED;

    // Encode with ccsds compat layer
    std::vector<uint8_t> encoded(data.size() * sample_bytes * 2 + 1024);
    struct aec_stream strm;
    std::memset(&strm, 0, sizeof(strm));
    strm.bits_per_sample = bpv;
    strm.block_size = bs;
    strm.rsi = rsi;
    strm.flags = flags;
    strm.next_in = reinterpret_cast<const unsigned char *>(data.data());
    strm.avail_in = data.size() * sample_bytes;
    strm.next_out = encoded.data();
    strm.avail_out = encoded.size();

    if (aec_encode_init(&strm) != AEC_OK) {
        check(false, label + " ccsds encode_init");
        return;
    }
    if (aec_encode(&strm, AEC_FLUSH) != AEC_OK) {
        aec_encode_end(&strm);
        check(false, label + " ccsds encode");
        return;
    }
    size_t enc_bytes = strm.total_out;
    aec_encode_end(&strm);

    // Decode with real libaec
    std::vector<T> decoded(data.size(), 0);
    struct aec_stream rstrm;
    std::memset(&rstrm, 0, sizeof(rstrm));
    rstrm.bits_per_sample = bpv;
    rstrm.block_size = bs;
    rstrm.rsi = rsi;
    rstrm.flags = flags;
    rstrm.next_in = encoded.data();
    rstrm.avail_in = enc_bytes;
    rstrm.next_out = reinterpret_cast<unsigned char *>(decoded.data());
    rstrm.avail_out = decoded.size() * sample_bytes;

    if (real_aec_decode_init(&rstrm) != AEC_OK) {
        check(false, label + " libaec decode_init");
        return;
    }
    int rc = real_aec_decode(&rstrm, AEC_FLUSH);
    real_aec_decode_end(&rstrm);
    check(rc == AEC_OK, label + " libaec decode");

    bool match = (data == decoded);
    check(match, label + " ccsds→libaec data match");
}

// -------------------------------------------------------------------
// Test 3: real libaec encode → ccsds decode (cross-validation)
// -------------------------------------------------------------------
template <typename T>
void test_libaec_to_ccsds(uint8_t bpv, uint8_t bs, uint16_t rsi,
                          bool restricted, const std::vector<T> &data,
                          const std::string &label) {
    size_t sample_bytes = sizeof(T);
    unsigned int flags = native_flags();
    if (restricted) flags |= AEC_RESTRICTED;

    // Encode with real libaec
    std::vector<uint8_t> encoded(data.size() * sample_bytes * 2 + 1024);
    struct aec_stream rstrm;
    std::memset(&rstrm, 0, sizeof(rstrm));
    rstrm.bits_per_sample = bpv;
    rstrm.block_size = bs;
    rstrm.rsi = rsi;
    rstrm.flags = flags;
    rstrm.next_in = reinterpret_cast<const unsigned char *>(data.data());
    rstrm.avail_in = data.size() * sample_bytes;
    rstrm.next_out = encoded.data();
    rstrm.avail_out = encoded.size();

    if (real_aec_encode_init(&rstrm) != AEC_OK) {
        check(false, label + " libaec encode_init");
        return;
    }
    int rc = real_aec_encode(&rstrm, AEC_FLUSH);
    size_t enc_bytes = rstrm.total_out;
    real_aec_encode_end(&rstrm);
    if (rc != AEC_OK) {
        check(false, label + " libaec encode");
        return;
    }

    // Decode with ccsds compat layer
    std::vector<T> decoded(data.size(), 0);
    struct aec_stream strm;
    std::memset(&strm, 0, sizeof(strm));
    strm.bits_per_sample = bpv;
    strm.block_size = bs;
    strm.rsi = rsi;
    strm.flags = flags;
    strm.next_in = encoded.data();
    strm.avail_in = enc_bytes;
    strm.next_out = reinterpret_cast<unsigned char *>(decoded.data());
    strm.avail_out = decoded.size() * sample_bytes;

    if (aec_decode_init(&strm) != AEC_OK) {
        check(false, label + " ccsds decode_init");
        return;
    }
    rc = aec_decode(&strm, AEC_FLUSH);
    check(rc == AEC_OK, label + " ccsds decode");
    aec_decode_end(&strm);

    bool match = (data == decoded);
    check(match, label + " libaec→ccsds data match");
}

// -------------------------------------------------------------------
// Test 4: aec_buffer_encode / aec_buffer_decode convenience functions
// -------------------------------------------------------------------
template <typename T>
void test_buffer_api(uint8_t bpv, uint8_t bs, uint16_t rsi, bool restricted,
                     const std::vector<T> &data, const std::string &label) {
    size_t sample_bytes = sizeof(T);
    unsigned int flags = native_flags();
    if (restricted) flags |= AEC_RESTRICTED;

    std::vector<uint8_t> encoded(data.size() * sample_bytes * 2 + 1024);
    struct aec_stream strm;
    std::memset(&strm, 0, sizeof(strm));
    strm.bits_per_sample = bpv;
    strm.block_size = bs;
    strm.rsi = rsi;
    strm.flags = flags;
    strm.next_in = reinterpret_cast<const unsigned char *>(data.data());
    strm.avail_in = data.size() * sample_bytes;
    strm.next_out = encoded.data();
    strm.avail_out = encoded.size();

    int rc = aec_buffer_encode(&strm);
    check(rc == AEC_OK, label + " buffer_encode");
    if (rc != AEC_OK) return;

    size_t enc_bytes = strm.total_out;

    std::vector<T> decoded(data.size(), 0);
    std::memset(&strm, 0, sizeof(strm));
    strm.bits_per_sample = bpv;
    strm.block_size = bs;
    strm.rsi = rsi;
    strm.flags = flags;
    strm.next_in = encoded.data();
    strm.avail_in = enc_bytes;
    strm.next_out = reinterpret_cast<unsigned char *>(decoded.data());
    strm.avail_out = decoded.size() * sample_bytes;

    rc = aec_buffer_decode(&strm);
    check(rc == AEC_OK, label + " buffer_decode");

    bool match = (data == decoded);
    check(match, label + " buffer API roundtrip");
}

// -------------------------------------------------------------------
// Test 5: Error handling
// -------------------------------------------------------------------
void test_error_handling() {
    struct aec_stream strm;

    // bpv = 0 → AEC_CONF_ERROR
    std::memset(&strm, 0, sizeof(strm));
    strm.bits_per_sample = 0;
    strm.block_size = 16;
    strm.rsi = 2;
    check(aec_encode_init(&strm) == AEC_CONF_ERROR, "bpv=0 encode_init");
    check(aec_decode_init(&strm) == AEC_CONF_ERROR, "bpv=0 decode_init");

    // bpv = 33 → AEC_CONF_ERROR
    strm.bits_per_sample = 33;
    check(aec_encode_init(&strm) == AEC_CONF_ERROR, "bpv=33 encode_init");

    // block_size = 0 → AEC_CONF_ERROR
    strm.bits_per_sample = 16;
    strm.block_size = 0;
    check(aec_encode_init(&strm) == AEC_CONF_ERROR, "bs=0 encode_init");

    // rsi = 0 → AEC_CONF_ERROR
    strm.block_size = 16;
    strm.rsi = 0;
    check(aec_encode_init(&strm) == AEC_CONF_ERROR, "rsi=0 encode_init");

    // invalid block size (not 8/16/32/64) without AEC_NOT_ENFORCE
    strm.rsi = 2;
    strm.block_size = 10;
    strm.flags = 0;
    check(aec_encode_init(&strm) == AEC_CONF_ERROR, "bs=10 encode_init");

    // null stream → AEC_STREAM_ERROR
    check(aec_encode_init(nullptr) == AEC_STREAM_ERROR, "null encode_init");
    check(aec_decode_init(nullptr) == AEC_STREAM_ERROR, "null decode_init");

    // encode_end / decode_end with null state
    strm.state = nullptr;
    check(aec_encode_end(&strm) == AEC_STREAM_ERROR, "null state encode_end");
    check(aec_decode_end(&strm) == AEC_STREAM_ERROR, "null state decode_end");
}

// -------------------------------------------------------------------
// Sweep: runs all tests across parameter combinations
// -------------------------------------------------------------------
template <typename T>
void sweep(const std::string &typeName, uint8_t bpvMin, uint8_t bpvMax) {
    const uint8_t block_sizes[] = {8, 16, 32, 64};
    const uint16_t rsiValues[] = {1, 2, 4, 16, 64, 256};

    for (uint8_t bpv = bpvMin; bpv <= bpvMax; ++bpv) {
        for (int r = 0; r < 2; ++r) {
            bool restricted = (r == 1);
            if (restricted && bpv <= 2) continue;
            if (restricted && bpv >= 5 && bpv <= 8) continue;

            for (uint8_t bs : block_sizes) {
                for (uint16_t rsi : rsiValues) {
                    size_t nSamples = static_cast<size_t>(rsi) * bs * 2 + bs / 2;
                    auto data = makeData<T>(nSamples, bpv, bpv * 1000 + bs);

                    std::string label = typeName +
                        " bpv=" + std::to_string(bpv) +
                        " bs=" + std::to_string(bs) +
                        " rsi=" + std::to_string(rsi) +
                        (restricted ? " restricted" : " basic");

                    test_roundtrip<T>(bpv, bs, rsi, restricted, data, label);
                    test_ccsds_to_libaec<T>(bpv, bs, rsi, restricted, data, label);
                    test_libaec_to_ccsds<T>(bpv, bs, rsi, restricted, data, label);
                    test_buffer_api<T>(bpv, bs, rsi, restricted, data, label);
                }
            }
        }
    }
}

int main() {
    std::cout << "=== libaec Compatibility Layer Tests ===" << std::endl;

    std::cout << "\n--- Error handling ---" << std::endl;
    test_error_handling();

    std::cout << "\n--- uint8_t (bpv 1-8) ---" << std::endl;
    sweep<uint8_t>("uint8", 1, 8);

    std::cout << "\n--- uint16_t (bpv 9-16) ---" << std::endl;
    sweep<uint16_t>("uint16", 9, 16);

    std::cout << "\n--- uint32_t (bpv 17-32) ---" << std::endl;
    sweep<uint32_t>("uint32", 17, 32);

    std::cout << "\n--- Summary ---" << std::endl;
    std::cout << passCount << "/" << testCount << " passed";
    if (failCount > 0) {
        std::cout << ", " << failCount << " FAILED";
    }
    std::cout << std::endl;

    return (failCount > 0) ? 1 : 0;
}
