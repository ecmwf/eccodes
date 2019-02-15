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
 * C Implementation: grib_set_keys
 *
 * Description: how to set key values in GRIB messages
 *
 */
#include <stdio.h>
#include <stdlib.h>

#include "eccodes.h"

int main(int argc, char** argv)
{
    int err = 0;
    long centre=80;
    long long_value=0;
    char string_value[100];
    size_t len = sizeof(string_value)/sizeof(char);
    size_t size=0;

    FILE* in = NULL;
    const char* infile = "../../data/regular_latlon_surface.grib1";
    FILE* out = NULL;
    const char* outfile = "out.set.grib1";
    codes_handle *h = NULL;
    const void* buffer = NULL;
    size_t str_len = 0; /* See the call to codes_set_string later */

    in = fopen(infile,"r");
    if(!in) {
        printf("ERROR: unable to open file %s\n",infile);
        return 1;
    }

    out = fopen(outfile,"w");
    if(!out) {
        printf("ERROR: unable to open file %s\n",outfile);
        fclose(in);
        return 1;
    }

    /* create a new handle from a message in a file */
    h = codes_handle_new_from_file(0, in, PRODUCT_GRIB, &err);
    if (h == NULL) {
        printf("Error: unable to create handle from file %s\n",infile);
    }

    /* set centre as a long */
    CODES_CHECK(codes_set_long(h,"centre",centre),0);

    /* set paramId and shortName - normally you would do one or the other */
    CODES_CHECK(codes_set_long(h,"paramId", 500004),0);
    /* the value of str_len is not used, it can be anything! */
    CODES_CHECK(codes_set_string(h,"shortName", "fis", &str_len),0);

    /* get centre as a long */
    CODES_CHECK(codes_get_long(h,"centre",&long_value),0);
    printf("centre long value=%ld\n",long_value);

    /* get centre as a string */
    CODES_CHECK(codes_get_string(h,"centre",string_value,&len),0);
    printf("centre string value=%s\n",string_value);

    /* get the coded message in a buffer */
    CODES_CHECK(codes_get_message(h,&buffer,&size),0);

    /* write the buffer in a file*/
    if(fwrite(buffer,1,size,out) != size)
    {
        perror(argv[1]);
        exit(1);
    }

    codes_handle_delete(h);
    fclose(in);
    fclose(out);

    return 0;
}
