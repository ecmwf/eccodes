/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "eccodes.h"
#undef NDEBUG
#include <assert.h>

int main(int argc, char* argv[])
{
    codes_handle* h = NULL;
    size_t len = 0;
    int err = 0;

    assert(argc == 4);

    const char* prod = argv[1];
    const char* infile = argv[2];
    FILE* in = fopen(infile, "rb");
    assert(in);
    char* key = argv[3];
    char kvalue[2] = {0,}; // deliberately short
    ProductKind productKind = PRODUCT_GRIB;
    if (strcmp(prod, "BUFR")==0) productKind = PRODUCT_BUFR;
    if (strcmp(prod, "GTS")==0) productKind = PRODUCT_GTS;
    if (strcmp(prod, "ANY")==0) productKind = PRODUCT_ANY;

    h = codes_handle_new_from_file(NULL, in, productKind, &err);
    assert(h);
    assert(!err);

    CODES_CHECK(codes_get_length(h, key, &len), 0);
    printf("Correct len=|%zu|\n", len);
    len = 1; // Cause it to fail

    err = codes_get_string(h, key, kvalue, &len);
    //printf("err=%d  kvalue=|%s|\n", err, kvalue);
    assert(err == CODES_BUFFER_TOO_SMALL);
    // The correct len should have been set
    assert(len > 1);

    codes_handle_delete(h);
    fclose(in);
    return 0;
}
