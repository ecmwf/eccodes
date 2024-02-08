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

    err = wmo_read_bufr_from_file(in, buffer, &len);
    if (err == GRIB_END_OF_FILE && len == 0)
        printf("end of file\n");

    printf("BUFR: size: %zu err: %d (%s)\n", len, err, grib_get_error_message(err));

    return 0;
}
