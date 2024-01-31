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
#include <ctype.h>

static void usage(const char* prog)
{
    printf("usage: %s in1.grib in2.grib\n", prog);
    exit(1);
}

int main(int argc, char* argv[])
{
    codes_handle *hfrom, *hto;
    FILE* in;
    char *in_name1, *in_name2;
    int i = 0, err = 0;
    const char* keys[]   = { "gridType", "pl", "values" };
    const int keys_count = sizeof(keys) / sizeof(keys[0]);

    if (argc != 3) usage(argv[0]);

    in_name1 = argv[1];
    in_name2 = argv[2];

    in = fopen(in_name1, "rb");
    if (!in) {
        perror(in_name1);
        exit(1);
    }

    hfrom = codes_handle_new_from_file(0, in, PRODUCT_GRIB, &err);
    CODES_CHECK(err, 0);
    fclose(in);

    in = fopen(in_name2, "rb");
    if (!in) {
        perror(in_name2);
        exit(1);
    }

    hto = codes_handle_new_from_file(0, in, PRODUCT_GRIB, &err);
    CODES_CHECK(err, 0);
    fclose(in);

    for (i = 0; i < keys_count; i++) {
        printf("Copying key: %s\n", keys[i]);
        err = codes_copy_key(hfrom, hto, keys[i], GRIB_TYPE_UNDEFINED);
        CODES_CHECK(err, 0);
    }

    /* codes_write_message(hto,  "temp.out", "w"); CODES_CHECK(err, 0); */
    {
        int dump_flags = GRIB_DUMP_FLAG_CODED | GRIB_DUMP_FLAG_OCTET | GRIB_DUMP_FLAG_VALUES | GRIB_DUMP_FLAG_READ_ONLY;
        codes_dump_content(hto, stdout, "wmo", dump_flags, NULL);
    }

    codes_handle_delete(hfrom);
    codes_handle_delete(hto);

    return err;
}
