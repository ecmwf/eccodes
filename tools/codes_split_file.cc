/*
 * (C) Copyright 2005- ECMWF.
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
 * 2019-07-26 W.Qu  Allow an input file to be split into each individual message (if nchunk=-1)
 *
 */

#include "grib_api_internal.h"
#include <assert.h>

static bool verbose = false;
static const char* OUTPUT_FILENAME_FORMAT = "%s_%03d"; /* x_001, x_002 etc */
static void usage(const char* prog)
{
    printf("Usage: %s [-v] nchunks infile\n", prog);
    printf("Setting nchunks=-1 splits infile into individual messages\n");
    exit(1);
}

static void print_num_messages_written(const char* output_filename, size_t n)
{
    printf("Wrote output file %s (%zu message%s)\n", output_filename, n, n==1?"":"s");
}

static int split_file(FILE* in, const char* filename, const int nchunks, unsigned long* count)
{
    void* mesg = NULL;
    size_t size = 0, read_size = 0, insize = 0, chunk_size, msg_size = 0, num_msg = 0;
    off_t offset = 0;
    int err = GRIB_SUCCESS, i = 0;
    grib_context* c = grib_context_get_default();
    assert(in);

    /* name of output file */
    size_t ofilenameMaxLen = strlen(filename) + 10;
    char* ofilename = (char*)calloc(1, ofilenameMaxLen);

    fseeko(in, 0, SEEK_END);
    insize = ftello(in);
    fseeko(in, 0, SEEK_SET);
    if (nchunks == -1) {
        chunk_size = size;
    }
    else {
        assert(nchunks > 0);
        chunk_size = insize / nchunks;
    }

    i = 1;
    snprintf(ofilename, ofilenameMaxLen, OUTPUT_FILENAME_FORMAT, filename, i);
    FILE* out = fopen(ofilename, "w");
    if (!out) {
        perror(ofilename);
        free(ofilename);
        return GRIB_IO_PROBLEM;
    }

    while (err != GRIB_END_OF_FILE) {
        mesg = wmo_read_any_from_file_malloc(in, 0, &size, &offset, &err);
        num_msg++;
        if (mesg != NULL && err == 0) {
            if (fwrite(mesg, 1, size, out) != size) {
                perror(ofilename);
                free(ofilename);
                fclose(out);
                return GRIB_IO_PROBLEM;
            }
            grib_context_free(c, mesg);
            read_size += size;
            msg_size += size;
            if (read_size > chunk_size && msg_size < insize) {
                if (verbose) {
                    print_num_messages_written(ofilename, num_msg);
                }
                fclose(out);
                i++;
                /* Start writing to the next file */
                /*printf("=2=%d\t%d\n",*count,msg_size);*/
                snprintf(ofilename, ofilenameMaxLen, OUTPUT_FILENAME_FORMAT, filename, i);
                out = fopen(ofilename, "w");
                if (!out) {
                    perror(ofilename);
                    free(ofilename);
                    return GRIB_IO_PROBLEM;
                }
                read_size = 0;
                num_msg   = 0;
            }
            (*count)++;
        }
    }
    if (verbose) {
        print_num_messages_written(ofilename, num_msg - 1);
    }
    fclose(out);
    free(ofilename);

    if (err == GRIB_END_OF_FILE)
        err = GRIB_SUCCESS;

    return err;
}

int main(int argc, char* argv[])
{
    int i, status = 0;
    int err = 0, nchunks = 0;
    unsigned long count = 0;

    if (argc < 3)
        usage(argv[0]);

    i = 1;
    if (strcmp(argv[i], "-v") == 0) {
        i++;
        verbose = true;
        if (argc != 4)
            usage(argv[0]);
    }

    /* add some error checking */
    nchunks = atoi(argv[i]);
    if (nchunks < 1 && nchunks != -1) {
        fprintf(stderr, "ERROR: Invalid number %d. Please specify a positive integer or -1\n", nchunks);
        return 1;
    }

    i++;
    const char* filename = argv[i];
    if (path_is_directory(filename)) {
        fprintf(stderr, "ERROR: %s: Is a directory\n", filename);
        return 1;
    }
    FILE* infh = fopen(filename, "rb");
    if (!infh) {
        perror(filename);
        return 1;
    }

    count = 0;
    err   = split_file(infh, filename, nchunks, &count);
    if (err) {
        fprintf(stderr, "ERROR: Failed to split file %s", filename);
        fprintf(stderr, "\n");
        status = 1;
    }
    else {
        if (verbose)
            printf("%7lu %s\n", count, filename);
    }

    fclose(infh);

    return status;
}
