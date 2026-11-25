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
 * Description: a repack that fails must leave the message unchanged, with the legacy and the new
 *              simple packing (ECCODES_USE_SP=1):
 *              - setBitsPerValue=64 fails (more than 63 bits per value are not supported) and left
 *                bitsPerValue=64 on the data packed with fewer bits
 *              - changeDecimalPrecision on a field whose range needs more than 64 bits at that
 *                precision fails with the new simple packing and left the new decimalScaleFactor
 *                and bitsPerValue=0 on the old data (all values decoded as the reference value)
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "eccodes.h"

static int failures = 0;

/* Copy of the coded message, to compare it after a failed repack */
static unsigned char* message_copy(codes_handle* h, size_t* size)
{
    const void* msg = NULL;
    unsigned char* copy = NULL;
    CODES_CHECK(codes_get_message(h, &msg, size), 0);
    copy = (unsigned char*)malloc(*size);
    memcpy(copy, msg, *size);
    return copy;
}

static void run(const char* sample, const char* key, long value)
{
    codes_handle* h = codes_grib_handle_new_from_samples(NULL, sample);
    size_t n = 0, i = 0, size_before = 0, size_after = 0;
    double* values = NULL;
    unsigned char* before = NULL;
    unsigned char* after = NULL;
    int err = 0;

    if (!h) {
        fprintf(stderr, "Cannot create a handle from sample %s\n", sample);
        exit(1);
    }
    CODES_CHECK(codes_get_size(h, "values", &n), 0);
    values = (double*)malloc(n * sizeof(double));
    for (i = 0; i < n; ++i) {
        /* Range of 1e25: representable with 16 bits, but not with 1 decimal digit in 64 bits */
        values[i] = (i % 2) ? 1e-25 : 1e25 * (1 + 1e-3 * (double)i);
    }
    CODES_CHECK(codes_set_long(h, "bitsPerValue", 16), 0);
    CODES_CHECK(codes_set_double_array(h, "values", values, n), 0);
    before = message_copy(h, &size_before);

    err = codes_set_long(h, key, value);
    after = message_copy(h, &size_after);
    if (err != CODES_SUCCESS && (size_after != size_before || memcmp(before, after, size_before) != 0)) {
        fprintf(stderr, "%s: %s=%ld failed (%s) and changed the message\n", sample, key, value, codes_get_error_message(err));
        ++failures;
    }
    printf("%s: %s=%ld: %s\n", sample, key, value, err ? codes_get_error_message(err) : "success");

    free(after);
    free(before);
    free(values);
    codes_handle_delete(h);
}

int main(void)
{
    const char* samples[] = { "GRIB2", "regular_ll_pl_grib1" };
    size_t i = 0;
    for (i = 0; i < sizeof(samples) / sizeof(samples[0]); ++i) {
        run(samples[i], "setBitsPerValue", 64);
        run(samples[i], "changeDecimalPrecision", 1);
    }
    if (failures) {
        fprintf(stderr, "%d failure(s)\n", failures);
        return 1;
    }
    return 0;
}
