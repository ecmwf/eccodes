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
#include <assert.h>

static int fail_on_error    = 1;
static const char* toolname = NULL;

static void usage(const char* prog)
{
    printf("Usage: %s [-s | -d] path\n", prog);
    exit(1);
}

int main(int argc, char* argv[])
{
    char* path        = NULL;
    char* full        = NULL;
    char* option      = NULL;
    char* resource    = NULL;
    int verbose       = 0;
    int export_sample = 0, export_definition = 0;
    grib_context* c = grib_context_get_default();

    toolname = argv[0];
    option   = argv[1];
    path     = argv[2];

    if (argc != 3) usage(toolname);

    if (strcmp(option, "-s") == 0) {
        export_sample = 1;
        resource = "sample";
    } else if (strcmp(option, "-d") == 0) {
        export_definition = 1;
        resource = "definition";
    } else {
        fprintf(stderr, "Invalid option: Specify either -s or -d\n");
        return 1;
    }

    if (export_sample) {
        char* t = strstr(path, ".tmpl");
        if (t) {
            *t = 0;  // get rid of the sample file extension
        }
        full = grib_external_template_path(c, path);
    }
    else if (export_definition) {
        full = grib_context_full_defs_path(c, path);
    }
    if (!full) {
        fprintf(stderr, "Failed to export %s: %s\n", resource, path);
        return 1;
    }
    printf("full = %s\n", full);
    assert(path_is_regular_file(full));
    grib_context_free(c, full);

    return 0;
}
