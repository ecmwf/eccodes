/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */
#include "grib_api.h"
#undef NDEBUG
#include "assert.h"

int main(int argc, char* argv[])
{
    int ret = GRIB_SUCCESS;

    if (argc != 2) return 1;

    const char* infile = argv[1];
    grib_index* index = grib_index_new(0, "shortName,level", &ret);
    assert(ret == GRIB_SUCCESS);

    ret = grib_index_add_file(index, infile);
    assert(ret == GRIB_SUCCESS);

    printf("Select nonexistent entries...\n");
    grib_index_select_string(index, "shortName", "frost");
    grib_index_select_long(index, "level", 666);

    grib_handle* h = grib_handle_new_from_index(index, &ret);
    assert(!h);
    assert(ret == GRIB_END_OF_INDEX);

    // Call it again. Should not crash
    h = grib_handle_new_from_index(index, &ret);
    assert(!h);
    assert(ret == GRIB_END_OF_INDEX);

    grib_index_delete(index);
    grib_context_delete(grib_context_get_default());

    return 0;
}
