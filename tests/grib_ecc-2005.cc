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
#undef NDEBUG
#include <assert.h>

#include "eccodes.h"

#define MAX_VAL_LEN 1024

int main(int argc, char* argv[])
{
    unsigned long key_iterator_filter_flags = CODES_KEYS_ITERATOR_ALL_KEYS;
    const char* name_space = "mars";

    int err = 0;
    char value[MAX_VAL_LEN];
    size_t vlen = MAX_VAL_LEN;

    assert(argc == 2);

    FILE* f = fopen(argv[1], "rb");
    assert(f);

    codes_handle* h = codes_handle_new_from_file(0, f, PRODUCT_GRIB, &err);
    assert(h);
    assert(!err);

    if (!codes_is_defined(h, "stream") && !codes_is_defined(h, "type")) {
        printf("NOTE: No mars keys so let's do setLocalDefinition=1\n");
        CODES_CHECK(codes_set_long(h, "setLocalDefinition", 1), 0);
    }
    else {
        printf("NOTE: message already has a local definition\n");
    }

    size_t alen = 1000;
    CODES_CHECK(codes_set_string(h, "stream", "eefh", &alen), 0);
    CODES_CHECK(codes_set_string(h, "type", "cf", &alen), 0);

    printf("Check the keys individually...\n");

    long lVal = 0;
    CODES_CHECK(codes_get_long(h, "stream", &lVal), 0);
    printf("\t stream=%ld\n", lVal);
    CODES_CHECK(codes_get_long(h, "type", &lVal), 0);
    printf("\t type=%ld\n", lVal);
    CODES_CHECK(codes_get_long(h, "hdate", &lVal), 0);
    printf("\t hdate=%ld\n", lVal);
    CODES_CHECK(codes_get_long(h, "mars.hdate", &lVal), 0);
    printf("\t mars.hdate=%ld\n", lVal);

    printf("Now launch keys iterator....\n");

    // Workaround: Cloning the original handle fixes the problem
    // codes_handle* clone_handle = codes_handle_clone(h);
    // kiter = codes_keys_iterator_new(h, key_iterator_filter_flags, name_space);
    codes_keys_iterator* kiter = codes_keys_iterator_new(h, key_iterator_filter_flags, name_space);
    assert(kiter);

    bool hdate_found = 0;
    while (codes_keys_iterator_next(kiter)) {
        const char* name = codes_keys_iterator_get_name(kiter);
        if (strcmp(name, "expver") == 0) continue;
        vlen = MAX_VAL_LEN;
        memset(value, 0, vlen);
        // printf("....get %s\n",name);
        int ntype = -1;
        err = codes_get_native_type(h, name, &ntype);
        if (!err) {
            if (ntype == GRIB_TYPE_STRING) {
                CODES_CHECK(codes_get_string(h, name, value, &vlen), name);
                printf("\t %s = %s\t", name, value);
            }
            else if (ntype == GRIB_TYPE_LONG) {
                CODES_CHECK(codes_get_long(h, name, &lVal), 0);
                printf("\t %s = %ld\t", name, lVal);
            }
        }
        if (strcmp(name, "hdate") == 0) {
            hdate_found = true;
            printf("<----- FOUND IT!");
        }
        printf("\n");
    }

    codes_keys_iterator_delete(kiter);
    codes_handle_delete(h);
    fclose(f);

    if (!hdate_found) {
        printf("ERROR: Key 'hdate' was not in the %s namespace!\n", name_space);
        return 1;
    }

    return 0;
}
