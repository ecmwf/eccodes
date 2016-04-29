/*
 * Copyright 2005-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

/*
 * C Implementation: grib_keys_iterator
 *
 * Description:
 * Example on how to use keys_iterator functions and the
 * codes_keys_iterator structure to get all the available
 * keys in a GRIB message.
 *
 */

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "eccodes.h"

#define MAX_VAL_LEN  1024

static void usage(char* progname);

int main(int argc, char *argv[])
{
    /* To skip read only and computed keys
     unsigned long key_iterator_filter_flags=CODES_KEYS_ITERATOR_SKIP_READ_ONLY |
                                             CODES_KEYS_ITERATOR_SKIP_COMPUTED;
     */
    unsigned long key_iterator_filter_flags = CODES_KEYS_ITERATOR_ALL_KEYS |
                                              CODES_KEYS_ITERATOR_SKIP_DUPLICATES;

    /* Choose a namespace. E.g. "ls", "time", "parameter", "geography", "statistics" */
    char* name_space="ls";

    /* name_space=NULL to get all the keys */
    /* char* name_space=0; */

    FILE* f = NULL;
    codes_handle* h=NULL;

    int err=0;
    int msg_count=0;

    char value[MAX_VAL_LEN];
    size_t vlen=MAX_VAL_LEN;

    if (argc != 2) usage(argv[0]);

    f = fopen(argv[1],"r");
    if(!f) {
        perror(argv[1]);
        exit(1);
    }

    while((h = codes_handle_new_from_file(0,f,PRODUCT_GRIB,&err)) != NULL)
    {
        codes_keys_iterator* kiter=NULL;
        msg_count++;
        printf("-- GRIB N. %d --\n",msg_count);
        if(!h) {
            printf("ERROR: Unable to create grib handle\n");
            exit(1);
        }

        kiter=codes_keys_iterator_new(h,key_iterator_filter_flags,name_space);
        if (!kiter) {
            printf("ERROR: Unable to create keys iterator\n");
            exit(1);
        }

        while(codes_keys_iterator_next(kiter))
        {
            const char* name = codes_keys_iterator_get_name(kiter);
            vlen=MAX_VAL_LEN;
            bzero(value,vlen);
            CODES_CHECK(codes_get_string(h,name,value,&vlen),name);
            printf("%s = %s\n",name,value);

            /* Alternative way of getting the string value */
            CODES_CHECK(codes_keys_iterator_get_string(kiter, value, &vlen),0);
        }

        codes_keys_iterator_delete(kiter);
        codes_handle_delete(h);
    }
    fclose(f);
    return 0;
}

static void usage(char* progname)
{
    printf("\nUsage: %s grib_file\n",progname);
    exit(1);
}
