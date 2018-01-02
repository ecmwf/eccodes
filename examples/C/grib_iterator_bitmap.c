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
 * Description: How to use an iterator on lat/lon/values and query the bitmap
 *              for missing values
 *              (rather than compare each value with the missingValue key)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "eccodes.h"

static void usage(const char* prog)
{
    printf("Usage: %s grib_file\n",prog);
    exit(1);
}

int main(int argc, char** argv)
{
    FILE* in = NULL;
    int err = 0;
    double lat,lon,value;
    int n=0;
    size_t bmp_len = 0, values_len = 0;
    char* filename = NULL;
    long bitmapPresent = 0;
    long *bitmap = NULL;
    double *values = NULL;
    codes_handle *h = NULL;
    codes_iterator* iter=NULL;

    if (argc != 2) usage(argv[0]);
    filename=argv[1];
    in = fopen(filename,"r");
    if(!in) {
        fprintf(stderr, "ERROR: unable to open file %s\n",filename);
        return 1;
    }

    while ((h = codes_handle_new_from_file(0, in, PRODUCT_GRIB, &err)) != NULL )
    {
        if (err != CODES_SUCCESS) CODES_CHECK(err,0);

        CODES_CHECK(codes_get_long(h,"bitmapPresent",&bitmapPresent),0);
        if (bitmapPresent)
        {
            CODES_CHECK(codes_get_size(h,"bitmap",&bmp_len),0);
            bitmap = (long*)malloc(bmp_len*sizeof(long));
            CODES_CHECK(codes_get_long_array(h,"bitmap",bitmap,&bmp_len),0);
            printf("Bitmap is present. Num = %lu\n", bmp_len);
        }
        /* Sanity check. Number of values must match number in bitmap */
        CODES_CHECK(codes_get_size(h,"values",&values_len),0);
        values = (double*)malloc(values_len*sizeof(double));
        CODES_CHECK(codes_get_double_array(h,"values",values,&values_len),0);
        if (bitmapPresent) {
            assert(values_len==bmp_len);
        }

        /* A new iterator on lat/lon/values is created from the message handle h */
        iter=codes_grib_iterator_new(h,0,&err);
        if (err != CODES_SUCCESS) CODES_CHECK(err,0);

        n = 0;
        /* Loop on all the lat/lon/values. Only print non-missing values */
        while(codes_grib_iterator_next(iter,&lat,&lon,&value))
        {
            /* Consult bitmap to see if the n'th value is missing */
            int is_missing_val = (bitmapPresent && bitmap[n] == 0);
            if (!is_missing_val) {
                printf("- %d - lat=%f lon=%f value=%f\n",n,lat,lon,value);
            }
            n++;
        }
        /* Check number of elements in iterator matches value count */
        assert(n == values_len);

        codes_grib_iterator_delete(iter);
        codes_handle_delete(h);
    }

    fclose(in);
    return 0;
}
