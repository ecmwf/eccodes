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
#include "grib_api_internal.h"

#undef NDEBUG
#include <assert.h>

int main(int argc, char* argv[])
{
    Assert(argc == 1);
    grib_handle* h = grib_handle_new_from_samples(0, "GRIB2");

    code_table_entry* entries = NULL;
    size_t num_entries = 0;
    const char* keyname = "indicatorOfUnitOfTimeRange";
    int err = codes_get_codetable_contents_malloc(h, keyname, &entries, &num_entries);
    Assert(!err);
    Assert(num_entries == 13);

    for (size_t i=0; i<num_entries;++i) {
        printf(" i=%zu |%s| |%s|\n", i, entries[i].abbreviation, entries[i].title);
    }
    free(entries);
    grib_handle_delete(h);

    return 0;
}
