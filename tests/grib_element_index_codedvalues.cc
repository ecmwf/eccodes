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
 * Description: element access to "values" (GRIB-564: the index relates to codedValues) of a GRIB2
 *              second order packed field and of a boustrophedonic GRIB1 field must fail when the
 *              index is the number of coded values: it read one element past the buffer and
 *              succeeded. The last valid index must succeed and give the last coded value.
 *              (codes_get_double_elements checks the size of "values" itself.)
 *              usage: grib_element_index_codedvalues <file>
 */
#include <stdio.h>
#include <stdlib.h>
#include "eccodes.h"

int main(int argc, char** argv)
{
    int err = 0, failures = 0;
    if (argc != 2) {
        fprintf(stderr, "usage: %s file\n", argv[0]);
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
    size_t n = 0;
    CODES_CHECK(codes_get_size(h, "codedValues", &n), 0);
    double* coded = (double*)malloc(n * sizeof(double));
    CODES_CHECK(codes_get_double_array(h, "codedValues", coded, &n), 0);

    double v = 0;
    err = codes_get_double_element(h, "values", n - 1, &v);
    if (err != GRIB_SUCCESS || v != coded[n - 1]) {
        fprintf(stderr, "FAIL %s: index %zu (the last): error %d, value %g instead of %g\n", argv[1], n - 1, err, v, coded[n - 1]);
        ++failures;
    }
    err = codes_get_double_element(h, "values", n, &v);
    if (err == GRIB_SUCCESS) {
        fprintf(stderr, "FAIL %s: index %zu (the number of coded values) succeeded\n", argv[1], n);
        ++failures;
    }
    /* codes_get_double_elements checks the indexes against the size of "values" (with the missing
       values of a bitmap) */
    size_t nValues = 0;
    CODES_CHECK(codes_get_size(h, "values", &nValues), 0);
    const int indexes[2] = { 0, (int)nValues };
    double two[2] = { 0, 0 };
    err = codes_get_double_elements(h, "values", indexes, 2, two);
    if (err == GRIB_SUCCESS) {
        fprintf(stderr, "FAIL %s: indexes {0, %zu} succeeded\n", argv[1], nValues);
        ++failures;
    }

    free(coded);
    codes_handle_delete(h);
    if (failures == 0) printf("OK %s\n", argv[1]);
    return failures == 0 ? 0 : 1;
}
