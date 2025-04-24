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

const char* keys[] = {
    "stationType",
    "stationType->index",
    "stationType->code",
    "stationType->width",
    "stationType->scale",
    "stationType->reference",
    "stationType->units",
    NULL
};

int main(int argc, char** argv)
{
    int err = 0;
    codes_handle* h = codes_bufr_handle_new_from_samples(NULL, "BUFR4");
    assert(h);

    CODES_CHECK(codes_set_long(h, "unpack", 1), 0);

    for (size_t i = 0; keys[i]; ++i) {
        const char* k = keys[i];
        printf("\tIs key=%s in the header?\t", k);
        int isHeader = codes_bufr_key_is_header(h, k, &err);
        assert(!err);
        assert(!isHeader);
        printf("No\n");
    }

    codes_handle_delete(h);

    return 0;
}
