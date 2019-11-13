/*
 * Test for ECC-604: GRIB decoding/encoding sequentially and parallel with POSIX threads
 */
#include <time.h>
#include <pthread.h>
#include <unistd.h>

#include "grib_api.h"

/* These are passed in via argv */
static size_t NUM_THREADS         = 0;
static size_t FILES_PER_ITERATION = 0;
static char*  INPUT_FILE          = NULL;
int opt_dump  = 0; /* If 1 then dump handle to /dev/null */
int opt_clone = 0; /* If 1 then clone source handle */
int opt_write = 0; /* If 1 write handle to file */

static int encode_values(grib_handle* h, char *output_file)
{
    double *values;
    const size_t DIM = 1000;
    size_t size = DIM * DIM;
    size_t i = 0;
    values = (double*)malloc(size*sizeof(double));
    for (i=0; i<size; ++i) {
        double v = i;
        if (i % DIM == 0) v = 0;
        values[i] = v;
    }
    GRIB_CHECK(grib_set_long(h,"bitsPerValue",16),0);
    GRIB_CHECK(grib_set_double_array(h,"values",values,size), 0);
    free(values);
    return GRIB_SUCCESS;
}

void do_encode(void *data);

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
    int parallel=1, index=0, c=0;
    const char* prog = argv[0];
    char* mode;
    if (argc<5 || argc>7) {
        fprintf(stderr, "Usage:\n\t%s [options] seq sample numRuns numIter\nOr\n\t%s [options] par sample numThreads numIter\n", prog, prog);
        return 1;
    }
    
    while ((c = getopt (argc, argv, "dcw")) != -1) {
        switch (c) {
            case 'd': opt_dump=1; break;
            case 'c': opt_clone=1; break;
            case 'w': opt_write=1; break;
        }
    }
    index = optind;
    mode = argv[index];
    INPUT_FILE = argv[index+1]; /* Has to be the name of a sample file (without tmpl extension) */
    NUM_THREADS = atol(argv[index+2]);
    FILES_PER_ITERATION = atol(argv[index+3]);

    if (strcmp(mode,"seq")==0) {
        parallel = 0;
    }
    if (parallel) {
        printf("Running parallel in %ld threads. %ld iterations (prod=%ld)\n", NUM_THREADS, FILES_PER_ITERATION, NUM_THREADS*FILES_PER_ITERATION);
        printf("Options: dump=%d, clone=%d, write=%d\n", opt_dump, opt_clone, opt_write);
    } else {
        printf("Running sequentially in %ld runs. %ld iterations\n", NUM_THREADS, FILES_PER_ITERATION);
    }

    {
        pthread_t workers[NUM_THREADS];
        for (i = 0; i < NUM_THREADS; i++) {
            struct v *data = (struct v *) malloc(sizeof(struct v));
            data->number = i;
            data->data = NULL;

            if (parallel) {
                /* Now we will create the thread passing it data as an argument */
                pthread_create(&workers[thread_counter], NULL, runner, data);
                thread_counter++;
            } else {
                do_encode(data);
            }
        }

        if (parallel) {
            for (i = 0; i < NUM_THREADS; i++)  {
                pthread_join(workers[i], NULL);
            }
        }
    }

    return 0;
}

void *runner(void *ptr)
{
    do_encode(ptr);
    pthread_exit(0);
}

void do_encode(void *ptr)
{
    /* Cast argument to struct v pointer */
    struct v *data = ptr;
    size_t i;
    char output_file[50];
    time_t ltime;
    struct tm result;
    char stime[32];
    grib_handle *hs = NULL;

    hs = grib_handle_new_from_samples(0, INPUT_FILE);

    for (i=0; i<FILES_PER_ITERATION;i++) {
        grib_handle *h = grib_handle_clone(hs);
        if (opt_write) {
            sprintf(output_file,"output/output_file_%ld-%ld.grib", data->number, i);
            encode_values(h,output_file);
        } else {
            encode_values(h,NULL);
        }
        grib_handle_delete(h);
    }

    ltime = time(NULL);
    localtime_r(&ltime, &result);
    strftime(stime, 32, "%H:%M:%S", &result); /* Try to get milliseconds here too*/
    /* asctime_r(&result, stime); */

    printf("%s: Worker %ld finished.\n", stime, data->number);
    grib_handle_delete(hs);
}
