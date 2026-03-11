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
 * Unit tests for geo iterators and nearest-neighbor search.
 *
 * Tests geoiterator creation, traversal, point counting, and nearest-point
 * lookup for the regular_ll grid type from GRIB2 samples.
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

/* ========== Test: geoiterator creation and basic traversal ========== */
static void test_iterator_regular_ll()
{
    printf("Running %s ...\n", __func__);
    int err = 0;

    grib_handle* h = grib_handle_new_from_samples(NULL, "GRIB2");
    TEST_ASSERT(h != NULL);

    /* Set up data values so the iterator has something to traverse */
    size_t numValues = 0;
    err = grib_get_size(h, "values", &numValues);
    TEST_ASSERT(err == GRIB_SUCCESS);
    TEST_ASSERT(numValues > 0);

    double* vals = (double*)calloc(numValues, sizeof(double));
    TEST_ASSERT(vals != NULL);
    for (size_t i = 0; i < numValues; i++) {
        vals[i] = 273.15 + (double)(i % 50);
    }
    err = grib_set_double_array(h, "values", vals, numValues);
    TEST_ASSERT(err == GRIB_SUCCESS);

    /* Create iterator */
    grib_iterator* iter = grib_iterator_new(h, 0, &err);
    TEST_ASSERT(err == GRIB_SUCCESS);
    TEST_ASSERT(iter != NULL);

    /* Traverse all points */
    double lat = 0, lon = 0, value = 0;
    long count = 0;
    while (grib_iterator_next(iter, &lat, &lon, &value)) {
        /* Latitude should be in [-90, 90] */
        TEST_ASSERT(lat >= -90.001 && lat <= 90.001);
        /* Longitude should be in [0, 360] or [-180, 180] range */
        TEST_ASSERT(lon >= -180.001 && lon <= 360.001);
        count++;
    }

    /* Point count should match numberOfPoints */
    long expectedPoints = 0;
    err = grib_get_long(h, "numberOfPoints", &expectedPoints);
    TEST_ASSERT(err == GRIB_SUCCESS);
    TEST_ASSERT(count == expectedPoints);
    TEST_ASSERT((size_t)count == numValues);

    grib_iterator_delete(iter);
    free(vals);
    grib_handle_delete(h);
    TEST_PASS();
}

/* ========== Test: iterator first/last point matches grid metadata ========== */
static void test_iterator_endpoints()
{
    printf("Running %s ...\n", __func__);
    int err = 0;

    grib_handle* h = grib_handle_new_from_samples(NULL, "GRIB2");
    TEST_ASSERT(h != NULL);

    /* Get expected first and last grid points from metadata */
    double latFirst = 0, lonFirst = 0, latLast = 0, lonLast = 0;
    err = grib_get_double(h, "latitudeOfFirstGridPointInDegrees", &latFirst);
    TEST_ASSERT(err == GRIB_SUCCESS);
    err = grib_get_double(h, "longitudeOfFirstGridPointInDegrees", &lonFirst);
    TEST_ASSERT(err == GRIB_SUCCESS);
    err = grib_get_double(h, "latitudeOfLastGridPointInDegrees", &latLast);
    TEST_ASSERT(err == GRIB_SUCCESS);
    err = grib_get_double(h, "longitudeOfLastGridPointInDegrees", &lonLast);
    TEST_ASSERT(err == GRIB_SUCCESS);

    /* Set some data */
    size_t numValues = 0;
    err = grib_get_size(h, "values", &numValues);
    TEST_ASSERT(err == GRIB_SUCCESS);
    double* vals = (double*)calloc(numValues, sizeof(double));
    for (size_t i = 0; i < numValues; i++) vals[i] = 1.0;
    err = grib_set_double_array(h, "values", vals, numValues);
    TEST_ASSERT(err == GRIB_SUCCESS);

    /* Create iterator and check first point */
    grib_iterator* iter = grib_iterator_new(h, 0, &err);
    TEST_ASSERT(err == GRIB_SUCCESS && iter != NULL);

    double lat = 0, lon = 0, value = 0;
    int has_next = grib_iterator_next(iter, &lat, &lon, &value);
    TEST_ASSERT(has_next);
    TEST_ASSERT(fabs(lat - latFirst) < 0.01);
    TEST_ASSERT(fabs(lon - lonFirst) < 0.01);

    /* Skip to last point */
    double lastLat = lat, lastLon = lon;
    while (grib_iterator_next(iter, &lat, &lon, &value)) {
        lastLat = lat;
        lastLon = lon;
    }
    TEST_ASSERT(fabs(lastLat - latLast) < 0.01);
    TEST_ASSERT(fabs(lastLon - lonLast) < 0.01);

    grib_iterator_delete(iter);
    free(vals);
    grib_handle_delete(h);
    TEST_PASS();
}

/* ========== Test: nearest-neighbor search ========== */
static void test_nearest_regular_ll()
{
    printf("Running %s ...\n", __func__);
    int err = 0;

    grib_handle* h = grib_handle_new_from_samples(NULL, "GRIB2");
    TEST_ASSERT(h != NULL);

    /* Set data values */
    size_t numValues = 0;
    err = grib_get_size(h, "values", &numValues);
    TEST_ASSERT(err == GRIB_SUCCESS);
    double* vals = (double*)calloc(numValues, sizeof(double));
    for (size_t i = 0; i < numValues; i++) vals[i] = (double)i;
    err = grib_set_double_array(h, "values", vals, numValues);
    TEST_ASSERT(err == GRIB_SUCCESS);

    /* Find nearest point to (45.0, 10.0) */
    grib_nearest* nearest = grib_nearest_new(h, &err);
    TEST_ASSERT(err == GRIB_SUCCESS);
    TEST_ASSERT(nearest != NULL);

    double inlat = 45.0, inlon = 10.0;
    double outLats[4], outLons[4], outValues[4], outDistances[4];
    int outIndexes[4];
    size_t len = 4;

    err = grib_nearest_find(nearest, h, inlat, inlon, 0,
                            outLats, outLons, outValues, outDistances, outIndexes, &len);
    TEST_ASSERT(err == GRIB_SUCCESS);
    TEST_ASSERT(len == 4);

    /* All returned points should be reasonably close to the input */
    for (size_t i = 0; i < len; i++) {
        TEST_ASSERT(fabs(outLats[i] - inlat) < 5.0); /* within 5 degrees */
        TEST_ASSERT(outDistances[i] < 500.0); /* within 500 km (rough check) */
        TEST_ASSERT(outIndexes[i] >= 0);
        TEST_ASSERT((size_t)outIndexes[i] < numValues);
    }

    grib_nearest_delete(nearest);
    free(vals);
    grib_handle_delete(h);
    TEST_PASS();
}

/* ========== Test: iterator with NO_VALUES flag ========== */
static void test_iterator_no_values()
{
    printf("Running %s ...\n", __func__);
    int err = 0;

    grib_handle* h = grib_handle_new_from_samples(NULL, "GRIB2");
    TEST_ASSERT(h != NULL);

    /* Create iterator with GRIB_GEOITERATOR_NO_VALUES flag */
    grib_iterator* iter = grib_iterator_new(h, GRIB_GEOITERATOR_NO_VALUES, &err);
    TEST_ASSERT(err == GRIB_SUCCESS);
    TEST_ASSERT(iter != NULL);

    double lat = 0, lon = 0, value = 0;
    long count = 0;
    while (grib_iterator_next(iter, &lat, &lon, &value)) {
        TEST_ASSERT(lat >= -90.001 && lat <= 90.001);
        count++;
    }
    TEST_ASSERT(count > 0);

    grib_iterator_delete(iter);
    grib_handle_delete(h);
    TEST_PASS();
}

int main(int argc, char** argv)
{
    printf("Running geo iterator and nearest unit tests...\n\n");

    test_iterator_regular_ll();
    test_iterator_endpoints();
    test_nearest_regular_ll();
    test_iterator_no_values();

    printf("\n========================================\n");
    printf("Geo tests: %d passed, %d failed\n", tests_passed, tests_failed);
    printf("========================================\n");

    return tests_failed > 0 ? 1 : 0;
}
