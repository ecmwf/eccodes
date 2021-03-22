/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include <assert.h>
#include "grib_api_internal.h"

int main(int argc, char** argv)
{
    char* filename  = NULL;
    FILE* fp        = NULL;
    char line[1024] = {0,};
    char** list = NULL;
    size_t i    = 0, line_number = 0;
    char* str_key = NULL;
    char* str_units = NULL;
    bufr_descriptor v;
    const size_t maxlen_keyName = sizeof(v.shortName);
    const size_t maxlen_units = sizeof(v.units);

    Assert(argc == 2);

    filename = argv[1];
    fp       = fopen(filename, "r");
    Assert(fp);

    while (fgets(line, sizeof(line) - 1, fp)) {
        ++line_number;
        Assert(strlen(line) > 0);
        if (line[0] == '#') continue; /* Ignore first line with column titles */
        list = string_split(line, "|");
        Assert(list);
        str_key = list[1];
        str_units = list[4];
        if (strlen(str_key) >= maxlen_keyName) {
            fprintf(stderr, "Error on line %lu: bufr_descriptor key name '%s' exceeds %lu characters.\n",
                    line_number, str_key, maxlen_keyName);
            return 1;
        }
        if (strlen(str_units) >= maxlen_units) {
            fprintf(stderr, "Error on line %lu: bufr_descriptor units '%s' exceeds %lu characters.\n",
                    line_number, str_units, maxlen_units);
            return 1;
        }

        for (i = 0; list[i] != NULL; ++i) free(list[i]);
        free(list);
    }

    fclose(fp);

    grib_context_delete(grib_context_get_default());
    return 0;
}
