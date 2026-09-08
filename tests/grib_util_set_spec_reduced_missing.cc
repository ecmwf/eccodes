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
 * Test that iDirectionIncrement and Ni are set to GRIB_MISSING_LONG for
 * reduced grids (reduced_gg, reduced_rotated_gg, reduced_ll).
 *
 * Reproduces the issue fixed in commit bc0ae2be9342:
 *   "eccodes + metkit: iDirectionIncrement/Ni=MISSING for reduced grids"
 *
 * The bug:
 *   grib_set_from_grid_spec() modifies a handle in-place without creating a
 *   fresh sample. When a regular_ll handle (where Ni and iDirectionIncrement
 *   have valid values) is repurposed as a reduced grid, those stale keys were
 *   not reset to GRIB_MISSING_LONG.
 *
 *   grib_util_set_spec_() had the same omission: although it loads a sample
 *   that typically has these keys as MISSING, the explicit setting ensures
 *   correctness regardless of the sample's content.
 */

#include "grib_api_internal.h"
#include "eccodes.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>

/* Check that a key's value equals GRIB_MISSING_LONG */
static void check_is_missing_long(grib_handle* h, const char* key, const char* context)
{
    long val = 0;
    int err = grib_get_long(h, key, &val);
    if (err) {
        fprintf(stderr, "FAIL [%s]: grib_get_long(\"%s\") returned error %d\n",
                context, key, err);
        exit(1);
    }
    if (val != GRIB_MISSING_LONG) {
        fprintf(stderr, "FAIL [%s]: expected \"%s\" = GRIB_MISSING_LONG (%ld), got %ld\n",
                context, key, (long)GRIB_MISSING_LONG, val);
        exit(1);
    }
    fprintf(stderr, "  OK [%s]: \"%s\" = GRIB_MISSING_LONG\n", context, key);
}

/*
 * Core bug reproducer: grib_set_from_grid_spec on a regular_ll handle
 * repurposed as a reduced grid.
 *
 * grib_set_from_grid_spec modifies the handle in-place (no fresh sample).
 * Before the fix, Ni and iDirectionIncrement retained the regular_ll values.
 */
static void test_set_from_grid_spec_reduced_gg()
{
    fprintf(stderr, "\n=== %s ===\n", __func__);
    int err = 0;

    /* Start with a regular_ll handle where Ni and iDirectionIncrement are set */
    grib_handle* h = grib_handle_new_from_samples(nullptr, "GRIB2");
    ECCODES_ASSERT(h);

    /* Verify the handle starts with real (non-missing) Ni */
    {
        long Ni = 0;
        CODES_CHECK(grib_get_long(h, "Ni", &Ni), 0);
        fprintf(stderr, "  Before: Ni=%ld (should be a real value, not MISSING)\n", Ni);
        ECCODES_ASSERT(Ni != GRIB_MISSING_LONG);
    }
    {
        long iDirInc = 0;
        CODES_CHECK(grib_get_long(h, "iDirectionIncrement", &iDirInc), 0);
        fprintf(stderr, "  Before: iDirectionIncrement=%ld (should be a real value)\n", iDirInc);
        ECCODES_ASSERT(iDirInc != GRIB_MISSING_LONG);
    }

    /* Set up the grid spec as reduced Gaussian */
    grib_util_grid_spec spec = {0,};
    spec.grid_type                          = GRIB_UTIL_GRID_SPEC_REDUCED_GG;
    spec.N                                  = 32;
    spec.Nj                                 = 2 * spec.N;
    spec.latitudeOfFirstGridPointInDegrees  = 87.863799;
    spec.longitudeOfFirstGridPointInDegrees = 0.0;
    spec.latitudeOfLastGridPointInDegrees   = -87.863799;
    spec.longitudeOfLastGridPointInDegrees  = 357.1875;
    spec.bitmapPresent                      = 0;

    grib_util_packing_spec packing_spec = {0,};

    err = grib_set_from_grid_spec(h, &spec, &packing_spec);
    ECCODES_ASSERT(err == 0);

    /* After converting to reduced_gg, Ni and iDirectionIncrement MUST be MISSING */
    check_is_missing_long(h, "Ni", "set_from_grid_spec/reduced_gg");
    check_is_missing_long(h, "iDirectionIncrement", "set_from_grid_spec/reduced_gg");

    /* Also verify ijDirectionIncrementGiven is 0 */
    {
        long given = -1;
        CODES_CHECK(grib_get_long(h, "ijDirectionIncrementGiven", &given), 0);
        ECCODES_ASSERT(given == 0);
        fprintf(stderr, "  OK [set_from_grid_spec/reduced_gg]: ijDirectionIncrementGiven=%ld\n", given);
    }

    grib_handle_delete(h);
}

/*
 * Same test but for reduced_ll
 */
static void test_set_from_grid_spec_reduced_ll()
{
    fprintf(stderr, "\n=== %s ===\n", __func__);
    int err = 0;

    grib_handle* h = grib_handle_new_from_samples(nullptr, "GRIB2");
    ECCODES_ASSERT(h);

    {
        long Ni = 0;
        CODES_CHECK(grib_get_long(h, "Ni", &Ni), 0);
        fprintf(stderr, "  Before: Ni=%ld\n", Ni);
        ECCODES_ASSERT(Ni != GRIB_MISSING_LONG);
    }

    grib_util_grid_spec spec = {0,};
    spec.grid_type                          = GRIB_UTIL_GRID_SPEC_REDUCED_LL;
    spec.Nj                                 = 4;
    spec.latitudeOfFirstGridPointInDegrees  = 90.0;
    spec.longitudeOfFirstGridPointInDegrees = 0.0;
    spec.latitudeOfLastGridPointInDegrees   = -90.0;
    spec.longitudeOfLastGridPointInDegrees  = 360.0;
    spec.bitmapPresent                      = 0;

    grib_util_packing_spec packing_spec = {0,};

    err = grib_set_from_grid_spec(h, &spec, &packing_spec);
    ECCODES_ASSERT(err == 0);

    check_is_missing_long(h, "Ni", "set_from_grid_spec/reduced_ll");
    check_is_missing_long(h, "iDirectionIncrement", "set_from_grid_spec/reduced_ll");

    {
        long given = -1;
        CODES_CHECK(grib_get_long(h, "ijDirectionIncrementGiven", &given), 0);
        ECCODES_ASSERT(given == 0);
        fprintf(stderr, "  OK [set_from_grid_spec/reduced_ll]: ijDirectionIncrementGiven=%ld\n", given);
    }

    grib_handle_delete(h);
}

/*
 * Test via grib_util_set_spec (the legacy path).
 * Although this path loads a fresh sample (which may already have MISSING),
 * the explicit SET_LONG_VALUE ensures correctness regardless of sample content.
 *
 * To demonstrate the issue, we start from a regular_ll input handle.
 */
static void test_util_set_spec_reduced_gg()
{
    fprintf(stderr, "\n=== %s ===\n", __func__);
    int err = 0;

    /* Start from a regular_ll input handle */
    grib_handle* handle = grib_handle_new_from_samples(nullptr, "GRIB2");
    ECCODES_ASSERT(handle);

    /* Get the pl array from a reduced_gg sample for realistic data */
    grib_handle* rgg_sample = grib_handle_new_from_samples(nullptr, "reduced_gg_pl_32_grib2");
    ECCODES_ASSERT(rgg_sample);

    size_t pl_size = 0;
    CODES_CHECK(grib_get_size(rgg_sample, "pl", &pl_size), 0);
    long* pl = (long*)malloc(sizeof(long) * pl_size);
    CODES_CHECK(grib_get_long_array(rgg_sample, "pl", pl, &pl_size), 0);

    size_t total_points = 0;
    for (size_t i = 0; i < pl_size; i++) total_points += pl[i];

    double* values = (double*)calloc(total_points, sizeof(double));
    values[0] = 4.4;
    values[1] = 5.5;
    for (size_t i = 2; i < total_points; i++) values[i] = (double)i * 0.01;

    grib_util_grid_spec spec = {0,};
    spec.grid_type                          = GRIB_UTIL_GRID_SPEC_REDUCED_GG;
    spec.N                                  = 32;
    spec.Nj                                 = 2 * spec.N;
    spec.latitudeOfFirstGridPointInDegrees  = 87.863799;
    spec.longitudeOfFirstGridPointInDegrees = 0.0;
    spec.latitudeOfLastGridPointInDegrees   = -87.863799;
    spec.longitudeOfLastGridPointInDegrees  = 357.1875;
    spec.bitmapPresent                      = 0;
    spec.pl                                 = pl;
    spec.pl_size                            = (long)pl_size;

    grib_util_packing_spec packing_spec = {0,};
    packing_spec.packing_type = GRIB_UTIL_PACKING_TYPE_GRID_SIMPLE;
    packing_spec.bitsPerValue = 16;
    packing_spec.accuracy     = GRIB_UTIL_ACCURACY_USE_PROVIDED_BITS_PER_VALUES;
    packing_spec.packing      = GRIB_UTIL_PACKING_USE_PROVIDED;

    int set_spec_flags = 0;
    grib_handle* result = grib_util_set_spec(
        handle, &spec, &packing_spec, set_spec_flags,
        values, total_points, &err);
    ECCODES_ASSERT(result);
    ECCODES_ASSERT(err == 0);

    check_is_missing_long(result, "Ni", "util_set_spec/reduced_gg");
    check_is_missing_long(result, "iDirectionIncrement", "util_set_spec/reduced_gg");

    {
        long given = -1;
        CODES_CHECK(grib_get_long(result, "ijDirectionIncrementGiven", &given), 0);
        ECCODES_ASSERT(given == 0);
        fprintf(stderr, "  OK [util_set_spec/reduced_gg]: ijDirectionIncrementGiven=%ld\n", given);
    }

    grib_handle_delete(result);
    grib_handle_delete(handle);
    grib_handle_delete(rgg_sample);
    free(values);
    free(pl);
}

/*
 * Contrast test: regular lat-lon grid should have valid (non-missing) Ni.
 */
static void test_regular_ll_not_missing()
{
    fprintf(stderr, "\n=== %s ===\n", __func__);
    int err = 0;

    grib_handle* h = grib_handle_new_from_samples(nullptr, "GRIB2");
    ECCODES_ASSERT(h);

    grib_util_grid_spec spec = {0,};
    spec.grid_type                          = GRIB_UTIL_GRID_SPEC_REGULAR_LL;
    spec.Ni                                 = 10;
    spec.Nj                                 = 5;
    spec.iDirectionIncrementInDegrees       = 1.0;
    spec.jDirectionIncrementInDegrees       = 1.0;
    spec.latitudeOfFirstGridPointInDegrees  = 90.0;
    spec.longitudeOfFirstGridPointInDegrees = 0.0;
    spec.latitudeOfLastGridPointInDegrees   = 86.0;
    spec.longitudeOfLastGridPointInDegrees  = 9.0;
    spec.bitmapPresent                      = 0;

    grib_util_packing_spec packing_spec = {0,};

    err = grib_set_from_grid_spec(h, &spec, &packing_spec);
    ECCODES_ASSERT(err == 0);

    {
        long Ni_val = 0;
        CODES_CHECK(grib_get_long(h, "Ni", &Ni_val), 0);
        ECCODES_ASSERT(Ni_val != GRIB_MISSING_LONG);
        ECCODES_ASSERT(Ni_val == 10);
        fprintf(stderr, "  OK [regular_ll]: Ni=%ld (not MISSING, as expected)\n", Ni_val);
    }

    grib_handle_delete(h);
}

int main()
{
    /* Core bug: grib_set_from_grid_spec on handle with stale Ni/iDirectionIncrement */
    test_set_from_grid_spec_reduced_gg();
    test_set_from_grid_spec_reduced_ll();

    /* Also test via grib_util_set_spec (legacy path) */
    test_util_set_spec_reduced_gg();

    /* Contrast: regular lat-lon should have valid Ni */
    test_regular_ll_not_missing();

    fprintf(stderr, "\nAll tests passed.\n");
    return 0;
}
