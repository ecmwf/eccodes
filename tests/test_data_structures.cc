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
 * Unit tests for internal data structures:
 * - grib_iarray (dynamic integer array)
 * - grib_darray (dynamic double array)
 * - grib_sarray (dynamic string array)
 * - codes_power (scaling function)
 */

#include "eccodes.h"
#include "grib_api_internal.h"
#include "grib_scaling.h"

#include <cstdio>
#include <cstring>
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

/* ========== Test: iarray basic operations ========== */
static void test_iarray_basic()
{
    printf("Running %s ...\n", __func__);

    grib_iarray* a = grib_iarray_new(4, 4);
    TEST_ASSERT(a != NULL);
    TEST_ASSERT(grib_iarray_used_size(a) == 0);

    grib_iarray_push(a, 10);
    grib_iarray_push(a, 20);
    grib_iarray_push(a, 30);
    TEST_ASSERT(grib_iarray_used_size(a) == 3);

    long* data = grib_iarray_get_array(a);
    TEST_ASSERT(data != NULL);
    TEST_ASSERT(data[0] == 10);
    TEST_ASSERT(data[1] == 20);
    TEST_ASSERT(data[2] == 30);
    free(data);

    grib_iarray_delete(a);
    TEST_PASS();
}

/* ========== Test: iarray growth beyond initial capacity ========== */
static void test_iarray_growth()
{
    printf("Running %s ...\n", __func__);

    /* Start with capacity of 2, increment of 2 */
    grib_iarray* a = grib_iarray_new(2, 2);
    TEST_ASSERT(a != NULL);

    /* Push more values than initial capacity */
    for (int i = 0; i < 100; i++) {
        grib_iarray_push(a, i * 10);
    }
    TEST_ASSERT(grib_iarray_used_size(a) == 100);

    long* data = grib_iarray_get_array(a);
    for (int i = 0; i < 100; i++) {
        TEST_ASSERT(data[i] == i * 10);
    }
    free(data);

    grib_iarray_delete(a);
    TEST_PASS();
}

/* ========== Test: darray basic operations ========== */
static void test_darray_basic()
{
    printf("Running %s ...\n", __func__);

    grib_darray* a = grib_darray_new(4, 4);
    TEST_ASSERT(a != NULL);
    TEST_ASSERT(grib_darray_used_size(a) == 0);

    grib_darray_push(a, 1.5);
    grib_darray_push(a, -2.7);
    grib_darray_push(a, 0.0);
    grib_darray_push(a, 999.999);
    TEST_ASSERT(grib_darray_used_size(a) == 4);

    /* Access internal array directly (no get_array API) */
    TEST_ASSERT(a->v != NULL);
    TEST_ASSERT(fabs(a->v[0] - 1.5) < 1e-10);
    TEST_ASSERT(fabs(a->v[1] - (-2.7)) < 1e-10);
    TEST_ASSERT(fabs(a->v[2] - 0.0) < 1e-10);
    TEST_ASSERT(fabs(a->v[3] - 999.999) < 1e-10);

    grib_darray_delete(a);
    TEST_PASS();
}

/* ========== Test: darray growth ========== */
static void test_darray_growth()
{
    printf("Running %s ...\n", __func__);

    grib_darray* a = grib_darray_new(1, 1);
    TEST_ASSERT(a != NULL);

    for (int i = 0; i < 500; i++) {
        grib_darray_push(a, (double)i * 0.1);
    }
    TEST_ASSERT(grib_darray_used_size(a) == 500);

    /* Access internal array directly */
    for (int i = 0; i < 500; i++) {
        TEST_ASSERT(fabs(a->v[i] - (double)i * 0.1) < 1e-10);
    }

    grib_darray_delete(a);
    TEST_PASS();
}

/* ========== Test: sarray basic operations ========== */
static void test_sarray_basic()
{
    printf("Running %s ...\n", __func__);

    grib_sarray* a = grib_sarray_new(4, 4);
    TEST_ASSERT(a != NULL);
    TEST_ASSERT(grib_sarray_used_size(a) == 0);

    char s1[] = "hello";
    char s2[] = "world";
    grib_sarray_push(a, s1);
    grib_sarray_push(a, s2);
    TEST_ASSERT(grib_sarray_used_size(a) == 2);

    grib_sarray_delete(a);
    TEST_PASS();
}

/* ========== Test: viarray (vector of iarray) ========== */
static void test_viarray()
{
    printf("Running %s ...\n", __func__);

    grib_viarray* va = grib_viarray_new(2, 2);
    TEST_ASSERT(va != NULL);

    grib_iarray* a1 = grib_iarray_new(4, 4);
    grib_iarray_push(a1, 1);
    grib_iarray_push(a1, 2);

    grib_iarray* a2 = grib_iarray_new(4, 4);
    grib_iarray_push(a2, 10);
    grib_iarray_push(a2, 20);
    grib_iarray_push(a2, 30);

    grib_viarray_push(va, a1);
    grib_viarray_push(va, a2);

    grib_iarray_delete(a1);
    grib_iarray_delete(a2);
    grib_viarray_delete(va);
    TEST_PASS();
}

/* ========== Test: codes_power template function ========== */
static void test_codes_power()
{
    printf("Running %s ...\n", __func__);

    /* 2^0 = 1 */
    TEST_ASSERT(fabs(codes_power<double>(0, 2) - 1.0) < 1e-10);

    /* 2^1 = 2 */
    TEST_ASSERT(fabs(codes_power<double>(1, 2) - 2.0) < 1e-10);

    /* 2^10 = 1024 */
    TEST_ASSERT(fabs(codes_power<double>(10, 2) - 1024.0) < 1e-10);

    /* 10^3 = 1000 */
    TEST_ASSERT(fabs(codes_power<double>(3, 10) - 1000.0) < 1e-10);

    /* 2^-1 = 0.5 */
    TEST_ASSERT(fabs(codes_power<double>(-1, 2) - 0.5) < 1e-10);

    /* 2^-3 = 0.125 */
    TEST_ASSERT(fabs(codes_power<double>(-3, 2) - 0.125) < 1e-10);

    /* 10^-2 = 0.01 */
    TEST_ASSERT(fabs(codes_power<double>(-2, 10) - 0.01) < 1e-10);

    TEST_PASS();
}

int main(int argc, char** argv)
{
    printf("Running data structure unit tests...\n\n");

    test_iarray_basic();
    test_iarray_growth();
    test_darray_basic();
    test_darray_growth();
    test_sarray_basic();
    test_viarray();
    test_codes_power();

    printf("\n========================================\n");
    printf("Data structure tests: %d passed, %d failed\n", tests_passed, tests_failed);
    printf("========================================\n");

    return tests_failed > 0 ? 1 : 0;
}
