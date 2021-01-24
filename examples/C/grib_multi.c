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
 * C Implementation: grib_multi
 *
 * Description: How to decode GRIB2 multi-field messages.
 *              Try to turn the multi support on and off to
 *              see the difference. Default is OFF.
 *              For all the tools default is multi support ON.
 */
#include <stdio.h>
#include <assert.h>

#include "eccodes.h"

int main(int argc, char** argv)
{
    int err                = 0;
    long parameterCategory = 0, parameterNumber = 0, discipline = 0;
    FILE* in             = NULL;
    const char* filename = "../../data/multi.grib2";
    codes_handle* h      = NULL;
    int mcount           = 0;

    /* turn on support for GRIB2 multi-field messages */
    codes_grib_multi_support_on(NULL);

    /* turn off support for GRIB2 multi-field messages */
    /* codes_multi_support_off(NULL); */

    in = fopen(filename, "rb");
    if (!in) {
        fprintf(stderr, "Error: unable to open file %s\n", filename);
        return 1;
    }

    CODES_CHECK(grib_count_in_file(NULL, in, &mcount), 0);
    assert(mcount == 56);
    printf("grib_count_in_file counted %d messages\n", mcount);

    mcount = 0;
    CODES_CHECK(grib_count_in_filename(NULL, filename, &mcount), 0);
    assert(mcount == 56);
    printf("grib_count_in_filename counted %d messages\n", mcount);

    mcount = 0;
    while ((h = codes_handle_new_from_file(0, in, PRODUCT_GRIB, &err)) != NULL) {
        CODES_CHECK(err, 0);
        ++mcount;

        CODES_CHECK(codes_get_long(h, "discipline", &discipline), 0);
        printf("discipline=%ld\n", discipline);

        CODES_CHECK(codes_get_long(h, "parameterCategory", &parameterCategory), 0);
        printf("parameterCategory=%ld\n", parameterCategory);

        CODES_CHECK(codes_get_long(h, "parameterNumber", &parameterNumber), 0);
        printf("parameterNumber=%ld\n", parameterNumber);

        if (discipline == 0 && parameterCategory == 2) {
            if (parameterNumber == 2) printf("-------- u -------\n");
            if (parameterNumber == 3) printf("-------- v -------\n");
        }
        codes_handle_delete(h);
    }
    assert(mcount == 56);
    printf("codes_handle_new_from_file counted %d messages\n", mcount);

    fclose(in);
    return 0;
}
