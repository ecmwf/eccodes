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
 * Description: when the requested decimal precision cannot be coded, the legacy simple packing
 *              reported success and wrote a field with bitsPerValue=0, i.e. a message that
 *              declares a constant field: every value decoded as the reference value, with a
 *              relative error of 1. The number of bits was searched with
 *              number_of_bits((unsigned long)ceil(fabs(max - min))): for a range that does not fit
 *              an unsigned long the cast is undefined and gave 0, which number_of_bits() then
 *              accepted as "0 bits per value". The new simple packing (ECCODES_USE_SP=1) refuses
 *              and leaves the message unchanged (see tests/grib_repack_failure.cc); the legacy path
 *              must do the same.
 *              - GRIB2: changeDecimalPrecision=0 on a field whose range is 5e26 (89 bits needed)
 *              - GRIB1: changeDecimalPrecision=40 on a field whose range is 5 (233 bits needed)
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "eccodes.h"

static int failures = 0;

/* Copy of the coded message, to compare it after a repack that must fail */
static unsigned char* message_copy(codes_handle* h, size_t* size)
{
    const void* message = NULL;
    unsigned char* copy = NULL;
    CODES_CHECK(codes_get_message(h, &message, size), 0);
    copy = (unsigned char*)malloc(*size);
    memcpy(copy, message, *size);
    return copy;
}

static void run(const char* what, const char* sample, double base, double step, long precision)
{
    size_t n = 0, size_before = 0, size_after = 0, i = 0;
    double* values = NULL;
    double* old = NULL;
    unsigned char* before = NULL;
    unsigned char* after = NULL;
    int err = 0;
    codes_handle* h = codes_grib_handle_new_from_samples(NULL, sample);

    if (!h) {
        fprintf(stderr, "Cannot create a handle from sample %s\n", sample);
        exit(1);
    }
    CODES_CHECK(codes_get_size(h, "values", &n), 0);
    values = (double*)malloc(n * sizeof(double));
    old = (double*)malloc(n * sizeof(double));
    for (i = 0; i < n; i++)
        values[i] = base + step * (double)i;

    /* A field that can be packed, and the message and the values it holds */
    CODES_CHECK(codes_set_double_array(h, "values", values, n), 0);
    CODES_CHECK(codes_get_double_array(h, "values", old, &n), 0);
    before = message_copy(h, &size_before);

    /* Repacking it with the precision that cannot be coded must fail and change nothing */
    err = codes_set_long(h, "changeDecimalPrecision", precision);
    after = message_copy(h, &size_after);

    if (err == GRIB_SUCCESS) {
        fprintf(stderr, "FAIL %s: a precision that cannot be coded was accepted\n", what);
        ++failures;
    }
    else {
        printf("%s: %s\n", what, codes_get_error_message(err));
    }

    if (size_after != size_before || memcmp(before, after, size_before) != 0) {
        fprintf(stderr, "FAIL %s: the failed repack changed the message\n", what);
        ++failures;
    }

    {
        size_t count = n;
        double* now = (double*)malloc(n * sizeof(double));
        CODES_CHECK(codes_get_double_array(h, "values", now, &count), 0);
        for (i = 0; i < n; i++) {
            if (now[i] != old[i]) {
                fprintf(stderr, "FAIL %s: value %zu is %.17g, expected %.17g\n", what, i, now[i], old[i]);
                ++failures;
                break;
            }
        }
        free(now);
    }

    free(after);
    free(before);
    free(old);
    free(values);
    codes_handle_delete(h);
}

int main(void)
{
    /* A range of 5e26 with 0 decimal digits needs 89 bits per value */
    run("GRIB2", "GRIB2", 1e30, 1e24, 0);
    /* A range of 6e2 with 40 decimal digits needs 233 bits per value */
    run("GRIB1", "GRIB1", 0.0, 1e-2, 40);
    return failures ? 1 : 0;
}
