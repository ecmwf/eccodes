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

int main(int argc, char** argv)
{
    int err = 0;
    codes_context* c = codes_context_get_default();

    assert(argc == 2);

    char* filename = argv[1];
    FILE* fp = fopen(filename, "rb");
    assert(fp);
    codes_handle* h = codes_handle_new_from_file(c, fp, PRODUCT_ANY, &err);
    assert(h);
    assert(!err);

    codes_dump_action_tree(c, stdout);

    codes_handle_delete(h);
    fclose(fp);

    return 0;
}
