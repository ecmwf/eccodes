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
#undef NDEBUG
#include <assert.h>
#include "eccodes.h"

int main(int argc, char* argv[])
{
    int err = 0;
    assert (argc == 3);

    char* mode = argv[1];
    char* filename = argv[2];

    FILE* in = fopen(filename, "rb");
    assert(in);

    codes_handle* h = codes_handle_new_from_file(0, in, PRODUCT_ANY, &err);
    assert(h);

    grib_dump_content(h, stdout,  mode, 0, NULL);

    codes_handle_delete(h);
    fclose(in);

    return 0;
}
