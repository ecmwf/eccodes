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
 * Unit tests for handle lifecycle functions (grib_handle.cc):
 * - codes_check_message_header / codes_check_message_footer
 * - codes_get_product_kind
 * - grib_handle_clone_headers_only
 * - grib_get_message_size
 * - grib_multi_handle (new / append / write / delete)
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

/* ========== Test: codes_check_message_header ========== */
static void test_check_message_header()
{
    printf("Running %s ...\n", __func__);

    /* Valid GRIB header */
    const char grib_header[] = "GRIB____";
    int err = codes_check_message_header(grib_header, 8, PRODUCT_GRIB);
    TEST_ASSERT(err == GRIB_SUCCESS);

    /* Valid BUFR header */
    const char bufr_header[] = "BUFR____";
    err = codes_check_message_header(bufr_header, 8, PRODUCT_BUFR);
    TEST_ASSERT(err == GRIB_SUCCESS);

    /* Wrong product type for these bytes */
    err = codes_check_message_header(grib_header, 8, PRODUCT_BUFR);
    TEST_ASSERT(err != GRIB_SUCCESS);

    err = codes_check_message_header(bufr_header, 8, PRODUCT_GRIB);
    TEST_ASSERT(err != GRIB_SUCCESS);

    /* Invalid header */
    const char bad_header[] = "XXXX____";
    err = codes_check_message_header(bad_header, 8, PRODUCT_GRIB);
    TEST_ASSERT(err != GRIB_SUCCESS);

    TEST_PASS();
}

/* ========== Test: codes_check_message_footer ========== */
static void test_check_message_footer()
{
    printf("Running %s ...\n", __func__);

    const char good_footer[] = "7777";
    int err = codes_check_message_footer(good_footer, 4, PRODUCT_GRIB);
    TEST_ASSERT(err == GRIB_SUCCESS);

    err = codes_check_message_footer(good_footer, 4, PRODUCT_BUFR);
    TEST_ASSERT(err == GRIB_SUCCESS);

    const char bad_footer[] = "7778";
    err = codes_check_message_footer(bad_footer, 4, PRODUCT_GRIB);
    TEST_ASSERT(err != GRIB_SUCCESS);

    const char zeros[] = "\0\0\0\0";
    err = codes_check_message_footer(zeros, 4, PRODUCT_GRIB);
    TEST_ASSERT(err != GRIB_SUCCESS);

    TEST_PASS();
}

/* ========== Test: codes_get_product_kind ========== */
static void test_get_product_kind()
{
    printf("Running %s ...\n", __func__);
    int err = 0;
    ProductKind kind;

    /* GRIB sample */
    grib_handle* hg = grib_handle_new_from_samples(NULL, "GRIB2");
    TEST_ASSERT(hg != NULL);
    err = codes_get_product_kind(hg, &kind);
    TEST_ASSERT(err == GRIB_SUCCESS);
    TEST_ASSERT(kind == PRODUCT_GRIB);
    grib_handle_delete(hg);

    /* BUFR sample */
    grib_handle* hb = codes_bufr_handle_new_from_samples(NULL, "BUFR4");
    TEST_ASSERT(hb != NULL);
    err = codes_get_product_kind(hb, &kind);
    TEST_ASSERT(err == GRIB_SUCCESS);
    TEST_ASSERT(kind == PRODUCT_BUFR);
    grib_handle_delete(hb);

    TEST_PASS();
}

/* ========== Test: grib_handle_clone_headers_only ========== */
static void test_clone_headers_only()
{
    printf("Running %s ...\n", __func__);
    int err = 0;

    grib_handle* h = grib_handle_new_from_samples(NULL, "GRIB2");
    TEST_ASSERT(h != NULL);

    grib_handle* hc = grib_handle_clone_headers_only(h);
    TEST_ASSERT(hc != NULL);

    /* Metadata keys should match */
    long ed_orig = 0, ed_clone = 0;
    err = grib_get_long(h, "edition", &ed_orig);
    TEST_ASSERT(err == GRIB_SUCCESS);
    err = grib_get_long(hc, "edition", &ed_clone);
    TEST_ASSERT(err == GRIB_SUCCESS);
    TEST_ASSERT(ed_orig == ed_clone);

    /* The clone's message should be smaller (no data section) */
    const void* mesg_orig = NULL;
    size_t len_orig = 0;
    grib_get_message(h, &mesg_orig, &len_orig);

    const void* mesg_clone = NULL;
    size_t len_clone = 0;
    grib_get_message(hc, &mesg_clone, &len_clone);

    printf("  Original size: %zu, headers-only clone size: %zu\n", len_orig, len_clone);
    TEST_ASSERT(len_clone <= len_orig);

    grib_handle_delete(hc);
    grib_handle_delete(h);
    TEST_PASS();
}

/* ========== Test: grib_get_message_size ========== */
static void test_get_message_size()
{
    printf("Running %s ...\n", __func__);
    int err = 0;

    grib_handle* h = grib_handle_new_from_samples(NULL, "GRIB2");
    TEST_ASSERT(h != NULL);

    /* Get size via grib_get_message_size */
    size_t size = 0;
    err = grib_get_message_size(h, &size);
    TEST_ASSERT(err == GRIB_SUCCESS);
    TEST_ASSERT(size > 0);

    /* Get size via grib_get_message and compare */
    const void* mesg = NULL;
    size_t mesg_len = 0;
    err = grib_get_message(h, &mesg, &mesg_len);
    TEST_ASSERT(err == GRIB_SUCCESS);
    TEST_ASSERT(size == mesg_len);

    grib_handle_delete(h);
    TEST_PASS();
}

/* ========== Test: multi-handle lifecycle ========== */
static void test_multi_handle_lifecycle()
{
    printf("Running %s ...\n", __func__);
    int err = 0;

    grib_multi_handle* mh = grib_multi_handle_new(NULL);
    TEST_ASSERT(mh != NULL);

    /* Create two different GRIB messages */
    grib_handle* h1 = grib_handle_new_from_samples(NULL, "GRIB2");
    TEST_ASSERT(h1 != NULL);
    err = grib_set_long(h1, "paramId", 130); /* Temperature */
    TEST_ASSERT(err == GRIB_SUCCESS);

    grib_handle* h2 = grib_handle_new_from_samples(NULL, "GRIB2");
    TEST_ASSERT(h2 != NULL);
    err = grib_set_long(h2, "paramId", 131); /* U-wind */
    TEST_ASSERT(err == GRIB_SUCCESS);

    /* Append both to multi-handle */
    err = grib_multi_handle_append(h1, 0, mh);
    TEST_ASSERT(err == GRIB_SUCCESS);
    err = grib_multi_handle_append(h2, 0, mh);
    TEST_ASSERT(err == GRIB_SUCCESS);

    /* Write to temp file */
    const char* tmpfile = "temp_test_multi_handle.grib";
    FILE* fp = fopen(tmpfile, "wb");
    TEST_ASSERT(fp != NULL);
    err = grib_multi_handle_write(mh, fp);
    TEST_ASSERT(err == GRIB_SUCCESS);
    fclose(fp);

    /* Count messages in written file */
    fp = fopen(tmpfile, "rb");
    TEST_ASSERT(fp != NULL);
    int n = 0;
    err = grib_count_in_file(NULL, fp, &n);
    TEST_ASSERT(err == GRIB_SUCCESS);
    TEST_ASSERT(n == 2);

    /* Read back and verify paramIds */
    rewind(fp);
    grib_handle* hr1 = grib_handle_new_from_file(NULL, fp, &err);
    TEST_ASSERT(hr1 != NULL);
    long paramId1 = 0;
    grib_get_long(hr1, "paramId", &paramId1);
    TEST_ASSERT(paramId1 == 130);

    grib_handle* hr2 = grib_handle_new_from_file(NULL, fp, &err);
    TEST_ASSERT(hr2 != NULL);
    long paramId2 = 0;
    grib_get_long(hr2, "paramId", &paramId2);
    TEST_ASSERT(paramId2 == 131);

    grib_handle_delete(hr2);
    grib_handle_delete(hr1);
    fclose(fp);

    grib_handle_delete(h2);
    grib_handle_delete(h1);
    grib_multi_handle_delete(mh);
    remove(tmpfile);
    TEST_PASS();
}

/* ========== Test: multi-handle delete on empty handle ========== */
static void test_multi_handle_empty()
{
    printf("Running %s ...\n", __func__);

    grib_multi_handle* mh = grib_multi_handle_new(NULL);
    TEST_ASSERT(mh != NULL);

    /* Delete without appending anything — should not crash */
    int err = grib_multi_handle_delete(mh);
    TEST_ASSERT(err == GRIB_SUCCESS);

    TEST_PASS();
}

int main(int argc, char** argv)
{
    printf("Running handle lifecycle unit tests...\n\n");

    test_check_message_header();
    test_check_message_footer();
    test_get_product_kind();
    test_clone_headers_only();
    test_get_message_size();
    test_multi_handle_lifecycle();
    test_multi_handle_empty();

    printf("\n========================================\n");
    printf("Handle lifecycle tests: %d passed, %d failed\n", tests_passed, tests_failed);
    printf("========================================\n");

    return tests_failed > 0 ? 1 : 0;
}
