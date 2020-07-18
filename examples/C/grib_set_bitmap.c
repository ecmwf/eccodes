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
 * C Implementation: grib_set_bitmap
 *
 * Description: how to set a bitmap in a GRIB message
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include "eccodes.h"

int main(int argc, char** argv)
{
    int err     = 0;
    size_t size = 0;

    FILE* in            = NULL;
    const char* infile  = "../../data/regular_latlon_surface.grib1";
    FILE* out           = NULL;
    const char* outfile = "out.set_bitmap_c.grib";
    codes_handle* h     = NULL;
    const void* buffer  = NULL;
    size_t values_len;
    double* values;
    double missing = 9999;
    int i          = 0;

    in = fopen(infile, "rb");
    if (!in) {
        fprintf(stderr, "Error: unable to open input file %s\n", infile);
        return 1;
    }

    out = fopen(outfile, "wb");
    if (!out) {
        fprintf(stderr, "Error: unable to open output file %s\n", outfile);
        fclose(in);
        return 1;
    }

    h = codes_handle_new_from_file(0, in, PRODUCT_GRIB, &err);
    if (h == NULL) {
        fprintf(stderr, "Error: unable to create handle from file %s\n", infile);
    }

    CODES_CHECK(codes_set_double(h, "missingValue", missing), 0);

    /* get the size of the values array*/
    CODES_CHECK(codes_get_size(h, "values", &values_len), 0);

    values = (double*)malloc(values_len * sizeof(double));

    /* get data values*/
    CODES_CHECK(codes_get_double_array(h, "values", values, &values_len), 0);

    CODES_CHECK(codes_set_long(h, "bitmapPresent", 1), 0);

    for (i = 0; i < 10; i++) {
        values[i] = missing;
    }

    CODES_CHECK(codes_set_double_array(h, "values", values, values_len), 0);

    /* get the coded message in a buffer */
    CODES_CHECK(codes_get_message(h, &buffer, &size), 0);

    /* write the buffer in a file*/
    if (fwrite(buffer, 1, size, out) != size) {
        perror(outfile);
        exit(1);
    }

    codes_handle_delete(h);
    free(values);
    fclose(in);
    fclose(out);

    return 0;
}
