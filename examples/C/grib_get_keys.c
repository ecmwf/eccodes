/*
 * Copyright 2005-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

/*
 * C Implementation: grib_get_keys
 *
 * Description: how to get values using keys from GRIB messages
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "eccodes.h"

int main(int argc, char** argv)
{
    int err = 0;
    double *values = NULL;
    size_t values_len= 0;
    size_t i = 0, len = 0;

    double latitudeOfFirstGridPointInDegrees;
    double longitudeOfFirstGridPointInDegrees;
    double latitudeOfLastGridPointInDegrees;
    double longitudeOfLastGridPointInDegrees;

    double jDirectionIncrementInDegrees;
    double iDirectionIncrementInDegrees;

    long numberOfPointsAlongAParallel;
    long numberOfPointsAlongAMeridian;

    double average = 0;
    char* packingType = NULL;

    FILE* in = NULL;
    const char* filename = "../../data/regular_latlon_surface.grib1";
    codes_handle *h = NULL;

    in = fopen(filename,"r");
    if(!in) {
        printf("ERROR: unable to open file %s\n",filename);
        return 1;
    }

    /* create new handle from the first message in the file*/
    h = codes_handle_new_from_file(0, in, PRODUCT_GRIB, &err);
    if (h == NULL) {
        printf("Error: unable to create handle from file %s\n",filename);
        return 1;
    }
    fclose(in);

    /* Store the filename in the key "file" for this handle */
    len = strlen(filename);
    CODES_CHECK(codes_set_string(h, "file", filename, &len), 0);

    /* get as a long*/
    CODES_CHECK(codes_get_long(h,"Ni",&numberOfPointsAlongAParallel),0);
    printf("numberOfPointsAlongAParallel=%ld\n",numberOfPointsAlongAParallel);

    /* get as a long*/
    CODES_CHECK(codes_get_long(h,"Nj",&numberOfPointsAlongAMeridian),0);
    printf("numberOfPointsAlongAMeridian=%ld\n",numberOfPointsAlongAMeridian);

    /* get as a double*/
    CODES_CHECK(codes_get_double(h,"latitudeOfFirstGridPointInDegrees",&latitudeOfFirstGridPointInDegrees),0);
    printf("latitudeOfFirstGridPointInDegrees=%g\n",latitudeOfFirstGridPointInDegrees);

    /* get as a double*/
    CODES_CHECK(codes_get_double(h,"longitudeOfFirstGridPointInDegrees",&longitudeOfFirstGridPointInDegrees),0);
    printf("longitudeOfFirstGridPointInDegrees=%g\n",longitudeOfFirstGridPointInDegrees);

    /* get as a double*/
    CODES_CHECK(codes_get_double(h,"latitudeOfLastGridPointInDegrees",&latitudeOfLastGridPointInDegrees),0);
    printf("latitudeOfLastGridPointInDegrees=%g\n",latitudeOfLastGridPointInDegrees);

    /* get as a double*/
    CODES_CHECK(codes_get_double(h,"longitudeOfLastGridPointInDegrees",&longitudeOfLastGridPointInDegrees),0);
    printf("longitudeOfLastGridPointInDegrees=%g\n",longitudeOfLastGridPointInDegrees);

    /* get as a double*/
    CODES_CHECK(codes_get_double(h,"jDirectionIncrementInDegrees",&jDirectionIncrementInDegrees),0);
    printf("jDirectionIncrementInDegrees=%g\n",jDirectionIncrementInDegrees);

    /* get as a double*/
    CODES_CHECK(codes_get_double(h,"iDirectionIncrementInDegrees",&iDirectionIncrementInDegrees),0);
    printf("iDirectionIncrementInDegrees=%g\n",iDirectionIncrementInDegrees);

    /* get as string */
    CODES_CHECK(codes_get_length(h, "packingType", &len), 0);
    packingType = (char*)malloc(len*sizeof(char));
    codes_get_string(h, "packingType", packingType, &len);
    printf("packingType=%s\n", packingType);
    free(packingType);

    /* get the size of the values array*/
    CODES_CHECK(codes_get_size(h,"values",&values_len),0);

    values = (double*)malloc(values_len*sizeof(double));

    /* get data values*/
    CODES_CHECK(codes_get_double_array(h,"values",values,&values_len),0);

    average = 0;
    for(i = 0; i < values_len; i++)
        average += values[i];

    average /=(double)values_len;

    free(values);

    printf("There are %d values, average is %g\n",(int)values_len,average);

    {
        int eq = 0;
        /* Now retrieve the value of the key "file" */
        char file[256]={0,};
        CODES_CHECK(codes_get_length(h, "file", &len),0);
        assert(len == 1+strlen(filename));
        codes_get_string(h, "file", file, &len);
        eq = strcmp(file, filename);
        if (eq != 0) assert(!"file and filename not equal");
    }

    {
        /* Example of getting bytes */
        char* name = "reservedNeedNotBePresent";
        unsigned char* byte_val = NULL ;
        size_t keySize = 0;
        CODES_CHECK(grib_get_size(h, name, &keySize), 0);
        byte_val = malloc(keySize*sizeof(char));
        GRIB_CHECK(grib_get_bytes(h, name, byte_val, &keySize), name);
    }
    
    codes_handle_delete(h);

    return 0;
}
