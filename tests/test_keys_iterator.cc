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
 * Unit tests for the keys iterator API (grib_keys_iterator.cc).
 *
 * Tests basic lifecycle, rewind, namespace filtering, skip flags,
 * and finding specific keys during iteration.
 */

#include "eccodes.h"
#include "grib_api_internal.h"

#include <cstdio>
#include <cstring>

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

/* ========== Test: basic lifecycle ========== */
static void test_basic_lifecycle()
{
    printf("Running %s ...\n", __func__);

    grib_handle* h = grib_handle_new_from_samples(NULL, "GRIB2");
    TEST_ASSERT(h != NULL);

    grib_keys_iterator* kiter = grib_keys_iterator_new(h, 0, NULL);
    TEST_ASSERT(kiter != NULL);

    int count = 0;
    while (grib_keys_iterator_next(kiter)) {
        const char* name = grib_keys_iterator_get_name(kiter);
        TEST_ASSERT(name != NULL);
        TEST_ASSERT(strlen(name) > 0);
        count++;
    }
    TEST_ASSERT(count > 0);
    printf("  Total keys (no filter): %d\n", count);

    grib_keys_iterator_delete(kiter);
    grib_handle_delete(h);
    TEST_PASS();
}

/* ========== Test: rewind gives same count ========== */
static void test_rewind()
{
    printf("Running %s ...\n", __func__);

    grib_handle* h = grib_handle_new_from_samples(NULL, "GRIB2");
    TEST_ASSERT(h != NULL);

    grib_keys_iterator* kiter = grib_keys_iterator_new(h, 0, NULL);
    TEST_ASSERT(kiter != NULL);

    /* First pass */
    int count1 = 0;
    while (grib_keys_iterator_next(kiter)) {
        count1++;
    }
    TEST_ASSERT(count1 > 0);

    /* Rewind */
    int err = grib_keys_iterator_rewind(kiter);
    TEST_ASSERT(err == GRIB_SUCCESS);

    /* Second pass */
    int count2 = 0;
    while (grib_keys_iterator_next(kiter)) {
        count2++;
    }

    TEST_ASSERT(count1 == count2);

    grib_keys_iterator_delete(kiter);
    grib_handle_delete(h);
    TEST_PASS();
}

/* ========== Test: namespace filter reduces keys ========== */
static void test_namespace_filter()
{
    printf("Running %s ...\n", __func__);

    grib_handle* h = grib_handle_new_from_samples(NULL, "GRIB2");
    TEST_ASSERT(h != NULL);

    /* Iterate all keys */
    grib_keys_iterator* kiter_all = grib_keys_iterator_new(h, 0, NULL);
    TEST_ASSERT(kiter_all != NULL);
    int count_all = 0;
    while (grib_keys_iterator_next(kiter_all)) {
        count_all++;
    }
    grib_keys_iterator_delete(kiter_all);

    /* Iterate only the "ls" namespace */
    grib_keys_iterator* kiter_ls = grib_keys_iterator_new(h, 0, "ls");
    TEST_ASSERT(kiter_ls != NULL);
    int count_ls = 0;
    while (grib_keys_iterator_next(kiter_ls)) {
        count_ls++;
    }
    grib_keys_iterator_delete(kiter_ls);

    /* "ls" namespace should have fewer keys than all */
    printf("  All keys: %d, 'ls' namespace: %d\n", count_all, count_ls);
    TEST_ASSERT(count_ls > 0);
    TEST_ASSERT(count_ls < count_all);

    grib_handle_delete(h);
    TEST_PASS();
}

/* ========== Test: skip flags reduce key count ========== */
static void test_skip_flags()
{
    printf("Running %s ...\n", __func__);

    grib_handle* h = grib_handle_new_from_samples(NULL, "GRIB2");
    TEST_ASSERT(h != NULL);

    /* No flags */
    grib_keys_iterator* kiter_none = grib_keys_iterator_new(h, 0, NULL);
    TEST_ASSERT(kiter_none != NULL);
    int count_none = 0;
    while (grib_keys_iterator_next(kiter_none)) {
        count_none++;
    }
    grib_keys_iterator_delete(kiter_none);

    /* Skip read-only + computed + duplicates */
    unsigned long flags = GRIB_KEYS_ITERATOR_SKIP_READ_ONLY |
                          GRIB_KEYS_ITERATOR_SKIP_COMPUTED |
                          GRIB_KEYS_ITERATOR_SKIP_DUPLICATES;
    grib_keys_iterator* kiter_skip = grib_keys_iterator_new(h, flags, NULL);
    TEST_ASSERT(kiter_skip != NULL);
    int count_skip = 0;
    while (grib_keys_iterator_next(kiter_skip)) {
        count_skip++;
    }
    grib_keys_iterator_delete(kiter_skip);

    /* Skipping should yield fewer keys */
    printf("  No flags: %d, with skip flags: %d\n", count_none, count_skip);
    TEST_ASSERT(count_skip < count_none);

    grib_handle_delete(h);
    TEST_PASS();
}

/* ========== Test: "editionNumber" key is found during iteration ========== */
static void test_find_edition_key()
{
    printf("Running %s ...\n", __func__);

    grib_handle* h = grib_handle_new_from_samples(NULL, "GRIB2");
    TEST_ASSERT(h != NULL);

    grib_keys_iterator* kiter = grib_keys_iterator_new(h, 0, NULL);
    TEST_ASSERT(kiter != NULL);

    /* Note: "edition" is an alias; the underlying key is "editionNumber" */
    int found_edition = 0;
    while (grib_keys_iterator_next(kiter)) {
        const char* name = grib_keys_iterator_get_name(kiter);
        if (name && strcmp(name, "editionNumber") == 0) {
            found_edition = 1;
            break;
        }
    }
    TEST_ASSERT(found_edition == 1);

    grib_keys_iterator_delete(kiter);
    grib_handle_delete(h);
    TEST_PASS();
}

int main(int argc, char** argv)
{
    printf("Running keys iterator unit tests...\n\n");

    test_basic_lifecycle();
    test_rewind();
    test_namespace_filter();
    test_skip_flags();
    test_find_edition_key();

    printf("\n========================================\n");
    printf("Keys iterator tests: %d passed, %d failed\n", tests_passed, tests_failed);
    printf("========================================\n");

    return tests_failed > 0 ? 1 : 0;
}
