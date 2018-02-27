/*
 * Copyright 2005-2018 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

/*
 * C Implementation: bufr_keys_iterator
 *
 * Description: how to use keys_iterator functions and the
 *              codes_bufr_keys_iterator structure to get all the available
 *              keys in a BUFR message.
 *
 */

#include "eccodes.h"

#define MAX_VAL_LEN  1024

static void usage(const char* prog) {
    printf("usage: %s infile\n",prog);
    exit(1);
}

int main(int argc,char* argv[])
{
    char* filename = NULL;
    FILE* in = NULL;

    /* message handle. Required in all the eccodes calls acting on a message.*/
    codes_handle* h=NULL;
    int err=0, cnt=0;
    int keyType = 0;

    char value[MAX_VAL_LEN];
    size_t vlen=MAX_VAL_LEN;
    size_t klen=0;

    if (argc!=2) usage(argv[0]);

    filename=argv[1];

    in=fopen(filename,"r");
    if (!in) {
        printf("ERROR: unable to open file %s\n", filename);
        return 1;
    }

    /* loop over the messages in the BUFR file */
    while ((h = codes_handle_new_from_file(NULL,in,PRODUCT_BUFR,&err)) != NULL || err != CODES_SUCCESS)
    {
        codes_bufr_keys_iterator* kiter=NULL;
        if (h == NULL) {
            printf("Error: unable to create handle for message %d\n",cnt);
            cnt++;
            continue;
        }

        printf("message: %d\n",cnt);

        /* we need to instruct ecCodes to unpack the data values */
        CODES_CHECK(codes_set_long(h,"unpack",1),0);

        /* get BUFR key iterator */
        kiter=codes_bufr_keys_iterator_new(h,0);
        if (!kiter) {
            printf("ERROR: Unable to create BUFR keys iterator\n");
            exit(1);
        }

        /* loop over the keys */
        while(codes_bufr_keys_iterator_next(kiter))
        {
            /* get key name */
            char* name = codes_bufr_keys_iterator_get_name(kiter);
            printf("  %s=",name);

            CODES_CHECK(codes_get_native_type(h,name,&keyType),0);

            /* get key size to see if it is an array */
            CODES_CHECK(codes_get_size(h,name,&klen),0);

            if (klen ==1)
            {
                /* not array */
                vlen=MAX_VAL_LEN;
                bzero(value,vlen);
                codes_get_string(h,name,value,&vlen);
                printf("%s\n",value);
            }
            else {
                /* for arrays */
                printf("(array of %lu)\n",klen);
            }
        }

        /* delete key iterator */
        codes_bufr_keys_iterator_delete(kiter);

        /* delete handle */
        codes_handle_delete(h);

        cnt++;
    }

    fclose(in);
    return 0;
}
