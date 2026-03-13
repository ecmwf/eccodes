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
 * Unit tests for pure utility functions from grib_util.cc:
 * - grib_get_reduced_row (non-legacy)
 * - grib_get_reduced_row_p (double precision)
 * - sum_of_pl_array
 */

#include "eccodes.h"
#include "grib_api_internal.h"

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

/* ========== Test: grib_get_reduced_row basic ========== */
static void test_get_reduced_row_basic()
{
    printf("Running %s ...\n", __func__);

    long npoints = 0, ilon_first = 0, ilon_last = 0;

    /* Full globe: 0 to 360 with 36 points on this latitude circle */
    grib_get_reduced_row(36, 0.0, 360.0, &npoints, &ilon_first, &ilon_last);
    TEST_ASSERT(npoints == 36);
    TEST_ASSERT(ilon_first == 0);

    /* Subsection of the globe */
    grib_get_reduced_row(36, 0.0, 180.0, &npoints, &ilon_first, &ilon_last);
    TEST_ASSERT(npoints > 0);
    TEST_ASSERT(npoints <= 36);

    /* Small section */
    grib_get_reduced_row(100, 10.0, 20.0, &npoints, &ilon_first, &ilon_last);
    TEST_ASSERT(npoints > 0);
    TEST_ASSERT(npoints <= 100);

    TEST_PASS();
}

/* ========== Test: grib_get_reduced_row_p matches grib_get_reduced_row ========== */
static void test_reduced_row_p_consistency()
{
    printf("Running %s ...\n", __func__);

    long npoints1 = 0, ilon_first1 = 0, ilon_last1 = 0;
    long npoints2 = 0;
    double olon_first2 = 0, olon_last2 = 0;

    /* Both functions should return the same npoints for same input */
    grib_get_reduced_row(48, 0.0, 100.0, &npoints1, &ilon_first1, &ilon_last1);
    grib_get_reduced_row_p(48, 0.0, 100.0, &npoints2, &olon_first2, &olon_last2);

    TEST_ASSERT(npoints1 == npoints2);
    TEST_ASSERT(npoints1 > 0);

    /* _p variant returns doubles; verify they are within the requested range */
    TEST_ASSERT(olon_first2 >= -0.001); /* small tolerance */
    TEST_ASSERT(olon_last2 <= 100.001);

    /* Test with wrap-around */
    grib_get_reduced_row(25, 350.0, 10.0, &npoints1, &ilon_first1, &ilon_last1);
    grib_get_reduced_row_p(25, 350.0, 10.0, &npoints2, &olon_first2, &olon_last2);
    TEST_ASSERT(npoints1 == npoints2);

    TEST_PASS();
}

/* ========== Test: grib_get_reduced_row edge cases ========== */
static void test_reduced_row_edge_cases()
{
    printf("Running %s ...\n", __func__);

    long npoints = 0, ilon_first = 0, ilon_last = 0;

    /* Same lon: lon_first == lon_last */
    grib_get_reduced_row(36, 90.0, 90.0, &npoints, &ilon_first, &ilon_last);
    /* Should return at least 1 point */
    TEST_ASSERT(npoints >= 1);

    /* Large pl value */
    grib_get_reduced_row(1000, 0.0, 359.0, &npoints, &ilon_first, &ilon_last);
    TEST_ASSERT(npoints > 0);
    TEST_ASSERT(npoints <= 1000);

    /* Very small subsection */
    grib_get_reduced_row(200, 0.0, 1.0, &npoints, &ilon_first, &ilon_last);
    TEST_ASSERT(npoints > 0);

    TEST_PASS();
}

/* ========== Test: sum_of_pl_array basic ========== */
static void test_sum_of_pl_array_basic()
{
    printf("Running %s ...\n", __func__);

    long pl1[] = {10, 20, 30, 40, 50};
    size_t sum = sum_of_pl_array(pl1, 5);
    TEST_ASSERT(sum == 150);

    /* Single element */
    long pl2[] = {42};
    sum = sum_of_pl_array(pl2, 1);
    TEST_ASSERT(sum == 42);

    /* All same values */
    long pl3[] = {8, 8, 8, 8};
    sum = sum_of_pl_array(pl3, 4);
    TEST_ASSERT(sum == 32);

    /* Typical Gaussian pl array (O32 first few) */
    long pl4[] = {20, 25, 36, 40, 45, 50, 60, 64};
    sum = sum_of_pl_array(pl4, 8);
    TEST_ASSERT(sum == 340);

    TEST_PASS();
}

/* ========== Test: sum_of_pl_array large ========== */
static void test_sum_of_pl_array_large()
{
    printf("Running %s ...\n", __func__);

    /* Large array of 1000 elements, all = 100 */
    const int N = 1000;
    long pl[1000];
    for (int i = 0; i < N; i++) {
        pl[i] = 100;
    }
    size_t sum = sum_of_pl_array(pl, N);
    TEST_ASSERT(sum == 100000);

    /* Ascending array */
    for (int i = 0; i < N; i++) {
        pl[i] = i + 1;
    }
    sum = sum_of_pl_array(pl, N);
    /* sum of 1..1000 = 1000*1001/2 = 500500 */
    TEST_ASSERT(sum == 500500);

    TEST_PASS();
}

int main(int argc, char** argv)
{
    printf("Running utility function unit tests...\n\n");

    test_get_reduced_row_basic();
    test_reduced_row_p_consistency();
    test_reduced_row_edge_cases();
    test_sum_of_pl_array_basic();
    test_sum_of_pl_array_large();

    printf("\n========================================\n");
    printf("Utility tests: %d passed, %d failed\n", tests_passed, tests_failed);
    printf("========================================\n");

    return tests_failed > 0 ? 1 : 0;
}
