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

int test_next(FILE* in, size_t* len, off_t offset_expected, off_t* offset) // , size_t* previous_end_of_message)
{
    *len = SIZE;

    int err = wmo_read_any_from_file_offset(in, buffer, len, offset);
    if (err == GRIB_END_OF_FILE && *len == 0)
        printf("end of file\n");

    ECCODES_ASSERT(*offset == offset_expected);

    // *previous_end_of_message = *offset + *len;
    *offset += *len;

    return err;
}

int main(int argc, char** argv)
{
    int err = 0;
    FILE* in = NULL;
    size_t len = SIZE;

    if (argc != 2) return 1;

    in = fopen(argv[1], "r");
    if (!in) return 1;

    off_t offset = 0;
    // size_t previous_end_of_message = 0;

    err = test_next(in, &len, 5, &offset); // , &previous_end_of_message);
    err = test_next(in, &len, 14161, &offset); // , &previous_end_of_message);
    err = test_next(in, &len, 27581, &offset); // , &previous_end_of_message);
    err = test_next(in, &len, 27689, &offset); // , &previous_end_of_message);
    err = test_next(in, &len, 42133, &offset); // , &previous_end_of_message);
    err = test_next(in, &len, 43333, &offset); // , &previous_end_of_message);

    return err;
}
