/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

 #include <cstdio>
#include "grib_api_internal.h"

static void check_validity(const grib_handle* handle)
{
    ECCODES_ASSERT(handle);
    long lVal = -1;
    GRIB_CHECK( grib_get_long(handle, "isMessageValid", &lVal), 0);
    ECCODES_ASSERT(lVal == 1);
}

// Headers-only clone, convert to edition 2, then headers-only clone
static void test1(const char* input)
{
    int err = 0;
    FILE* fin = fopen(input, "rb");
    ECCODES_ASSERT(fin);

    grib_handle* h1 = grib_handle_new_from_file(0, fin, &err);
    grib_handle* c = grib_handle_clone_headers_only(h1);
    GRIB_CHECK( grib_set_long(c, "edition", 2), 0);
    check_validity(c);
    grib_handle* c2 = grib_handle_clone_headers_only(c);
    check_validity(c2);

    grib_handle_delete(h1);
    grib_handle_delete(c);
    grib_handle_delete(c2);
    fclose(fin);
}

// Normal clone, convert to edition 2, then headers-only clone
static void test2(const char* input)
{
    int err = 0;
    FILE* fin = fopen(input, "rb");
    ECCODES_ASSERT(fin);

    grib_handle* h1 = grib_handle_new_from_file(0, fin, &err);
    ECCODES_ASSERT(h1);
    grib_handle* c = grib_handle_clone(h1);
    ECCODES_ASSERT(c);
    GRIB_CHECK( grib_set_long(c, "edition", 2), 0);
    check_validity(c);
    grib_handle* c2 = grib_handle_clone_headers_only(c);
    check_validity(c2);

    grib_handle_delete(h1);
    grib_handle_delete(c);
    grib_handle_delete(c2);
    fclose(fin);
}

int main(int argc, char* argv[])
{
    ECCODES_ASSERT(argc == 2);

    test1(argv[1]);
    test2(argv[1]);

    printf("All OK.\n");
    return 0;
}
