/*
 * Copyright 2005-2015 ECMWF.
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
 * Description: Example on how to use keys_iterator functions and the
 *               codes_keys_iterator structure to get all the available
 *               keys in a BUFR message.
 *
 */

#include "eccodes.h"

#define MAX_KEY_LEN  255
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
    long longVal;
    int err=0, cnt=0;
    
    /* To skip read only and not coded keys
     unsigned long key_iterator_filter_flags=CODES_KEYS_ITERATOR_SKIP_READ_ONLY ||
     CODES_KEYS_ITERATOR_SKIP_COMPUTED;
    */
    unsigned long key_iterator_filter_flags=CODES_KEYS_ITERATOR_ALL_KEYS;
 
    /* name_space=NULL to get all the keys */
    char* name_space=0;
    
    char value[MAX_VAL_LEN];
    size_t vlen=MAX_VAL_LEN;
    
    if (argc!=2) usage(argv[0]);
    
    filename=argv[1];

    in=fopen(filename,"r");
    if (!in) {
        printf("ERROR: unable to open file %s\n", filename);
        return 1;
    }
    
    /* loop over the messages in the bufr file */
    while ((h = bufr_new_from_file(NULL,in,&err)) != NULL || err != CODES_SUCCESS)
    {
        if (h == NULL) {
            printf("Error: unable to create handle for message %d\n",cnt);
            cnt++;
            continue;
        }
        
        printf("message: %d\n",cnt);
    
        codes_keys_iterator* kiter=NULL;
    
        /* get key oterator */
        kiter=codes_keys_iterator_new(h,key_iterator_filter_flags,name_space);
        if (!kiter) {
            printf("ERROR: Unable to create keys iterator\n");
            exit(1);
        }
        
        /* loop over the keys */
        while(codes_keys_iterator_next(kiter))
        {
            /* print key name*/
            const char* name = codes_keys_iterator_get_name(kiter);
            printf("  %s\n",name);
        }
 
        codes_keys_iterator_delete(kiter);
    
        /* delete handle */
        codes_handle_delete(h);
        
        cnt++;
    }
    
    fclose(in);
    return 0;
}
