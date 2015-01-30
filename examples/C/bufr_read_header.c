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
    
    double *values = NULL;
    long *desc = NULL;
    long longVal;
    size_t values_len=0;
    int err=0, i;
    
    if (argc!=2) usage(argv[0]);
    
    filename=argv[1];

    in=fopen(filename,"r");
    if (!in) {
        printf("ERROR: unable to open file %s\n", filename);
        return 1;
    }

    /* create new handle from a message in a file*/
    h=bufr_new_from_file(NULL,in,&err);
   
    if (h == NULL) {
        printf("Error: unable to create handle from file %s\n",filename);
    }
    
    /* check for errors after reading a message. */
    if (err != CODES_SUCCESS) CODES_CHECK(err,0);
    
    /* get and print some keys form the BUFR header */ 
    CODES_CHECK(codes_get_long(h,"centre",&longVal),0);
    printf("\tcentre: %ld\n",longVal);
    
    CODES_CHECK(codes_get_long(h,"subCentre",&longVal),0);
    printf("\tsubCentre: %ld\n",longVal);
    
    CODES_CHECK(codes_get_long(h,"masterTablesVersionNumber",&longVal),0);
    printf("\tmasterTablesVersionNumber: %ld\n",longVal);
    
    CODES_CHECK(codes_get_long(h,"localTablesVersionNumber",&longVal),0);
    printf("\tlocalTablesVersionNumber: %ld\n",longVal);
    
    CODES_CHECK(codes_get_long(h,"numberOfSubsets",&longVal),0);
    printf("\numberOfSubsets: %ld\n",longVal);
    
    /* delete hanle */
    codes_handle_delete(h);

    fclose(in);
    return 0;
}
