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
 * C Implementation: grib_precision
 *
 * Description: how to control decimal precision when packing fields.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include "eccodes.h"

#define EPSILON  1e-6
#define EXPECTED_MIN 270.466796875
#define EXPECTED_MAX 311.096796875

int main(int argc, char** argv)
{
    int err = 0;
    size_t size=0;

    FILE* in = NULL;
    char* infile = "../../data/regular_latlon_surface.grib1";
    FILE* out = NULL;
    char* outfile = "out.precision.grib1";
    codes_handle *h = NULL;
    const void* buffer = NULL;
    double* values1=NULL;
    double* values2=NULL;
    double maxa=0;
    double maxv=0,minv=0;
    double maxr=0,r=0;
    long decimalPrecision;
    long bitsPerValue1=0, bitsPerValue2=0;
    int i=0;

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

    /* bitsPerValue before changing the packing parameters */
    CODES_CHECK(codes_get_long(h,"bitsPerValue",&bitsPerValue1),0);
    assert(bitsPerValue1 == 16);

    /* get the size of the values array*/
    CODES_CHECK(codes_get_size(h,"values",&size),0);
    assert(size == 496);

    values1 = (double*)malloc(size*sizeof(double));
    /* get data values before changing the packing parameters*/
    CODES_CHECK(codes_get_double_array(h,"values",values1,&size),0);

    /* changing decimal precision to 2 means that 2 decimal digits
     are preserved when packing.  */
    decimalPrecision=2;
    CODES_CHECK(codes_set_long(h,"changeDecimalPrecision",decimalPrecision),0);

    /* bitsPerValue after changing the packing parameters */
    CODES_CHECK(codes_get_long(h,"bitsPerValue",&bitsPerValue2),0);
    assert(bitsPerValue2 == 12);

    values2 = (double*)malloc(size*sizeof(double));
    /* get data values after changing the packing parameters*/
    CODES_CHECK(codes_get_double_array(h,"values",values2,&size),0);

    /* computing error */
    maxa=0;
    maxr=0;
    maxv=values2[0];
    minv=maxv;
    for (i=0;i<size;i++) {
        double a=fabs(values2[i]-values1[i]);
        if ( values2[i] > maxv ) maxv=values2[i];
        if ( values2[i] < minv ) minv=values2[i];
        if ( values2[i] !=0 ) r=fabs((values2[i]-values1[i])/values2[i]);
        if ( a > maxa ) maxa=a;
        if ( r > maxr ) maxr=r;
    }
    printf("max absolute error = %g\n",maxa);
    printf("max relative error = %g\n",maxr);
    printf("min value = %g\n",minv);
    printf("max value = %g\n",maxv);
    assert(fabs(minv - EXPECTED_MIN) < EPSILON);
    assert(fabs(maxv - EXPECTED_MAX) < EPSILON);

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
