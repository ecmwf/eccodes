/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#undef NDEBUG
#include <assert.h>
#include "eccodes.h"

#define MAX_VAL_LEN 1024

static void check_for_levelist(codes_handle* h, bool should_exist)
{
    codes_keys_iterator* kiter = codes_keys_iterator_new(h, 0, "mars");
    bool mars_levelist_found = false;
    char value[MAX_VAL_LEN];

    while (codes_keys_iterator_next(kiter)) {
        const char* name = codes_keys_iterator_get_name(kiter);
        size_t vlen = MAX_VAL_LEN;
        memset(value, 0, vlen);
        CODES_CHECK(codes_get_string(h, name, value, &vlen), name);
        // printf("  name=%s  val=%s\n", name, value);
        if (strcmp(name, "levelist")) {
            mars_levelist_found = true;
            assert( strcmp(value, "42") );
        }
    }
    assert( mars_levelist_found == should_exist );
    codes_keys_iterator_delete(kiter);
}

int main(int argc, char* argv[])
{
    codes_handle* h = grib_handle_new_from_samples(0, "GRIB2");
    size_t alen     = 1000;
    CODES_CHECK(codes_set_string(h, "typeOfLevel", "isobaricInhPa", &alen), 0);
    CODES_CHECK(codes_set_long(h, "level", 42), 0);
    //codes_dump_content(h, stdout, "wmo", GRIB_DUMP_FLAG_CODED | GRIB_DUMP_FLAG_OCTET | GRIB_DUMP_FLAG_VALUES | GRIB_DUMP_FLAG_READ_ONLY, 0);
    check_for_levelist(h, true);

    // TODO(maee): This is related to
    //   ECC-1898: GRIB: Key 'levelist' persists despite being taken out of the mars namespace
    // Once the bug is fixed we should remove the comments
    // alen = 1000;
    // CODES_CHECK(codes_set_string(h, "typeOfLevel", "surface", &alen), 0);
    // CODES_CHECK(codes_set_missing(h, "scaleFactorOfFirstFixedSurface"), 0);
    // CODES_CHECK(codes_set_missing(h, "scaledValueOfFirstFixedSurface"), 0);
    // check_for_levelist(h, false);

    codes_handle_delete(h);

    return 0;
}
