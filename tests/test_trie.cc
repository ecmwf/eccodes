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
 * Unit tests for the internal grib_trie data structure (grib_trie.cc).
 *
 * Tests insert/get round-trips, overwrite semantics, insert_no_replace,
 * missing key lookup, clear, many-key growth, and special characters.
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

/* ========== Test: basic insert and get ========== */
static void test_insert_get()
{
    printf("Running %s ...\n", __func__);
    grib_context* c = grib_context_get_default();
    grib_trie* t = grib_trie_new(c);
    TEST_ASSERT(t != NULL);

    int val1 = 42;
    int val2 = 99;

    /* Insert and retrieve */
    void* old = grib_trie_insert(t, "edition", &val1);
    TEST_ASSERT(old == NULL); /* no previous value */

    void* got = grib_trie_get(t, "edition");
    TEST_ASSERT(got == &val1);
    TEST_ASSERT(*(int*)got == 42);

    /* Insert a second key */
    old = grib_trie_insert(t, "step", &val2);
    TEST_ASSERT(old == NULL);

    got = grib_trie_get(t, "step");
    TEST_ASSERT(got == &val2);
    TEST_ASSERT(*(int*)got == 99);

    /* First key still there */
    got = grib_trie_get(t, "edition");
    TEST_ASSERT(got == &val1);

    grib_trie_delete_container(t);
    TEST_PASS();
}

/* ========== Test: overwrite returns old value ========== */
static void test_overwrite()
{
    printf("Running %s ...\n", __func__);
    grib_context* c = grib_context_get_default();
    grib_trie* t = grib_trie_new(c);
    TEST_ASSERT(t != NULL);

    int val1 = 10;
    int val2 = 20;

    grib_trie_insert(t, "paramId", &val1);
    void* old = grib_trie_insert(t, "paramId", &val2);
    TEST_ASSERT(old == &val1); /* returns the old value */

    void* got = grib_trie_get(t, "paramId");
    TEST_ASSERT(got == &val2);
    TEST_ASSERT(*(int*)got == 20);

    grib_trie_delete_container(t);
    TEST_PASS();
}

/* ========== Test: insert_no_replace does not overwrite ========== */
static void test_insert_no_replace()
{
    printf("Running %s ...\n", __func__);
    grib_context* c = grib_context_get_default();
    grib_trie* t = grib_trie_new(c);
    TEST_ASSERT(t != NULL);

    int val1 = 100;
    int val2 = 200;

    /* First insert succeeds */
    void* result = grib_trie_insert_no_replace(t, "gridType", &val1);
    TEST_ASSERT(result == &val1);

    /* Second insert with same key returns existing value, does NOT replace */
    result = grib_trie_insert_no_replace(t, "gridType", &val2);
    TEST_ASSERT(result == &val1); /* still the original */

    void* got = grib_trie_get(t, "gridType");
    TEST_ASSERT(got == &val1);
    TEST_ASSERT(*(int*)got == 100);

    grib_trie_delete_container(t);
    TEST_PASS();
}

/* ========== Test: get on missing key returns NULL ========== */
static void test_missing_key()
{
    printf("Running %s ...\n", __func__);
    grib_context* c = grib_context_get_default();
    grib_trie* t = grib_trie_new(c);
    TEST_ASSERT(t != NULL);

    void* got = grib_trie_get(t, "nonexistent");
    TEST_ASSERT(got == NULL);

    /* Insert something, then check a different key */
    int val = 1;
    grib_trie_insert(t, "level", &val);
    got = grib_trie_get(t, "levels"); /* similar but different */
    TEST_ASSERT(got == NULL);

    got = grib_trie_get(t, "leve"); /* prefix of existing */
    TEST_ASSERT(got == NULL);

    grib_trie_delete_container(t);
    TEST_PASS();
}

/* ========== Test: clear zeros all entries ========== */
static void test_clear()
{
    printf("Running %s ...\n", __func__);
    grib_context* c = grib_context_get_default();
    grib_trie* t = grib_trie_new(c);
    TEST_ASSERT(t != NULL);

    int val1 = 1, val2 = 2, val3 = 3;
    grib_trie_insert(t, "a", &val1);
    grib_trie_insert(t, "b", &val2);
    grib_trie_insert(t, "c", &val3);

    TEST_ASSERT(grib_trie_get(t, "a") == &val1);
    TEST_ASSERT(grib_trie_get(t, "b") == &val2);
    TEST_ASSERT(grib_trie_get(t, "c") == &val3);

    grib_trie_clear(t);

    /* All should be NULL now */
    TEST_ASSERT(grib_trie_get(t, "a") == NULL);
    TEST_ASSERT(grib_trie_get(t, "b") == NULL);
    TEST_ASSERT(grib_trie_get(t, "c") == NULL);

    /* Trie is still usable after clear */
    int val4 = 4;
    grib_trie_insert(t, "a", &val4);
    TEST_ASSERT(grib_trie_get(t, "a") == &val4);

    grib_trie_delete_container(t);
    TEST_PASS();
}

/* ========== Test: many keys exercise trie growth ========== */
static void test_many_keys()
{
    printf("Running %s ...\n", __func__);
    grib_context* c = grib_context_get_default();
    grib_trie* t = grib_trie_new(c);
    TEST_ASSERT(t != NULL);

    const int N = 200;
    int values[200];
    char keys[200][32];

    for (int i = 0; i < N; i++) {
        values[i] = i * 10;
        snprintf(keys[i], sizeof(keys[i]), "key_%d", i);
        grib_trie_insert(t, keys[i], &values[i]);
    }

    /* Verify all keys are retrievable */
    for (int i = 0; i < N; i++) {
        void* got = grib_trie_get(t, keys[i]);
        TEST_ASSERT(got != NULL);
        TEST_ASSERT(*(int*)got == i * 10);
    }

    grib_trie_delete_container(t);
    TEST_PASS();
}

/* ========== Test: keys with special characters ========== */
static void test_special_chars()
{
    printf("Running %s ...\n", __func__);
    grib_context* c = grib_context_get_default();
    grib_trie* t = grib_trie_new(c);
    TEST_ASSERT(t != NULL);

    int v1 = 1, v2 = 2, v3 = 3, v4 = 4, v5 = 5;

    /* Keys with underscores (common in ecCodes keys) */
    grib_trie_insert(t, "grid_type", &v1);
    TEST_ASSERT(grib_trie_get(t, "grid_type") == &v1);

    /* Keys with hash (used in BUFR rank: #1#pressure) */
    grib_trie_insert(t, "#1#pressure", &v2);
    TEST_ASSERT(grib_trie_get(t, "#1#pressure") == &v2);

    /* Keys with digits */
    grib_trie_insert(t, "section3", &v3);
    TEST_ASSERT(grib_trie_get(t, "section3") == &v3);

    /* Single character keys */
    grib_trie_insert(t, "x", &v4);
    TEST_ASSERT(grib_trie_get(t, "x") == &v4);

    /* Uppercase */
    grib_trie_insert(t, "GRIB", &v5);
    TEST_ASSERT(grib_trie_get(t, "GRIB") == &v5);

    grib_trie_delete_container(t);
    TEST_PASS();
}

int main(int argc, char** argv)
{
    printf("Running trie unit tests...\n\n");

    test_insert_get();
    test_overwrite();
    test_insert_no_replace();
    test_missing_key();
    test_clear();
    test_many_keys();
    test_special_chars();

    printf("\n========================================\n");
    printf("Trie tests: %d passed, %d failed\n", tests_passed, tests_failed);
    printf("========================================\n");

    return tests_failed > 0 ? 1 : 0;
}
