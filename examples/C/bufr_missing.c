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
 * C Implementation: bufr_missing
 *
 * Description: how to handle missing values in BUFR messages.
 *
 */

#include "eccodes.h"


int main(int argc,char* argv[])
{
    FILE* in = NULL;
    
    /* message handle. Required in all the eccodes calls acting on a message.*/
    codes_handle* h=NULL;
    
    char* units= NULL;
    char* unitsPercent= NULL;
    long longVal;
    double doubleVal;
    size_t values_len=0, desc_len=0, len=0;
    int i, err=0;
    int cnt=0;
    char* infile = "../../data/bufr/syno_1.bufr";

    in=fopen(infile,"r");
    if (!in) {
        printf("ERROR: unable to open file %s\n", infile);
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
    
        /* we need to instruct ecCodes to expand the descriptors 
          i.e. unpack the data values */
        CODES_CHECK(codes_set_long(h,"unpack",1),0);
    
        /* This value is missing */
        CODES_CHECK(codes_get_double(h,"relativeHumidity",&doubleVal),0);
        printf("  relativeHumidity: %.2f %d\n",doubleVal,err);
    
        /* This value exists */
        CODES_CHECK(codes_get_double(h,"airTemperatureAt2M",&doubleVal),0);
        printf("  airTemperatureAt2M: %.2f %d\n",doubleVal,err);
    
        /*longVal=10;
        err=codes_is_missing(h,"relativeHumidity",&longVal);
        printf(" missing: %ld %ld\n",longVal,err);
        
        err=codes_is_missing(h,"airTemperatureAt2M",&longVal);
        printf(" missing: %ld %ld\n",longVal,err);*/
    
        /* delete handle */
        codes_handle_delete(h);
        
        cnt++;
    }
    
    fclose(in);
    return 0;
}
