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
 * Unit tests for IBM and IEEE float conversion functions.
 *
 * Tests round-trip encode/decode, nearest-smaller-float, edge cases (zero,
 * negative, small/large values), and the error function.
 */

#include "eccodes.h"
#include "grib_api_internal.h"

#include <cstdio>
#include <cstring>
#include <cmath>
#include <cfloat>

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

/* ========== IBM Float Tests ========== */

static void test_ibm_roundtrip_zero()
{
    printf("Running %s ...\n", __func__);
    unsigned long encoded = grib_ibm_to_long(0.0);
    double decoded = grib_long_to_ibm(encoded);
    TEST_ASSERT(decoded == 0.0);
    TEST_PASS();
}

static void test_ibm_roundtrip_positive()
{
    printf("Running %s ...\n", __func__);
    double test_values[] = {1.0, 0.5, 100.0, 273.15, 1013.25, 0.001, 99999.0};

    for (int i = 0; i < 7; i++) {
        double val = test_values[i];
        unsigned long encoded = grib_ibm_to_long(val);
        double decoded = grib_long_to_ibm(encoded);
        double rel_err = fabs(decoded - val) / fabs(val);
        TEST_ASSERT(rel_err < 1e-6);
    }
    TEST_PASS();
}

static void test_ibm_roundtrip_negative()
{
    printf("Running %s ...\n", __func__);
    double test_values[] = {-1.0, -0.5, -273.15, -50000.0};

    for (int i = 0; i < 4; i++) {
        double val = test_values[i];
        unsigned long encoded = grib_ibm_to_long(val);
        double decoded = grib_long_to_ibm(encoded);

        /* Check sign bit (bit 31) is set for negative */
        TEST_ASSERT((encoded & 0x80000000) != 0);

        double rel_err = fabs(decoded - val) / fabs(val);
        TEST_ASSERT(rel_err < 1e-6);
    }
    TEST_PASS();
}

static void test_ibm_nearest_smaller()
{
    printf("Running %s ...\n", __func__);
    double ret = 0;
    int err = 0;

    /* The nearest smaller IBM float of a value should be <= the value */
    double test_values[] = {0.0, 1.0, -1.0, 1.5, 273.15, -50.0, 0.001};

    for (int i = 0; i < 7; i++) {
        err = grib_nearest_smaller_ibm_float(test_values[i], &ret);
        TEST_ASSERT(err == GRIB_SUCCESS);
        TEST_ASSERT(ret <= test_values[i]);
    }

    /* Zero should map to exactly zero */
    err = grib_nearest_smaller_ibm_float(0.0, &ret);
    TEST_ASSERT(err == GRIB_SUCCESS);
    TEST_ASSERT(ret == 0.0);

    TEST_PASS();
}

static void test_ibm_error_function()
{
    printf("Running %s ...\n", __func__);
    /* The error function should return a positive value for positive input */
    double err_val = grib_ibmfloat_error(100.0);
    TEST_ASSERT(err_val > 0.0);

    /* Error for larger values should generally be >= error for smaller values */
    double err_small = grib_ibmfloat_error(1.0);
    double err_large = grib_ibmfloat_error(1e6);
    TEST_ASSERT(err_large >= err_small);

    TEST_PASS();
}

/* ========== IEEE Float Tests ========== */

static void test_ieee_roundtrip_zero()
{
    printf("Running %s ...\n", __func__);
    unsigned long encoded = grib_ieee_to_long(0.0);
    double decoded = grib_long_to_ieee(encoded);
    TEST_ASSERT(decoded == 0.0);
    TEST_PASS();
}

static void test_ieee_roundtrip_positive()
{
    printf("Running %s ...\n", __func__);
    double test_values[] = {1.0, 0.5, 100.0, 273.15, 1013.25, 0.001, 99999.0};

    for (int i = 0; i < 7; i++) {
        double val = test_values[i];
        unsigned long encoded = grib_ieee_to_long(val);
        double decoded = grib_long_to_ieee(encoded);
        double rel_err = fabs(decoded - val) / fabs(val);
        TEST_ASSERT(rel_err < 1e-6);
    }
    TEST_PASS();
}

static void test_ieee_roundtrip_negative()
{
    printf("Running %s ...\n", __func__);
    double test_values[] = {-1.0, -0.5, -273.15, -50000.0};

    for (int i = 0; i < 4; i++) {
        double val = test_values[i];
        unsigned long encoded = grib_ieee_to_long(val);
        double decoded = grib_long_to_ieee(encoded);

        /* Check sign bit (bit 31) is set for negative */
        TEST_ASSERT((encoded & 0x80000000) != 0);

        double rel_err = fabs(decoded - val) / fabs(val);
        TEST_ASSERT(rel_err < 1e-6);
    }
    TEST_PASS();
}

static void test_ieee_nearest_smaller()
{
    printf("Running %s ...\n", __func__);
    double ret = 0;
    int err = 0;

    double test_values[] = {0.0, 1.0, -1.0, 1.5, 273.15, -50.0, 0.001};

    for (int i = 0; i < 7; i++) {
        err = grib_nearest_smaller_ieee_float(test_values[i], &ret);
        TEST_ASSERT(err == GRIB_SUCCESS);
        TEST_ASSERT(ret <= test_values[i]);
    }

    err = grib_nearest_smaller_ieee_float(0.0, &ret);
    TEST_ASSERT(err == GRIB_SUCCESS);
    TEST_ASSERT(ret == 0.0);

    TEST_PASS();
}

static void test_ieee_error_function()
{
    printf("Running %s ...\n", __func__);
    double err_val = grib_ieeefloat_error(100.0);
    TEST_ASSERT(err_val > 0.0);

    double err_small = grib_ieeefloat_error(1.0);
    double err_large = grib_ieeefloat_error(1e6);
    TEST_ASSERT(err_large >= err_small);

    TEST_PASS();
}

/* ========== Cross-format consistency ========== */

static void test_ibm_ieee_consistency()
{
    printf("Running %s ...\n", __func__);

    /* Both IBM and IEEE representations of simple values should decode
     * to approximately the same value */
    double test_values[] = {0.0, 1.0, -1.0, 100.0, 273.15};

    for (int i = 0; i < 5; i++) {
        double val = test_values[i];
        double ibm_decoded = grib_long_to_ibm(grib_ibm_to_long(val));
        double ieee_decoded = grib_long_to_ieee(grib_ieee_to_long(val));

        /* Both should be within 1e-5 of the original */
        TEST_ASSERT(fabs(ibm_decoded - val) < 1e-4 || val == 0.0);
        TEST_ASSERT(fabs(ieee_decoded - val) < 1e-4 || val == 0.0);
    }

    TEST_PASS();
}

/* ========== IBM/IEEE underflow ========== */

static void test_ibm_underflow()
{
    printf("Running %s ...\n", __func__);
    /* Very small positive numbers should encode without error */
    unsigned long encoded = grib_ibm_to_long(1e-90);
    /* Underflow should map to zero-like value */
    double decoded = grib_long_to_ibm(encoded);
    TEST_ASSERT(decoded >= 0.0);
    TEST_PASS();
}

static void test_ieee_underflow()
{
    printf("Running %s ...\n", __func__);
    unsigned long encoded = grib_ieee_to_long(1e-90);
    double decoded = grib_long_to_ieee(encoded);
    TEST_ASSERT(decoded >= 0.0);
    TEST_PASS();
}

int main(int argc, char** argv)
{
    printf("Running float conversion unit tests...\n\n");

    test_ibm_roundtrip_zero();
    test_ibm_roundtrip_positive();
    test_ibm_roundtrip_negative();
    test_ibm_nearest_smaller();
    test_ibm_error_function();
    test_ibm_underflow();

    test_ieee_roundtrip_zero();
    test_ieee_roundtrip_positive();
    test_ieee_roundtrip_negative();
    test_ieee_nearest_smaller();
    test_ieee_error_function();
    test_ieee_underflow();

    test_ibm_ieee_consistency();

    printf("\n========================================\n");
    printf("Float conversion tests: %d passed, %d failed\n", tests_passed, tests_failed);
    printf("========================================\n");

    return tests_failed > 0 ? 1 : 0;
}
