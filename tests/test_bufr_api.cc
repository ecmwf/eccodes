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
 * Unit tests for BUFR public API functions:
 * - codes_bufr_handle_new_from_file
 * - wmo_read_bufr_from_file / wmo_read_bufr_from_file_malloc
 * - codes_bufr_keys_iterator (including rewind)
 * - codes_bufr_key_is_header
 * - codes_bufr_multi_element_constant_arrays_on/off
 * - BUFR3 vs BUFR4 edition handling
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

/* Helper: write a BUFR sample to a temp file and return its path */
static const char* write_bufr_sample(const char* sample_name, const char* tmpfile)
{
    int err = 0;
    grib_handle* h = codes_bufr_handle_new_from_samples(NULL, sample_name);
    if (!h) return NULL;

    const void* mesg = NULL;
    size_t mesg_len = 0;
    err = grib_get_message(h, &mesg, &mesg_len);
    if (err != GRIB_SUCCESS) {
        grib_handle_delete(h);
        return NULL;
    }

    FILE* fp = fopen(tmpfile, "wb");
    if (!fp) {
        grib_handle_delete(h);
        return NULL;
    }
    fwrite(mesg, 1, mesg_len, fp);
    fclose(fp);
    grib_handle_delete(h);
    return tmpfile;
}

/* ========== Test: BUFR handle from file roundtrip ========== */
static void test_bufr_handle_from_file()
{
    printf("Running %s ...\n", __func__);
    int err = 0;

    const char* tmpfile = "temp_test_bufr_from_file.bufr";
    const char* result = write_bufr_sample("BUFR4", tmpfile);
    TEST_ASSERT(result != NULL);

    /* Read back using codes_bufr_handle_new_from_file */
    FILE* fp = fopen(tmpfile, "rb");
    TEST_ASSERT(fp != NULL);

    grib_handle* h = codes_bufr_handle_new_from_file(NULL, fp, &err);
    TEST_ASSERT(err == GRIB_SUCCESS);
    TEST_ASSERT(h != NULL);

    /* Verify it's a BUFR4 message */
    long edition = 0;
    err = grib_get_long(h, "edition", &edition);
    TEST_ASSERT(err == GRIB_SUCCESS);
    TEST_ASSERT(edition == 4);

    /* Verify product kind */
    ProductKind kind;
    err = codes_get_product_kind(h, &kind);
    TEST_ASSERT(err == GRIB_SUCCESS);
    TEST_ASSERT(kind == PRODUCT_BUFR);

    /* Second read should return NULL (only one message) */
    grib_handle* h2 = codes_bufr_handle_new_from_file(NULL, fp, &err);
    TEST_ASSERT(h2 == NULL);

    grib_handle_delete(h);
    fclose(fp);
    remove(tmpfile);
    TEST_PASS();
}

/* ========== Test: wmo_read_bufr_from_file_malloc ========== */
static void test_wmo_read_bufr_malloc()
{
    printf("Running %s ...\n", __func__);
    int err = 0;

    const char* tmpfile = "temp_test_wmo_bufr_malloc.bufr";
    const char* result = write_bufr_sample("BUFR4", tmpfile);
    TEST_ASSERT(result != NULL);

    FILE* fp = fopen(tmpfile, "rb");
    TEST_ASSERT(fp != NULL);

    size_t size = 0;
    off_t offset = 0;
    void* mesg = wmo_read_bufr_from_file_malloc(fp, 0, &size, &offset, &err);
    TEST_ASSERT(err == GRIB_SUCCESS);
    TEST_ASSERT(mesg != NULL);
    TEST_ASSERT(size > 0);
    TEST_ASSERT(offset == 0); /* First message starts at offset 0 */

    /* Verify BUFR magic number */
    unsigned char* buf = (unsigned char*)mesg;
    TEST_ASSERT(buf[0] == 'B');
    TEST_ASSERT(buf[1] == 'U');
    TEST_ASSERT(buf[2] == 'F');
    TEST_ASSERT(buf[3] == 'R');

    /* Verify 7777 terminator */
    TEST_ASSERT(buf[size - 4] == '7');
    TEST_ASSERT(buf[size - 3] == '7');
    TEST_ASSERT(buf[size - 2] == '7');
    TEST_ASSERT(buf[size - 1] == '7');

    /* Second read should give END_OF_FILE */
    size_t size2 = 0;
    off_t offset2 = 0;
    void* mesg2 = wmo_read_bufr_from_file_malloc(fp, 0, &size2, &offset2, &err);
    TEST_ASSERT(mesg2 == NULL);
    TEST_ASSERT(err == GRIB_END_OF_FILE);

    grib_context_free(NULL, mesg);
    fclose(fp);
    remove(tmpfile);
    TEST_PASS();
}

/* ========== Test: wmo_read_bufr_from_file (fixed buffer) ========== */
static void test_wmo_read_bufr_fixed_buffer()
{
    printf("Running %s ...\n", __func__);
    int err = 0;

    const char* tmpfile = "temp_test_wmo_bufr_fixed.bufr";
    const char* result = write_bufr_sample("BUFR4", tmpfile);
    TEST_ASSERT(result != NULL);

    FILE* fp = fopen(tmpfile, "rb");
    TEST_ASSERT(fp != NULL);

    /* Use a buffer large enough for the sample */
    unsigned char buffer[4096];
    size_t len = sizeof(buffer);
    err = wmo_read_bufr_from_file(fp, buffer, &len);
    TEST_ASSERT(err == GRIB_SUCCESS);
    TEST_ASSERT(len > 0);
    TEST_ASSERT(len <= sizeof(buffer));

    /* Verify magic */
    TEST_ASSERT(buffer[0] == 'B');
    TEST_ASSERT(buffer[1] == 'U');
    TEST_ASSERT(buffer[2] == 'F');
    TEST_ASSERT(buffer[3] == 'R');

    fclose(fp);
    remove(tmpfile);
    TEST_PASS();
}

/* ========== Test: BUFR keys iterator with rewind ========== */
static void test_bufr_keys_iterator_rewind()
{
    printf("Running %s ...\n", __func__);

    grib_handle* h = codes_bufr_handle_new_from_samples(NULL, "BUFR4");
    TEST_ASSERT(h != NULL);

    bufr_keys_iterator* kiter = codes_bufr_keys_iterator_new(h, 0);
    TEST_ASSERT(kiter != NULL);

    /* First pass: count keys */
    int count1 = 0;
    while (codes_bufr_keys_iterator_next(kiter)) {
        char* name = codes_bufr_keys_iterator_get_name(kiter);
        TEST_ASSERT(name != NULL);
        TEST_ASSERT(strlen(name) > 0);
        count1++;
    }
    TEST_ASSERT(count1 > 0);
    printf("  BUFR4 header keys (first pass): %d\n", count1);

    /* Rewind */
    int err = codes_bufr_keys_iterator_rewind(kiter);
    TEST_ASSERT(err == GRIB_SUCCESS);

    /* Second pass: should get same count */
    int count2 = 0;
    while (codes_bufr_keys_iterator_next(kiter)) {
        count2++;
    }
    TEST_ASSERT(count1 == count2);

    codes_bufr_keys_iterator_delete(kiter);
    grib_handle_delete(h);
    TEST_PASS();
}

/* ========== Test: BUFR keys iterator finds expected header keys ========== */
static void test_bufr_keys_iterator_header_keys()
{
    printf("Running %s ...\n", __func__);

    grib_handle* h = codes_bufr_handle_new_from_samples(NULL, "BUFR4");
    TEST_ASSERT(h != NULL);

    bufr_keys_iterator* kiter = codes_bufr_keys_iterator_new(h, 0);
    TEST_ASSERT(kiter != NULL);

    int found_edition = 0;
    int found_masterTableNumber = 0;
    int found_bufrHeaderCentre = 0;

    while (codes_bufr_keys_iterator_next(kiter)) {
        char* name = codes_bufr_keys_iterator_get_name(kiter);
        if (name) {
            if (strcmp(name, "edition") == 0) found_edition = 1;
            if (strcmp(name, "masterTableNumber") == 0) found_masterTableNumber = 1;
            if (strcmp(name, "bufrHeaderCentre") == 0) found_bufrHeaderCentre = 1;
        }
    }

    TEST_ASSERT(found_edition == 1);
    TEST_ASSERT(found_masterTableNumber == 1);
    TEST_ASSERT(found_bufrHeaderCentre == 1);

    codes_bufr_keys_iterator_delete(kiter);
    grib_handle_delete(h);
    TEST_PASS();
}

/* ========== Test: codes_bufr_key_is_header ========== */
static void test_bufr_key_is_header()
{
    printf("Running %s ...\n", __func__);
    int err = 0;

    grib_handle* h = codes_bufr_handle_new_from_samples(NULL, "BUFR4");
    TEST_ASSERT(h != NULL);

    /* "edition" should be a header key */
    int is_header = codes_bufr_key_is_header(h, "edition", &err);
    TEST_ASSERT(err == GRIB_SUCCESS);
    TEST_ASSERT(is_header == 1);

    /* "masterTableNumber" should be a header key */
    is_header = codes_bufr_key_is_header(h, "masterTableNumber", &err);
    TEST_ASSERT(err == GRIB_SUCCESS);
    TEST_ASSERT(is_header == 1);

    /* "bufrHeaderCentre" should be a header key */
    is_header = codes_bufr_key_is_header(h, "bufrHeaderCentre", &err);
    TEST_ASSERT(err == GRIB_SUCCESS);
    TEST_ASSERT(is_header == 1);

    /* Non-existent key */
    is_header = codes_bufr_key_is_header(h, "nonExistentKey_xyz", &err);
    TEST_ASSERT(err != GRIB_SUCCESS);

    grib_handle_delete(h);
    TEST_PASS();
}

/* ========== Test: multi_element_constant_arrays flag ========== */
static void test_multi_element_constant_arrays()
{
    printf("Running %s ...\n", __func__);

    grib_context* c = grib_context_get_default();
    TEST_ASSERT(c != NULL);

    /* Turn on and verify flag */
    codes_bufr_multi_element_constant_arrays_on(c);
    TEST_ASSERT(c->bufr_multi_element_constant_arrays == 1);

    /* Turn off and verify flag */
    codes_bufr_multi_element_constant_arrays_off(c);
    TEST_ASSERT(c->bufr_multi_element_constant_arrays == 0);

    /* Toggle again to confirm idempotency */
    codes_bufr_multi_element_constant_arrays_on(c);
    codes_bufr_multi_element_constant_arrays_on(c);
    TEST_ASSERT(c->bufr_multi_element_constant_arrays == 1);

    codes_bufr_multi_element_constant_arrays_off(c);
    TEST_ASSERT(c->bufr_multi_element_constant_arrays == 0);

    TEST_PASS();
}

/* ========== Test: BUFR3 vs BUFR4 edition handling ========== */
static void test_bufr3_vs_bufr4()
{
    printf("Running %s ...\n", __func__);
    int err = 0;

    /* BUFR3 sample */
    grib_handle* h3 = codes_bufr_handle_new_from_samples(NULL, "BUFR3");
    TEST_ASSERT(h3 != NULL);
    long edition3 = 0;
    err = grib_get_long(h3, "edition", &edition3);
    TEST_ASSERT(err == GRIB_SUCCESS);
    TEST_ASSERT(edition3 == 3);

    /* BUFR4 sample */
    grib_handle* h4 = codes_bufr_handle_new_from_samples(NULL, "BUFR4");
    TEST_ASSERT(h4 != NULL);
    long edition4 = 0;
    err = grib_get_long(h4, "edition", &edition4);
    TEST_ASSERT(err == GRIB_SUCCESS);
    TEST_ASSERT(edition4 == 4);

    /* Both should be PRODUCT_BUFR */
    ProductKind kind3, kind4;
    err = codes_get_product_kind(h3, &kind3);
    TEST_ASSERT(err == GRIB_SUCCESS);
    TEST_ASSERT(kind3 == PRODUCT_BUFR);

    err = codes_get_product_kind(h4, &kind4);
    TEST_ASSERT(err == GRIB_SUCCESS);
    TEST_ASSERT(kind4 == PRODUCT_BUFR);

    /* Message sizes should differ (BUFR4 has different section 1 format) */
    const void* mesg3 = NULL;
    size_t len3 = 0;
    grib_get_message(h3, &mesg3, &len3);

    const void* mesg4 = NULL;
    size_t len4 = 0;
    grib_get_message(h4, &mesg4, &len4);

    printf("  BUFR3 size: %zu, BUFR4 size: %zu\n", len3, len4);
    TEST_ASSERT(len3 > 0);
    TEST_ASSERT(len4 > 0);

    grib_handle_delete(h3);
    grib_handle_delete(h4);
    TEST_PASS();
}

/* ========== Test: BUFR local satellite sample ========== */
static void test_bufr_local_satellite_sample()
{
    printf("Running %s ...\n", __func__);
    int err = 0;

    grib_handle* h = codes_bufr_handle_new_from_samples(NULL, "BUFR4_local_satellite");
    TEST_ASSERT(h != NULL);

    long edition = 0;
    err = grib_get_long(h, "edition", &edition);
    TEST_ASSERT(err == GRIB_SUCCESS);
    TEST_ASSERT(edition == 4);

    /* Should have a larger message than the basic BUFR4 sample */
    const void* mesg = NULL;
    size_t len = 0;
    grib_get_message(h, &mesg, &len);

    grib_handle* h_basic = codes_bufr_handle_new_from_samples(NULL, "BUFR4");
    TEST_ASSERT(h_basic != NULL);
    const void* mesg_basic = NULL;
    size_t len_basic = 0;
    grib_get_message(h_basic, &mesg_basic, &len_basic);

    printf("  BUFR4_local_satellite size: %zu, BUFR4 basic size: %zu\n", len, len_basic);
    TEST_ASSERT(len >= len_basic);

    grib_handle_delete(h_basic);
    grib_handle_delete(h);
    TEST_PASS();
}

int main(int argc, char** argv)
{
    printf("Running BUFR API unit tests...\n\n");

    test_bufr_handle_from_file();
    test_wmo_read_bufr_malloc();
    test_wmo_read_bufr_fixed_buffer();
    test_bufr_keys_iterator_rewind();
    test_bufr_keys_iterator_header_keys();
    test_bufr_key_is_header();
    test_multi_element_constant_arrays();
    test_bufr3_vs_bufr4();
    test_bufr_local_satellite_sample();

    printf("\n========================================\n");
    printf("BUFR API tests: %d passed, %d failed\n", tests_passed, tests_failed);
    printf("========================================\n");

    return tests_failed > 0 ? 1 : 0;
}
