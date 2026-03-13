/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

/*
 * Unit tests for bit manipulation functions (grib_bits.cc / grib_bits_any_endian.cc)
 *
 * Tests encode/decode round-trips at various bit widths, boundary values,
 * signed encoding, string encoding, and the grib_is_all_bits_one function.
 */

#include "eccodes.h"
#include "grib_api_internal.h"

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <cmath>

static int tests_passed = 0;
static int tests_failed = 0;

#define TEST_ASSERT(cond)                                                      \
    do {                                                                        \
        if (!(cond)) {                                                          \
            fprintf(stderr, "FAIL: %s:%d: %s\n", __FILE__, __LINE__, #cond);    \
            tests_failed++;                                                     \
            return;                                                             \
        }                                                                       \
    } while (0)

#define TEST_PASS() tests_passed++

/* ========== Test: unsigned long encode/decode round-trip ========== */
static void test_unsigned_long_roundtrip()
{
    printf("Running %s ...\n", __func__);
    unsigned char buf[16];

    /* Test various bit widths from 1 to 32 */
    for (int nbits = 1; nbits <= 32; nbits++) {
        unsigned long maxVal = (1UL << nbits) - 1;
        unsigned long testVals[] = {0, 1, maxVal / 2, maxVal - 1, maxVal};

        for (int t = 0; t < 5; t++) {
            unsigned long val = testVals[t];
            if (val > maxVal) continue;

            memset(buf, 0, sizeof(buf));
            long bitp_enc = 0;
            int err = grib_encode_unsigned_long(buf, val, &bitp_enc, nbits);
            TEST_ASSERT(err == GRIB_SUCCESS);
            TEST_ASSERT(bitp_enc == nbits);

            long bitp_dec = 0;
            unsigned long decoded = grib_decode_unsigned_long(buf, &bitp_dec, nbits);
            TEST_ASSERT(bitp_dec == nbits);
            if (decoded != val) {
                fprintf(stderr, "  MISMATCH nbits=%d val=%lu decoded=%lu\n", nbits, val, decoded);
            }
            TEST_ASSERT(decoded == val);
        }
    }
    TEST_PASS();
}

/* ========== Test: unaligned bit offsets ========== */
static void test_unaligned_offsets()
{
    printf("Running %s ...\n", __func__);
    unsigned char buf[32];

    /* Write multiple values at non-byte-aligned positions */
    memset(buf, 0, sizeof(buf));
    long bitp = 0;

    /* Write 3-bit value = 5 (101) */
    grib_encode_unsigned_long(buf, 5, &bitp, 3);
    TEST_ASSERT(bitp == 3);

    /* Write 5-bit value = 17 (10001) */
    grib_encode_unsigned_long(buf, 17, &bitp, 5);
    TEST_ASSERT(bitp == 8);

    /* Write 13-bit value = 4096 */
    grib_encode_unsigned_long(buf, 4096, &bitp, 13);
    TEST_ASSERT(bitp == 21);

    /* Write 11-bit value = 1023 */
    grib_encode_unsigned_long(buf, 1023, &bitp, 11);
    TEST_ASSERT(bitp == 32);

    /* Now decode them back */
    bitp = 0;
    unsigned long v1 = grib_decode_unsigned_long(buf, &bitp, 3);
    TEST_ASSERT(v1 == 5);

    unsigned long v2 = grib_decode_unsigned_long(buf, &bitp, 5);
    TEST_ASSERT(v2 == 17);

    unsigned long v3 = grib_decode_unsigned_long(buf, &bitp, 13);
    TEST_ASSERT(v3 == 4096);

    unsigned long v4 = grib_decode_unsigned_long(buf, &bitp, 11);
    TEST_ASSERT(v4 == 1023);

    TEST_PASS();
}

/* ========== Test: signed long encode/decode ========== */
static void test_signed_long_roundtrip()
{
    printf("Running %s ...\n", __func__);
    unsigned char buf[16];

    /* Test byte-level signed encode/decode */
    long test_vals[] = {0, 1, -1, 127, -127, 255, -255, 32767, -32767, 100000, -100000};

    for (int i = 0; i < 11; i++) {
        long val = test_vals[i];
        memset(buf, 0, sizeof(buf));

        /* Use 4 bytes */
        grib_encode_signed_long(buf, val, 0, 4);
        long decoded = grib_decode_signed_long(buf, 0, 4);
        TEST_ASSERT(decoded == val);
    }

    /* Test bit-level signed encode/decode */
    for (int nbits = 2; nbits <= 32; nbits++) {
        long maxPos = (1L << (nbits - 1)) - 1;
        long testBitVals[] = {0, 1, -1, maxPos, -maxPos};

        for (int t = 0; t < 5; t++) {
            long val = testBitVals[t];
            memset(buf, 0, sizeof(buf));

            long bitp_enc = 0;
            grib_encode_signed_longb(buf, val, &bitp_enc, nbits);

            long bitp_dec = 0;
            long decoded = grib_decode_signed_longb(buf, &bitp_dec, nbits);
            TEST_ASSERT(decoded == val);
        }
    }

    TEST_PASS();
}

/* ========== Test: get/set individual bits ========== */
static void test_get_set_bit()
{
    printf("Running %s ...\n", __func__);
    unsigned char buf[4];

    memset(buf, 0, sizeof(buf));

    /* Set specific bits and verify */
    long bitp = 0;
    grib_set_bit_on(buf, &bitp); /* bit 0 = 1 */
    TEST_ASSERT(bitp == 1);
    TEST_ASSERT(grib_get_bit(buf, 0) != 0);

    grib_set_bit_on(buf, &bitp); /* bit 1 = 1 */
    TEST_ASSERT(grib_get_bit(buf, 1) != 0);

    /* bit 2 should still be 0 */
    TEST_ASSERT(grib_get_bit(buf, 2) == 0);

    /* Use grib_set_bit to explicitly set and clear */
    grib_set_bit(buf, 7, 1); /* set bit 7 */
    TEST_ASSERT(grib_get_bit(buf, 7) != 0);

    grib_set_bit(buf, 7, 0); /* clear bit 7 */
    TEST_ASSERT(grib_get_bit(buf, 7) == 0);

    TEST_PASS();
}

/* ========== Test: zero bits decode ========== */
static void test_zero_bits()
{
    printf("Running %s ...\n", __func__);
    unsigned char buf[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

    long bitp = 0;
    unsigned long result = grib_decode_unsigned_long(buf, &bitp, 0);
    TEST_ASSERT(result == 0);
    TEST_ASSERT(bitp == 0); /* bitp should not advance */

    TEST_PASS();
}

/* ========== Test: grib_is_all_bits_one ========== */
static void test_is_all_bits_one()
{
    printf("Running %s ...\n", __func__);

    /* 8 bits all one = 255 = 0xFF */
    TEST_ASSERT(grib_is_all_bits_one(0xFF, 8) == 1);
    TEST_ASSERT(grib_is_all_bits_one(0xFE, 8) == 0);

    /* 1 bit all one = 1 */
    TEST_ASSERT(grib_is_all_bits_one(1, 1) == 1);
    TEST_ASSERT(grib_is_all_bits_one(0, 1) == 0);

    /* 16 bits all one = 65535 */
    TEST_ASSERT(grib_is_all_bits_one(0xFFFF, 16) == 1);
    TEST_ASSERT(grib_is_all_bits_one(0xFFFE, 16) == 0);

    /* 32 bits all one */
    TEST_ASSERT(grib_is_all_bits_one(0xFFFFFFFF, 32) == 1);

    TEST_PASS();
}

/* ========== Test: string encode/decode ========== */
static void test_string_encode_decode()
{
    printf("Running %s ...\n", __func__);
    unsigned char buf[64];
    char decoded[32];

    /* Byte-aligned string encode/decode */
    memset(buf, 0, sizeof(buf));
    memset(decoded, 0, sizeof(decoded));

    long bitp = 0;
    const char* test_str = "GRIB";
    int err = grib_encode_string(buf, &bitp, 4, test_str);
    TEST_ASSERT(err == 0);
    TEST_ASSERT(bitp == 32);

    bitp = 0;
    grib_decode_string(buf, &bitp, 4, decoded);
    TEST_ASSERT(memcmp(decoded, "GRIB", 4) == 0);

    /* Non-byte-aligned string encode/decode */
    memset(buf, 0, sizeof(buf));
    bitp = 3; /* Start at bit offset 3 */
    err = grib_encode_string(buf, &bitp, 4, "TEST");
    TEST_ASSERT(err == 0);
    TEST_ASSERT(bitp == 35);

    bitp = 3;
    memset(decoded, 0, sizeof(decoded));
    grib_decode_string(buf, &bitp, 4, decoded);
    TEST_ASSERT(memcmp(decoded, "TEST", 4) == 0);

    TEST_PASS();
}

/* ========== Test: unsigned byte long decode ========== */
static void test_decode_unsigned_byte_long()
{
    printf("Running %s ...\n", __func__);
    unsigned char buf[] = {0x01, 0x02, 0x03, 0x04};

    /* Read 1 byte at offset 0 */
    unsigned long v = grib_decode_unsigned_byte_long(buf, 0, 1);
    TEST_ASSERT(v == 0x01);

    /* Read 2 bytes at offset 0 */
    v = grib_decode_unsigned_byte_long(buf, 0, 2);
    TEST_ASSERT(v == 0x0102);

    /* Read 4 bytes at offset 0 */
    v = grib_decode_unsigned_byte_long(buf, 0, 4);
    TEST_ASSERT(v == 0x01020304);

    /* Read 2 bytes at offset 2 */
    v = grib_decode_unsigned_byte_long(buf, 2, 2);
    TEST_ASSERT(v == 0x0304);

    TEST_PASS();
}

/* ========== Test: sequential encode/decode of multiple values ========== */
static void test_sequential_values()
{
    printf("Running %s ...\n", __func__);

    /* Buffer must be large enough for sum of all bit widths.
     * widths cycle 1..32, so 100 values: 3 full cycles (1..32) + partial.
     * Sum of 1..32 = 528, * 3 = 1584, plus partial = ~1650 bits = ~207 bytes.
     * Use 256 bytes to be safe. */
    unsigned char buf[256];
    memset(buf, 0, sizeof(buf));

    /* Encode a series of values with varying bit widths */
    long bitp_enc = 0;
    const int count = 100;
    unsigned long values[100];
    int widths[100];

    srand(42);
    for (int i = 0; i < count; i++) {
        widths[i] = (i % 16) + 1; /* 1 to 16 bits */
        unsigned long maxVal = (1UL << widths[i]) - 1;
        values[i] = (unsigned long)(rand()) % (maxVal + 1);

        grib_encode_unsigned_long(buf, values[i], &bitp_enc, widths[i]);
    }

    /* Decode and verify */
    long bitp_dec = 0;
    for (int i = 0; i < count; i++) {
        unsigned long decoded = grib_decode_unsigned_long(buf, &bitp_dec, widths[i]);
        if (decoded != values[i]) {
            fprintf(stderr, "  MISMATCH at i=%d width=%d val=%lu decoded=%lu\n",
                    i, widths[i], values[i], decoded);
        }
        TEST_ASSERT(decoded == values[i]);
    }

    TEST_PASS();
}

int main(int argc, char** argv)
{
    printf("Running bit manipulation unit tests...\n\n");

    test_unsigned_long_roundtrip();
    test_unaligned_offsets();
    test_signed_long_roundtrip();
    test_get_set_bit();
    test_zero_bits();
    test_is_all_bits_one();
    test_string_encode_decode();
    test_decode_unsigned_byte_long();
    test_sequential_values();

    printf("\n========================================\n");
    printf("Bit tests: %d passed, %d failed\n", tests_passed, tests_failed);
    printf("========================================\n");

    return tests_failed > 0 ? 1 : 0;
}
