/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 *
 * Modified for Performance Study by: CS GMBH
 */

#include "grib_api_internal.h"

/* extern grib_iarrayPOOL iarrayPOOL; */
/* extern grib_darrayPOOL darrayPOOL; */

int main(int argc, char* argv[])
{
    int i    = 0;
    int fail = 0;

    grib_context* c = grib_context_get_default();
    grib_action* a  = NULL;

    iarrayPoolInit(c);
    darrayPoolInit();

    for (i = 1; i < argc; i++) {
        printf("%s ... ", argv[i]);
        fflush(stdout);
        if (!(a = grib_parse_file(c, argv[i]))) {
            fail++;
            printf("FAILED\n");
            exit(1);
        }
        else {
            printf("OK\n");
        }
    }
    return fail;
}
