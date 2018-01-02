/*
 * Copyright 2005-2018 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

/*
 * Description:
 * Split an input file (GRIB, BUFR etc) into chunks of roughly the same size.
 * The output files are named input_01, input_02 etc. This is much faster than grib_copy/bufr_copy
 *
 */

#include "grib_api_internal.h"
#include <assert.h>

static int verbose = 0;
static const char* OUTPUT_FILENAME_FORMAT = "%s_%02d"; /* x_01, x_02 etc */
static void usage(const char* prog)
{
    printf("usage: %s [-v] nchunks infile\n",prog);
    exit(1);
}

static int split_file(FILE* in, const char* filename, const int nchunks, unsigned long *count)
{
    void* mesg=NULL;
    FILE* out;
    size_t size=0,read_size=0,insize=0,chunk_size;
    off_t offset=0;
    int err=GRIB_SUCCESS;
    int i;
    char* ofilename;
    grib_context* c=grib_context_get_default();

    if (!in) return 1;

    /* name of output file */
    ofilename=(char*)calloc(1,strlen(filename)+10);

    fseeko(in, 0, SEEK_END);
    insize = ftello(in);
    fseeko(in, 0, SEEK_SET);
    assert(nchunks > 0);
    chunk_size=insize/nchunks;

    i=1;
    sprintf(ofilename, OUTPUT_FILENAME_FORMAT, filename, i);
    out=fopen(ofilename,"w");
    if (!out) {
        perror(ofilename);
        free(ofilename);
        return GRIB_IO_PROBLEM;
    }

    while ( err!=GRIB_END_OF_FILE ) {
        mesg=wmo_read_any_from_file_malloc(in, 0, &size, &offset, &err);
        if (mesg!=NULL && err==0) {
            if (fwrite(mesg,1,size,out)!=size) {
                perror(ofilename);
                free(ofilename);
                fclose(out);
                return GRIB_IO_PROBLEM;
            }
            grib_context_free(c,mesg);
            read_size+=size;
            if (read_size>chunk_size) {
                if (verbose) printf("Wrote output file %s\n", ofilename);
                fclose(out);
                i++;
                /* Start writing to the next file */
                sprintf(ofilename, OUTPUT_FILENAME_FORMAT, filename, i);
                out=fopen(ofilename,"w");
                if (!out) {
                    perror(ofilename);
                    free(ofilename);
                    return GRIB_IO_PROBLEM;
                }
                read_size=0;
            }
            (*count)++;
        }
    }
    if (verbose) printf("Wrote output file %s\n", ofilename);
    fclose(out);
    free(ofilename);

    if (err==GRIB_END_OF_FILE) err=GRIB_SUCCESS;

    return err;
}

int main(int argc,char* argv[])
{
    FILE* infh = NULL;
    char* filename;
    int i, status=0;
    struct stat s;
    int err=0,nchunks=0;
    unsigned long count=0;

    if (argc <3) usage(argv[0]);

    i=1;
    if (strcmp(argv[i], "-v")==0) {
        i++;
        verbose = 1;
        if (argc !=4) usage(argv[0]);
    }

    /* add some error checking */
    nchunks=atoi(argv[i]);
    if (nchunks<1) {
        fprintf(stderr,"ERROR: Invalid number %d. Please specify a positive integer.\n", nchunks);
        return 1;
    }

    i++;
    filename=argv[i];
    if (stat(filename, &s)==0) {
        if (S_ISDIR(s.st_mode)) {
            fprintf(stderr, "ERROR: %s: Is a directory\n", filename);
            return 1;
        }
    }
    infh=fopen(filename,"r");
    if (!infh) {
        perror(filename);
        return 1;
    }

    count=0;
    err=split_file(infh, filename, nchunks, &count);
    if (err) {
        fprintf(stderr,"ERROR: Failed to split file %s", filename);
        fprintf(stderr,"\n");
        status = 1;
    } else {
        if (verbose) printf ("%7lu %s\n", count, filename);
    }

    fclose(infh);

    return status;
}
