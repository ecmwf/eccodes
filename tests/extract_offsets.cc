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
    char *filename = NULL;
    char *what = NULL; // offsets or sizes
    int err = 0;
    int num_messages = 0, i =0;
    off_t* offsets = NULL;
    size_t* sizes = NULL;
    codes_context* c = codes_context_get_default();
    const int strict_mode = 1;

    /* Usage: prog mode file */
    assert(argc == 3);

    what = argv[1];
    filename = argv[2];

    if (strcmp(what, "-o")==0) {
        err = codes_extract_offsets_malloc(c, filename, PRODUCT_ANY, &offsets, &num_messages, strict_mode);
        if (err) return err;

        for (i = 0; i < num_messages; ++i) {
            printf("%lu\n", (unsigned long)offsets[i]);
        }
        free(offsets);
    }

    if (strcmp(what, "-s")==0) {
        // Version getting offsets as well as sizes of messages
        err = codes_extract_offsets_sizes_malloc(c, filename, PRODUCT_ANY, &offsets, &sizes, &num_messages, strict_mode);
        if (err) return err;

        for (i = 0; i < num_messages; ++i) {
            printf("%zu\n", sizes[i]);
        }
        free(offsets);
        free(sizes);
    }

    return 0;
}
