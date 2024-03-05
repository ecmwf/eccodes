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
#include "eccodes.h"
#undef NDEBUG
#include <assert.h>

int main(int argc, char* argv[])
{
    FILE* in                    = NULL;
    FILE* out                   = NULL;
    codes_handle* source_handle = NULL;
    const void* buffer          = NULL;
    int err                     = 0;

    long totalLength_src = 0, totalLength_dst = 0;
    long edition = 0, isGridded_src = 0, bitmapPresent = 0;
    long isConstant_src = 0, isConstant_dst = 0;
    long dataSectionLength_src = 0, dataSectionLength_dst = 0;
    size_t messageLength_src = 0, messageLength_dst = 0;

    if (argc != 3) {
        // Usage: prog input_file ouput_file
        return 1;
    }

    in  = fopen(argv[1], "rb");
    assert(in);
    out = fopen(argv[2], "wb");
    assert(out);

    while ((source_handle = codes_handle_new_from_file(0, in, PRODUCT_GRIB, &err)) != NULL) {
        codes_handle* clone_handle = codes_handle_clone_headers_only(source_handle);
        assert(clone_handle);

        codes_get_long(source_handle, "isConstant", &isConstant_src);
        codes_get_long(source_handle, "isGridded", &isGridded_src);
        if (isGridded_src && !isConstant_src) {

            CODES_CHECK(codes_get_message(source_handle, &buffer, &messageLength_src), 0);
            CODES_CHECK(codes_get_message(clone_handle, &buffer, &messageLength_dst), 0);
            assert( messageLength_src > messageLength_dst );

            CODES_CHECK(codes_get_long(source_handle, "totalLength", &totalLength_src), 0);
            CODES_CHECK(codes_get_long(clone_handle, "totalLength", &totalLength_dst), 0);
            assert(totalLength_src > totalLength_dst);

            CODES_CHECK(codes_get_long(source_handle, "edition", &edition), 0);
            if (edition == 1) {
                CODES_CHECK(codes_get_long(source_handle, "section4Length", &dataSectionLength_src), 0);
                CODES_CHECK(codes_get_long(clone_handle, "section4Length", &dataSectionLength_dst), 0);
            } else if (edition == 2) {
                CODES_CHECK(codes_get_long(source_handle, "section7Length", &dataSectionLength_src), 0);
                CODES_CHECK(codes_get_long(clone_handle, "section7Length", &dataSectionLength_dst), 0);
            }
            assert( dataSectionLength_src > dataSectionLength_dst );

            codes_get_long(clone_handle, "bitmapPresent", &bitmapPresent);
            assert(bitmapPresent == 0);
            codes_get_long(clone_handle, "isConstant", &isConstant_dst);
            assert(isConstant_dst == 1);
        }

        // write out the cloned buffer
        if (fwrite(buffer, 1, messageLength_dst, out) != messageLength_dst) {
            perror(argv[1]);
            return 1;
        }
        codes_handle_delete(clone_handle);
        codes_handle_delete(source_handle);
    }

    fclose(out);
    fclose(in);

    printf("All OK\n");
    return 0;
}
