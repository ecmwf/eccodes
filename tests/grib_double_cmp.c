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
 * Description: compare values retrieved with grib_get_double_array and
 *               grib_get_double_element
 *
 * Author: Cristian D. Codorean
 *
 */
#include "grib_api.h"

void usage(char* prog) {
    printf("usage: %s filename\n",prog);
    exit(1);
}

int main(int argc, char** argv)
{
    int err = 0,i = 0;
    double *values = NULL;
    size_t values_len= 0;
    double element_value=0;

    FILE* in = NULL;
    char* filename = NULL;
    grib_handle *h = NULL;

    if (argc<2) usage(argv[0]);
    filename=argv[1];

    in = fopen(filename,"r");
    if(!in) {
        printf("ERROR: unable to open file %s\n",filename);
        return 1;
    }

    /* create new handle from a message in a file*/
    h = grib_handle_new_from_file(0,in,&err);
    if (h == NULL) {
        printf("Error: unable to create handle from file %s\n",filename);
        return 1;
    }

    /* get the size of the values array */
    GRIB_CHECK(grib_get_size(h,"values",&values_len),0);
    values = (double*)malloc(values_len*sizeof(double));

    /* get data values*/
    GRIB_CHECK(grib_get_double_array(h,"values",values,&values_len),0);

    if (values_len < 100) exit(1);

    for (i = 0; i < 100; i++) {
        /* get a particular data value */
        GRIB_CHECK(grib_get_double_element(h, "values", i, &element_value),0);

        if (element_value != values[i]) {
            exit(1);
        }
    }

    free(values);
    grib_handle_delete(h);

    fclose(in);
    return 0;
}
