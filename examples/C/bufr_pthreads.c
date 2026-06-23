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
#include <assert.h>
#include <pthread.h>

#include "eccodes.h"
#define NUM_THREADS 3

static void* process_bufr(void* arg)
{
    codes_handle* h = (codes_handle*)arg;
    long numSubsets = 0, lVal = 0;
    size_t size = 0, i = 0;
    double* dValues = NULL;

    assert(h);

    /* Check expected values for this BUFR file */
    CODES_CHECK(codes_get_long(h, "numberOfSubsets", &numSubsets), 0);
    assert(numSubsets == 1);
    CODES_CHECK(codes_get_long(h, "rectimeSecond", &lVal), 0);
    assert(lVal == 27);

    CODES_CHECK(codes_set_long(h, "unpack", 1), 0);

    dValues = (double*)malloc(numSubsets * sizeof(double));
    assert(dValues);
    size = numSubsets;
    CODES_CHECK(codes_get_double_array(h, "latitude", dValues, &size), 0);
    for (i = 0; i < size; ++i) {
        /* Specific test for latitudes in this BUFR file */
        assert(dValues[0] < 79 && dValues[0] > 70);
    }
    free(dValues);

    /* Some encoding too */
    CODES_CHECK(codes_set_long(h, "bufrHeaderCentre", 88), 0);
    CODES_CHECK(codes_set_long(h, "blockNumber", 2), 0);
    CODES_CHECK(codes_set_long(h, "#3#verticalSignificanceSurfaceObservations", 8), 0);
    CODES_CHECK(codes_set_long(h, "pack", 1), 0);

    codes_handle_delete(h);
    pthread_exit(NULL);
}

int main(int argc, char** argv)
{
    pthread_t threads[NUM_THREADS];
    codes_handle* handles[NUM_THREADS];
    int err = 0, i = 0;
    FILE* fin = fopen("../../data/bufr/syno_multi.bufr", "rb");
    assert(fin);

    /* Read all messages sequentially (FILE* is not thread-safe) */
    for (i = 0; i < NUM_THREADS; i++) {
        handles[i] = codes_handle_new_from_file(NULL, fin, PRODUCT_BUFR, &err);
        assert(handles[i]);
    }
    fclose(fin);

    /* Process each message in its own thread */
    for (i = 0; i < NUM_THREADS; i++) {
        err = pthread_create(&threads[i], NULL, process_bufr, (void*)handles[i]);
        if (err) return 1;
    }

    for (i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}
