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
 * Description: values that cannot be coded must be reported as an error, not kill the process.
 * Two places assert (ecCodes assertion failed), which aborts and leaves the application no way to
 * recover:
 *   ibm:                  grib_ibm_to_long() asserts for a number whose absolute value is larger
 *                         than the largest IBM float (7.237e75). grib_nearest_smaller_ibm_float()
 *                         only checks the positive side, so the reference value of a GRIB1 field
 *                         with a large negative minimum reaches it.
 *   binary_scale_factor:  grib_get_binary_scale_fact() asserts that the binary scale factor it
 *                         searched for is not larger than 127. The spectral complex packing scales
 *                         the values with the decimal scale factor before asking for it, so a
 *                         positive decimal scale factor on a large field reaches it.
 * usage: grib_number_too_large ibm | binary_scale_factor
 */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "eccodes.h"

static int failures = 0;

static void check(const char* what, const char* sample, double mag, long decimal_scale_factor)
{
    size_t i = 0, n = 0;
    int err = 0;
    long n_vals = 0;
    double* values = NULL;
    codes_handle* h = codes_grib_handle_new_from_samples(NULL, sample);

    if (!h) {
        fprintf(stderr, "Cannot create a handle from sample %s\n", sample);
        exit(1);
    }
    CODES_CHECK(codes_get_size(h, "values", &n), 0);
    values = (double*)malloc(n * sizeof(double));
    for (i = 0; i < n; i++)
        values[i] = (i % 2) ? mag : -mag;

    CODES_CHECK(codes_set_long(h, "decimalScaleFactor", decimal_scale_factor), 0);

    err = codes_set_double_array(h, "values", values, n);
    if (err == 0) {
        fprintf(stderr, "FAIL %s: %g with decimalScaleFactor %ld was accepted, expected an error\n",
                what, mag, decimal_scale_factor);
        ++failures;
    }
    else {
        printf("%s: %s\n", what, codes_get_error_message(err));
    }

    /* The handle must still be usable */
    err = codes_get_long(h, "numberOfValues", &n_vals);
    if (err != 0 || n_vals != (long)n) {
        fprintf(stderr, "FAIL %s: the handle is not usable after the error (error=%d, numberOfValues=%ld)\n",
                what, err, n_vals);
        ++failures;
    }

    free(values);
    codes_handle_delete(h);
}

int main(int argc, char** argv)
{
    const char* what = argc > 1 ? argv[1] : "ibm";

    if (!strcmp(what, "ibm"))
        check("ibm", "GRIB1", 1e300, -1);
    else if (!strcmp(what, "binary_scale_factor"))
        check("binary_scale_factor", "sh_pl_grib1", 1e12, 40);
    else {
        fprintf(stderr, "unknown case %s\n", what);
        return 1;
    }
    return failures ? 1 : 0;
}
