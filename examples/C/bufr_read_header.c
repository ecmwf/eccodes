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
 * C Implementation: bufr_read_header
 *
 * Description: how to read the header of BUFR messages.
 *
 */

#include "eccodes.h"

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
        if (h == NULL) {
            printf("Error: unable to create handle for message %d\n",cnt);
            cnt++;
            continue;
        }

        printf("message: %d\n",cnt);
    
        /* get and print some keys form the BUFR header */ 
    
        CODES_CHECK(codes_get_long(h,"dataCategory",&longVal),0);
        printf("  dataCategory: %ld\n",longVal);
    
        CODES_CHECK(codes_get_long(h,"dataSubCategory",&longVal),0);
        printf("  dataSubCategory: %ld\n",longVal);
        
        CODES_CHECK(codes_get_long(h,"typicalDate",&longVal),0);
        printf("  typicalDate: %ld\n",longVal);
    
        CODES_CHECK(codes_get_long(h,"bufrHeaderCentre",&longVal),0);
        printf("  bufrHeaderCentre: %ld\n",longVal);
    
        CODES_CHECK(codes_get_long(h,"bufrHeaderSubCentre",&longVal),0);
        printf("  bufrHeaderSubCentre: %ld\n",longVal);
    
        CODES_CHECK(codes_get_long(h,"masterTablesVersionNumber",&longVal),0);
        printf("  masterTablesVersionNumber: %ld\n",longVal);
    
        CODES_CHECK(codes_get_long(h,"localTablesVersionNumber",&longVal),0);
        printf("  localTablesVersionNumber: %ld\n",longVal);
    
        CODES_CHECK(codes_get_long(h,"numberOfSubsets",&longVal),0);
        printf("  numberOfSubsets: %ld\n",longVal);
    
        /* delete handle */
        codes_handle_delete(h);
        
        cnt++;
    }
    
    fclose(in);
    return 0;
}
