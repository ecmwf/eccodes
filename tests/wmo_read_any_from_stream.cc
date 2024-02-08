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

static long readcb(void* data, void* buffer, long len)
{
    //long l = handle->read(buffer, len);
    // ecCodes interprets a -1 as EOF
    //return l == 0 ? -1 : l;
    return -1;
}

int main(int argc, char** argv)
{
    int err = 0;
    FILE* in = NULL;
    size_t len = SIZE;
    grib_handle* h = NULL;

    if (argc != 2) return 1;

    in = fopen(argv[1], "r");
    if (!in) return 1;

    h = grib_handle_new_from_file(0, in, &err);
    if (!h) return 1;

    const void* data = NULL;
    GRIB_CHECK(grib_get_message(h, &data, &len), 0);

    err = wmo_read_any_from_stream( (void*)data, &readcb, buffer, &len );
    printf("err = %d \n",err);

    return 0;
}
