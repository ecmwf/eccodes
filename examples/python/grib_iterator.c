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
 * C Implementation: grib_iterator
 *
 * Description: how to use an iterator on lat/lon/values for GRIB messages
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include "grib_api.h"

static void usage(const char* prog)
{
    printf("Usage: %s grib_file\n", prog);
    exit(1);
}

int main(int argc, char** argv)
{
    FILE* in = NULL;
    int err  = 0;
    double lat, lon, value;
    double missingValue = 1e+20; /* A value out of range */
    int n               = 0;
    long bitmapPresent  = 0;
    char* filename      = NULL;

    /* Message handle. Required in all the ecCodes calls acting on a message.*/
    grib_handle* h = NULL;
    /* Iterator on lat/lon/values.*/
    grib_iterator* iter = NULL;

    if (argc != 2) usage(argv[0]);

    filename = argv[1];

    in = fopen(filename, "rb");
    if (!in) {
        fprintf(stderr, "Error: unable to open file %s\n", filename);
        return 1;
    }

    /* Loop on all the messages in a file.*/
    while ((h = grib_handle_new_from_file(0, in, &err)) != NULL) {
        /* Check of errors after reading a message. */
        if (err != GRIB_SUCCESS) GRIB_CHECK(err, 0);

        /* Check if a bitmap applies */
        GRIB_CHECK(grib_get_long(h, "bitmapPresent", &bitmapPresent), 0);

        if (bitmapPresent) {
            /* Set the double representing the missing value in the field. */
            /* Choose a missingValue that does not correspond to any real value in the data array */
            GRIB_CHECK(grib_set_double(h, "missingValue", missingValue), 0);
        }

        /* A new iterator on lat/lon/values is created from the message handle h. */
        iter = grib_iterator_new(h, 0, &err);
        if (err != GRIB_SUCCESS) GRIB_CHECK(err, 0);

        n = 0;
        /* Loop on all the lat/lon/values. */
        while (grib_iterator_next(iter, &lat, &lon, &value)) {
            /* You can now print lat and lon,  */
            printf("- %d - lat=%.6e lon=%.6e value=", n, lat, lon);
            /* decide what to print if a missing value is found. */
            if (bitmapPresent && value == missingValue) printf("missing\n");
            /* and print the value if is not missing. */
            else
                printf("%f\n", value);
            n++;
        }

        /* At the end the iterator is deleted to free memory. */
        grib_iterator_delete(iter);

        /* At the end the grib_handle is deleted to free memory. */
        grib_handle_delete(h);
    }

    fclose(in);

    return 0;
}
