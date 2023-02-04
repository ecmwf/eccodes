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

#ifndef ECCODES_ON_WINDOWS
#include <unistd.h>
#endif

#include "grib_api_internal.h"

int main(int argc, char** argv)
{
    int err = 0, is_missing = 0;
    double* values    = NULL;
    char format[1024] = {0,};
    size_t values_len = 0, i = 0;
    long Ni        = 0;
    FILE* in       = NULL;
    grib_handle* h = NULL;

    if (argc > 1)
        in = fopen(argv[1], "rb");
    else {
        printf("Usage : %s filename [format]\n", argv[0]);
        exit(1);
    }
    if (!in) {
        fprintf(stderr, "ERROR: Could not open file %s\n", argv[1]);
        return 1;
    }

    if (argc > 2)
        strcpy(format, argv[2]);
    else
        strcpy(format, "%g ");

    h = grib_handle_new_from_file(0, in, &err);
    while (h) {
        if ((err = grib_get_long(h, "Ni", &Ni)) != GRIB_SUCCESS) {
            fprintf(stderr, "ERROR: %s", grib_get_error_message(err));
            return 1;
        }
        is_missing = grib_is_missing(h, "Ni", &err);
        if (is_missing) {
            fprintf(stderr, "ERROR: Key Ni is missing\n");
            return 1;
        }

        if ((err = grib_get_size(h, "values", &values_len)) != GRIB_SUCCESS) {
            fprintf(stderr, "ERROR: %s", grib_get_error_message(err));
            return 1;
        }

        values = malloc(values_len * sizeof(double));

        if ((err = grib_get_double_array(h, "values", values, &values_len)) != GRIB_SUCCESS)
            fprintf(stderr, "ERROR: %s", grib_get_error_message(err));

        for (i = 0; i < values_len; i++) {
            if (i != 0 && (i % Ni == 0)) printf("\n");
            printf(format, values[i]);
        }
        printf("\n\n");
        printf("%lu values\n", i);
        free(values);

        grib_handle_delete(h);

        h = grib_handle_new_from_file(0, in, &err);

        if (h)
            printf("************************************************************\n");
    }
    fclose(in);
    return 0;
}
