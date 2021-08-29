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
 *  Description:  test setting the number of bits per value before decoding and then encoding;
 *                setting the wrong number of bpv should result in an error
 */

#include <assert.h>
#include "grib_api.h"

static double compare_double_absolute(double a, double b, double tolerance)
{
    double ret = 0;
    double d   = fabs(a - b);
    if (d > tolerance) {
        ret = d;
    }
    return ret;
}

static void usage(const char* prog)
{
    fprintf(stderr, "usage: %s input\n", prog);
    exit(1);
}

static int check_error_code(int err)
{
    if (err == GRIB_INVALID_BPV || err == GRIB_DECODING_ERROR)
        return GRIB_SUCCESS;
    return GRIB_INVALID_ARGUMENT;
}

int main(int argc, char** argv)
{
    int err           = 0, i;
    size_t values_len = 0;
    double* values    = NULL;
    char error_msg[100];
    const double tolerance = 1e-5;
    size_t slong           = sizeof(long) * 8;
    grib_handle* h;
    char* filename;

    if (argc < 2) usage(argv[0]);
    filename = argv[1];

    for (i = 0; i < 255; i++) {
        FILE* in = fopen(filename, "rb");
        assert(in);
        h = grib_handle_new_from_file(0, in, &err);
        assert(h);

        /* get the size of the values array*/
        GRIB_CHECK(grib_get_size(h, "values", &values_len), 0);

        values = (double*)malloc(values_len * sizeof(double));

        err = grib_get_double_array(h, "values", values, &values_len);
        if (compare_double_absolute(values[0], 2.7900000000e+02, tolerance) != 0)
            printf("oops i=%d  values[0]=%g\n", i, values[0]);
        if (compare_double_absolute(values[1], 2.7996093750e+02, tolerance) != 0)
            printf("oops i=%d  values[1]=%g\n", i, values[1]);

        GRIB_CHECK(grib_set_long(h, "bitsPerValue", i), 0);

        err = grib_set_double_array(h, "values", values, values_len);

        free(values);
        values = 0;

        /*
         * check if encoding of the data fails when the number of bpv
         * is not supported; bpv allowed on decoding is bpv < size of long
         */

        if (i < slong && err == 0) {
            /* do nothing */
        }
        else if (i >= slong && check_error_code(err) == GRIB_SUCCESS) {
            /* do nothing  */
        }
        else {
            sprintf(error_msg, "Error decoding when bpv=%d. Error message:%s", i, grib_get_error_message(err));
            perror(error_msg);
            exit(1);
        }

        values = (double*)malloc(values_len * sizeof(double));
        err    = grib_get_double_array(h, "values", values, &values_len);

        /*
         * check if decoding of the data fails when the number of bpv
         * is not supported; bpv allowed on decoding is bpv <= size of long
         */

        if (i < slong && err == 0) {
            /* do nothing */
        }
        else if (i >= slong && check_error_code(err) == GRIB_SUCCESS) {
            /* do nothing  */
        }
        else {
            sprintf(error_msg, "Error decoding when bpv=%d. Error message:%s", i, grib_get_error_message(err));
            perror(error_msg);
            exit(1);
        }

        free(values);
        values = 0;
        grib_handle_delete(h);
        h = 0;
        fclose(in);
    }

    grib_context_delete(grib_context_get_default());
    return 0;
}
