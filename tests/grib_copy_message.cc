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
    FILE* in                     = NULL;
    codes_handle* source_handle  = NULL;
    int err                      = 0;
    size_t totalLength = 0, size = 0;
    unsigned char* buffer        = NULL;
    codes_handle* new_handle     = NULL;

    assert (argc == 3);

    in = fopen(argv[1], "rb");
    assert(in);

    source_handle = codes_handle_new_from_file(0, in, PRODUCT_GRIB, &err);
    assert(source_handle);

    // How big is the input GRIB message?
    CODES_CHECK(codes_get_message_size(source_handle, &totalLength), 0);

    // Allocate a buffer large enough to hold the message
    buffer = (unsigned char*)malloc(totalLength);
    size = totalLength;

    // Take a copy of the message into buffer. Now we own it
    CODES_CHECK(codes_get_message_copy(source_handle, buffer, &size), 0);
    assert(size == totalLength);
    codes_handle_delete(source_handle);

    // Now buffer contains a copy of the message
    new_handle = codes_handle_new_from_message(0, buffer, totalLength);
    assert(new_handle);

    // Change something and write it out
    CODES_CHECK(codes_set_long(new_handle, "hour", 18), 0);
    CODES_CHECK(codes_write_message(new_handle, argv[2], "w"), 0);
    codes_handle_delete(new_handle);
    free(buffer);

    fclose(in);

    return 0;
}
