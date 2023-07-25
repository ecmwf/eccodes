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
    FILE* f1       = NULL;
    FILE* f2       = NULL;
    grib_handle* h1 = NULL;
    grib_handle* h2 = NULL;
    int err       = 0;
    size_t count  = 0;

    Assert(argc == 3);
    f1 = fopen(argv[1], "rb");
    f2 = fopen(argv[2], "rb");
    Assert(f1 && f2);

    while ((h1 = grib_handle_new_from_file(0, f1, &err)) != NULL && (h2 = grib_handle_new_from_file(0, f2, &err)) != NULL) {
        grib_keys_iterator* kiter = NULL;

        // Use namespace of NULL to get ALL keys
        // Set flags to 0 to not filter any keys
        //kiter = grib_keys_iterator_new(h1, /*flags=*/0, /*namespace=*/NULL);
        kiter = grib_keys_iterator_new(h1, /*flags=*/GRIB_KEYS_ITERATOR_SKIP_COMPUTED, /*namespace=*/NULL);
        Assert(kiter);

        while (grib_keys_iterator_next(kiter)) {
            const char* name = grib_keys_iterator_get_name(kiter);
            Assert(name);
            err = codes_compare_key(h1, h2, name, 0);
            if (err) {
                fprintf(stderr, "key: %s  (%s)\n", name, grib_get_error_message(err));
                ++count;
            }
        }

        grib_keys_iterator_delete(kiter);
        grib_handle_delete(h1);
        grib_handle_delete(h2);
    }
    fclose(f1);
    fclose(f2);
    if (count > 0) {
        fprintf(stderr, "\nComparison failed: %zu differences\n", count);
        return 1;
    }
    return 0;
}
