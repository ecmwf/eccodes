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
 * C Implementation: bufr_keys_iterator
 *
 * Description: how to use keys_iterator functions and the
 *               codes_keys_iterator structure to get all the available
 *               keys in a BUFR message.
 *
 */

#include "eccodes.h"

#define MAX_VAL_LEN  1024

void usage(char* prog) {
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
    int keyType;

    /* To skip certain keys use the combination of these flags:

       unsigned long key_iterator_filter_flags=
            CODES_KEYS_ITERATOR_SKIP_READ_ONLY |
            CODES_KEYS_ITERATOR_SKIP_COMPUTED |
            CODES_KEYS_ITERATOR_SKIP_CODED |
            CODES_KEYS_ITERATOR_SKIP_EDITION_SPECIFIC |
            CODES_KEYS_ITERATOR_SKIP_DUPLICATES |
            CODES_KEYS_ITERATOR_SKIP_FUNCTION; */

    unsigned long key_iterator_filter_flags=CODES_KEYS_ITERATOR_ALL_KEYS;

    /* name_space=NULL to get all the keys. Other namespaces are e.g. "ls" */
    char* name_space=0;

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

    /* loop over the messages in the bufr file */
    while ((h = codes_handle_new_from_file(NULL,in,PRODUCT_BUFR,&err)) != NULL || err != CODES_SUCCESS)
    {
        codes_keys_iterator* kiter=NULL;
        if (h == NULL) {
            printf("Error: unable to create handle for message %d\n",cnt);
            cnt++;
            continue;
        }

        printf("message: %d\n",cnt);

        /* get key iterator */
        kiter=codes_keys_iterator_new(h,key_iterator_filter_flags,name_space);
        if (!kiter) {
            printf("ERROR: Unable to create keys iterator\n");
            exit(1);
        }

        /* we need to instruct ecCodes to unpack the data values */
        CODES_CHECK(codes_set_long(h,"unpack",1),0);

        /* loop over the keys */
        while(codes_keys_iterator_next(kiter))
        {
            /* get key name*/
            const char* name = codes_keys_iterator_get_name(kiter);
            printf("  %s=",name);

            CODES_CHECK(codes_get_native_type(h,name,&keyType),0);

            /* get key size to see if it is an array */
            CODES_CHECK(codes_get_size(h,name,&klen),0);

            /* not array */
            if(klen ==1)
            {    
                vlen=MAX_VAL_LEN;
                bzero(value,vlen);

                codes_get_string(h,name,value,&vlen);
                printf("%s\n",value);                             
            }
            /* for arrays */    
            else
                printf("(array of %ld)\n",klen);
        }

        /* delete key iterator */
        codes_keys_iterator_delete(kiter);

        /* delete handle */
        codes_handle_delete(h);

        cnt++;
    }

    fclose(in);
    return 0;
}
