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
#include <time.h>
#include <pthread.h>
#include <string.h>
#include <assert.h>
#include "grib_api.h"

#define NUM_THREADS 8
#define FILES_PER_ITERATION 10

char* INPUT_FILE = NULL;
void do_stuff(void *data);

void *runner(void *ptr); /* the thread function */

int main(int argc, char **argv)
{
    long i;
    int thread_counter = 0;
    pthread_t workers[NUM_THREADS];
    INPUT_FILE = argv[1];
    
    /* Create worker threads */
    for (i = 0; i < NUM_THREADS; i++) {
        /* Now we will create the thread passing it data as a paramater*/
        pthread_create(&workers[thread_counter], NULL, runner, (void*)i);
        thread_counter++;
    }

    /* Waiting for threads to complete */
    for (i = 0; i < NUM_THREADS; i++) {
       pthread_join(workers[i], NULL);
    }

    return 0;
}

void *runner(void *ptr)
{    
    do_stuff(ptr);
    pthread_exit(0);
}

static int encode_file(char *input_file, char *output_file)
{
    grib_handle *source_handle = NULL;
    const void *buffer = NULL;
    int err = 0;
 
    FILE* in = fopen(input_file,"r");
    FILE* out = fopen(output_file,"w");
    assert(in);
    assert(out);

    while ((source_handle = grib_handle_new_from_file(0, in, &err))!=NULL)
    {
        size_t size=0, values_len=0;
        int i;
        double *values = NULL;
        long count;
        double d,e;

        grib_handle *clone_handle = grib_handle_clone(source_handle);
        assert(clone_handle);
 
        GRIB_CHECK(grib_get_size(clone_handle, "values", &values_len),0);
        values = (double*)malloc(values_len*sizeof(double));

        d=10e-8;
        e=d;
        count=1;
        for (i=0;i<values_len;i++) {
            if (count>100) {e*=10; count=1;}
            values[i]=d;
            d+=e;
            count++;
        }
        GRIB_CHECK(grib_set_long(clone_handle,"bitsPerValue",16),0);
        GRIB_CHECK(grib_set_double_array(clone_handle,"values",values,values_len),0);
 
        /* get the coded message in a buffer */
        GRIB_CHECK(grib_get_message(clone_handle,&buffer,&size),0);
        /* write the buffer to a file */
        if(fwrite(buffer,1,size,out) != size) {
            perror(output_file);
            return 1;
        }
        grib_handle_delete(clone_handle);
        grib_handle_delete(source_handle);
        free(values);
    }
 
    fclose(out);
    fclose(in);
 
    return 0;
}

void do_stuff(void *arg)
{
    long number = (long)arg;
    char output_file[50];
    int i;

    for (i=0; i<FILES_PER_ITERATION;i++) {
        sprintf(output_file,"temp.grib_encode_pthreads.out_%d-%d.grib", (int)number, i);
        encode_file(INPUT_FILE,output_file);
    }
}
