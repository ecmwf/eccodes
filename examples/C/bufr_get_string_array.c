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
 * C Implementation: bufr_get_string_array
 *
 * Description: how to get an array of strings from a BUFR message.
 *
 */

#include "eccodes.h"

int main(int argc,char* argv[])
{
    codes_handle* h=NULL;

    size_t i=0, size = 0;
    size_t len=0;
    int err=0, cnt=0;
    char** strArray = NULL; /* array of strings */
    const char* infile = "../../data/bufr/pgps_110.bufr";

    FILE* in = fopen(infile,"r");
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

        /* we need to instruct ecCodes to expand the descriptors
          i.e. unpack the data values */
        CODES_CHECK(codes_set_long(h, "unpack", 1),0);

        /* get the size and allocate memory */
        CODES_CHECK(codes_get_size(h, "stationOrSiteName", &len), 0);
        strArray = (char**)malloc(len * sizeof(char*));
        if (!strArray) {
            printf("Error: Could not allocate memory\n");
            return 1;
        }
        CODES_CHECK(codes_get_string_array(h, "stationOrSiteName", strArray, &size), 0);
        for(i=0; i<size; ++i) {
            printf("%s\n", strArray[i]);
        }
        printf("stationOrSiteName string array size = %lu\n", (unsigned long)size);

        /* free allocated arrays */
        for(i=0; i<size; ++i) free( strArray[i] );
        free(strArray);

        /* delete handle */
        codes_handle_delete(h);

        cnt++;
    }

    fclose(in);
    return 0;
}
