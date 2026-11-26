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
 * Description: setting the values of the first field of a multi-field GRIB2 message (opened
 *              without multi-field support) succeeds and repacks the values, with the legacy and
 *              the new simple packing (ECCODES_USE_SP=1). The new simple packing returned the
 *              error of updating the message (GRIB_DECODING_ERROR), which the legacy code ignores.
 */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "eccodes.h"

int main(int argc, char** argv)
{
    int err = 0;
    if (argc != 2) {
        fprintf(stderr, "usage: %s multi_created.grib2\n", argv[0]);
        return 1;
    }
    FILE* f = fopen(argv[1], "rb");
    if (!f) {
        perror(argv[1]);
        return 1;
    }
    codes_handle* h = codes_handle_new_from_file(NULL, f, PRODUCT_GRIB, &err);
    fclose(f);
    if (!h) {
        fprintf(stderr, "cannot read %s (%s)\n", argv[1], codes_get_error_message(err));
        return 1;
    }
    CODES_CHECK(codes_set_long(h, "bitsPerValue", 8), 0);
    size_t n = 0;
    CODES_CHECK(codes_get_size(h, "values", &n), 0);
    double* values = (double*)malloc(n * sizeof(double));
    CODES_CHECK(codes_get_double_array(h, "values", values, &n), 0);

    err = codes_set_double_array(h, "values", values, n);
    if (err != GRIB_SUCCESS) {
        fprintf(stderr, "FAIL: setting the values returned %d (%s)\n", err, codes_get_error_message(err));
        return 1;
    }

    /* The statistics belong to the new values */
    double expectedMax = values[0], max = 0;
    for (size_t i = 1; i < n; ++i)
        expectedMax = values[i] > expectedMax ? values[i] : expectedMax;
    CODES_CHECK(codes_get_double(h, "max", &max), 0);
    if (fabs(max - expectedMax) > 1.0) {
        fprintf(stderr, "FAIL: max is %g, the maximum of the values %g\n", max, expectedMax);
        return 1;
    }

    free(values);
    codes_handle_delete(h);
    printf("OK\n");
    return 0;
}
