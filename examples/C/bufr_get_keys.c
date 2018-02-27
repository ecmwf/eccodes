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
 * C Implementation: bufr_get_keys
 *
 * Description: how to read values of different type of keys from BUFR messages.
 *
 */

#include "eccodes.h"

int main(int argc,char* argv[])
{
    FILE* in = NULL;

    /* message handle. Required in all the eccodes calls acting on a message.*/
    codes_handle* h=NULL;

    long *descriptors=NULL;
    double *values = NULL;
    char* typicalDate= NULL;
    long longVal;
    double doubleVal;
    size_t values_len=0, desc_len=0, len=0;
    int i, err=0;
    int cnt=0;
    const char* infile = "../../data/bufr/syno_multi.bufr";

    in=fopen(infile,"r");
    if (!in) {
        printf("ERROR: unable to open file %s\n", infile);
        return 1;
    }

    /* loop over the messages in the bufr file */
    while ((h = codes_handle_new_from_file(NULL, in, PRODUCT_BUFR, &err)) != NULL || err != CODES_SUCCESS)
    {
        if (h == NULL) {
            printf("Error: unable to create handle for message %d\n",cnt);
            cnt++;
            continue;
        }

        printf("message: %d\n",cnt);

        /* we need to instruct ecCodes to expand the descriptors
          i.e. unpack the data values */
        CODES_CHECK(codes_set_long(h, "unpack", 1),0);

        /* read and print some data values */

        /* long value */
        CODES_CHECK(codes_get_long(h, "blockNumber", &longVal),0);
        printf("  blockNumber: %ld\n", longVal);

        /* long value */
        CODES_CHECK(codes_get_long(h, "stationNumber", &longVal),0);
        printf("  stationNumber: %ld\n", longVal);

        /* double value */
        CODES_CHECK(codes_get_double(h, "airTemperatureAt2M", &doubleVal),0);
        printf("  airTemperatureAt2M: %f\n", doubleVal);

        /* ---- string value  -----------------*/

        /* get the size and allocate memory*/
        CODES_CHECK(codes_get_length(h, "typicalDate", &len), 0);
        typicalDate = (char*)malloc(len*sizeof(char));

        /* get the values*/
        codes_get_string(h, "typicalDate", typicalDate, &len);
        printf("  typicalDate: %s\n", typicalDate);

        /* ---- array of long ----------------*/

        /* get the size and allocate memory*/
        CODES_CHECK(codes_get_size(h, "bufrdcExpandedDescriptors", &desc_len),0);
        descriptors = (long*)malloc(desc_len*sizeof(long));

        /* get the values */
        CODES_CHECK(codes_get_long_array(h, "bufrdcExpandedDescriptors", descriptors, &desc_len),0);
        printf("  bufrdcExpandedDescriptors:\n");
        for(i = 0; i < desc_len; i++)
        {
            printf("   %ld\n",descriptors[i]);
        }

        /* ---- array of double ---------------*/

        /* get the size and allocate memory*/
        CODES_CHECK(codes_get_size(h, "numericValues", &values_len),0);
        values = (double*)malloc(values_len*sizeof(double));

        /* get the values*/
        CODES_CHECK(codes_get_double_array(h, "numericValues", values, &values_len),0);
        printf("  numericValues:\n");
        for(i = 0; i < values_len; i++)
        {
            printf("   %.10e\n",values[i]);
        }

        /* free allocated arrays */
        free(descriptors);
        free(values);
        free(typicalDate);

        /* delete handle */
        codes_handle_delete(h);

        cnt++;
    }

    fclose(in);
    return 0;
}
