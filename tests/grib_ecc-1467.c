/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */
#include <stdio.h>
#include <stdlib.h>
#undef NDEBUG
#include <assert.h>
#include "eccodes.h"

int main(int argc, char** argv)
{
    int err           = 0;
    float* fvalues    = NULL; /* data values as floats */
    double* dvalues   = NULL; /* data values as doubles */
    size_t values_len = 0;  // number of data points
    size_t cvalues_len = 0; // coded values excluding missing
    size_t i = 0;
    int mode = 2; // 1=single-precision, 2=double-precision

    double daverage    = 0;
    float  faverage    = 0;

    FILE* in             = NULL;
    const char* filename = 0;
    codes_handle* h      = NULL;

    if (argc!=3) {fprintf(stderr,"usage: %s mode file\n",argv[0]); return 1;}
    if (strcmp(argv[1], "double")==0) mode=2;
    else if (strcmp(argv[1], "float")==0) mode=1;
    else { fprintf(stderr,"Invalid mode: Use float or double\n");return 1; }
    filename = argv[2];

    printf( "Opening %s, mode=%s\n",filename, (mode==1?"float":"double") );
    in = fopen(filename, "rb");
    assert(in);

    /* create new handle from the first message in the file*/
    h = codes_handle_new_from_file(0, in, PRODUCT_GRIB, &err);
    assert(h);
    fclose(in);

    /* get the size of the values array*/
    CODES_CHECK(codes_get_size(h, "values", &values_len), 0);

    if (mode==1)
        fvalues = (float*)malloc(values_len * sizeof(float));
    if (mode==2)
        dvalues = (double*)malloc(values_len * sizeof(double));

    /* get data values*/
    if(mode==1)
        CODES_CHECK(codes_get_float_array(h, "values", fvalues, &values_len), 0);
    if(mode==2)
        CODES_CHECK(codes_get_double_array(h, "values", dvalues, &values_len), 0);

    faverage = 0;
    daverage = 0;
    if(mode==1) {
        for (i = 0; i < values_len; i++) {
            if (fvalues[i] != 9999) {
                //if(i<10)printf("%10.15f\n",fvalues[i]);
                faverage += fvalues[i];
                cvalues_len++;
            }
        }
        faverage /= (float)cvalues_len;
        free(fvalues);
        printf("\tThere are %zu total values, %zu coded, float average =  %10.15f\n", values_len, cvalues_len, faverage);
    }
    if (mode==2){
        for (i = 0; i < values_len; i++) {
            if (dvalues[i] != 9999) {
                //if(i<10)printf("%10.15f\n",dvalues[i]);
                daverage += dvalues[i];
                cvalues_len++;
            }
        }
        daverage /= (double)cvalues_len;
        free(dvalues);
        printf("\tThere are %zu total values, %zu coded, double average = %10.15g\n", values_len, cvalues_len, daverage);
    }

    codes_handle_delete(h);

    return 0;
}
