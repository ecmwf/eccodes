/*
 * Copyright 2005-2017 ECMWF.
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

void usage(const char* prog)
{
    printf("usage: %s infile\n",prog);
    exit(1);
}

int main(int argc,char* argv[])
{
    int err = 0;
    codes_handle* h = NULL;
    codes_bufr_keys_iterator* kiter = NULL;
    char* input_filename = NULL;
    FILE* f = NULL;
    /*grib_context* c = grib_context_get_default();*/
    
    if (argc!=2) usage(argv[0]);
    input_filename = argv[1];
    f = fopen(input_filename, "r");
    assert(f);
    h = codes_handle_new_from_file(NULL, f, PRODUCT_BUFR, &err);
    assert(h);

    CODES_CHECK(codes_set_long(h,"unpack",1), 0);

    /*kiter=codes_bufr_data_section_keys_iterator_new(h);*/
    kiter = codes_bufr_keys_iterator_new(h, 0);
    while(codes_bufr_keys_iterator_next(kiter))
    {
        char* kname = codes_bufr_keys_iterator_get_name(kiter);
        printf("%s\n", kname);
    }
    codes_bufr_keys_iterator_delete(kiter);
    codes_handle_delete(h);
    return 0;
}
