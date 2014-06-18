/*
 * Copyright 2005-2014 ECMWF.
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
#include "grib_api.h"

void usage(char* prog)
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
    grib_handle *h = NULL;
    grib_iterator* iter=NULL;

    if (argc != 2) usage(argv[0]);
    filename=argv[1];
    in = fopen(filename,"r");
    if(!in) {
        fprintf(stderr, "ERROR: unable to open file %s\n",filename);
        return 1;
    }

    while ((h = grib_handle_new_from_file(0,in,&err)) != NULL )
    {
        if (err != GRIB_SUCCESS) GRIB_CHECK(err,0);

        GRIB_CHECK(grib_get_long(h,"bitmapPresent",&bitmapPresent),0);
        if (bitmapPresent)
        {
            GRIB_CHECK(grib_get_size(h,"bitmap",&bmp_len),0);
            bitmap = malloc(bmp_len*sizeof(long));
            GRIB_CHECK(grib_get_long_array(h,"bitmap",bitmap,&bmp_len),0);
            printf("Bitmap is present. Num = %ld\n", bmp_len);
        }
        /* Sanity check. Number of values must match number in bitmap */
        GRIB_CHECK(grib_get_size(h,"values",&values_len),0);
        values = malloc(values_len*sizeof(double));
        GRIB_CHECK(grib_get_double_array(h,"values",values,&values_len),0);
        if (bitmapPresent) {
            assert(values_len==bmp_len);
        }

        /* A new iterator on lat/lon/values is created from the message handle h */
        iter=grib_iterator_new(h,0,&err);
        if (err != GRIB_SUCCESS) GRIB_CHECK(err,0);

        n = 0;
        /* Loop on all the lat/lon/values. Only print non-missing values */
        while(grib_iterator_next(iter,&lat,&lon,&value))
        {
            int is_missing_val = (bitmapPresent && bitmap[n] == 0);
            if (!is_missing_val) {
                printf("- %d - lat=%f lon=%f value=%f\n",n,lat,lon,value);
            }
            n++;
        }
        /* Check number of elements in iterator matches value count */
        assert(n == values_len);

        grib_iterator_delete(iter);
        grib_handle_delete(h);
    }

    fclose(in);
    return 0;
}
