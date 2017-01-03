/*
 * Copyright 2005-2017 ECMWF.
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

static int DBL_EQUAL(double d1, double d2, double tolerance)
{
    return fabs(d1-d2) <= tolerance;
}

static void* process_bufr(void* arg)
{
    codes_handle* h = (codes_handle*)arg;
    long numSubsets = 0;
    size_t size = 0;
    double* dValues = NULL;

    CODES_CHECK(codes_set_long(h, "unpack", 1),0);
    CODES_CHECK(codes_get_long(h,"numberOfSubsets", &numSubsets),0);
    /*printf("numberOfSubsets=%ld\n", numSubsets);*/
    assert( numSubsets == 2016 || numSubsets == 128 );

    dValues = (double*)malloc(numSubsets*sizeof(double));
    assert(dValues);
    size = numSubsets;
    CODES_CHECK(codes_get_double_array(h, "latitude", dValues, &size), 0);
    assert( DBL_EQUAL(dValues[0], 43.119, 0.01) || DBL_EQUAL(dValues[0], -58.174, 0.01) );

    free(dValues);
    pthread_exit(NULL);
}

int main(int argc, char** argv)
{
    pthread_t thread1, thread2;
    int err = 0;
    FILE* fin = 0;
    codes_handle* h1 = 0;
    codes_handle* h2 = 0;
    fin = fopen("../../data/bufr/asca_139.bufr", "r");
    h1 = codes_handle_new_from_file(NULL, fin, PRODUCT_BUFR, &err); assert(h1);
    fclose(fin);
    fin = fopen("../../data/bufr/aaen_55.bufr", "r");
    h2 = codes_handle_new_from_file(NULL, fin, PRODUCT_BUFR, &err); assert(h2);
    fclose(fin);

    err = pthread_create(&thread1, NULL, process_bufr, (void *)h1);
    assert(!err);

    err = pthread_create(&thread2, NULL, process_bufr, (void *)h2);
    assert(!err);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    codes_handle_delete(h1);
    codes_handle_delete(h2);
    return 0;
}
