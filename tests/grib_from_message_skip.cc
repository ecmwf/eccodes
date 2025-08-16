/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include <stdio.h>
#include "eccodes.h"

#undef NDEBUG
#include <assert.h>

#include <unistd.h>
#include <sys/stat.h>

int main(int argc, char* argv[])
{
    struct stat finfo;
    char *filename = NULL;
    int padding_present = 0, oc = 0;

    assert (argc == 2 || argc == 3);
    while ((oc = getopt(argc, argv, "p")) != -1) {
        switch (oc) {
            case 'p':
                padding_present = 1; // padding is expected. So must skip
                break;
        }
    }
    filename = argv[optind];

    FILE* in = fopen(filename, "rb");
    assert(in);
    stat(filename, &finfo);
    size_t fsize = finfo.st_size;

    unsigned char* data = (unsigned char*)malloc(fsize);
    assert(data);

    if (fread(data, 1, fsize, in) != fsize) {
        perror(filename);
        exit(1);
    }
    printf("File=%s\t",filename);

    if (padding_present) {
        // Try without skipping the padding. Should fail
        printf("Input GRIB has padding: So expect failure from codes_handle_new_from_message...\n");
        codes_handle* h = codes_handle_new_from_message(0, data, fsize);

        bool ok = false;
        char identifier[254] = {0,};
        size_t len = 254;
        int err = grib_get_string(h, "identifier", identifier, &len);
        if (!err) {
            printf("id=%s \n", identifier);
            ok = (strcmp(identifier, "GRIB") != 0);
        } else {
            ok = true;
        }
        assert(ok);
        //long isValid = -1;
        //int err = codes_get_long(h, "isMessageValid", &isValid);
        //printf("err=%d\n",err);
        //printf("isv=%ld\n",isValid);
        //assert( codes_is_defined(h, "7777") == 0);
    }

    // Now try with GRIB-specific call which will skip the padding
    codes_handle* h = codes_grib_handle_new_from_message(0, data, fsize);
    if (!h) {
        printf("Could not handle it!\n");
        return 1;
    }

    long totalLength = -1;
    CODES_CHECK(codes_get_long(h, "totalLength", &totalLength), 0);
    printf("totalLength=%ld\n", totalLength);

    codes_handle_delete(h);
    fclose(in);
    free(data);
    return 0;
}
