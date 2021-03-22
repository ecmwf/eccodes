/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_tools.h"

grib_option grib_options[] = {
    /*  {id, args, help}, on, command_line, value*/
    { "f", 0, 0, 0, 1, 0 },
    { "p:", 0, 0, 0, 1, 0 },
    { "F:", 0, 0, 1, 1, "%g" },
    { "B:", 0, 0, 0, 1, 0 },
    { "l:", 0, 0, 0, 1, 0 },
    { "P:", 0, 0, 0, 1, 0 },
    { "w:", 0, 0, 0, 1, 0 },
    { "n:", 0, 0, 1, 1, 0 },
    { "s:", 0, 0, 0, 1, 0 },
    { "V", 0, 0, 0, 1, 0 },
    { "W:", 0, 0, 1, 1, "8" },
    { "m", 0, 0, 0, 1, 0 },
    { "M", 0, 0, 0, 1, 0 },
    { "S", 0, 0, 1, 0, 0 },
    { "g", 0, 0, 0, 1, 0 },
    { "G", 0, 0, 0, 1, 0 },
    { "T:", 0, 0, 0, 1, 0 },
    { "7", 0, 0, 0, 1, 0 },
    { "v", 0, 0, 1, 0, 0 },
    { "X:", 0, 0, 0, 1, 0 },
    { "i:", 0, 0, 0, 1, 0 }
};

const char* tool_description =
    "Get values of some keys from a GRIB file."
    "\n\tIt is similar to grib_ls, but fails returning an error code "
    "\n\twhen an error occurs (e.g. key not found).";
const char* tool_name  = "grib_get";
const char* tool_usage = "[options] grib_file grib_file ...";

int grib_options_count       = sizeof(grib_options) / sizeof(grib_option);
static double lat            = 0;
static double lon            = 0;
static int mode              = 0;
static grib_nearest* nearest = NULL;

int main(int argc, char* argv[])
{
    return grib_tool(argc, argv);
}

int grib_tool_before_getopt(grib_runtime_options* options)
{
    options->print_keys_count = -1;
    return 0;
}

int grib_tool_init(grib_runtime_options* options)
{
    char *theEnd = NULL, *end1 = NULL;
    size_t size = 4;
    int ret     = 0;
    double min = 0, max = 0;
    int i   = 0;
    char* p = NULL;

    options->print_header        = 0;
    options->print_statistics    = 0;
    options->default_print_width = -1;

    if (options->latlon) {
        lat = strtod(options->latlon, &theEnd);
        if (*theEnd != ',') {
            fprintf(stderr, "Error %s: wrong latitude value. Please use 'latitude,longitude'\n", tool_name);
            exit(1);
        }
        lon = strtod(++theEnd, &end1);

        mode = GRIB_NEAREST_SAME_POINT | GRIB_NEAREST_SAME_GRID;

        if (end1 && *end1 == ',') {
            end1++;
            if (*end1 != '0') {
                p = end1;
                while (*p != ',' && *p != '\0')
                    p++;
                if (*end1 == '4') {
                    options->latlon_mode = 4;
                }
                else if (*end1 == '1') {
                    options->latlon_mode = 1;
                }
                else {
                    fprintf(stderr, "Error %s: wrong mode given in option -l\n", tool_name);
                    exit(1);
                }
            }
            Assert(p);
            if (p && *p == ',') {
                p++;
                options->latlon_mask = strdup(p);
            }
        }
    }

    if (options->latlon && options->latlon_mask) {
        grib_handle* hh;
        FILE* f = fopen(options->latlon_mask, "r");
        if (!f) {
            perror(options->latlon_mask);
            exit(1);
        }
        hh = grib_handle_new_from_file(0, f, &ret);
        fclose(f);
        GRIB_CHECK_NOLINE(ret, 0);
        nearest = grib_nearest_new(hh, &ret);
        GRIB_CHECK_NOLINE(ret, 0);
        GRIB_CHECK_NOLINE(grib_nearest_find(nearest, hh, lat, lon, mode,
                                            options->lats, options->lons, options->mask_values, options->distances, options->indexes, &size),
                          0);
        grib_nearest_delete(nearest);
        nearest = NULL;
        grib_handle_delete(hh);

        options->latlon_idx = -1;
        max                 = options->distances[0];
        for (i = 0; i < 4; i++)
            if (max < options->distances[i]) {
                max = options->distances[i];
            }
        min = max;
        for (i = 0; i < 4; i++) {
            if ((min >= options->distances[i]) && (options->mask_values[i] >= 0.5)) {
                options->latlon_idx = i;
                min                 = options->distances[i];
            }
        }

        if (options->latlon_idx < 0) {
            min                 = 0;
            options->latlon_idx = 0;
            for (i = 1; i < 4; i++) {
                if (min > options->distances[i]) {
                    min                 = options->distances[i];
                    options->latlon_idx = i;
                }
            }
        }
    }

    return 0;
}

int grib_tool_new_filename_action(grib_runtime_options* options, const char* file)
{
    return 0;
}

int grib_tool_new_file_action(grib_runtime_options* options, grib_tools_file* file)
{
    exit_if_input_is_directory(tool_name, file->name);
    if (nearest)
        grib_nearest_delete(nearest);
    nearest = NULL;
    return 0;
}

int grib_tool_new_handle_action(grib_runtime_options* options, grib_handle* h)
{
    size_t size = 4;
    int err     = 0;

    if (!options->skip) {
        if (options->set_values_count != 0) {
            err = grib_set_values(h, options->set_values, options->set_values_count);
        }

        if (err != GRIB_SUCCESS && options->fail)
            exit(err);
    }

    if (options->latlon) {
        int i;
        double min;
        err = 0;
        if (!nearest)
            nearest = grib_nearest_new(h, &err);
        GRIB_CHECK_NOLINE(err, 0);
        GRIB_CHECK_NOLINE(grib_nearest_find(nearest, h, lat, lon, 0,
                                            options->lats, options->lons, options->values,
                                            options->distances, options->indexes, &size),
                          0);
        min                 = options->distances[0];
        options->latlon_idx = 0;
        for (i = 1; i < 4; i++) {
            if (min > options->distances[i]) {
                min                 = options->distances[i];
                options->latlon_idx = i;
            }
        }
    }

    return 0;
}

int grib_tool_skip_handle(grib_runtime_options* options, grib_handle* h)
{
    grib_handle_delete(h);
    return 0;
}

void grib_tool_print_key_values(grib_runtime_options* options, grib_handle* h)
{
    grib_print_key_values(options, h);
}

int grib_tool_finalise_action(grib_runtime_options* options)
{
    if (nearest)
        grib_nearest_delete(nearest);
    nearest = NULL;
    return 0;
}

int grib_no_handle_action(grib_runtime_options* options, int err)
{
    fprintf(dump_file, "\t\t\"ERROR: unreadable message\"\n");
    return 0;
}
