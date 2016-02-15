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
 * C Implementation: get_product_kind
 *
 * Description: how to process a file containing a mix of messages
 *              and print the kind of product (e.g. GRIB, BUFR etc)
 */
#include "eccodes.h"
void usage(const char *app)
{
    fprintf(stderr,"Usage: %s file\n", app);
}

int main(int argc,char* argv[])
{
    FILE* in = NULL;
    codes_handle* h=NULL;
    char* kind= NULL;
    size_t len=0;
    int err=0, cnt=0;
    char* infile = NULL;

    if (argc != 2) {
        usage(argv[0]);
        return 1;
    }
    infile = argv[1];
    in=fopen(infile,"r");
    if (!in) {
        printf("ERROR: unable to open file %s\n", infile);
        return 1;
    }
    
    while ((h = codes_handle_new_from_file(NULL,in,PRODUCT_ANY,&err)) != NULL || err != CODES_SUCCESS)
    {
        if (h == NULL) {
            printf("Error: unable to create handle for message %d\n",cnt);
            cnt++;
            continue;
        }
        printf("message: %d\n",cnt);

        CODES_CHECK(codes_get_length(h, "kindOfProduct", &len), 0);
        kind = (char*)malloc(len*sizeof(char));
        
        codes_get_string(h, "kindOfProduct", kind, &len);
        printf("  product: %s\n", kind);
    
        free(kind);
        codes_handle_delete(h);
        cnt++;
    }
    fclose(in);
    return 0;
}
