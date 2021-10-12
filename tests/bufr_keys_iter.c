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

static void usage(const char* prog)
{
    printf("usage: %s [-a|-d] infile\n", prog);
    exit(1);
}

#define ITER_ALL_KEYS 1
#define ITER_DATA_KEYS 2

int main(int argc, char* argv[])
{
    int err                         = 0;
    int i                           = 0;
    codes_handle* h                 = NULL;
    codes_bufr_keys_iterator* kiter = NULL;
    char* input_filename            = NULL;
    const char* prog                = argv[0];
    FILE* f                         = NULL;
    int iterator_mode               = ITER_ALL_KEYS;

    if (argc == 1 || argc > 3)
        usage(prog);

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-a") == 0) {
            iterator_mode = ITER_ALL_KEYS;
        }
        else if (strcmp(argv[i], "-d") == 0) {
            iterator_mode = ITER_DATA_KEYS;
        }
        else {
            input_filename = argv[i];
            break; /* Only 1 file allowed */
        }
    }

    assert(input_filename);
    f = fopen(input_filename, "rb");
    assert(f);
    h = codes_handle_new_from_file(NULL, f, PRODUCT_BUFR, &err);
    assert(h);

    CODES_CHECK(codes_set_long(h, "unpack", 1), 0);

    if (iterator_mode == ITER_ALL_KEYS) {
        /*printf("Dumping ALL keys\n");*/
        kiter = codes_bufr_keys_iterator_new(h, 0);
    }
    else {
        /*printf("Dumping only DATA SECTION keys\n");*/
        assert(iterator_mode == ITER_DATA_KEYS);
        kiter = codes_bufr_data_section_keys_iterator_new(h);
    }

    while (codes_bufr_keys_iterator_next(kiter)) {
        char* kname = codes_bufr_keys_iterator_get_name(kiter);
        printf("%s\n", kname);
    }
    codes_bufr_keys_iterator_delete(kiter);
    codes_handle_delete(h);
    codes_context_delete(codes_context_get_default());

    return 0;
}
