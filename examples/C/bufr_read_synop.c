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
 * C Implementation: bufr_read_synop
 *
 * Description: how to read SYNOP BUFR messages.
 *
 */

/*
 * Please note that SYNOP reports can be encoded in various ways in BUFR. Therefore the code
 * below might not work directly for other types of SYNOP messages than the one used in the
 * example. It is advised to use bufr_dump to understand the structure of the messages.
 */


#include "eccodes.h"

int main(int argc,char* argv[])
{
    FILE* in = NULL;

    /* message handle. Required in all the eccodes calls acting on a message.*/
    codes_handle* h=NULL;

    long longVal;
    double doubleVal;
    int err=0;
    int cnt=0;
    const char* infile = "../../data/bufr/syno_multi.bufr";

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

        /* we need to instruct ecCodes to unpack the data values */
        CODES_CHECK(codes_set_long(h,"unpack",1),0);

        /* station id*/
        CODES_CHECK(codes_get_long(h,"blockNumber",&longVal),0);
        printf("  blockNumber: %ld\n",longVal);

        CODES_CHECK(codes_get_long(h,"stationNumber",&longVal),0);
        printf("  stationNumber: %ld\n",longVal);

        /* location*/
        CODES_CHECK(codes_get_double(h,"latitude",&doubleVal),0);
        printf("  latitude: %f\n",doubleVal);

        CODES_CHECK(codes_get_double(h,"longitude",&doubleVal),0);
        printf("  longitude: %f\n",doubleVal);

        /* 2m temperature */
        CODES_CHECK(codes_get_double(h,"airTemperatureAt2M",&doubleVal),0);
        printf("  airTemperatureAt2M: %f\n",doubleVal);

        /* 2m dewpoint temperature */
        CODES_CHECK(codes_get_double(h,"dewpointTemperatureAt2M",&doubleVal),0);
        printf("  dewpointTemperatureAt2M: %f\n",doubleVal);

        /* 10 wind */
        CODES_CHECK(codes_get_double(h,"windSpeedAt10M",&doubleVal),0);
        printf("  windSpeedAt10M: %f\n",doubleVal);

        CODES_CHECK(codes_get_double(h,"windDirectionAt10M",&doubleVal),0);
        printf("  windDirectionAt10M: %f\n",doubleVal);

        /* The cloud information is stored in several blocks in the
         * SYNOP message and the same key means a different thing in different
         * parts of the message. In this example we will read the first
         * cloud block introduced by the key
         * verticalSignificanceSurfaceObservations=1.
         * We know that this is the first occurrence of the keys we want to
         * read so we will use the # (occurrence) operator accordingly. */

        /* Cloud amount (low and middleclouds) */
        CODES_CHECK(codes_get_long(h,"#1#cloudAmount",&longVal),0);
        printf("  cloudAmount (low and middle): %ld\n",longVal);

        /* Height of cloud base */
        CODES_CHECK(codes_get_long(h,"#1#heightOfBaseOfCloud",&longVal),0);
        printf("  heightOfBaseOfCloud: %ld\n",longVal);

        /* Cloud type (low clouds) */
        CODES_CHECK(codes_get_long(h,"#1#cloudType",&longVal),0);
        printf("  cloudType (low): %ld\n",longVal);

        /* Cloud type (middle clouds) */
        CODES_CHECK(codes_get_long(h,"#2#cloudType",&longVal),0);
        printf("  cloudType (middle): %ld\n",longVal);

        /* Cloud type (high clouds) */
        CODES_CHECK(codes_get_long(h,"#3#cloudType",&longVal),0);
        printf("  cloudType (high): %ld\n",longVal);

        /* delete handle */
        codes_handle_delete(h);

        cnt++;
    }

    fclose(in);
    return 0;
}
