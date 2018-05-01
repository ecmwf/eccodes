#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <assert.h>
#include <string.h>

#include "grib_api.h"

#define NUM_THREADS 8
#define FILES_PER_ITERATION 200
static char* INPUT_FILE = NULL;

static int encode_file(char *template_file, char *output_file)
{
    FILE *in, *out;
    grib_handle *source_handle = NULL;
    const void *buffer = NULL;
    size_t size = 0;
    int err = 0;
    double *values;

    in = fopen(template_file,"r"); assert(in);
    out = fopen(output_file,"w");  assert(out);

    /* loop over the messages in the source GRIB and clone them */
    while ((source_handle = grib_handle_new_from_file(0, in, &err))!=NULL) {
        int i;
        long count;
        double d,e;
        size_t values_len= 0;

        grib_handle *clone_handle = grib_handle_clone(source_handle);
        assert(clone_handle);

        /*GRIB_CHECK(grib_set_long(clone_handle, "centre", 250),0);*/
        GRIB_CHECK(grib_get_size(clone_handle, "values", &values_len),0);

        values = (double*)malloc(values_len*sizeof(double));
        d=10e-8;
        e=d;
        count=1;
        for (i=0;i<values_len;i++) {
            if (count>100) {e*=10; count=1;}
            values[i]=d;
            /*printf("%g \n",values[i]);*/
            d+=e;
            count++;
        }

        GRIB_CHECK(grib_set_long(clone_handle,"bitsPerValue",16),0);

        /* set data values */
        GRIB_CHECK(grib_set_double_array(clone_handle,"values",values,values_len),0);

        GRIB_CHECK(grib_get_message(clone_handle,&buffer,&size),0);
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

void do_stuff(void *data);

/* Structure for passing data to threads */
struct v {
    int number;
    char *data;
};

void *runner(void *ptr); /* the thread */

int main(int argc, char **argv)
{
    int i;
    int thread_counter = 0;
    int parallel = 1;
    if (argc<2) {
        return 1;
    }
    INPUT_FILE= argv[1];
    if (argc>2 && strcmp(argv[2],"seq")==0) {
        parallel = 0;
    }
    if (parallel) {
        printf("Running parallel in %d threads.\n", NUM_THREADS);
    } else {
        printf("Running sequentially in %d runs.\n", NUM_THREADS);
    }

    pthread_t workers[NUM_THREADS];

    /* We have to create M * N worker threads */
    for (i = 0; i < NUM_THREADS; i++) {
        struct v *data = (struct v *) malloc(sizeof(struct v));
        data->number = i;
        data->data = NULL;

        if (parallel) {
            /* Now we will create the thread passing it data as an argument */
            pthread_create(&workers[thread_counter], NULL, runner, data);
            /*pthread_join(workers[thread_counter], NULL);*/
            thread_counter++;
        } else {
            do_stuff(data);
        }
    }

    if (parallel) {
        for (i = 0; i < NUM_THREADS; i++)  {
            pthread_join(workers[i], NULL);
        }
    }

    return 0;
}

void *runner(void *ptr)
{
    do_stuff(ptr);
    pthread_exit(0);
}

void do_stuff(void *ptr)
{
    /* Cast argument to struct v pointer */
    struct v *data = ptr;

    char output_file[50];

    for (int i=0; i<FILES_PER_ITERATION;i++) {
        sprintf(output_file,"output/output_file_%d-%d.grib",data->number,i);
        encode_file(INPUT_FILE,output_file);
    }

    time_t ltime;
    struct tm result;
    char stime[32];

    ltime = time(NULL);
    localtime_r(&ltime, &result);
    strftime(stime, 32, "%H:%M:%S", &result); /* Try to get milliseconds here too*/
    /* asctime_r(&result, stime); */

    printf("%s: Worker %d finished.\n", stime,data->number);
}
