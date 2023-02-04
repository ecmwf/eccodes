/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

/*
 * C Implementation: multi2.c
 *
 * Description: Repeatedly print data contained in a multi-field GRIB2 message
 */

#include "eccodes.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#ifndef HAVE_FSEEKO
#define fseeko fseek
#define ftello ftell
#endif

const int NUM_FIELDS  = 4;
const int COUNT       = 5;
const char* file_path = "../../data/multi_created.grib2";

static void read_data(FILE* fp, int num_msgs)
{
    int err         = 0, i;
    long stepRange  = 0;
    codes_handle* h = NULL;

    printf("File offset start = %ld\n", ftello(fp));
    for (i = 0; i < num_msgs; ++i) {
        h = codes_handle_new_from_file(0, fp, PRODUCT_GRIB, &err);
        CODES_CHECK(err, 0);

        CODES_CHECK(codes_get_long(h, "stepRange", &stepRange), 0);
        printf("%d : stepRange=%ld\n", i, stepRange);
        codes_handle_delete(h);
        /* These tests make sure we always start from 1st field of the grib msg */
        /* and not where we left off last time */
        if (i == 0) assert(stepRange == 0);  /* 1st field */
        if (i == 1) assert(stepRange == 12); /* 2nd field */
        if (i == 2) assert(stepRange == 24); /* 3rd field */
        if (i == 3) assert(stepRange == 36); /* 4th field */
    }
}

int main(int argc, char** argv)
{
    int i;
    FILE* fp = fopen(file_path, "rb");
    if (!fp) {
        fprintf(stderr, "ERROR: unable to open GRIB file %s)\n", file_path);
        exit(1);
    }

    /* turn on support for multi-field messages */
    codes_grib_multi_support_on(NULL);

    for (i = 1; i < COUNT; ++i) {
        printf("Pass %d: \n", i);
        fseeko(fp, 0, SEEK_SET);
        read_data(fp, NUM_FIELDS);

        /* Reset the internal engine state using this file pointer for the next round */
        codes_grib_multi_support_reset_file(codes_context_get_default(), fp);
    }

    fclose(fp);
    printf("All OK\n");
    return 0;
}
