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
 * C Implementation: grib_set_data
 *
 * Description: set the data contained in a GRIB file
 *
 */
#include <stdio.h>
#include <stdlib.h>

#include "eccodes.h"

void usage(const char* prog)
{
    printf("usage: %s out.grib\n",prog);
    exit(1);
}

int main(int argc, char** argv)
{
    int i;
    double *values = NULL;
    size_t values_len= 0;

    /* Note: the full name of the sample file is "regular_ll_pl_grib1.tmpl" */
    /* Sample files are stored in the samples directory (use codes_info to  */
    /* see where that is).  The default sample path can be changed by       */
    /* setting the environment variable ECCODES_SAMPLES_PATH                */
    const char* sample_filename = "regular_ll_pl_grib1";
    /* Here is how you can get the samples path */
    const char* samples_path = codes_samples_path(NULL);
    codes_handle *h = NULL;
    double d,e;
    long count;

    if (argc!=2) usage(argv[0]);
    /* create new handle from message in sample file */
    printf("Using samples path: %s\n", samples_path);
    h = codes_grib_handle_new_from_samples(0, sample_filename);
    if (h == NULL) {
        printf("Error: unable to create handle from sample file %s\n", sample_filename);
        exit(1);
    }

    /* Here we're changing the data values only, so the number of values */
    /* will be the same as the sample GRIB. */
    /* But if your data array has a different size, then specify the grid geometry */
    /* (e.g. keys Ni, Nj etc) and set the correct number of data values */
    CODES_CHECK(codes_get_size(h, "values", &values_len),0);
    
    values = (double*)malloc(values_len*sizeof(double));
    d=10e-8;
    e=d;
    count=1;
    for (i=0;i<values_len;i++) {
        if (count>100) {e*=10; count=1;}
        values[i]=d;
        printf("%g \n",values[i]);
        d+=e;
        count++;
    }

    CODES_CHECK(codes_set_long(h,"bitsPerValue",16),0);

    /* set data values */
    CODES_CHECK(codes_set_double_array(h,"values",values,values_len),0);

    CODES_CHECK(codes_write_message(h, argv[1], "w"), 0);

    free(values);
    codes_handle_delete(h);

    return 0;
}
