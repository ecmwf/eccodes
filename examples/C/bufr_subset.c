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
 * C Implementation: bufr_subset
 *
 * Description: how to read data values from a given subset of a BUFR message.
 *
 */

#include "eccodes.h"

int main(int argc,char* argv[])
{
    char key[200]={0,};
    FILE* in = NULL;

    /* message handle. Required in all the eccodes calls acting on a message.*/
    codes_handle* h=NULL;

    long numberOfSubsets=0;
    long longVal;
    double doubleVal;
    size_t stringLen;
    char stringVal[100]={0,};
    int i,err=0;
    int cnt=0;
    const char* infile = "../../data/bufr/synop_multi_subset.bufr";

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

        /* we need to instruct ecCodes to expand all the descriptors i.e. unpack the data values */
        CODES_CHECK(codes_set_long(h,"unpack",1),0);

        /* find out the number of subsets */
        CODES_CHECK(codes_get_long(h,"numberOfSubsets",&numberOfSubsets),0);
        printf("  numberOfSubsets: %ld\n",numberOfSubsets);

        /* loop over the subsets */
        for(i=1; i <= numberOfSubsets; i++)
        {
            sprintf(key,"/subsetNumber=%d/blockNumber",i);

            printf("  subsetNumber=%d",i);
            /* read and print some data values */
            CODES_CHECK(codes_get_long(h,key,&longVal),0);
            printf("  blockNumber=%ld",longVal);

            sprintf(key,"/subsetNumber=%d/stationNumber",i);
            CODES_CHECK(codes_get_long(h,key,&longVal),0);
            printf("  stationNumber=%ld",longVal);

            sprintf(key,"/subsetNumber=%d/stationOrSiteName",i);
            stringLen=100;
            CODES_CHECK(codes_get_string(h,key,stringVal,&stringLen),0);
            printf("  stationOrSiteName=\"%s\"",stringVal);

            sprintf(key,"/subsetNumber=%d/airTemperature",i);
            CODES_CHECK(codes_get_double(h,key,&doubleVal),0);
            printf("  airTemperature=%g\n",doubleVal);

        }

        /* delete handle */
        codes_handle_delete(h);

        cnt++;
    }

    fclose(in);
    return 0;
}
