/*
 * Copyright 2005-2018 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

/*
 * Description: Reads a GRIB message from file, measures read time.
 *
 */
#include <stdio.h>
#include <assert.h>
#include "grib_api_internal.h"

static void usage(const char* prog) {
    printf("usage: %s filename\n",prog);
    exit(1);
}

#ifdef GRIB_TIMER
int main(int argc, char** argv)
{
    grib_timer *tes = grib_get_timer(0,"decoding", 0, 0);
    FILE *in = NULL;
    int err = 0, i = 0;
    grib_handle *h = NULL;
    size_t values_len = 0;
    double *values = NULL;
    double duration_actual = 0;
    const double duration_max = 3; /* seconds */
    const int num_repetitions = 1000;

    if (argc<2) usage(argv[0]);

    in = fopen(argv[1],"r");
    if(!in) {
        printf("ERROR: unable to open file %s\n",argv[1]);
        return 1;
    }

    /* create new handle from a message in a file*/
    err = 0;
    h = grib_handle_new_from_file(0,in,&err);
    if (h == NULL) {
        printf("Error: unable to create handle from file.\n");
        return 1;
    }

    /* get the size of the values array*/
    GRIB_CHECK(grib_get_size(h,"values",&values_len),0);

    values = malloc(values_len*sizeof(double));

    /* get data values*/
    grib_timer_start(tes);
    for (i=0; i<num_repetitions; i++) {
        GRIB_CHECK(grib_get_double_array(h,"values",values,&values_len),0);
    }
    grib_timer_stop(tes,0);
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
#else
int main(int argc, char** argv) { return 0; }
#endif
