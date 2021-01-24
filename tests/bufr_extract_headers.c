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
#include <assert.h>

#define MAX_KEYS 100
#if 0
int main(int argc, char* argv[])
{
    char *filename;
    int err = 0;
    int num_messages = 0, i =0;
    off_t* offsets = NULL;
    codes_context* c = codes_context_get_default();
    const int strict_mode = 1;

    /* Usage: prog file */
    assert(argc == 2);

    filename = argv[1];
    err = codes_extract_offsets_malloc(c, filename, PRODUCT_GRIB, &offsets, &num_messages, strict_mode);
    assert(!err);
    
    for (i = 0; i < num_messages; ++i) {
        printf("Message #%d: %lu\n", i, offsets[i]);
    }
    free(offsets);
    return 0;
}
#endif
#if 1
int main(int argc, char* argv[])
{
    char *filename, *keys;
    int i, err = 0;
    int num_messages                = 0;
    codes_bufr_header* header_array = NULL;
    codes_context* c                = codes_context_get_default();
    const int strict_mode           = 1;
    int requested_print_keys_count  = MAX_KEYS;
    codes_values requested_print_keys[MAX_KEYS];

    /* Usage: prog keys file */
    assert(argc == 3);

    keys     = argv[1]; /* comma-separated like bufr_ls/bufr_get */
    filename = argv[2];

    err = codes_bufr_extract_headers_malloc(c, filename, &header_array, &num_messages, strict_mode);
    if (err) {
        printf("ERROR: %s\n", grib_get_error_message(err));
        return 1;
    }

    /* Mimic the behaviour of bufr_get -f -p keys for testing */
    err = parse_keyval_string(NULL, keys, 0, GRIB_TYPE_UNDEFINED, requested_print_keys, &requested_print_keys_count);
    if (err) return 1;
    assert(requested_print_keys_count > 0);
    for (i = 0; i < num_messages; ++i) {
        int j;
        for (j = 0; j < requested_print_keys_count; ++j) {
            size_t vlen     = 0;
            char value[512] = {0,};
            CODES_CHECK(codes_bufr_header_get_string(&header_array[i], requested_print_keys[j].name, value, &vlen), 0);
            assert(vlen > 0);
            if (j > 0) printf(" ");
            printf("%s", value);
        }
        printf("\n");
    }

    free(header_array);
    for (i = 0; i < requested_print_keys_count; ++i) {
        free((char*)requested_print_keys[i].name);
    }

    return 0;
}
#endif
