/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "grib_api_internal.h"

int main(int argc, char* argv[])
{
    grib_handle* h = NULL;
    int err = 0;

    Assert(argc == 2);
    const char* fname = argv[1];
    FILE* f = fopen(fname, "rb");
    Assert(f);

    unsigned long flags[] = {
        GRIB_KEYS_ITERATOR_SKIP_READ_ONLY,
        GRIB_KEYS_ITERATOR_SKIP_OPTIONAL,
        GRIB_KEYS_ITERATOR_SKIP_EDITION_SPECIFIC,
        GRIB_KEYS_ITERATOR_SKIP_CODED,
        GRIB_KEYS_ITERATOR_SKIP_COMPUTED,
        GRIB_KEYS_ITERATOR_SKIP_DUPLICATES,
        GRIB_KEYS_ITERATOR_SKIP_FUNCTION
    };
    const unsigned long N = sizeof(flags)/sizeof(unsigned long);

    while ((h = grib_handle_new_from_file(0, f, &err)) != NULL) {

        for (unsigned long i=0; i<N; ++i) {
            size_t count = 0;
            grib_keys_iterator* kiter = grib_keys_iterator_new(h, flags[i], NULL);
            Assert(kiter);

            while (grib_keys_iterator_next(kiter)) {
                const char* name = grib_keys_iterator_get_name(kiter);
                Assert(name);
                Assert(strlen(name) > 0);
                ++count;
            }
            grib_keys_iterator_delete(kiter);
            printf("File=%s: Flag=%lu, count=%zu\n", fname, flags[i], count);
        }

        grib_handle_delete(h);
    }

    fclose(f);
    return 0;
}
