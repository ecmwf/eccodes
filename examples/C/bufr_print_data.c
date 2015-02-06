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
 * C Implementation: bufr_print_data
 *
 * Description: how to read/print data values from synop BUFR messages.
 *
 */

#include "eccodes.h"

void usage(char* prog) {
    printf("usage: %s infile\n",prog);
    exit(1);
}

int main(int argc,char* argv[])
{
    FILE* in = NULL;
    
    /* message handle. Required in all the eccodes calls acting on a message.*/
    codes_handle* h=NULL;

    long longVal;
    double doubleVal;
    int err=0;
    int cnt=0;
    char* infile = "../../data/bufr/syno_multi.bufr";

    in=fopen(infile,"r");
    if (!in) {
        printf("ERROR: unable to open file %s\n", infile);
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
    
        /* we need to instruct ecCodes to unpack the data values */
        CODES_CHECK(codes_set_long(h,"unpack",1),0);
    
        /* read and print some data values */ 
    
        CODES_CHECK(codes_get_long(h,"blockNumber",&longVal),0);
        printf("  blockNumber: %ld\n",longVal);
    
        CODES_CHECK(codes_get_long(h,"stationNumber",&longVal),0);
        printf("  stationNumber: %ld\n",longVal);
    
        /* in the current BUFR message this key represents the 2m temperature. 
           it might not be available in other type of SYNOP messages */
        CODES_CHECK(codes_get_double(h,"airTemperatureAt2M",&doubleVal),0);
        printf("  airTemperatureAt2M %f\n",doubleVal);
    
        /* delete handle */
        codes_handle_delete(h);
        
        cnt++;
    }
    
    fclose(in);
    return 0;
}
