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
 * Unit tests for BUFR descriptor and descriptor array data structures:
 * - grib_bufr_descriptors_array (new / push / get / pop_front / used_size / delete)
 * - grib_bufr_descriptor (clone / delete / set_scale / can_be_missing)
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

/* Helper: manually create a bufr_descriptor without needing a table accessor */
static bufr_descriptor* make_test_descriptor(int code, const char* shortName,
                                             const char* units, long scale,
                                             double reference, long width, int type)
{
    grib_context* c = grib_context_get_default();
    bufr_descriptor* d = (bufr_descriptor*)grib_context_malloc_clear(c, sizeof(bufr_descriptor));
    if (!d) return NULL;
    d->context   = c;
    d->code      = code;
    d->F         = code / 100000;
    d->X         = (code - d->F * 100000) / 1000;
    d->Y         = (code - d->F * 100000) % 1000;
    strncpy(d->shortName, shortName, sizeof(d->shortName) - 1);
    strncpy(d->units, units, sizeof(d->units) - 1);
    d->scale     = scale;
    d->factor    = codes_power<double>(-scale, 10);
    d->reference = reference;
    d->width     = width;
    d->type      = type;
    d->nokey     = 0;
    return d;
}

/* ========== Test: descriptors_array basic lifecycle ========== */
static void test_descriptors_array_lifecycle()
{
    printf("Running %s ...\n", __func__);

    bufr_descriptors_array* a = grib_bufr_descriptors_array_new(10, 20);
    TEST_ASSERT(a != NULL);
    TEST_ASSERT(grib_bufr_descriptors_array_used_size(a) == 0);

    /* Push 3 descriptors */
    bufr_descriptor* d1 = make_test_descriptor(12101, "airTemperature", "K", 2, 0, 16, BUFR_DESCRIPTOR_TYPE_DOUBLE);
    bufr_descriptor* d2 = make_test_descriptor(11001, "windDirection", "deg", 0, 0, 9, BUFR_DESCRIPTOR_TYPE_LONG);
    bufr_descriptor* d3 = make_test_descriptor(11002, "windSpeed", "m/s", 1, 0, 12, BUFR_DESCRIPTOR_TYPE_DOUBLE);
    TEST_ASSERT(d1 != NULL);
    TEST_ASSERT(d2 != NULL);
    TEST_ASSERT(d3 != NULL);

    a = grib_bufr_descriptors_array_push(a, d1);
    a = grib_bufr_descriptors_array_push(a, d2);
    a = grib_bufr_descriptors_array_push(a, d3);
    TEST_ASSERT(grib_bufr_descriptors_array_used_size(a) == 3);

    /* Get and verify */
    bufr_descriptor* got = grib_bufr_descriptors_array_get(a, 0);
    TEST_ASSERT(got == d1);
    TEST_ASSERT(got->code == 12101);

    got = grib_bufr_descriptors_array_get(a, 1);
    TEST_ASSERT(got == d2);
    TEST_ASSERT(got->code == 11001);

    got = grib_bufr_descriptors_array_get(a, 2);
    TEST_ASSERT(got == d3);
    TEST_ASSERT(got->code == 11002);

    grib_bufr_descriptors_array_delete(a);
    TEST_PASS();
}

/* ========== Test: pop_front removes from front ========== */
static void test_descriptors_array_pop_front()
{
    printf("Running %s ...\n", __func__);

    bufr_descriptors_array* a = grib_bufr_descriptors_array_new(10, 20);
    TEST_ASSERT(a != NULL);

    bufr_descriptor* d1 = make_test_descriptor(1001, "wmoBlockNumber", "Numeric", 0, 0, 7, BUFR_DESCRIPTOR_TYPE_LONG);
    bufr_descriptor* d2 = make_test_descriptor(1002, "wmoStationNumber", "Numeric", 0, 0, 10, BUFR_DESCRIPTOR_TYPE_LONG);
    bufr_descriptor* d3 = make_test_descriptor(1003, "wmoRegionSubArea", "Numeric", 0, 0, 3, BUFR_DESCRIPTOR_TYPE_LONG);
    TEST_ASSERT(d1 != NULL && d2 != NULL && d3 != NULL);

    a = grib_bufr_descriptors_array_push(a, d1);
    a = grib_bufr_descriptors_array_push(a, d2);
    a = grib_bufr_descriptors_array_push(a, d3);
    TEST_ASSERT(grib_bufr_descriptors_array_used_size(a) == 3);

    /* Pop front should return d1 */
    bufr_descriptor* popped = grib_bufr_descriptors_array_pop_front(a);
    TEST_ASSERT(popped == d1);
    TEST_ASSERT(popped->code == 1001);
    TEST_ASSERT(grib_bufr_descriptors_array_used_size(a) == 2);

    /* Next element should now be d2 */
    bufr_descriptor* front = grib_bufr_descriptors_array_get(a, 0);
    TEST_ASSERT(front == d2);

    /* Pop front again */
    popped = grib_bufr_descriptors_array_pop_front(a);
    TEST_ASSERT(popped == d2);
    TEST_ASSERT(grib_bufr_descriptors_array_used_size(a) == 1);

    /* Free the popped descriptors manually (they were removed from the array) */
    grib_bufr_descriptor_delete(d1);
    grib_bufr_descriptor_delete(d2);

    grib_bufr_descriptors_array_delete(a);
    TEST_PASS();
}

/* ========== Test: array growth beyond initial capacity ========== */
static void test_descriptors_array_growth()
{
    printf("Running %s ...\n", __func__);

    /* Start with a small initial size */
    bufr_descriptors_array* a = grib_bufr_descriptors_array_new(5, 10);
    TEST_ASSERT(a != NULL);

    const int N = 50;
    for (int i = 0; i < N; i++) {
        bufr_descriptor* d = make_test_descriptor(i * 1000 + 1, "test", "Numeric", 0, 0, 8, BUFR_DESCRIPTOR_TYPE_LONG);
        TEST_ASSERT(d != NULL);
        a = grib_bufr_descriptors_array_push(a, d);
        TEST_ASSERT(a != NULL);
    }
    TEST_ASSERT(grib_bufr_descriptors_array_used_size(a) == (size_t)N);

    /* Verify all elements */
    for (int i = 0; i < N; i++) {
        bufr_descriptor* got = grib_bufr_descriptors_array_get(a, i);
        TEST_ASSERT(got != NULL);
        TEST_ASSERT(got->code == i * 1000 + 1);
    }

    grib_bufr_descriptors_array_delete(a);
    TEST_PASS();
}

/* ========== Test: descriptor clone produces independent copy ========== */
static void test_descriptor_clone()
{
    printf("Running %s ...\n", __func__);

    bufr_descriptor* orig = make_test_descriptor(12101, "airTemperature", "K", 2, 0.0, 16, BUFR_DESCRIPTOR_TYPE_DOUBLE);
    TEST_ASSERT(orig != NULL);

    bufr_descriptor* clone = grib_bufr_descriptor_clone(orig);
    TEST_ASSERT(clone != NULL);
    TEST_ASSERT(clone != orig); /* Different pointers */

    /* Values should match */
    TEST_ASSERT(clone->code == orig->code);
    TEST_ASSERT(clone->F == orig->F);
    TEST_ASSERT(clone->X == orig->X);
    TEST_ASSERT(clone->Y == orig->Y);
    TEST_ASSERT(clone->scale == orig->scale);
    TEST_ASSERT(clone->width == orig->width);
    TEST_ASSERT(clone->reference == orig->reference);
    TEST_ASSERT(clone->type == orig->type);
    TEST_ASSERT(strcmp(clone->shortName, orig->shortName) == 0);
    TEST_ASSERT(strcmp(clone->units, orig->units) == 0);

    /* Modifying clone should not affect original */
    clone->scale = 5;
    TEST_ASSERT(orig->scale == 2);

    grib_bufr_descriptor_delete(clone);
    grib_bufr_descriptor_delete(orig);
    TEST_PASS();
}

/* ========== Test: clone of NULL returns NULL ========== */
static void test_descriptor_clone_null()
{
    printf("Running %s ...\n", __func__);

    bufr_descriptor* clone = grib_bufr_descriptor_clone(NULL);
    TEST_ASSERT(clone == NULL);

    /* delete of NULL should not crash */
    grib_bufr_descriptor_delete(NULL);

    TEST_PASS();
}

/* ========== Test: set_scale changes type and factor ========== */
static void test_descriptor_set_scale()
{
    printf("Running %s ...\n", __func__);

    bufr_descriptor* d = make_test_descriptor(12101, "airTemperature", "K", 0, 0, 16, BUFR_DESCRIPTOR_TYPE_LONG);
    TEST_ASSERT(d != NULL);

    /* Initially scale=0, type is LONG */
    TEST_ASSERT(d->scale == 0);
    TEST_ASSERT(d->type == BUFR_DESCRIPTOR_TYPE_LONG);

    /* Setting non-zero scale should change type to DOUBLE */
    grib_bufr_descriptor_set_scale(d, 3);
    TEST_ASSERT(d->scale == 3);
    TEST_ASSERT(d->type == BUFR_DESCRIPTOR_TYPE_DOUBLE);
    /* factor should be 10^(-3) = 0.001 */
    TEST_ASSERT(fabs(d->factor - 0.001) < 1e-10);

    /* Setting scale to 0 keeps type as-is (only non-zero changes type) */
    grib_bufr_descriptor_set_scale(d, 0);
    TEST_ASSERT(d->scale == 0);
    /* factor should be 10^0 = 1.0 */
    TEST_ASSERT(fabs(d->factor - 1.0) < 1e-10);

    /* Negative scale => factor > 1 */
    grib_bufr_descriptor_set_scale(d, -2);
    TEST_ASSERT(d->scale == -2);
    TEST_ASSERT(d->type == BUFR_DESCRIPTOR_TYPE_DOUBLE);
    /* factor should be 10^2 = 100.0 */
    TEST_ASSERT(fabs(d->factor - 100.0) < 1e-10);

    grib_bufr_descriptor_delete(d);
    TEST_PASS();
}

/* ========== Test: can_be_missing logic ========== */
static void test_descriptor_can_be_missing()
{
    printf("Running %s ...\n", __func__);

    /* Normal descriptor: can be missing */
    bufr_descriptor* d1 = make_test_descriptor(12101, "airTemperature", "K", 2, 0, 16, BUFR_DESCRIPTOR_TYPE_DOUBLE);
    TEST_ASSERT(d1 != NULL);
    TEST_ASSERT(grib_bufr_descriptor_can_be_missing(d1) == 1);

    /* Code 31031 (data present indicator): cannot be missing */
    bufr_descriptor* d2 = make_test_descriptor(31031, "dataPresentIndicator", "FLAG TABLE", 0, 0, 1, BUFR_DESCRIPTOR_TYPE_FLAG);
    TEST_ASSERT(d2 != NULL);
    TEST_ASSERT(grib_bufr_descriptor_can_be_missing(d2) == 0);

    /* Code 999999 (associated field significance): cannot be missing */
    bufr_descriptor* d3 = make_test_descriptor(999999, "associatedField", "Numeric", 0, 0, 8, BUFR_DESCRIPTOR_TYPE_LONG);
    TEST_ASSERT(d3 != NULL);
    TEST_ASSERT(grib_bufr_descriptor_can_be_missing(d3) == 0);

    /* Width == 1: cannot be missing */
    bufr_descriptor* d4 = make_test_descriptor(20010, "cloudCover", "%", 0, 0, 1, BUFR_DESCRIPTOR_TYPE_LONG);
    TEST_ASSERT(d4 != NULL);
    TEST_ASSERT(grib_bufr_descriptor_can_be_missing(d4) == 0);

    grib_bufr_descriptor_delete(d1);
    grib_bufr_descriptor_delete(d2);
    grib_bufr_descriptor_delete(d3);
    grib_bufr_descriptor_delete(d4);
    TEST_PASS();
}

/* ========== Test: F/X/Y decomposition ========== */
static void test_descriptor_fxy()
{
    printf("Running %s ...\n", __func__);

    /* Element descriptor: F=0, X=12, Y=101 -> code 012101 */
    bufr_descriptor* d1 = make_test_descriptor(12101, "airTemp", "K", 0, 0, 16, BUFR_DESCRIPTOR_TYPE_DOUBLE);
    TEST_ASSERT(d1 != NULL);
    TEST_ASSERT(d1->F == 0);
    TEST_ASSERT(d1->X == 12);
    TEST_ASSERT(d1->Y == 101);

    /* Replication descriptor: F=1, X=2, Y=3 -> code 102003 */
    bufr_descriptor* d2 = make_test_descriptor(102003, "replication", "", 0, 0, 0, BUFR_DESCRIPTOR_TYPE_REPLICATION);
    TEST_ASSERT(d2 != NULL);
    TEST_ASSERT(d2->F == 1);
    TEST_ASSERT(d2->X == 2);
    TEST_ASSERT(d2->Y == 3);

    /* Operator descriptor: F=2, X=1, Y=129 -> code 201129 */
    bufr_descriptor* d3 = make_test_descriptor(201129, "operator", "", 0, 0, 0, BUFR_DESCRIPTOR_TYPE_OPERATOR);
    TEST_ASSERT(d3 != NULL);
    TEST_ASSERT(d3->F == 2);
    TEST_ASSERT(d3->X == 1);
    TEST_ASSERT(d3->Y == 129);

    /* Sequence descriptor: F=3, X=1, Y=11 -> code 301011 */
    bufr_descriptor* d4 = make_test_descriptor(301011, "sequence", "", 0, 0, 0, BUFR_DESCRIPTOR_TYPE_SEQUENCE);
    TEST_ASSERT(d4 != NULL);
    TEST_ASSERT(d4->F == 3);
    TEST_ASSERT(d4->X == 1);
    TEST_ASSERT(d4->Y == 11);

    grib_bufr_descriptor_delete(d1);
    grib_bufr_descriptor_delete(d2);
    grib_bufr_descriptor_delete(d3);
    grib_bufr_descriptor_delete(d4);
    TEST_PASS();
}

int main(int argc, char** argv)
{
    printf("Running BUFR descriptor unit tests...\n\n");

    test_descriptors_array_lifecycle();
    test_descriptors_array_pop_front();
    test_descriptors_array_growth();
    test_descriptor_clone();
    test_descriptor_clone_null();
    test_descriptor_set_scale();
    test_descriptor_can_be_missing();
    test_descriptor_fxy();

    printf("\n========================================\n");
    printf("BUFR descriptor tests: %d passed, %d failed\n", tests_passed, tests_failed);
    printf("========================================\n");

    return tests_failed > 0 ? 1 : 0;
}
