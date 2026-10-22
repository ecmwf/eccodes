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
#undef NDEBUG
#include <assert.h>

int main(int argc, char* argv[])
{
    if (argc != 2) {
        // Usage: prog input_file
        return 1;
    }

    FILE* in = fopen(argv[1], "rb");
    assert(in);

    size_t size = 0;
    int err = grib_get_header_length(in, &size);
    if (err) {
        printf("Error code: %s\n", grib_get_error_message(err));
        return 1;
    }
    printf("%zu\n", size);

    fclose(in);
    return 0;
}
