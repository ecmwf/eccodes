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

#include "grib_api_internal.h"

#define SIZE 1024 * 1024
char buffer[SIZE];

int main(int argc, char** argv)
{
    int err = 0;
    FILE* in = NULL;
    size_t len = SIZE;

    if (argc != 2) return 1;

    in = fopen(argv[1], "r");
    if (!in) return 1;

    grib_context* c = grib_context_get_default();
    err = grib_read_any_headers_only_from_file(c, in, buffer, &len);
    printf("ANY (headers only): err=%s, len=%zu\n", grib_get_error_message(err), len);

    rewind(in);
    len = SIZE;

    err = wmo_read_any_from_file(in, buffer, &len);
    if (err == GRIB_END_OF_FILE && len == 0)
        printf("end of file\n");

    printf("ANY: size: %zu err: %d (%s)\n", len, err, grib_get_error_message(err));

    return err;
}
