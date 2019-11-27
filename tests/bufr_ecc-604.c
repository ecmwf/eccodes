/*
 * Test for ECC-604: Each thread creates a new BUFR handle, optionaly clone it and/or write it out
 */
#include <time.h>
#include <pthread.h>
#include <assert.h>
#include <unistd.h>

#include "eccodes.h"
#include "grib_api_internal.h"

/* These are passed in via argv */
static size_t NUM_THREADS         = 0;
static size_t FILES_PER_ITERATION = 0;
static char*  INPUT_FILE          = NULL;
int opt_dump  = 0; /* If 1 then dump handle to /dev/null */
int opt_clone = 0; /* If 1 then clone source handle */
int opt_write = 0; /* If 1 write handle to file */

static int encode_file(char *template_file, char *output_file)
{
    FILE *in, *out=NULL;
    codes_handle *source_handle = NULL;
    const void *buffer = NULL;
    size_t size = 0;
    int err = 0;
    long numSubsets = 0;

    assert(template_file);
    in = fopen(template_file,"r"); assert(in);
    if (opt_write) {
        assert(output_file);
        out = fopen(output_file,"w");  assert(out);
    }

    /* loop over the messages in the source BUFR and clone them */
    while ((source_handle = codes_handle_new_from_file(NULL, in, PRODUCT_BUFR, &err)) != NULL || err != CODES_SUCCESS) {
        codes_handle *h = source_handle;
        
        if (opt_clone) {
            h = codes_handle_clone(source_handle); assert(h);
        }

        CODES_CHECK(codes_get_long(h,"numberOfSubsets", &numSubsets),0);
        CODES_CHECK(codes_set_long(h, "unpack", 1),0);

        CODES_CHECK(codes_get_message(h,&buffer,&size),0);
        if (opt_write) {
            if(fwrite(buffer,1,size,out) != size) {
                perror(output_file);
                return 1;
            }
        }
        if (opt_dump) {
            FILE *devnull = fopen("/dev/null", "w");
            grib_dumper* dumper = NULL;
            const char* dumper_name = "bufr_simple";
            unsigned long dump_flags = CODES_DUMP_FLAG_ALL_DATA;
            /* codes_dump_content(source_handle,devnull, "json", 1024, NULL); */ /* JSON dump with all attributes */
            dumper=grib_dump_content_with_dumper(source_handle, dumper, devnull, dumper_name, dump_flags, NULL);
            assert(dumper);
            fclose(devnull);
        }

        codes_handle_delete(source_handle);
        if(opt_clone) codes_handle_delete(h);
    }

    if (opt_write) fclose(out);
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
    int parallel=1, index=0, c=0;
    const char* prog = argv[0];
    char* mode;
    if (argc<5 || argc>7) {
        fprintf(stderr, "Usage:\n\t%s [options] seq file numRuns numIter\nOr\n\t%s [options] par file numThreads numIter\n", prog, prog);
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
    INPUT_FILE = argv[index+1];
    NUM_THREADS = atol(argv[index+2]);
    FILES_PER_ITERATION = atol(argv[index+3]);

    if (strcmp(mode,"seq")==0) {
        parallel = 0;
    }
    if (parallel) {
        printf("Running parallel in %ld threads. %ld iterations\n", NUM_THREADS, FILES_PER_ITERATION);
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
    time_t ltime;
    struct tm result;
    char stime[32];

    for (i=0; i<FILES_PER_ITERATION;i++) {
        if (opt_write) {
            sprintf(output_file,"output/output_file_%ld-%ld.bufr", data->number, i);
            encode_file(INPUT_FILE,output_file);
        } else {
            encode_file(INPUT_FILE,NULL);
        }
    }

    ltime = time(NULL);
    localtime_r(&ltime, &result);
    strftime(stime, 32, "%H:%M:%S", &result); /* Try to get milliseconds here too*/
    /* asctime_r(&result, stime); */

    printf("%s: Worker %ld finished.\n", stime, data->number);
}
