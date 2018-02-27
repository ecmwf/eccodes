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
 * C Implementation: bufr_attributes
 *
 * Description: how to read attributes of keys in BUFR messages.
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
    size_t len=0;
    int err=0;
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

        /* ----------------------------------------------------------------
           We will read the value and all the attributes available for
           the 2m temperature.
        -------------------------------------------------------------------*/

        /* get the value as double */
        CODES_CHECK(codes_get_double(h, "airTemperatureAt2M", &doubleVal),0);
        printf("  airTemperatureAt2M: %.2f\n",doubleVal);

        /* get the element's code (see BUFR code table B) */
        CODES_CHECK(codes_get_long(h, "airTemperatureAt2M->code", &longVal),0);
        printf("  airTemperatureAt2M->code: %ld\n",longVal);

        /* get the element's units (see BUFR code table B) */

        /* get the size and allocate memory*/
        CODES_CHECK(codes_get_length(h, "airTemperatureAt2M->units", &len), 0);
        units = (char*)malloc(len*sizeof(char));

        /* get the values*/
        codes_get_string(h, "airTemperatureAt2M->units", units, &len);
        printf("  airTemperatureAt2M->units: %s\n", units);

        /* get the element's scale (see BUFR code table B) */
        CODES_CHECK(codes_get_long(h, "airTemperatureAt2M->scale", &longVal),0);
        printf("  airTemperatureAt2M->scale: %ld\n",longVal);

        /* get the element's reference (see BUFR code table B) */
        CODES_CHECK(codes_get_long(h, "airTemperatureAt2M->reference", &longVal),0);
        printf("  airTemperatureAt2M->reference: %ld\n",longVal);

        /* get the element's width (see BUFR code table B) */
        CODES_CHECK(codes_get_long(h, "airTemperatureAt2M->width", &longVal),0);
        printf("  airTemperatureAt2M->width: %ld\n",longVal);

        /* -----------------------------------------------------------------
           The 2m temperature data element in this message has an associated
           field: percentConfidence. Its value and attributes can be accessed
           in a similar manner as was shown above for 2m temperature.
           ----------------------------------------------------------------- */

        /* get the value as long */
        CODES_CHECK(codes_get_long(h, "airTemperatureAt2M->percentConfidence", &longVal),0);
        printf("  airTemperatureAt2M->percentConfidence: %ld\n",longVal);

        /* get the element's code (see BUFR code table B) */
        CODES_CHECK(codes_get_long(h, "airTemperatureAt2M->percentConfidence->code", &longVal),0);
        printf("  airTemperatureAt2M->percentConfidence->code: %ld\n",longVal);

        /* get the element's units (see BUFR code table B) */

        /* get the size and allocate memory*/
        CODES_CHECK(codes_get_length(h, "airTemperatureAt2M->percentConfidence->units", &len), 0);
        unitsPercent = (char*)malloc(len*sizeof(char));

        /* get the values*/
        codes_get_string(h, "airTemperatureAt2M->percentConfidence->units", unitsPercent, &len);
        printf("  airTemperatureAt2M->percentConfidence->units: %s\n", unitsPercent);

        /* get the element's scale (see BUFR code table B) */
        CODES_CHECK(codes_get_long(h, "airTemperatureAt2M->percentConfidence->scale", &longVal),0);
        printf("  airTemperatureAt2M->percentConfidence->scale: %ld\n",longVal);

        /* get the element's reference (see BUFR code table B) */
        CODES_CHECK(codes_get_long(h, "airTemperatureAt2M->percentConfidence->reference", &longVal),0);
        printf("  airTemperatureAt2M->percentConfidence->reference: %ld\n",longVal);

        /* get the element's width (see BUFR code table B) */
        CODES_CHECK(codes_get_long(h, "airTemperatureAt2M->percentConfidence->width", &longVal),0);
        printf("  airTemperatureAt2M->percentConfidence->width: %ld\n",longVal);

        /* free allocated arrays */
        free(units);
        free(unitsPercent);

        /* delete handle */
        codes_handle_delete(h);

        cnt++;
    }

    fclose(in);
    return 0;
}
