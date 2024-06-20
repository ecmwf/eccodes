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
 * Description: Reads a GRIB message from file, measures time taken
 */
#include <stdio.h>
#include "grib_api_internal.h"

int main(int argc, char** argv)
{
    grib_timer* tes = grib_get_timer(0, "decoding", 0, 0);
    FILE* in        = NULL;
    int err = 0, i = 0;
    grib_handle* h            = NULL;
    size_t values_len         = 0;
    double* values            = NULL;
    double duration_actual    = 0;
    const double duration_max = 3.6; /* seconds */
    const int num_repetitions = 100;

    if (argc < 2) return 1;

    in = fopen(argv[1], "rb");
    Assert(in);

    /* create new handle */
    err = 0;
    h   = grib_handle_new_from_file(0, in, &err);
    Assert(h);

    /* get the size of the values array*/
    GRIB_CHECK(grib_get_size(h, "values", &values_len), 0);

    values = (double*)malloc(values_len * sizeof(double));

    /* get data values*/
    grib_timer_start(tes);
    for (i = 0; i < num_repetitions; i++) {
        GRIB_CHECK(grib_get_double_array(h, "values", values, &values_len), 0);
    }
    grib_timer_stop(tes, 0);
    duration_actual = grib_timer_value(tes);
    if (duration_actual > duration_max) {
        fprintf(stderr, "Decoding took longer than expected! actual time=%g, expected to take less than %g seconds",
                duration_actual, duration_max);
        return 1;
    }
    printf("Test passed. Actual decode time=%g\n", duration_actual);
    free(values);
    grib_handle_delete(h);
    fclose(in);
    return 0;
}
