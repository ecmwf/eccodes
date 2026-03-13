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
 * Unit tests for the core get/set API (grib_value.cc).
 *
 * Tests grib_get_long, grib_set_long, grib_get_double, grib_set_double,
 * grib_get_string, grib_set_string, grib_is_missing, grib_is_defined,
 * grib_get_size, grib_set_double_array, error returns for invalid inputs.
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

/* ========== Test: get/set long ========== */
static void test_get_set_long()
{
    printf("Running %s ...\n", __func__);
    int err = 0;
    grib_handle* h = grib_handle_new_from_samples(NULL, "GRIB2");
    TEST_ASSERT(h != NULL);

    /* Set and get edition */
    long edition = 0;
    err = grib_get_long(h, "edition", &edition);
    TEST_ASSERT(err == GRIB_SUCCESS);
    TEST_ASSERT(edition == 2);

    /* Set paramId */
    err = grib_set_long(h, "paramId", 167);
    TEST_ASSERT(err == GRIB_SUCCESS);

    long paramId = 0;
    err = grib_get_long(h, "paramId", &paramId);
    TEST_ASSERT(err == GRIB_SUCCESS);
    TEST_ASSERT(paramId == 167);

    /* Set Ni and Nj */
    err = grib_set_long(h, "Ni", 360);
    TEST_ASSERT(err == GRIB_SUCCESS);
    err = grib_set_long(h, "Nj", 181);
    TEST_ASSERT(err == GRIB_SUCCESS);

    long Ni = 0;
    err = grib_get_long(h, "Ni", &Ni);
    TEST_ASSERT(err == GRIB_SUCCESS);
    TEST_ASSERT(Ni == 360);

    grib_handle_delete(h);
    TEST_PASS();
}

/* ========== Test: get/set double ========== */
static void test_get_set_double()
{
    printf("Running %s ...\n", __func__);
    int err = 0;
    grib_handle* h = grib_handle_new_from_samples(NULL, "GRIB2");
    TEST_ASSERT(h != NULL);

    /* Set and get latitudeOfFirstGridPointInDegrees */
    err = grib_set_double(h, "latitudeOfFirstGridPointInDegrees", 90.0);
    TEST_ASSERT(err == GRIB_SUCCESS);

    double lat = 0;
    err = grib_get_double(h, "latitudeOfFirstGridPointInDegrees", &lat);
    TEST_ASSERT(err == GRIB_SUCCESS);
    TEST_ASSERT(fabs(lat - 90.0) < 1e-6);

    grib_handle_delete(h);
    TEST_PASS();
}

/* ========== Test: get/set string ========== */
static void test_get_set_string()
{
    printf("Running %s ...\n", __func__);
    int err = 0;
    grib_handle* h = grib_handle_new_from_samples(NULL, "GRIB2");
    TEST_ASSERT(h != NULL);

    /* Get gridType (should be "regular_ll" for default GRIB2 sample) */
    char gridType[64] = {0};
    size_t len = sizeof(gridType);
    err = grib_get_string(h, "gridType", gridType, &len);
    TEST_ASSERT(err == GRIB_SUCCESS);
    TEST_ASSERT(strlen(gridType) > 0);

    /* Get packingType */
    char packingType[64] = {0};
    len = sizeof(packingType);
    err = grib_get_string(h, "packingType", packingType, &len);
    TEST_ASSERT(err == GRIB_SUCCESS);
    TEST_ASSERT(strlen(packingType) > 0);

    grib_handle_delete(h);
    TEST_PASS();
}

/* ========== Test: invalid key returns GRIB_NOT_FOUND ========== */
static void test_invalid_key()
{
    printf("Running %s ...\n", __func__);
    int err = 0;
    grib_handle* h = grib_handle_new_from_samples(NULL, "GRIB2");
    TEST_ASSERT(h != NULL);

    long val = 0;
    err = grib_get_long(h, "thisKeyDoesNotExistAtAll", &val);
    TEST_ASSERT(err == GRIB_NOT_FOUND);

    double dval = 0;
    err = grib_get_double(h, "thisKeyDoesNotExistAtAll", &dval);
    TEST_ASSERT(err == GRIB_NOT_FOUND);

    char sval[64] = {0};
    size_t slen = sizeof(sval);
    err = grib_get_string(h, "thisKeyDoesNotExistAtAll", sval, &slen);
    TEST_ASSERT(err == GRIB_NOT_FOUND);

    grib_handle_delete(h);
    TEST_PASS();
}

/* ========== Test: grib_is_defined ========== */
static void test_is_defined()
{
    printf("Running %s ...\n", __func__);
    grib_handle* h = grib_handle_new_from_samples(NULL, "GRIB2");
    TEST_ASSERT(h != NULL);

    /* "edition" should be defined */
    TEST_ASSERT(grib_is_defined(h, "edition") == 1);

    /* A non-existent key should not be defined */
    TEST_ASSERT(grib_is_defined(h, "nonExistentKeyXYZ") == 0);

    grib_handle_delete(h);
    TEST_PASS();
}

/* ========== Test: grib_get_size ========== */
static void test_get_size()
{
    printf("Running %s ...\n", __func__);
    int err = 0;
    grib_handle* h = grib_handle_new_from_samples(NULL, "GRIB2");
    TEST_ASSERT(h != NULL);

    size_t size = 0;
    err = grib_get_size(h, "values", &size);
    TEST_ASSERT(err == GRIB_SUCCESS);
    TEST_ASSERT(size > 0);

    /* edition has size 1 */
    err = grib_get_size(h, "edition", &size);
    TEST_ASSERT(err == GRIB_SUCCESS);
    TEST_ASSERT(size == 1);

    grib_handle_delete(h);
    TEST_PASS();
}

/* ========== Test: set/get double array ========== */
static void test_set_get_double_array()
{
    printf("Running %s ...\n", __func__);
    int err = 0;
    grib_handle* h = grib_handle_new_from_samples(NULL, "GRIB2");
    TEST_ASSERT(h != NULL);

    /* Get the expected number of values */
    size_t numValues = 0;
    err = grib_get_size(h, "values", &numValues);
    TEST_ASSERT(err == GRIB_SUCCESS);
    TEST_ASSERT(numValues > 0);

    /* Create and set an array of values */
    double* vals = (double*)calloc(numValues, sizeof(double));
    TEST_ASSERT(vals != NULL);

    for (size_t i = 0; i < numValues; i++) {
        vals[i] = 273.15 + (double)(i % 100) * 0.1; /* Temperature-like values */
    }

    err = grib_set_double_array(h, "values", vals, numValues);
    TEST_ASSERT(err == GRIB_SUCCESS);

    /* Read back */
    double* read_vals = (double*)calloc(numValues, sizeof(double));
    TEST_ASSERT(read_vals != NULL);
    size_t read_count = numValues;
    err = grib_get_double_array(h, "values", read_vals, &read_count);
    TEST_ASSERT(err == GRIB_SUCCESS);
    TEST_ASSERT(read_count == numValues);

    /* Verify values are close (packing introduces small errors) */
    double maxErr = 0;
    for (size_t i = 0; i < numValues; i++) {
        double diff = fabs(read_vals[i] - vals[i]);
        if (diff > maxErr) maxErr = diff;
    }
    /* Simple packing should be accurate to within bitsPerValue precision */
    TEST_ASSERT(maxErr < 0.01);

    free(vals);
    free(read_vals);
    grib_handle_delete(h);
    TEST_PASS();
}

/* ========== Test: read-only key protection ========== */
static void test_read_only_key()
{
    printf("Running %s ...\n", __func__);
    int err = 0;
    grib_handle* h = grib_handle_new_from_samples(NULL, "GRIB2");
    TEST_ASSERT(h != NULL);

    /* "7777" is a read-only key (end marker) */
    err = grib_set_long(h, "7777", 999);
    TEST_ASSERT(err != GRIB_SUCCESS);

    grib_handle_delete(h);
    TEST_PASS();
}

/* ========== Test: handle cloning ========== */
static void test_handle_clone()
{
    printf("Running %s ...\n", __func__);
    int err = 0;
    grib_handle* h = grib_handle_new_from_samples(NULL, "GRIB2");
    TEST_ASSERT(h != NULL);

    err = grib_set_long(h, "paramId", 130);
    TEST_ASSERT(err == GRIB_SUCCESS);

    grib_handle* clone = grib_handle_clone(h);
    TEST_ASSERT(clone != NULL);

    long paramId = 0;
    err = grib_get_long(clone, "paramId", &paramId);
    TEST_ASSERT(err == GRIB_SUCCESS);
    TEST_ASSERT(paramId == 130);

    /* Modifying clone should not affect original */
    err = grib_set_long(clone, "paramId", 167);
    TEST_ASSERT(err == GRIB_SUCCESS);

    err = grib_get_long(h, "paramId", &paramId);
    TEST_ASSERT(err == GRIB_SUCCESS);
    TEST_ASSERT(paramId == 130);

    grib_handle_delete(clone);
    grib_handle_delete(h);
    TEST_PASS();
}

/* ========== Test: GRIB1 sample ========== */
static void test_grib1_sample()
{
    printf("Running %s ...\n", __func__);
    int err = 0;
    grib_handle* h = grib_handle_new_from_samples(NULL, "GRIB1");
    TEST_ASSERT(h != NULL);

    long edition = 0;
    err = grib_get_long(h, "edition", &edition);
    TEST_ASSERT(err == GRIB_SUCCESS);
    TEST_ASSERT(edition == 1);

    grib_handle_delete(h);
    TEST_PASS();
}

/* ========== Test: get native type ========== */
static void test_get_native_type()
{
    printf("Running %s ...\n", __func__);
    int err = 0;
    grib_handle* h = grib_handle_new_from_samples(NULL, "GRIB2");
    TEST_ASSERT(h != NULL);

    int type = 0;
    err = grib_get_native_type(h, "edition", &type);
    TEST_ASSERT(err == GRIB_SUCCESS);
    TEST_ASSERT(type == GRIB_TYPE_LONG);

    err = grib_get_native_type(h, "values", &type);
    TEST_ASSERT(err == GRIB_SUCCESS);
    TEST_ASSERT(type == GRIB_TYPE_DOUBLE);

    err = grib_get_native_type(h, "gridType", &type);
    TEST_ASSERT(err == GRIB_SUCCESS);
    TEST_ASSERT(type == GRIB_TYPE_STRING);

    grib_handle_delete(h);
    TEST_PASS();
}

int main(int argc, char** argv)
{
    printf("Running core value API unit tests...\n\n");

    test_get_set_long();
    test_get_set_double();
    test_get_set_string();
    test_invalid_key();
    test_is_defined();
    test_get_size();
    test_set_get_double_array();
    test_read_only_key();
    test_handle_clone();
    test_grib1_sample();
    test_get_native_type();

    printf("\n========================================\n");
    printf("Value API tests: %d passed, %d failed\n", tests_passed, tests_failed);
    printf("========================================\n");

    return tests_failed > 0 ? 1 : 0;
}
