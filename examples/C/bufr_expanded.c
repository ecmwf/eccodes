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
 * C Implementation: bufr_expanded
 *
 * Description: how to read all the expanded data values from BUFR messages.
 *
 */

#include "eccodes.h"

static void usage(const char* prog) {
    printf("usage: %s infile\n",prog);
    exit(1);
}

int main(int argc,char* argv[])
{
    char* filename = NULL;
    FILE* in = NULL;

    /* message handle. Required in all the ecCodes calls acting on a message.*/
    codes_handle* h=NULL;

    double *values = NULL;
    size_t values_len=0;
    int i, err=0;
    int cnt=0;

    if (argc!=2) usage(argv[0]);

    filename=argv[1];

    /* open bufr file */
    in=fopen(filename,"r");
    if (!in) {
        printf("ERROR: unable to open file %s\n", filename);
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

        /* get the size of the values array*/
        CODES_CHECK(codes_get_size(h, "numericValues", &values_len),0);
        printf("  number of expanded values: %lu\n", values_len);

        /* allocate array for data values */
        values = (double*)malloc(values_len*sizeof(double));

        /* get the expanded data values*/
        CODES_CHECK(codes_get_double_array(h, "numericValues", values, &values_len),0);

        for(i = 0; i < values_len; i++)
        {
            printf("  %.10e\n",values[i]);
        }

        free(values);

        /* delete handle */
        codes_handle_delete(h);

        cnt++;
    }

    fclose(in);
    return 0;
}
