/*
 * Copyright 2005-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_api_internal.h"

void usage(const char* prog)
{
    printf("usage: %s [-v] infile1 infile2 ... \n",prog);
    exit(1);
}

static int count_messages(FILE* in, int message_type, unsigned long *count)
{
    void* mesg=NULL;
    size_t size=0;
    off_t offset=0;
    int err=GRIB_SUCCESS;
    typedef void* (*wmo_read_proc)(FILE *, int, size_t *, off_t *, int *);
    wmo_read_proc wmo_read = NULL;
    grib_context* c=grib_context_get_default();

    if (!in) return 1;
    /* printf("message_type=%d\n", message_type); */
    if (message_type == CODES_GRIB)        wmo_read=wmo_read_grib_from_file_malloc;
    else if (message_type == CODES_BUFR)   wmo_read=wmo_read_bufr_from_file_malloc;
    else                                   wmo_read=wmo_read_any_from_file_malloc;

    while ( (mesg=wmo_read(in,0,  &size,&offset,&err))!=NULL && err==GRIB_SUCCESS) {
        grib_context_free(c,mesg);
        (*count)++;
    }

    if (err==GRIB_END_OF_FILE) err=GRIB_SUCCESS;

    return err;
}

int main(int argc,char* argv[])
{
    FILE* infh = NULL;
    char* filename;
    int i, verbose=0;
    int err=0;
    unsigned long count_total=0, count_curr=0;
    int message_type = 0; /* GRIB, BUFR etc */

    if (argc <2) usage(argv[0]);
    
    if (strstr(argv[0], "grib_count")) message_type = CODES_GRIB;
    if (strstr(argv[0], "bufr_count")) message_type = CODES_BUFR;

    count_total=0;
    for (i=1;i<argc;i++) {
        if (strcmp(argv[i], "-v")==0) {
            verbose = 1;
            if (argc <3) usage(argv[0]);
            continue;
        }
        filename=argv[i];
        infh=fopen(filename,"r");
        if (!infh) {
            perror(filename);
            exit(1);
        }

        count_curr=0;
        err=count_messages(infh, message_type, &count_curr);
        if (err) {
            fprintf(stderr,"Invalid message(s) found in %s\n", filename);
            exit(err);
#ifdef DONT_EXIT_ON_BAD_APPLE
            /* If we did not want to fail but warn and continue */
            fclose(infh);
            continue;
#endif
        }
        if (verbose) printf ("%7lu %s\n", count_curr, filename);
        count_total += count_curr;

        fclose(infh);
    }
    if (verbose) {
        printf("%7lu %s\n", count_total, "total");
    } else {
        printf("%lu\n", count_total);
    }

    return 0;
}
