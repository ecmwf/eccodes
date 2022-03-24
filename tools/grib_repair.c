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

static const char* ENV_VAR = "ECCODES_GRIB_REPAIR_MAX_NUM_MESSAGES";

static void usage(const char* name)
{
    fprintf(stderr, "Usage: %s in out [bad]\n", name);
    exit(1);
}

#define SIZE 50 * 1024 * 1024
char buffer[SIZE];

int main(int argc, char** argv)
{
    FILE *in, *out, *bad;
    char *cout, *cbad;

    size_t data_len = SIZE;
    unsigned long count = 0;
    unsigned long MAX_NUM_MESSAGES = 100*1000;
    unsigned char* data;
    char* sMaxNumMessages = NULL;

    if (argc != 3 && argc != 4)
        usage(argv[0]);

    in = fopen(argv[1], "r");
    if (!in) {
        perror(argv[1]);
        exit(1);
    }

    cout = argv[2];
    out  = fopen(argv[2], "w");
    if (!out) {
        perror(argv[2]);
        exit(1);
    }

    if (argc == 4) {
        cbad = argv[3];
        bad  = fopen(argv[3], "w");
        if (!bad) {
            perror(argv[3]);
            exit(1);
        }
    }
    else {
        bad  = out;
        cbad = cout;
    }

    sMaxNumMessages = getenv(ENV_VAR);
    if (sMaxNumMessages) {
        long lmax = 0;
        if (string_to_long(sMaxNumMessages, &lmax) == GRIB_SUCCESS) {
            MAX_NUM_MESSAGES = lmax;
        }
    }

    for (;;) {
        size_t len = SIZE;
        long ret   = wmo_read_grib_from_file(in, buffer, &len);
        if (ret == GRIB_END_OF_FILE && len == 0)
            break;
        if (count > MAX_NUM_MESSAGES) {
            /* ECC-1265 */
            printf("\nExceeded the maximum number of messages. Limit = %lu.\n", MAX_NUM_MESSAGES);
            printf("This limit can be overridden by setting the environment variable %s.\n", ENV_VAR);
            break;
        }

        printf("GRIB %lu: size: %ld code: %ld (%s)\n", ++count, (long)len, ret, grib_get_error_message(ret));

        switch (ret) {
            case 0:
                if (fwrite(buffer, 1, len, out) != len) {
                    perror(cout);
                    exit(1);
                }
                break;

            case GRIB_WRONG_LENGTH:
            case GRIB_PREMATURE_END_OF_FILE:
                fseek(in, -4, SEEK_CUR);
                memset(buffer + len - 4, '7', 4); /* add in 7777 at end */
                len = data_len = SIZE;
                data           = (unsigned char*)&buffer[0];
                ret            = grib_read_any_from_memory(NULL, &data, &data_len, buffer, &len);
                printf("   -> GRIB %lu: size: %ld code: %ld (%s)\n", count, (long)len, ret, grib_get_error_message(ret));
                if (ret == 0) {
                    if (fwrite(buffer, 1, len, bad) != len) {
                        perror(cbad);
                        exit(1);
                    }
                }
                break;
        }
    }

    if (fclose(in)) {
        perror(argv[1]);
        exit(1);
    }

    if (fclose(out)) {
        perror(argv[2]);
        exit(1);
    }

    if (argc == 4) {
        if (fclose(bad)) {
            perror(argv[3]);
            exit(1);
        }
    }

    return 0;
}
