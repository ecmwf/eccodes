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

int main(int argc, char* argv[])
{
    grib_handle* h = NULL;
    FILE* inf      = NULL;
    FILE* ouf      = NULL;
    char* infile   = 0;
    char* oufile   = 0;
    int err        = 0;
    size_t size    = 0;
    const void* buffer = NULL;

    if (argc < 3) return 1; // prog in out

    infile = argv[1];
    oufile = argv[2];

    inf = fopen(infile, "rb");
    Assert(inf);

    ouf = fopen(oufile, "wb");
    Assert(ouf);

    while ((h = grib_handle_new_from_file(0, inf, &err)) != NULL) {
        grib_update_sections_lengths(h);
        GRIB_CHECK(grib_get_message(h, &buffer, &size), 0);
        printf("size = %zu\n", size);
        if (fwrite(buffer, 1, size, ouf) != size) {
            perror(oufile);
            exit(1);
        }
    }

    grib_handle_delete(h);
    fclose(inf);
    fclose(ouf);

    return 0;
}
