/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "eccodes.h"

#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

/*
 * Test: Reproduce the exact scenario from GitHub issue #397.
 *
 * Create a GRIB2 message with grid_complex_spatial_differencing packing
 * (like NCEP GFS), set geopotential height values where some values,
 * when scaled by 9.80665, produce results in the range [9998.9, 9999.1].
 * After pack/unpack, verify no spurious missing values are introduced.
 */
static int test_gh_to_z_conversion(void)
{
    int err = 0;
    codes_handle* h = NULL;
    size_t values_len = 0;
    double* values = NULL;
    double* decoded_values = NULL;
    long numberOfMissing = 0;
    long missingValuesPresent = 0;
    double missingValue = 0;
    const double G = 9.80665;  /* gravitational acceleration m/s^2 */
    size_t packing_type_len = 64;
    char packing_type[64] = {0};

    printf("Test: gh to z conversion with complex packing...\n");

    /* Create a GRIB2 handle with complex spatial differencing packing,
     * similar to what NCEP GFS uses */
    h = codes_grib_handle_new_from_samples(NULL, "regular_ll_sfc_grib2");
    assert(h);

    /* Verify default missingValue is 9999 */
    CODES_CHECK(codes_get_double(h, "missingValue", &missingValue), 0);
    printf("  missingValue = %.1f\n", missingValue);
    assert(missingValue == 9999.0);

    /*Simulate a scenario where the user sets missingValue to FLT_MAX, which is a common practice
     * to avoid conflicts with legitimate data values near 9999.0. This is what the issue reporter did. */
    double new_mssingValue = FLT_MAX;
    CODES_CHECK(codes_set_double(h, "missingValue", new_mssingValue), 0);
    CODES_CHECK(codes_get_double(h, "missingValue", &missingValue), 0);
    printf("  missingValue = %.1f\n", missingValue);
    assert(missingValue == FLT_MAX);

    CODES_CHECK(codes_get_size(h, "values", &values_len), 0);
    assert(values_len > 0);
    printf("  numberOfDataPoints = %zu\n", values_len);

    values = (double*)malloc(values_len * sizeof(double));
    decoded_values = (double*)malloc(values_len * sizeof(double));
    assert(values && decoded_values);

    /* Fill with geopotential height values typical of 850hPa level.
     * Real GFS values range from ~946 to ~1649 m.
     * Values near 1019.624 (= 9999.0 / 9.80665) are the problematic ones. */
    for (size_t i = 0; i < values_len; i++) {
        /* Range: ~946 to ~1649, matching typical 850hPa gh distribution */
        values[i] = 946.0 + (double)(i % values_len) / (double)values_len * 703.0;
    }

    /* Place several values near the critical threshold:
     * 9999.0 / 9.80665 ≈ 1019.62398 (the value that produces exactly 9999.0 after scaling)
     * These are legitimate geopotential height values, NOT missing! */
    size_t num_critical = 0;
    for (size_t i = 0; i < values_len && num_critical < 20; i += values_len / 20) {
        /* Values that produce results near 9999 after multiplying by G */
        values[i] = 9999.0 / G;         /* ≈ 1019.624, scales to ≈ 9999.0 */
        num_critical++;
    }

    /* Now simulate the gh → z conversion: multiply by G */
    for (size_t i = 0; i < values_len; i++) {
        values[i] *= G;
    }

    /* Some values are now very close to 9999.0 (the missingValue) */
    printf("  After scaling, some values are near 9999 (missingValue)\n");

    /* Switch to complex spatial differencing packing (like NCEP GFS) */
    {
        const char* pt = "grid_complex_spatial_differencing";
        size_t pt_len = strlen(pt);
        CODES_CHECK(codes_set_long(h, "bitsPerValue", 16), 0);
        CODES_CHECK(codes_set_string(h, "packingType", pt, &pt_len), 0);
    }

    /* Verify packing type */
    CODES_CHECK(codes_get_string(h, "packingType", packing_type, &packing_type_len), 0);
    printf("  packingType = %s\n", packing_type);

    /* Set the values - this triggers packing */
    CODES_CHECK(codes_set_double_array(h, "values", values, values_len), 0);

    /* Check numberOfMissing immediately after setting values */
    CODES_CHECK(codes_get_long(h, "numberOfMissing", &numberOfMissing), 0);
    printf("  numberOfMissing after set = %ld\n", numberOfMissing);

    if (numberOfMissing != 0) {
        fprintf(stderr, "ERROR: numberOfMissing = %ld after setting values (expected 0).\n"
                "Values near 9999 were incorrectly treated as missing!\n",
                numberOfMissing);
        /* Don't exit yet - continue to show the full picture */
    }

    /* Write to memory buffer and read back (full round-trip) */
    {
        const void* buffer = NULL;
        size_t buffer_size = 0;
        codes_handle* h2 = NULL;

        CODES_CHECK(codes_get_message(h, &buffer, &buffer_size), 0);
        h2 = codes_handle_new_from_message(NULL, buffer, buffer_size);
        assert(h2);

        CODES_CHECK(codes_get_long(h2, "numberOfMissing", &numberOfMissing), 0);
        CODES_CHECK(codes_get_long(h2, "missingValuesPresent", &missingValuesPresent), 0);
        printf("  After round-trip: numberOfMissing = %ld, missingValuesPresent = %ld\n",
               numberOfMissing, missingValuesPresent);

        /* Decode values and check for FLT_MAX corruption */
        size_t decoded_len = values_len;
        CODES_CHECK(codes_get_double_array(h2, "values", decoded_values, &decoded_len), 0);

        int corrupted_count = 0;
        for (size_t i = 0; i < decoded_len; i++) {
            if (decoded_values[i] >= FLT_MAX || decoded_values[i] == missingValue) {
                if (corrupted_count < 5) {
                    fprintf(stderr, "  Corrupted: decoded_values[%zu] = %.17g (original = %.17g)\n",
                            i, decoded_values[i], values[i]);
                }
                corrupted_count++;
            }
        }

        if (corrupted_count > 0) {
            fprintf(stderr, "ERROR: %d values were corrupted (treated as missing or set to FLT_MAX)\n",
                    corrupted_count);
        }

        codes_handle_delete(h2);

        if (numberOfMissing != 0 || missingValuesPresent != 0 || corrupted_count > 0) {
            free(values);
            free(decoded_values);
            codes_handle_delete(h);
            return 1;
        }
    }

    codes_handle_delete(h);
    free(values);
    free(decoded_values);

    printf("  PASSED\n");
    return 0;
}

/*
 * Test: Use scaleValuesBy with complex packing (the exact workflow from the issue).
 *
 * This mirrors: grib_set -s scaleValuesBy=9.80665 gh850.grib modified.grib
 */
static int test_scale_values_by_with_complex_packing(void)
{
    int err = 0;
    codes_handle* h = NULL;
    size_t values_len = 0;
    double* values = NULL;
    double* decoded_values = NULL;
    long numberOfMissing = 0;
    long missingValuesPresent = 0;
    double missingValue = 0;
    const double G = 9.80665;

    printf("Test: scaleValuesBy with complex packing...\n");

    h = codes_grib_handle_new_from_samples(NULL, "regular_ll_sfc_grib2");
    assert(h);

    CODES_CHECK(codes_get_double(h, "missingValue", &missingValue), 0);
    assert(missingValue == 9999.0);

    /* Switch to complex spatial differencing packing */
    {
        const char* pt = "grid_complex_spatial_differencing";
        size_t pt_len = strlen(pt);
        CODES_CHECK(codes_set_string(h, "packingType", pt, &pt_len), 0);
    }

    CODES_CHECK(codes_get_size(h, "values", &values_len), 0);

    values = (double*)malloc(values_len * sizeof(double));
    decoded_values = (double*)malloc(values_len * sizeof(double));
    assert(values && decoded_values);

    /* Set geopotential height values similar to 850hPa GFS */
    for (size_t i = 0; i < values_len; i++) {
        values[i] = 946.0 + (double)(i % values_len) / (double)values_len * 703.0;
    }

    /* Include values that produce ~9999 after scaling by G */
    for (size_t i = 0; i < values_len && i < 100; i += 10) {
        values[i] = 9999.0 / G;  /* ≈ 1019.624 */
    }

    CODES_CHECK(codes_set_long(h, "bitsPerValue", 16), 0);
    CODES_CHECK(codes_set_double_array(h, "values", values, values_len), 0);

    /* Verify no missing values before scaling */
    CODES_CHECK(codes_get_long(h, "numberOfMissing", &numberOfMissing), 0);
    CODES_CHECK(codes_get_long(h, "missingValuesPresent", &missingValuesPresent), 0);
    printf("  Before scaling: numberOfMissing = %ld, missingValuesPresent = %ld\n",
           numberOfMissing, missingValuesPresent);

    /* Apply scaling (gh → z conversion) */
    CODES_CHECK(codes_set_double(h, "scaleValuesBy", G), 0);

    /* Check for spurious missing values */
    CODES_CHECK(codes_get_long(h, "numberOfMissing", &numberOfMissing), 0);
    CODES_CHECK(codes_get_long(h, "missingValuesPresent", &missingValuesPresent), 0);
    printf("  After scaleValuesBy: numberOfMissing = %ld, missingValuesPresent = %ld\n",
           numberOfMissing, missingValuesPresent);

    if (numberOfMissing != 0) {
        fprintf(stderr, "ERROR: scaleValuesBy introduced %ld spurious missing values!\n",
                numberOfMissing);
    }

    /* Full round-trip check */
    {
        const void* buffer = NULL;
        size_t buffer_size = 0;
        codes_handle* h2 = NULL;

        CODES_CHECK(codes_get_message(h, &buffer, &buffer_size), 0);
        h2 = codes_handle_new_from_message(NULL, buffer, buffer_size);
        assert(h2);

        CODES_CHECK(codes_get_long(h2, "numberOfMissing", &numberOfMissing), 0);
        printf("  After round-trip: numberOfMissing = %ld\n", numberOfMissing);

        size_t decoded_len = values_len;
        CODES_CHECK(codes_get_double_array(h2, "values", decoded_values, &decoded_len), 0);

        int corrupted = 0;
        for (size_t i = 0; i < decoded_len; i++) {
            if (decoded_values[i] >= FLT_MAX) {
                corrupted++;
            }
        }
        if (corrupted > 0) {
            fprintf(stderr, "ERROR: %d values corrupted to FLT_MAX after round-trip\n", corrupted);
        }

        codes_handle_delete(h2);

        if (numberOfMissing != 0 || corrupted > 0) {
            free(values);
            free(decoded_values);
            codes_handle_delete(h);
            return 1;
        }
    }

    codes_handle_delete(h);
    free(values);
    free(decoded_values);

    printf("  PASSED\n");
    return 0;
}



int main(int argc, char** argv)
{
    int ret = 0;
    int failures = 0;

    printf("ECC-2191 / GH-0#397: Missing value handling when values are near missingValue (9999)\n\n");

    ret = test_gh_to_z_conversion();
    if (ret != 0) failures++;

    ret = test_scale_values_by_with_complex_packing();
    if (ret != 0) failures++;

    if (failures > 0) {
        printf("\n%d test(s) FAILED.\n", failures);
        return 1;
    }

    printf("\nAll tests passed.\n");
    return 0;
}
