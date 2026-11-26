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
 * Description: codes_get_double_element(s) of "values" with an index beyond the number of values
 *              must fail with GRIB_INVALID_ARGUMENT, with and without a bitmap, for simple and
 *              CCSDS packing. With a bitmap, out-of-range indexes returned the missing value or
 *              read beyond the bitmap values.
 */
#include <stdio.h>
#include <string.h>
#include "eccodes.h"
#include "grib_api_internal.h"

static int failures = 0;

static void check(const char* what, int err, size_t idx)
{
    if (err != GRIB_INVALID_ARGUMENT) {
        fprintf(stderr, "FAIL %s, index %zu: error %d (%s), expected GRIB_INVALID_ARGUMENT\n", what, idx, err,
                codes_get_error_message(err));
        failures++;
    }
}

static void run(const char* packing, int bitmap)
{
    const size_t n = 604;
    double values[604];
    char what[64];
    size_t len = strlen(packing), i;
    int err = 0;
    codes_handle* h = codes_grib_handle_new_from_samples(NULL, "GRIB2");
    snprintf(what, sizeof(what), "%s %s bitmap", packing, bitmap ? "with" : "without");
    for (i = 0; i < n; i++)
        values[i] = (bitmap && i % 4 == 0) ? 9999 : (double)(i % 17);
    CODES_CHECK(codes_set_long(h, "Ni", n), 0);
    CODES_CHECK(codes_set_long(h, "Nj", 1), 0);
    CODES_CHECK(codes_set_string(h, "packingType", packing, &len), 0);
    CODES_CHECK(codes_set_long(h, "bitsPerValue", 16), 0);
    if (bitmap) {
        CODES_CHECK(codes_set_long(h, "bitmapPresent", 1), 0);
        CODES_CHECK(codes_set_double(h, "missingValue", 9999), 0);
    }
    CODES_CHECK(codes_set_double_array(h, "values", values, n), 0);

    {
        const size_t bad[] = { n, n + 5, 100000 };
        for (i = 0; i < 3; i++) {
            double v = 0;
            int index[2] = { 0, (int)bad[i] };
            double vv[2];
            err = codes_get_double_element(h, "values", bad[i], &v);
            check(what, err, bad[i]);
            err = codes_get_double_elements(h, "values", index, 2, vv);
            check(what, err, bad[i]);
        }
    }
    codes_handle_delete(h);
}

int main(void)
{
    run("grid_simple", 0);
    run("grid_simple", 1);
    run("grid_ccsds", 0);
    run("grid_ccsds", 1);
    return failures ? 1 : 0;
}
