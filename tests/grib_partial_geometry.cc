/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "eccodes.h"

#undef NDEBUG
#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_VAL_LEN  1024
#define METADATA_LEN 17000 // guess! works for nearly all GRIB2
const int verbose = 1;

#if defined(HAVE_ECKIT_GEO)
static int print_keys(codes_handle* h)
{
    char value[MAX_VAL_LEN] = {0,};
    size_t vlen = sizeof(value);
    int err = grib_get_string(h, "gridSpec", value, &vlen);
    if (!err) {
        printf("%s\n", value);
    } else {
        fprintf(stderr, "Error: %s\n", grib_get_error_message(err));
    }
    return err;
}
#else
static int print_keys(codes_handle* h)
{
    grib_keys_iterator* kiter = grib_keys_iterator_new(h, 0, "geography");
    while (grib_keys_iterator_next(kiter)) {
        const char* name = grib_keys_iterator_get_name(kiter);
        if (strcmp(name, "pl")==0) continue; // this one is an array!
        char value[MAX_VAL_LEN] = {0,};
        size_t vlen = sizeof(value);
        int err = grib_get_string(h, name, value, &vlen);
        if (!err) printf("%s = %s \n", name, value);
        else      { fprintf(stderr, "Error: %s\n", grib_get_error_message(err)); return err; }
    }
    grib_keys_iterator_delete(kiter);

    // long l=0;
    // int err = grib_get_long(h, "bitsPerValue", &l);
    // assert(err != GRIB_SUCCESS);
    return GRIB_SUCCESS;
}
#endif

static int process_messages_full(const char* filename)
{
    FILE* fin = fopen(filename, "rb");
    assert(fin);

    int err = 0, i = 0;
    codes_handle* h = 0;
    while ((h = codes_handle_new_from_file(0, fin, PRODUCT_GRIB, &err)) != NULL) {
        if (verbose) {
            printf("\nMsg %d\n--------\n", i+1);
        }
        err = print_keys(h);
        if (err) return err;
        codes_handle_delete(h);
        i++;
    }
    return err;
}

static int process_messages_partial(const char* filename)
{
    int num_messages = 0, i = 0;
    off_t* offsets        = NULL;  // array of message offsets
    size_t* sizes         = NULL;  // array of message sizes
    codes_context* c      = codes_context_get_default();
    const int strict_mode = 1;

    int err = codes_extract_offsets_sizes_malloc(c, filename, PRODUCT_ANY, &offsets, &sizes, &num_messages, strict_mode);
    if (err) return err;

    FILE* fin = fopen(filename, "rb");
    assert(fin);

    for (i = 0; i < num_messages; ++i) {
        if (verbose) {
            printf("\nMsg %d\n--------\n", i+1);
            // printf("\nMsg %d: offset=%lu size=%zu\n========================\n", i+1, (unsigned long)offsets[i], sizes[i]);
        }
        err = fseek(fin, offsets[i], SEEK_SET);
        assert(!err);
        char buf[METADATA_LEN];
        size_t num_bytes_to_read = sizeof(buf);
        if (sizes[i] < num_bytes_to_read) { // is message size smaller than the buffer size?
            num_bytes_to_read = sizes[i];
            //printf("Setting num_bytes_to_read to %zu \n", num_bytes_to_read);
        }
        if (fread(buf, 1, num_bytes_to_read, fin) != num_bytes_to_read) {
            perror(filename);
            exit(1);
        }

        codes_handle* h = codes_handle_new_from_partial_message(NULL, buf, num_bytes_to_read);
        assert(h);
        err = print_keys(h);
        if (err) return err;
        codes_handle_delete(h);
    }

    free(sizes);
    free(offsets);
    return err;
}

// Usage:
//     prog -f file
// or  prog -p file
int main(int argc, char* argv[])
{
    char* filename   = NULL;
    int err          = 0;
    int do_full = 0, do_partial = 0, oc = 0;

    assert(argc == 3);

    while ((oc = getopt(argc, argv, "fp")) != -1) {
        switch (oc) {
            case 'f':
                do_full = 1;
                break;
            case 'p':
                do_partial = 1;
                break;
        }
    }
    filename = argv[optind];

    if (do_full) {
        err = process_messages_full(filename);
    } else {
        assert(do_partial);
        err = process_messages_partial(filename);
    }

    return err;
}
