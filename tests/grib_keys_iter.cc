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

#define MAX_VAL_LEN 1024

int main(int argc, char* argv[])
{
    grib_handle* h = NULL;
    int err = 0;

    ECCODES_ASSERT(argc == 2);
    FILE* f = fopen(argv[1], "rb");
    ECCODES_ASSERT(f);

    while ((h = grib_handle_new_from_file(0, f, &err)) != NULL) {
        grib_keys_iterator* kiter = NULL;

        /* Use namespace of NULL to get ALL keys */
        /* Set flags to 0 to not filter any keys */
        kiter = grib_keys_iterator_new(h, /*flags=*/0, /*namespace=*/NULL);
        ECCODES_ASSERT(kiter);

        while (grib_keys_iterator_next(kiter)) {
            const char* name = grib_keys_iterator_get_name(kiter);
            ECCODES_ASSERT(name);
            int type = 0;
            GRIB_CHECK(grib_get_native_type(h, name, &type), 0);
            ECCODES_ASSERT( type > 0 && type < 7 );
            int ktype = grib_keys_iterator_get_native_type(kiter);
            if (type != ktype) {
                fprintf(stderr, "ERROR: key=%s type=%s ktype=%s\n", name, grib_get_type_name(type), grib_get_type_name(ktype));
                return 1;
            }
            const char* type_name = grib_get_type_name(type);
            if (STR_EQUAL(type_name, "unknown")) {
                fprintf(stderr, "ERROR: key=%s type is unknown!\n", name);
                return 1;
            }
            printf("%s = %s (%d)\n", name, type_name, type);

            if (STR_EQUAL(type_name, "label")) {
                char value[MAX_VAL_LEN] = {0,};
                size_t vlen = MAX_VAL_LEN;
                GRIB_CHECK(grib_get_string(h, name, value, &vlen), name);
                ECCODES_ASSERT( strlen(value) > 0 );
                int e = grib_keys_iterator_get_string(kiter, value, &vlen);
                ECCODES_ASSERT(!e);
                ECCODES_ASSERT( STR_EQUAL(name, value) );
            }
            if (STR_EQUAL(name, "editionNumber")) {
                long lVal = 0;
                size_t llen = 1;
                int e = grib_keys_iterator_get_long(kiter, &lVal, &llen);
                ECCODES_ASSERT(!e);
                ECCODES_ASSERT(lVal == 1 || lVal == 2);
                ECCODES_ASSERT(codes_key_is_computed(h, name, &e) == 0 && !e);
            }
            if (STR_EQUAL(name, "gridType")) {
                int e = 0;
                ECCODES_ASSERT(codes_key_is_computed(h, name, &e) == 1 && !e);
            }
            if (STR_EQUAL(name, "longitudeOfLastGridPointInDegrees")) {
                int e = 0;
                ECCODES_ASSERT(codes_key_is_computed(h, name, &e) == 1 && !e);
            }
            if (STR_EQUAL(name, "longitudeOfLastGridPoint")) {
                int e = 0;
                ECCODES_ASSERT(codes_key_is_computed(h, name, &e) == 0 && !e);
            }
        }

        grib_keys_iterator_delete(kiter);
        grib_handle_delete(h);
    }
    fclose(f);
    return 0;
}
