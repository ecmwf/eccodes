/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

/*
 * C Implementation: nearest
 *
 * Description: How to get nearest point(s).
 *              Assumes all input GRIB messages have the same grid geometry
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "eccodes.h"

static void usage(const char* prog)
{
    printf("Usage: %s grib_file grib_file ...\n", prog);
    exit(1);
}

int main(int argc, char** argv)
{
    int err   = 0;
    long step = 0;
    size_t nfiles;
    int i               = 0;
    codes_fieldset* set = NULL;
    codes_handle* h     = NULL;
    char shortName[20]      = {0,};
    size_t len     = 20;
    double lats[4] = {0,};
    double lons[4] = {0,};
    double values[4] = {0,};
    double distances[4] = {0,};
    int indexes[4] = {0,};
    const char* order_by = "shortName,step:i";

    size_t size = 4;
    double lat = -40, lon = 15;
    int mode = 0;
    int count;
    const char** filenames = NULL;
    codes_nearest* nearest = NULL;

    if (argc < 2) usage(argv[0]);

    nfiles    = argc - 1;
    filenames = (const char**)malloc(sizeof(char*) * nfiles);
    for (i = 0; i < nfiles; i++)
        filenames[i] = strdup(argv[i + 1]);

    set = codes_fieldset_new_from_files(0, filenames, nfiles, 0, 0, 0, order_by, &err);
    CODES_CHECK(err, 0);

    printf("\nordering by %s\n", order_by);
    printf("\n%d fields in the fieldset\n", codes_fieldset_count(set));

    mode  = CODES_NEAREST_SAME_GRID | CODES_NEAREST_SAME_POINT;
    count = 1;

    while ((h = codes_fieldset_next_handle(set, &err)) != NULL) {
        CODES_CHECK(codes_get_long(h, "step", &step), 0);
        len = 20;
        CODES_CHECK(codes_get_string(h, "shortName", shortName, &len), 0);

        printf("Msg #%d, step=%ld, shortName=%s", count, step, shortName);
        if (!nearest) nearest = codes_grib_nearest_new(h, &err);
        CODES_CHECK(err, 0);
        CODES_CHECK(codes_grib_nearest_find(nearest, h, lat, lon, mode, lats, lons, values, distances, indexes, &size), 0);
        printf("\nIdx\tlat\tlon\tdist\tval\n");
        for (i = 0; i < 4; i++)
            printf("%d\t%.2f\t%.2f\t%g\t%g\n",
                   (int)indexes[i], lats[i], lons[i], distances[i], values[i]);
        printf("\n");

        codes_handle_delete(h);
        count++;
    }

    if (nearest) codes_grib_nearest_delete(nearest);

    if (set) codes_fieldset_delete(set);

    for (i = 0; i < nfiles; i++)
        free((char*)filenames[i]);
    free(filenames);

    return 0;
}
