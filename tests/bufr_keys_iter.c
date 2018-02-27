/*
 * Copyright 2005-2018 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */
#include "grib_api_internal.h"
#include "eccodes.h"
#include <assert.h>

static void usage(const char* prog)
{
    printf("usage: %s [-a|-d] infile\n",prog);
    exit(1);
}

#define ITER_ALL_KEYS  1
#define ITER_DATA_KEYS 2

int main(int argc,char* argv[])
{
    int err = 0, opt = 0;
    codes_handle* h = NULL;
    codes_bufr_keys_iterator* kiter = NULL;
    char* input_filename = NULL;
    const char* prog = argv[0];
    FILE* f = NULL;
    int iterator_mode = ITER_ALL_KEYS;
    
    while ((opt = getopt(argc, argv, "ad")) != -1) {
        switch (opt) {
            case 'a':
                iterator_mode = ITER_ALL_KEYS;
                break;
            case 'd':
                iterator_mode = ITER_DATA_KEYS;
                break;
            default:
                usage(prog);
                break;
        }
    }
    /* After option processing expect just 1 file */
    if (argc-optind != 1) usage(prog);

    input_filename = argv[argc-1];
    f = fopen(input_filename, "r");
    assert(f);
    h = codes_handle_new_from_file(NULL, f, PRODUCT_BUFR, &err);
    assert(h);

    CODES_CHECK(codes_set_long(h,"unpack",1), 0);
    
    if (iterator_mode == ITER_ALL_KEYS) {
        /*printf("Dumping ALL keys\n");*/
        kiter = codes_bufr_keys_iterator_new(h, 0);
    } else {
        /*printf("Dumping only DATA SECTION keys\n");*/
        assert(iterator_mode == ITER_DATA_KEYS);
        kiter=codes_bufr_data_section_keys_iterator_new(h);
    }

    while(codes_bufr_keys_iterator_next(kiter))
    {
        char* kname = codes_bufr_keys_iterator_get_name(kiter);
        printf("%s\n", kname);
    }
    codes_bufr_keys_iterator_delete(kiter);
    codes_handle_delete(h);
    return 0;
}
