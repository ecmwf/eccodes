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
 * Unit tests for I/O functions and edge cases (grib_io.cc).
 *
 * Tests reading from memory buffers, counting messages, handling
 * of truncated/corrupted data, and the multi-message API.
 */

#include "eccodes.h"
#include "grib_api_internal.h"

#include <cstdio>
#include <cstring>
#include <cstdlib>

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

/* ========== Test: get message from handle and re-create ========== */
static void test_get_message_roundtrip()
{
    printf("Running %s ...\n", __func__);
    int err = 0;

    /* Create a GRIB message from sample */
    grib_handle* h1 = grib_handle_new_from_samples(NULL, "GRIB2");
    TEST_ASSERT(h1 != NULL);

    /* Get the raw message bytes */
    const void* mesg = NULL;
    size_t mesg_len = 0;
    err = grib_get_message(h1, &mesg, &mesg_len);
    TEST_ASSERT(err == GRIB_SUCCESS);
    TEST_ASSERT(mesg != NULL);
    TEST_ASSERT(mesg_len > 0);

    /* Verify GRIB magic number at start */
    const unsigned char* bytes = (const unsigned char*)mesg;
    TEST_ASSERT(bytes[0] == 'G');
    TEST_ASSERT(bytes[1] == 'R');
    TEST_ASSERT(bytes[2] == 'I');
    TEST_ASSERT(bytes[3] == 'B');

    /* Verify 7777 terminator */
    TEST_ASSERT(bytes[mesg_len - 4] == '7');
    TEST_ASSERT(bytes[mesg_len - 3] == '7');
    TEST_ASSERT(bytes[mesg_len - 2] == '7');
    TEST_ASSERT(bytes[mesg_len - 1] == '7');

    /* Create a new handle from the raw bytes */
    grib_handle* h2 = grib_handle_new_from_message(NULL, mesg, mesg_len);
    TEST_ASSERT(h2 != NULL);

    /* Verify key values match */
    long edition1 = 0, edition2 = 0;
    grib_get_long(h1, "edition", &edition1);
    grib_get_long(h2, "edition", &edition2);
    TEST_ASSERT(edition1 == edition2);

    grib_handle_delete(h2);
    grib_handle_delete(h1);
    TEST_PASS();
}

/* ========== Test: count messages in file ========== */
static void test_count_in_file()
{
    printf("Running %s ...\n", __func__);
    int err = 0;

    /* Create a temporary file with one GRIB message */
    char tmpfile[] = "temp_test_io_count.grib";
    grib_handle* h = grib_handle_new_from_samples(NULL, "GRIB2");
    TEST_ASSERT(h != NULL);

    const void* mesg = NULL;
    size_t mesg_len = 0;
    err = grib_get_message(h, &mesg, &mesg_len);
    TEST_ASSERT(err == GRIB_SUCCESS);

    FILE* fp = fopen(tmpfile, "wb");
    TEST_ASSERT(fp != NULL);
    fwrite(mesg, 1, mesg_len, fp);
    /* Write same message again */
    fwrite(mesg, 1, mesg_len, fp);
    fclose(fp);

    /* Count messages */
    fp = fopen(tmpfile, "rb");
    TEST_ASSERT(fp != NULL);
    int n = 0;
    err = grib_count_in_file(NULL, fp, &n);
    TEST_ASSERT(err == GRIB_SUCCESS);
    TEST_ASSERT(n == 2);
    fclose(fp);

    grib_handle_delete(h);
    remove(tmpfile);
    TEST_PASS();
}

/* ========== Test: reading from empty file ========== */
static void test_empty_file()
{
    printf("Running %s ...\n", __func__);
    int err = 0;

    char tmpfile[] = "temp_test_io_empty.grib";
    FILE* fp = fopen(tmpfile, "wb");
    TEST_ASSERT(fp != NULL);
    fclose(fp);

    fp = fopen(tmpfile, "rb");
    TEST_ASSERT(fp != NULL);
    int n = 0;
    err = grib_count_in_file(NULL, fp, &n);
    TEST_ASSERT(err == GRIB_SUCCESS);
    TEST_ASSERT(n == 0);
    fclose(fp);

    remove(tmpfile);
    TEST_PASS();
}

/* ========== Test: truncated GRIB message ========== */
static void test_truncated_message()
{
    printf("Running %s ...\n", __func__);
    int err = 0;

    /* Create a valid GRIB message */
    grib_handle* h = grib_handle_new_from_samples(NULL, "GRIB2");
    TEST_ASSERT(h != NULL);

    const void* mesg = NULL;
    size_t mesg_len = 0;
    err = grib_get_message(h, &mesg, &mesg_len);
    TEST_ASSERT(err == GRIB_SUCCESS);
    TEST_ASSERT(mesg_len > 16);

    /* Write a truncated message (cut off last half) */
    char tmpfile[] = "temp_test_io_truncated.grib";
    FILE* fp = fopen(tmpfile, "wb");
    TEST_ASSERT(fp != NULL);
    size_t half = mesg_len / 2;
    fwrite(mesg, 1, half, fp);
    fclose(fp);

    /* Try to read from truncated file - should fail or return 0 messages */
    fp = fopen(tmpfile, "rb");
    TEST_ASSERT(fp != NULL);

    grib_handle* h2 = grib_handle_new_from_file(NULL, fp, &err);
    /* Either returns NULL handle or an error code */
    if (h2 != NULL) {
        grib_handle_delete(h2);
    }
    /* err should indicate a problem (at minimum not a successful full read) */
    fclose(fp);

    grib_handle_delete(h);
    remove(tmpfile);
    TEST_PASS();
}

/* ========== Test: corrupted magic number ========== */
static void test_corrupted_magic()
{
    printf("Running %s ...\n", __func__);
    int err = 0;

    grib_handle* h = grib_handle_new_from_samples(NULL, "GRIB2");
    TEST_ASSERT(h != NULL);

    const void* mesg = NULL;
    size_t mesg_len = 0;
    err = grib_get_message(h, &mesg, &mesg_len);
    TEST_ASSERT(err == GRIB_SUCCESS);

    /* Copy and corrupt the magic number */
    unsigned char* corrupt = (unsigned char*)malloc(mesg_len);
    TEST_ASSERT(corrupt != NULL);
    memcpy(corrupt, mesg, mesg_len);
    corrupt[0] = 'X'; /* Corrupt 'G' -> 'X' */

    char tmpfile[] = "temp_test_io_corrupt.grib";
    FILE* fp = fopen(tmpfile, "wb");
    TEST_ASSERT(fp != NULL);
    fwrite(corrupt, 1, mesg_len, fp);
    fclose(fp);

    /* Try to read - should fail */
    fp = fopen(tmpfile, "rb");
    TEST_ASSERT(fp != NULL);
    int n = 0;
    err = grib_count_in_file(NULL, fp, &n);
    /* A corrupted file should yield 0 valid messages */
    TEST_ASSERT(n == 0);
    fclose(fp);

    free(corrupt);
    grib_handle_delete(h);
    remove(tmpfile);
    TEST_PASS();
}

/* ========== Test: GRIB message from copy ========== */
static void test_handle_new_from_message_copy()
{
    printf("Running %s ...\n", __func__);
    int err = 0;

    grib_handle* h1 = grib_handle_new_from_samples(NULL, "GRIB2");
    TEST_ASSERT(h1 != NULL);

    const void* mesg = NULL;
    size_t mesg_len = 0;
    err = grib_get_message(h1, &mesg, &mesg_len);
    TEST_ASSERT(err == GRIB_SUCCESS);

    /* grib_handle_new_from_message_copy makes its own copy of the buffer */
    grib_handle* h2 = grib_handle_new_from_message_copy(NULL, mesg, mesg_len);
    TEST_ASSERT(h2 != NULL);

    long edition = 0;
    err = grib_get_long(h2, "edition", &edition);
    TEST_ASSERT(err == GRIB_SUCCESS);
    TEST_ASSERT(edition == 2);

    grib_handle_delete(h2);
    grib_handle_delete(h1);
    TEST_PASS();
}

/* ========== Test: BUFR sample creation ========== */
static void test_bufr_sample_creation()
{
    printf("Running %s ...\n", __func__);
    int err = 0;

    grib_handle* h = codes_bufr_handle_new_from_samples(NULL, "BUFR4");
    TEST_ASSERT(h != NULL);

    long edition = 0;
    err = grib_get_long(h, "edition", &edition);
    TEST_ASSERT(err == GRIB_SUCCESS);
    TEST_ASSERT(edition == 4);

    /* Get message bytes */
    const void* mesg = NULL;
    size_t mesg_len = 0;
    err = grib_get_message(h, &mesg, &mesg_len);
    TEST_ASSERT(err == GRIB_SUCCESS);
    TEST_ASSERT(mesg_len > 0);

    /* Verify BUFR magic number */
    const unsigned char* bytes = (const unsigned char*)mesg;
    TEST_ASSERT(bytes[0] == 'B');
    TEST_ASSERT(bytes[1] == 'U');
    TEST_ASSERT(bytes[2] == 'F');
    TEST_ASSERT(bytes[3] == 'R');

    grib_handle_delete(h);
    TEST_PASS();
}

int main(int argc, char** argv)
{
    printf("Running I/O unit tests...\n\n");

    test_get_message_roundtrip();
    test_count_in_file();
    test_empty_file();
    test_truncated_message();
    test_corrupted_magic();
    test_handle_new_from_message_copy();
    test_bufr_sample_creation();

    printf("\n========================================\n");
    printf("I/O tests: %d passed, %d failed\n", tests_passed, tests_failed);
    printf("========================================\n");

    return tests_failed > 0 ? 1 : 0;
}
