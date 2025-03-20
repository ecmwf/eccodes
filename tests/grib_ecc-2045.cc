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
    int err = 0;
    long lVal=0;

    ECCODES_ASSERT(argc == 2);

    printf("\nDoing file %s...\n", argv[1]);
    FILE* in  = fopen(argv[1], "rb");
    ECCODES_ASSERT(in);

    grib_handle* handle = grib_handle_new_from_file(0, in, &err);
    ECCODES_ASSERT(handle);
    ECCODES_ASSERT(!err);

    grib_handle* c = grib_handle_clone_headers_only(handle);
    ECCODES_ASSERT(c);
    lVal=-1; GRIB_CHECK( grib_get_long(c, "isMessageValid", &lVal), 0); printf("0. isMessageValid=%ld\n", lVal);
    printf("0. /////// dump the 1st clone (grib1 headers_only)\n");
    grib_dump_content(c, stdout, "wmo", 0, 0);

    printf("\nSetting edition=2...\n");
    GRIB_CHECK( grib_set_long(c, "edition", 2), 0);
    ECCODES_ASSERT( c->sections_count == 8 );
    printf("1. /////// dump the 1st clone (grib2 headers_only)\n");
    grib_dump_content(c, stdout, "wmo", 0, 0);
    lVal=-1; GRIB_CHECK( grib_get_long(c, "isMessageValid", &lVal), 0); printf("1. isMessageValid=%ld\n", lVal);

    printf("\nClone the clone...\n");
    grib_handle* c2 = grib_handle_clone_headers_only(c);
    ECCODES_ASSERT(c2);
    printf("2. /////// dump the 2nd clone (grib2 headers_only)\n");
    grib_dump_content(c2, stdout, "wmo", 0, 0);
    lVal=-1; GRIB_CHECK( grib_get_long(c2, "isMessageValid", &lVal), 0); printf("2. isMessageValid=%ld\n", lVal);

    grib_handle_delete(handle);
    grib_handle_delete(c);
    grib_handle_delete(c2);
    fclose(in);
    printf("All OK.\n");
    return 0;
}
