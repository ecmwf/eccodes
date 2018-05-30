/*
 * Test for ECC-604: Each thread creates a new GRIB handle, clones it and writes it out
 */
#include <time.h>
#include <pthread.h>
#include <assert.h>

#include "grib_api.h"

/* These are passed in via argv */
static size_t NUM_THREADS         = 0;
static size_t FILES_PER_ITERATION = 0;
static char*  INPUT_FILE          = NULL;

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
        size_t values_len = 0;
        size_t str_len = 20;

        grib_handle *clone_handle = grib_handle_clone(source_handle);
        assert(clone_handle);

        /*GRIB_CHECK(grib_set_long(clone_handle, "centre", 250),0);*/
        GRIB_CHECK(grib_get_size(clone_handle, "values", &values_len),0);

        values = (double*)malloc(values_len*sizeof(double));
        GRIB_CHECK(grib_get_double_array(clone_handle, "values", values, &values_len),0);

        for (i=0;i<values_len;i++) {
            values[i] *= 0.9;
        }

        GRIB_CHECK(grib_set_string(clone_handle,"stepUnits", "s", &str_len),0);
        GRIB_CHECK(grib_set_long(clone_handle, "startStep", 43200), 0);
        GRIB_CHECK(grib_set_long(clone_handle, "endStep", 86400), 0);
        GRIB_CHECK(grib_set_long(clone_handle, "bitsPerValue", 16),0);

        /* set data values */
        GRIB_CHECK(grib_set_double_array(clone_handle,"values",values,values_len),0);

        GRIB_CHECK(grib_get_message(clone_handle,&buffer,&size),0);
        if(fwrite(buffer,1,size,out) != size) {
            perror(output_file);
            return 1;
        }
        {
            FILE *devnull = fopen("/dev/null", "w");
            grib_dump_content(source_handle,devnull,"wmo",0,NULL);
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
    size_t number;
    char *data;
};

void *runner(void *ptr); /* the thread */

int main(int argc, char **argv)
{
    size_t i;
    int thread_counter = 0;
    int parallel = 1;
    const char* prog = argv[0];
    char* mode;
    if (argc!=5) {
        fprintf(stderr, "Usage:\n\t%s seq file numRuns numIter\nOr\n\t%s par file numThreads numIter\n", prog, prog);
        return 1;
    }
    mode = argv[1];
    INPUT_FILE= argv[2];
    NUM_THREADS = atol(argv[3]);
    FILES_PER_ITERATION = atol(argv[4]);

    if (strcmp(mode,"seq")==0) {
        parallel = 0;
    }
    if (parallel) {
        printf("Running parallel in %ld threads. %ld iterations\n", NUM_THREADS, FILES_PER_ITERATION);
        printf("Each thread creates a new GRIB handle, clones it and writes it out\n");
    } else {
        printf("Running sequentially in %ld runs. %ld iterations\n", NUM_THREADS, FILES_PER_ITERATION);
    }

    pthread_t workers[NUM_THREADS];
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
    size_t i;
    char output_file[50];

    for (i=0; i<FILES_PER_ITERATION;i++) {
        sprintf(output_file,"output/output_file_%ld-%ld.grib", data->number, i);
        encode_file(INPUT_FILE,output_file);
    }

    time_t ltime;
    struct tm result;
    char stime[32];

    ltime = time(NULL);
    localtime_r(&ltime, &result);
    strftime(stime, 32, "%H:%M:%S", &result); /* Try to get milliseconds here too*/
    /* asctime_r(&result, stime); */

    printf("%s: Worker %ld finished.\n", stime, data->number);
}
