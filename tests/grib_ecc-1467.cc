/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include <math.h>
#include "eccodes.h"
#include "grib_api_internal.h"

int main(int argc, char** argv)
{
    int err            = 0;
    float* fvalues     = NULL; // data values as floats
    double* dvalues    = NULL; // data values as doubles
    size_t values_len  = 0;    // number of data points
    size_t i           = 0;

    double abs_error     = 0;
    const double max_abs_error = 1e-03;
    const double tolerance     = 1e-03;
    double dmin;
    double dmax;
    float fval;

    FILE* in             = NULL;
    const char* filename = 0;
    codes_handle* h      = NULL;

    if (argc != 2) {
        fprintf(stderr, "usage: %s file\n", argv[0]);
        return 1;
    }
    filename = argv[1];

    printf("Opening %s\n", filename);
    in = fopen(filename, "rb");
    Assert(in);

    h = codes_handle_new_from_file(0, in, PRODUCT_GRIB, &err);
    Assert(h);

    CODES_CHECK(codes_get_size(h, "values", &values_len), 0);

    fvalues = (float*)malloc(values_len * sizeof(float));
    dvalues = (double*)malloc(values_len * sizeof(double));
    CODES_CHECK(codes_get_float_array(h, "values", fvalues, &values_len), 0);
    CODES_CHECK(codes_get_double_array(h, "values", dvalues, &values_len), 0);

    for (i = 0; i < values_len; i++) {
        abs_error = fabs(dvalues[i] - (double)fvalues[i]);
        if (abs_error > max_abs_error) {
            fprintf(stderr, "ERROR:\n\tfvalue %e\n\tdvalue %e\n\terror %e\n\tmax_abs_error %e\n",
                    fvalues[i], dvalues[i], abs_error, max_abs_error);
            Assert(!"Absolute error test failed\n");
        }

        dmin = dvalues[i] >= 0 ? dvalues[i] / (1 + tolerance) : dvalues[i] * (1 + tolerance);
        dmax = dvalues[i] >= 0 ? dvalues[i] * (1 + tolerance) : dvalues[i] / (1 + tolerance);
        fval = fvalues[i];

        if (!((dmin <= fval) && (fval <= dmax))) {
            fprintf(stderr, "Error:\n");
            fprintf(stderr, "dvalue: %f, fvalue: %f\n", dvalues[i], fvalues[i]);
            fprintf(stderr, "\tmin < fvalue < max = %.20e < %.20e < %.20e FAILED\n",
                    dmin, fvalues[i], dmax);
            fprintf(stderr, "\tfvalue - min = %.20e (%s)\n",
                    fvalues[i] - dmin, fvalues[i] - dmin >= 0 ? "OK" : "FAILED (should be positive)");
            fprintf(stderr, "\tmax - fvalue = %.20e (%s)\n",
                    dmax - fvalues[i], dmax - fvalues[i] >= 0 ? "OK" : "FAILED (should be positive)");

            Assert(!"Relative tolerance test failed\n");
        }
    }

    free(fvalues);
    free(dvalues);

    codes_handle_delete(h);
    fclose(in);
    return 0;
}
