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
    { "V", 0, 0, 0, 1, 0 },
    { "p:", 0, 0, 0, 1, 0 }, /* print keys */
    { "S", 0, 0, 1, 0, 0 },  /* needed for skip */
    { "w:", 0, 0, 0, 1, 0 }  /* 'where' clause */
};
int grib_options_count = sizeof(grib_options) / sizeof(grib_option);

const char* tool_description = "Histogram of GRIB files";
const char* tool_name        = "grib_histogram";
const char* tool_usage       = "[options] grib_file grib_file ...";

int main(int argc, char* argv[])
{
    return grib_tool(argc, argv);
}

int grib_tool_before_getopt(grib_runtime_options* options)
{
    return 0;
}

int grib_tool_init(grib_runtime_options* options)
{
    return 0;
}

int grib_tool_new_filename_action(grib_runtime_options* options, const char* file)
{
    return 0;
}

int grib_tool_new_file_action(grib_runtime_options* options, grib_tools_file* file)
{
    exit_if_input_is_directory(tool_name, file->name);
    return 0;
}

/*
 A new handle is available from the current input file and can be processed here.
 The handle available in this function is in the set of messages satisfying the constraint
 of the -w option. They are not to be skipped.
*/
int grib_tool_new_handle_action(grib_runtime_options* options, grib_handle* h)
{
    int i, j, err = 0;
    size_t last_size = 0;
    long missingValuesPresent;
    double delta;

    double* values = NULL;
    size_t size;
    double min, max;
    long count = 10;
    long intervals[10];
    const char* names[1024];
    int name_count = 0;

    for (i = 0; i < options->requested_print_keys_count; i++) {
        names[name_count++] = options->requested_print_keys[i].name;
    }

    if (!options->skip) {
        if (options->set_values_count != 0)
            err = grib_set_values(h, options->set_values, options->set_values_count);

        if (err != GRIB_SUCCESS && options->fail)
            exit(err);
    }

    GRIB_CHECK(grib_get_long(h, "missingValuesPresent", &missingValuesPresent), 0);

    GRIB_CHECK(grib_get_size(h, "values", &size), 0);

    if (size > last_size) {
        if (values)
            free(values);
        values    = (double*)malloc(size * sizeof(double));
        /*last_size = size;*/
        if (!values) {
            fprintf(stderr, "Failed to allocate memory for values (%lu bytes)\n", size * sizeof(double));
            exit(1);
        }
    }

    GRIB_CHECK(grib_get_double_array(h, "values", values, &size), 0);

    for (j = 0; j < count; j++)
        intervals[j] = 0;

    if (missingValuesPresent) {
        double missing;
        GRIB_CHECK(grib_get_double(h, "missingValue", &missing), 0);

        min = max = missing;

        for (j = 0; j < size; j++) {
            if ((min == missing) || ((values[j] != missing) && (min > values[j])))
                min = values[j];
            if ((max == missing) || ((values[j] != missing) && (max < values[j])))
                max = values[j];
        }

        delta = max - min;
        if (delta == 0)
            delta = 1;

        for (j = 0; j < size; j++) {
            if (values[j] != missing) {
                int x = (values[j] - min) / delta * count;
                if (x == count)
                    x = x - 1; /*handle the absolute  maximum */
                intervals[x]++;
            }
        }
    }
    else {
        min = max = values[0];
        for (j = 1; j < size; j++) {
            if (min > values[j])
                min = values[j];
            if (max < values[j])
                max = values[j];
        }

        delta = max - min;
        if (delta == 0)
            delta = 1;

        for (j = 0; j < size; j++) {
            int x = (values[j] - min) / delta * count;
            if (x == count)
                x = x - 1; /*handle the absolute  maximum */
            intervals[x]++;
        }
    }

    for (j = 0; j < name_count; j++) {
        char v[1024];
        size_t s = sizeof(v);
        GRIB_CHECK(grib_get_string(h, names[j], v, &s), names[j]);
        printf(" %s", v);
    }

    printf("\nmin=%g max=%g size=%ld\n", min, max, (long)size);
    for (j = 0; j < count; j++)
        printf(" %g:%g", j * (max - min) / count + min, (j + 1) * (max - min) / count + min);
    printf("\n");

    for (j = 0; j < count; j++)
        printf(" %ld", intervals[j]);
    printf("\n");

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
    return 0;
}

int grib_no_handle_action(grib_runtime_options* options, int err)
{
    fprintf(dump_file, "\t\t\"ERROR: unreadable message\"\n");
    return 0;
}
