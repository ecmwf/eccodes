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
 * C Implementation: grib_multi
 *
 * Description: How to decode GRIB messages containing multiple
 *              fields. Try to turn on and off multi support to
 *              see the difference. Default is OFF.
 *              For all the tools defalut is multi support ON.
 *
 */
#include <stdio.h>
#include <stdlib.h>

#include "eccodes.h"

int main(int argc, char** argv)
{
    int err = 0;
    long parameterCategory=0,parameterNumber=0,discipline=0;
    FILE* in = NULL;
    const char* filename = "../../data/multi.grib2";
    codes_handle *h = NULL;

    /* turn on support for multi fields messages */
    codes_grib_multi_support_on(0);

    /* turn off support for multi fields messages */
    /* codes_multi_support_off(0); */

    in = fopen(filename,"r");
    if(!in) {
        printf("ERROR: unable to open file %s\n",filename);
        return 1;
    }

    while ((h = codes_handle_new_from_file(0, in, PRODUCT_GRIB, &err)) != NULL ) {

        CODES_CHECK(err,0);

        CODES_CHECK(codes_get_long(h,"discipline",&discipline),0);
        printf("discipline=%ld\n",discipline);

        CODES_CHECK(codes_get_long(h,"parameterCategory",&parameterCategory),0);
        printf("parameterCategory=%ld\n",parameterCategory);

        CODES_CHECK(codes_get_long(h,"parameterNumber",&parameterNumber),0);
        printf("parameterNumber=%ld\n",parameterNumber);

        if ( discipline == 0 && parameterCategory==2) {
            if (parameterNumber == 2) printf("-------- u -------\n");
            if (parameterNumber == 3) printf("-------- v -------\n");
        }
        codes_handle_delete(h);
    }

    fclose(in);
    return 0;
}
