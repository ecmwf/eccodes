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
 * C Implementation: grib_precipitation
 *
 * Description: convert fields accumulated from 0 step in
 *              fields accumulated in the interval between
 *			    a field and another. In the input file the fields
 *              must be in ascending order in the step.
 *              step must be the most external loop (slower varying index)
 *
 *
 *
 */
#include <stdio.h>
#include <stdlib.h>

#include "eccodes.h"

void usage(const char* prog)
{
    printf("usage: %s filename\n", prog);
    exit(1);
}

int main(int argc, char** argv)
{
    int err        = 0, i;
    double* values = NULL;
    size_t values_len = 0;

    FILE* in = NULL;
    char* filename;
    codes_handle* h = NULL;

    if (argc < 2) usage(argv[0]);
    filename = argv[1];

    in = fopen(filename, "rb");
    if (!in) {
        fprintf(stderr, "ERROR: unable to open file %s\n", filename);
        return 1;
    }

    /* create new handle from a message in a file*/
    while ((h = codes_handle_new_from_file(0, in, &err)) != NULL) {
        /* get the size of the values array*/
        CODES_CHECK(codes_get_size(h, "values", &values_len), 0);

        values = malloc(values_len * sizeof(double));

        /* get data values*/
        CODES_CHECK(codes_get_double_array(h, "values", values, &values_len), 0);

        for (i = 0; i < values_len; i++)
            printf("%d  %.10e\n", i + 1, values[i]);

        free(values);

        codes_handle_delete(h);
    }

    fclose(in);
    return 0;
}
