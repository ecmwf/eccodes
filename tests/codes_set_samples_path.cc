/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_api_internal.h"

/* Windows always has a colon in pathnames e.g. C:\temp\file. So instead we use semi-colons as delimiter */
/* in order to have multiple definitions/samples directories */
#ifdef ECCODES_ON_WINDOWS
#define ECC_PATH_DELIMITER_CHAR ';'
#else
#define ECC_PATH_DELIMITER_CHAR ':'
#endif

int main(int argc, char** argv)
{
    grib_handle* h       = NULL;
    char full_path[2048] = {0,};
    const char* sample_name = NULL;
    const char* new_dir     = NULL;
    grib_context* c         = NULL;
    long paramId            = 0;

    Assert(argc == 3);

    sample_name = argv[1];
    new_dir     = argv[2]; /* The directory containing the given sample */

    printf("Initial samples path = %s\n", grib_samples_path(c));

    /* Should fail - default samples path does not include ifs_samples dirs */
    h = grib_handle_new_from_samples(c, sample_name);
    Assert(!h);

    snprintf(full_path, 2048, "%s%c%s", new_dir, ECC_PATH_DELIMITER_CHAR, grib_samples_path(c));
    printf("Change samples_path to: %s\n", full_path);
    grib_context_set_samples_path(c, full_path);

    h = grib_handle_new_from_samples(c, sample_name);
    Assert(h);
    GRIB_CHECK(grib_get_long(h, "paramId", &paramId), NULL);
    Assert(paramId == 130);
    grib_handle_delete(h);
    printf("Success\n");

    return 0;
}
