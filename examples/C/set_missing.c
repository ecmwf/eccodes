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
 * C Implementation: set_missing
 *
 * Description: how to set a key as missing.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "eccodes.h"

int main(int argc, char** argv)
{
    int err = 0, is_missing = 0;

    FILE* in = NULL;
    const char* infile = "../../data/reduced_gaussian_pressure_level.grib2";
    FILE* out = NULL;
    const char* outfile = "out_surface_level.grib2";
    codes_handle *h = NULL;
    const void* buffer = NULL;
    size_t size=0;
    char str[]="sfc";
    size_t str_len=3;
    long Ni = 0;

    in = fopen(infile,"r");
    if(!in) {
        printf("ERROR: unable to open input file %s\n",infile);
        return 1;
    }

    out = fopen(outfile,"w");
    if(!out) {
        printf("ERROR: unable to open output file %s\n",outfile);
        fclose(in);
        return 1;
    }

    /* create a new handle from a message in a file */
    h = codes_handle_new_from_file(0, in, PRODUCT_GRIB, &err);
    if (h == NULL) {
        printf("Error: unable to create handle from file %s\n",infile);
    }

    CODES_CHECK(codes_set_string(h,"typeOfFirstFixedSurface",str,&str_len),0);
    CODES_CHECK(codes_set_missing(h,"scaleFactorOfFirstFixedSurface"),0);
    CODES_CHECK(codes_set_missing(h,"scaledValueOfFirstFixedSurface"),0);

    /* See GRIB-490 */
    CODES_CHECK(codes_get_long(h,"Ni",&Ni),0);
    is_missing = codes_is_missing(h, "Ni", &err);
    CODES_CHECK(err,0);
    if (is_missing != 1) assert(!"Ni should be missing");
    CODES_CHECK(codes_set_long(h,"Ni", Ni),0);

    /* get the coded message in a buffer */
    CODES_CHECK(codes_get_message(h,&buffer,&size),0);

    /* write the buffer in a file*/
    if(fwrite(buffer,1,size,out) != size)
    {
        perror(argv[1]);
        exit(1);
    }

    /* delete handle */
    codes_handle_delete(h);

    fclose(in);
    fclose(out);

    return 0;
}
