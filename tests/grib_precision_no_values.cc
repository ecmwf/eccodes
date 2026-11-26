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
 * Description: changing the decimal precision of a field that has no values must not fail.
 *              DecimalPrecision::pack_long() allocated the values with grib_context_malloc(c, 0),
 *              which returns NULL, and reported GRIB_OUT_OF_MEMORY (-17) for a field whose number
 *              of coded values is 0 (here: a GRIB1 field with bitmapPresent=1 and no unmasked
 *              point). It must do what it does when it has no values accessor at all: set the
 *              packing keys and return success.
 */
#include <stdio.h>
#include <stdlib.h>
#include "eccodes.h"

static int failures = 0;

int main(void)
{
    codes_handle* h = codes_grib_handle_new_from_samples(NULL, "GRIB1");
    size_t n = 1;
    long bitsPerValue = -1, decimalScaleFactor = -1;
    int err = 0;

    if (!h) {
        fprintf(stderr, "Cannot create a handle from sample GRIB1\n");
        return 1;
    }

    CODES_CHECK(codes_set_long(h, "bitmapPresent", 1), 0);
    CODES_CHECK(codes_get_size(h, "values", &n), 0);
    if (n != 0) {
        fprintf(stderr, "FAIL: a GRIB1 field with bitmapPresent=1 has %zu values, expected 0\n", n);
        return 1;
    }

    err = codes_set_long(h, "changeDecimalPrecision", 2);
    if (err != GRIB_SUCCESS) {
        fprintf(stderr, "FAIL changeDecimalPrecision on a field without values: %d (%s)\n", err, codes_get_error_message(err));
        ++failures;
    }

    /* The keys that describe the precision are set, like for any other field */
    if ((err = codes_get_long(h, "decimalScaleFactor", &decimalScaleFactor)) != 0 || decimalScaleFactor != 2) {
        fprintf(stderr, "FAIL decimalScaleFactor is %ld (error %d), expected 2\n", decimalScaleFactor, err);
        ++failures;
    }
    if ((err = codes_get_long(h, "bitsPerValue", &bitsPerValue)) != 0 || bitsPerValue != 0) {
        fprintf(stderr, "FAIL bitsPerValue is %ld (error %d), expected 0\n", bitsPerValue, err);
        ++failures;
    }

    codes_handle_delete(h);
    return failures ? 1 : 0;
}
