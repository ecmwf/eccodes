/*
 * Copyright 2005-2018 ECMWF.
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

/*static int DBL_EQUAL(double d1, double d2, double tolerance)
{
    return fabs(d1-d2) <= tolerance;
}*/

static void* process_bufr(void* arg)
{
    FILE* fin = (FILE*)arg;
    int err = 0;
    codes_handle* h = NULL;
    long numSubsets = 0, lVal = 0;
    size_t size = 0, i=0;
    double* dValues = NULL;
    /* Each thread gets a different message handle */
    h = codes_handle_new_from_file(NULL, fin, PRODUCT_BUFR, &err);
    assert(h);

    /* Check expected values for this BUFR file */
    CODES_CHECK(codes_get_long(h,"numberOfSubsets", &numSubsets),0);
    assert( numSubsets == 1 );
    CODES_CHECK(codes_get_long(h, "rectimeSecond", &lVal), 0);
    assert( lVal == 27 );

    CODES_CHECK(codes_set_long(h, "unpack", 1),0);

    dValues = (double*)malloc(numSubsets*sizeof(double));
    assert(dValues);
    size = numSubsets;
    CODES_CHECK(codes_get_double_array(h, "latitude", dValues, &size), 0);
    for(i=0; i<size; ++i) {
        /* Specific test for latitudes in this BUFR file */
        assert( dValues[0] < 79 && dValues[0] > 70 );
    }
    free(dValues);
    
    /* Some encoding too */
    CODES_CHECK(codes_set_long(h, "bufrHeaderCentre", 88),0);
    CODES_CHECK(codes_set_long(h, "blockNumber", 2), 0);
    CODES_CHECK(codes_set_long(h, "#3#verticalSignificanceSurfaceObservations", 8), 0);
    CODES_CHECK(codes_set_long(h, "pack", 1), 0);

    pthread_exit(NULL);
}

int main(int argc, char** argv)
{
    pthread_t thread1, thread2, thread3;
    int err = 0;
    FILE* fin = 0;
    codes_handle* h1 = 0;
    codes_handle* h2 = 0;
    fin = fopen("../../data/bufr/syno_multi.bufr", "r");
    assert(fin);

    err = pthread_create(&thread1, NULL, process_bufr, (void *)fin);
    assert(!err);

    err = pthread_create(&thread2, NULL, process_bufr, (void *)fin);
    assert(!err);

    err = pthread_create(&thread3, NULL, process_bufr, (void *)fin);
    assert(!err);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    pthread_join(thread3, NULL);

    fclose(fin);
    codes_handle_delete(h1);
    codes_handle_delete(h2);
    return 0;
}
