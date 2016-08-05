/*
 * Copyright 2005-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>

#include "eccodes.h"
#define NUM_THREADS 4

static void* process_grib(void* threadID)
{
    const long tid = (long)threadID;
    size_t str_len = 20;
    long indicatorOfUnitOfTimeRange = 1, step = 0;
    char mystring[100];
    double* values = NULL;
    size_t values_len = 0;
    double min=0,max=0;
    double pv[4]={1,2,3,4};
    const size_t pvsize=4;

    codes_handle* h = codes_grib_handle_new_from_samples(0, "regular_ll_pl_grib2");
    assert(h);
    printf("Thread %ld running\n", tid);

    CODES_CHECK(codes_set_long(h,"indicatorOfUnitOfTimeRange", indicatorOfUnitOfTimeRange),0);
    CODES_CHECK(codes_set_string(h,"indicatorOfUnitOfTimeRange", "s", &str_len),0);
    CODES_CHECK(codes_set_string(h,"stepUnits", "s", &str_len),0);
    CODES_CHECK(codes_set_long(h, "endStep", 86400), 0);
    CODES_CHECK(codes_set_long(h,"centre", 80),0);

    CODES_CHECK(codes_get_long(h,"endStep", &step),0);
    CODES_CHECK(codes_get_string(h, "indicatorOfUnitOfTimeRange", mystring, &str_len),0);

    CODES_CHECK(codes_set_long(h,"PVPresent", 1),0);
    CODES_CHECK(codes_set_double_array(h, "pv", pv, pvsize),0);

    CODES_CHECK(codes_get_size(h,"values",&values_len),0);
    values = (double*)malloc(values_len*sizeof(double));
    CODES_CHECK(codes_get_double_array(h, "values", values, &values_len),0);
    free(values);

    CODES_CHECK(codes_get_double(h, "min", &min),0);
    CODES_CHECK(codes_get_double(h, "max", &max),0);

    codes_handle_delete(h);
    pthread_exit(NULL);
}

int main(int argc, char** argv)
{
    pthread_t threads[NUM_THREADS];
    int error = 0;
    long i = 0;
    for( i=0; i<NUM_THREADS; ++i) {
        printf("Creating thread %ld\n", i);
        error = pthread_create(&threads[i], NULL, process_grib, (void *)i);
        if (error) {
            return 1;
        }
    }
    pthread_exit(NULL);
    return 0;
}
