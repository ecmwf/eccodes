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
#include <unistd.h>

int main(int argc, char* argv[])
{
    char *filename = NULL;
    int err = 0;
    int num_messages = 0, i = 0, do_offsets = 0, do_sizes = 0, oc = 0;
    off_t* offsets = NULL;  // array of message offsets
    size_t* sizes = NULL;   // array of message sizes
    codes_context* c = codes_context_get_default();
    const int strict_mode = 1;

    assert(argc == 3 || argc == 4);

    while ((oc = getopt(argc, argv, "os")) != -1) {
        switch (oc) {
            case 'o':
                do_offsets = 1;
                break;
            case 's':
                do_sizes = 1;
                break;
        }
    }
    filename = argv[optind];

    if (do_offsets) {
        err = codes_extract_offsets_malloc(c, filename, PRODUCT_ANY, &offsets, &num_messages, strict_mode);
        if (err) return err;

        for (i = 0; i < num_messages; ++i) {
            printf("%lu\n", (unsigned long)offsets[i]);
        }
    }

    if (do_sizes) {
        // Version getting offsets as well as sizes of messages
        err = codes_extract_offsets_sizes_malloc(c, filename, PRODUCT_ANY, &offsets, &sizes, &num_messages, strict_mode);
        if (err) return err;

        for (i = 0; i < num_messages; ++i) {
            printf("%zu\n", sizes[i]);
        }
        free(sizes);
    }

    free(offsets);
    return 0;
}
