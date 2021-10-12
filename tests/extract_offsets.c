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
#include <assert.h>

int main(int argc, char* argv[])
{
    char *filename;
    int err = 0;
    int num_messages = 0, i =0;
    off_t* offsets = NULL;
    codes_context* c = codes_context_get_default();
    const int strict_mode = 1;

    /* Usage: prog file */
    assert(argc == 2);

    filename = argv[1];
    err = codes_extract_offsets_malloc(c, filename, PRODUCT_ANY, &offsets, &num_messages, strict_mode);
    if (err) return err;
    
    for (i = 0; i < num_messages; ++i) {
        printf("%lu\n", (unsigned long)offsets[i]);
    }
    free(offsets);
    return 0;
}
