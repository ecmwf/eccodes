#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <assert.h>
#include <string.h>

#include "grib_api.h"

#define NUM_THREADS 8
#define FILES_PER_ITERATION 500
#define TEMPLATE "../../share/eccodes/samples/gg_sfc_grib2.tmpl"


static int encode_file(char *template_file, char *output_file)
{
    FILE *in = NULL;
    FILE *out = NULL;
    grib_handle *source_handle = NULL;
    const void *buffer = NULL;
    size_t size = 0;
    int err = 0;
 
    in = fopen(template_file,"r"); assert(in);
    out = fopen(output_file,"w");  assert(out);
 
    /* loop over the messages in the source grib and clone them */
    while ((source_handle = grib_handle_new_from_file(0, in, &err))!=NULL)
    {
        grib_handle *clone_handle = grib_handle_clone(source_handle);
 
        if (clone_handle == NULL) {
            perror("ERROR: could not clone field");
            return 1;
        }
 
        //GRIB_CHECK(grib_set_long(clone_handle, "centre", 250),0);

        size_t values_len= 0;
        GRIB_CHECK(grib_get_size(clone_handle, "values", &values_len),0);

        int i;
        double *values = NULL; 
           values = (double*)malloc(values_len*sizeof(double));
        long count;
        double d,e;


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
 
 
        /* get the coded message in a buffer */
        GRIB_CHECK(grib_get_message(clone_handle,&buffer,&size),0);
        /* write the buffer to a file */
        if(fwrite(buffer,1,size,out) != size) {
            perror(output_file);
            return 1;
        }
        grib_handle_delete(clone_handle);
        grib_handle_delete(source_handle);
    }
 
    fclose(out);
    fclose(in);
 
    return 0;
}

void* do_stuff(void *data);

/* Structure for passing data to threads */
struct v
{
    int number; 
    char *data; /* column */
};

void *runner(void *ptr); /* the thread */

int main(int argc, char **argv)
{
    int i;
    int thread_counter = 0;
    int parallel = !(argc>1 && strcmp(argv[1],"seq")==0);

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
            /* Now we will create the thread passing it data as a paramater*/
            pthread_create(&workers[thread_counter], NULL, runner, data);
            //pthread_join(workers[thread_counter], NULL);
            thread_counter++;
        } else {
            do_stuff(data);
        }
    }
    
    /* Waiting for threads to complete */
    if (parallel) {
        for (i = 0; i < NUM_THREADS; i++)
        {
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

void *do_stuff(void *ptr)
{
    /* Casting paramater to struct v pointer */
    struct v *data = ptr;

    char output_file[50];

    for (int i=0; i<FILES_PER_ITERATION;i++) {
        sprintf(output_file,"output/output_file_%d-%d.grib",data->number,i);
        encode_file(TEMPLATE,output_file);
    }

    time_t ltime;
    struct tm result;
    char stime[32];

    ltime = time(NULL);
    localtime_r(&ltime, &result);
    strftime(stime, 32, "%H:%M:%S", &result); // Try to get milliseconds here too
//    asctime_r(&result, stime);

    printf("%s: Worker %d finished.\n", stime,data->number);
}
