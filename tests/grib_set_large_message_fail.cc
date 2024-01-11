/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include <stdio.h>
#include <stdlib.h>
#undef NDEBUG
#include <assert.h>

#include "eccodes.h"

int main(int argc, char** argv)
{
    int err = 0, i = 0, NUM_MISSING = 10;
    codes_handle* h   = NULL;
    size_t values_len = 0;
    long Ni = 0, Nj = 0;
    double* values = NULL;
    const double missing = 1.0e36;
    bool use_bitmap = false;

    if (argc == 2 && strcmp(argv[1], "-b")==0) {
         use_bitmap = true;
    }

    h = codes_grib_handle_new_from_samples(NULL, "GRIB1");
    assert(h);

    CODES_CHECK(codes_set_double(h, "missingValue", missing), 0);

    Ni = Nj = 20000;
    values_len = Ni * Nj;

    values = (double*)calloc(values_len, sizeof(double));

    CODES_CHECK(codes_set_long(h, "Ni", Ni), 0);
    CODES_CHECK(codes_set_long(h, "Nj", Nj), 0);

    if (use_bitmap) {
        printf("Adding a bitmap...\n");
        CODES_CHECK(codes_set_long(h, "bitmapPresent", 1), 0);
        for (i = 0; i < NUM_MISSING; i++) {
            values[i] = missing;
        }
    } else {
        printf("Not adding a bitmap...\n");
        values[0] = 42;
        values[1] = 52;
    }

    printf("Setting the values array...\n");
    err = codes_set_double_array(h, "values", values, values_len);
    if (err) {
        printf("codes_set_double_array failed as expected: err=%s\n", codes_get_error_message(err));
    } else {
        fprintf(stderr, "Error: codes_set_double_array should have failed!\n");
        return 1;
    }

    codes_handle_delete(h);
    free(values);

    return 0;
}
