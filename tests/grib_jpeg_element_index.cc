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
 * Description: element access to "values" of a JPEG2000 packed field (GRIB-564: the index relates
 *              to codedValues) must fail with GRIB_INVALID_ARGUMENT when the index is not smaller
 *              than the number of values:
 *              - index numberOfValues read one element past the codedValues buffer and succeeded
 *              - on a constant field (bitsPerValue=0) every index returned the reference value
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "eccodes.h"

static int failures = 0;

static void check_bad(const char* what, size_t idx, int err)
{
    if (err != GRIB_INVALID_ARGUMENT) {
        fprintf(stderr, "FAIL %s, index %zu: error %d (%s), expected GRIB_INVALID_ARGUMENT\n",
                what, idx, err, codes_get_error_message(err));
        ++failures;
    }
}

static void check_good(const char* what, size_t idx, int err)
{
    if (err != GRIB_SUCCESS) {
        fprintf(stderr, "FAIL %s, index %zu: error %d (%s), expected GRIB_SUCCESS\n",
                what, idx, err, codes_get_error_message(err));
        ++failures;
    }
}

static void run(const char* what, int constant)
{
    const char* packing = "grid_jpeg";
    const size_t n = 12;
    double values[12];
    double v = 0;
    size_t i = 0;
    size_t len = strlen(packing);
    int err = 0;
    codes_handle* h = codes_grib_handle_new_from_samples(NULL, "GRIB2");

    if (!h) {
        fprintf(stderr, "Cannot create a handle from sample GRIB2\n");
        exit(1);
    }

    for (i = 0; i < n; i++)
        values[i] = constant ? 5.0 : (double)(i + 1);

    CODES_CHECK(codes_set_long(h, "Ni", n), 0);
    CODES_CHECK(codes_set_long(h, "Nj", 1), 0);
    CODES_CHECK(codes_set_string(h, "packingType", packing, &len), 0);
    if (!constant)
        CODES_CHECK(codes_set_long(h, "bitsPerValue", 12), 0);
    CODES_CHECK(codes_set_double_array(h, "values", values, n), 0);

    {
        long bits_per_value = -1;
        CODES_CHECK(codes_get_long(h, "bitsPerValue", &bits_per_value), 0);
        if (constant && bits_per_value != 0) {
            fprintf(stderr, "FAIL %s: bitsPerValue is %ld, expected 0\n", what, bits_per_value);
            ++failures;
        }
    }

    for (i = 0; i < n; i++) {
        err = codes_get_double_element(h, "values", i, &v);
        check_good(what, i, err);
    }

    {
        const size_t bad[] = { n, n + 5, 100000 };
        for (i = 0; i < 3; i++) {
            int index[2] = { 0, (int)bad[i] };
            double vv[2] = { 0, 0 };
            err = codes_get_double_element(h, "values", bad[i], &v);
            check_bad(what, bad[i], err);
            err = codes_get_double_elements(h, "values", index, 2, vv);
            check_bad(what, bad[i], err);
        }
    }

    codes_handle_delete(h);
}

int main(void)
{
    run("grid_jpeg", 0);
    run("grid_jpeg constant", 1);
    return failures ? 1 : 0;
}
