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
#include <stdlib.h>
#include <assert.h>
#include "eccodes.h"

int main(int argc, char** argv)
{
    int err = 0;
    size_t size = 0;
    FILE* in            = NULL;
    const char* infile  = "../data/test_uuid.grib2";
    FILE* out           = NULL;
    const char* outfile = "temp.grib_set_bytes.grib";
    codes_handle* h     = NULL;
    const void* buffer  = NULL;

    unsigned char uuid_short[] = { /* not enough bytes */
        0x07, 0x20, 0x40, 0x51,
        0x07, 0x20, 0x40, 0x51,
        0x07, 0x20, 0x40, 0x52,
        0x07, 0x20, 0x40
    };
    unsigned char uuid_good[] = { /* Just the right size i.e. 16 bytes */
        0x07, 0x20, 0x40, 0x51,
        0x07, 0x20, 0x40, 0x51,
        0x07, 0x20, 0x40, 0x52,
        0x07, 0x20, 0x40, 0x53
    };
    unsigned char uuid_long[] = { /* Too many bytes */
        0x07, 0x20, 0x40, 0x51,
        0x07, 0x20, 0x40, 0x51,
        0x07, 0x20, 0x40, 0x52,
        0x07, 0x20, 0x40, 0x53,
        0x00, 0x01
    };
    size_t uuid_short_len = sizeof (uuid_short);
    size_t uuid_good_len = sizeof (uuid_good);
    size_t uuid_long_len = sizeof (uuid_long);

    in = fopen(infile, "rb");
    assert(in);
    out = fopen(outfile, "wb");
    assert(out);

    h = codes_handle_new_from_file(0, in, PRODUCT_GRIB, &err);
    assert(h);

    /* The uuidOfVGrid key is 16 bytes long */
    err = codes_set_bytes(h, "uuidOfVGrid", uuid_short, &uuid_short_len);
    assert(err == CODES_BUFFER_TOO_SMALL);
    err = codes_set_bytes(h, "uuidOfVGrid", uuid_long, &uuid_long_len);
    assert(err == CODES_BUFFER_TOO_SMALL);

    /* This one should work */
    err = codes_set_bytes(h, "uuidOfVGrid", uuid_good, &uuid_good_len);
    assert(err == 0);

    CODES_CHECK(codes_get_message(h, &buffer, &size), 0);
    if (fwrite(buffer, 1, size, out) != size) {
        perror(argv[1]);
        exit(1);
    }

    codes_handle_delete(h);
    fclose(in);
    fclose(out);
    return 0;
}
