/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_api_internal.h"

static int fail_on_error    = 1;
static const char* toolname = NULL;

static void usage(const char* prog)
{
    printf("Usage: %s [-v] [-f] infile1 infile2 ... \n", prog);
    exit(1);
}

// This version uses malloc and reads the whole contents of each message
static int count_messages_slow(FILE* in, int message_type, unsigned long* count)
{
    void* mesg   = NULL;
    size_t size  = 0;
    off_t offset = 0;
    int err      = GRIB_SUCCESS;
    typedef void* (*wmo_read_proc)(FILE*, int, size_t*, off_t*, int*);
    wmo_read_proc wmo_read = NULL;
    grib_context* c        = grib_context_get_default();

    if (message_type == CODES_GRIB)
        wmo_read = wmo_read_grib_from_file_malloc;
    else if (message_type == CODES_BUFR)
        wmo_read = wmo_read_bufr_from_file_malloc;
    else if (message_type == CODES_GTS)
        wmo_read = wmo_read_gts_from_file_malloc;
    else
        wmo_read = wmo_read_any_from_file_malloc;

    if (fail_on_error) {
        while ((mesg = wmo_read(in, 0, &size, &offset, &err)) != NULL && err == GRIB_SUCCESS) {
            grib_context_free(c, mesg);
            (*count)++;
        }
    }
    else {
        int done = 0;
        while (!done) {
            mesg = wmo_read(in, 0, &size, &offset, &err);
            // printf("Count so far=%ld, mesg=%x, err=%d (%s)\n", *count, mesg, err, grib_get_error_message(err));
            if (!mesg) {
                if (err == GRIB_END_OF_FILE || err == GRIB_PREMATURE_END_OF_FILE) {
                    done = 1; // reached the end
                }
            }
            if (mesg && !err) {
                (*count)++;
            }
            grib_context_free(c, mesg);
        }
    }

    if (err == GRIB_END_OF_FILE)
        err = GRIB_SUCCESS;

    if (mesg) grib_context_free(c, mesg);

    return err;
}

// This version does not store the message contents (no malloc). Much faster
static int count_messages_fast(FILE* in, int message_type, unsigned long* count)
{
    int err      = GRIB_SUCCESS;
    // Signature: FILE* f, size_t* msg_len, off_t* msg_offset
    typedef int (*wmo_read_proc)(FILE* , size_t*, off_t*);
    wmo_read_proc wmo_read = NULL;
    size_t msg_len = 0;
    off_t msg_offset = 0;

    if (message_type == CODES_GRIB)
        wmo_read = wmo_read_grib_from_file_fast;
    else if (message_type == CODES_BUFR)
        wmo_read = wmo_read_bufr_from_file_fast;
    else if (message_type == CODES_GTS)
        wmo_read = wmo_read_gts_from_file_fast;
    else
        wmo_read = wmo_read_any_from_file_fast;

    if (fail_on_error) {
        while ((err = wmo_read(in, &msg_len, &msg_offset)) == GRIB_SUCCESS) {
            //printf("%zu %ld\n", msg_len,msg_offset);
            (*count)++;
        }
    }
    else {
        int done = 0;
        while (!done) {
            err = wmo_read(in, &msg_len, &msg_offset);
            if (err) {
                if (err == GRIB_END_OF_FILE || err == GRIB_PREMATURE_END_OF_FILE) {
                    done = 1; // reached the end
                }
            }
            else {
                (*count)++;
            }
        }
    }

    if (err == GRIB_END_OF_FILE)
        err = GRIB_SUCCESS;

    return err;
}

int main(int argc, char* argv[])
{
    FILE* infh = NULL;
    char* filename;
    int i, verbose = 0;
    int err = 0, files_processed = 0;
    unsigned long count_total = 0, count_curr = 0;
    int message_type = 0; // GRIB, BUFR etc
    typedef int (*count_proc)(FILE*, int, unsigned long*);
    count_proc do_count = count_messages_fast;

    toolname = argv[0];
    if (argc < 2)
        usage(toolname);

    if (strstr(toolname, "grib_count"))
        message_type = CODES_GRIB;
    if (strstr(toolname, "bufr_count"))
        message_type = CODES_BUFR;
    if (strstr(toolname, "gts_count"))
        message_type = CODES_GTS;

    count_total = 0;
    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-v") == 0) {
            verbose = 1;
            continue;
        }
        if (strcmp(argv[i], "-f") == 0) {
            fail_on_error = 0;
            continue;
        }
        filename = argv[i];
        if (path_is_directory(filename)) {
            fprintf(stderr, "%s: ERROR: \"%s\": Is a directory\n", toolname, filename);
            continue;
        }

        if (strcmp(filename, "-") == 0) {
            infh = stdin;
            do_count = count_messages_slow; // cannot do fseek on stdin
        } else {
            infh = fopen(filename, "rb");
        }
        if (!infh) {
            perror(filename);
            exit(1);
        }

        files_processed = 1; // At least one file processed
        count_curr      = 0;
        err             = do_count(infh, message_type, &count_curr);
        if (err && fail_on_error) {
            fprintf(stderr, "Invalid message(s) found in %s", filename);
            if (count_curr > 0)
                fprintf(stderr, " (got as far as %lu)", count_curr);
            fprintf(stderr, "\n");
            exit(err);
#ifdef DONT_EXIT_ON_BAD_APPLE
            // If we did not want to fail but warn and continue
            fclose(infh);
            continue;
#endif
        }
        if (verbose)
            printf("%7lu %s\n", count_curr, filename);
        count_total += count_curr;

        fclose(infh);
    }

    if (!files_processed)
        usage(argv[0]);
    if (verbose) {
        printf("%7lu %s\n", count_total, "total");
    }
    else {
        printf("%lu\n", count_total);
    }

    return 0;
}
