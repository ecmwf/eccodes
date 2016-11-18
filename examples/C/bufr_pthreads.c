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
#define NUM_THREADS 3

static void* process_bufr(void* hand)
{
    codes_handle* h = (codes_handle*)hand;
    long longVal = 0;

    CODES_CHECK(codes_set_long(h, "unpack", 1),0);
    CODES_CHECK(codes_get_long(h,"numberOfSubsets", &longVal),0);
    printf("numberOfSubsets=%ld\n", longVal);

    codes_handle_delete(h);
    pthread_exit(NULL);
}

int main(int argc, char** argv)
{
    pthread_t thread1, thread2;
    int error = 0, err = 0;
    long i = 0;
    FILE* fin = 0;
    codes_handle* h1 = 0;
    codes_handle* h2 = 0;
    fin = fopen("../../data/bufr/asca_139.bufr", "r");
    h1 = codes_handle_new_from_file(NULL, fin, PRODUCT_BUFR, &err);
    fclose(fin);
    fin = fopen("../../data/bufr/aaen_55.bufr", "r");
    h2 = codes_handle_new_from_file(NULL, fin, PRODUCT_BUFR, &err);
    fclose(fin);

    printf("Start thread 1...\n");
    error = pthread_create(&thread1, NULL, process_bufr, (void *)h1); if(error)return 1;
    printf("Start thread 2...\n");
    error = pthread_create(&thread2, NULL, process_bufr, (void *)h2); if(error)return 1;
    printf("All done...\n");
#if 0
    for( i=0; i<NUM_THREADS; ++i) {
        FILE* fin = 0;
        char* filename = NULL;
        codes_handle* h = 0;
        int err = 0;
        if ( i == 0 ) filename = "../../data/bufr/asca_139.bufr";
        if ( i == 1 ) filename = "../../data/bufr/temp_101.bufr";
        if ( i == 2 ) filename = "../../data/bufr/syno_1.bufr";
        assert(filename);
        fin = fopen(filename, "r");
        assert(fin);
        h = codes_handle_new_from_file(NULL, fin, PRODUCT_BUFR, &err);
        assert(h);
        printf("Creating thread %ld\n", i);
        error = pthread_create(&threads[i], NULL, process_bufr, (void *)h);
        if (error) {
            return 1;
        }
        fclose(fin);
    }
#endif
    pthread_join( thread1, NULL);
    pthread_join( thread2, NULL);
    /*pthread_exit(NULL);*/
    return 0;
}
