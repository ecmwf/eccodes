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
 * C Implementation: grib_ls
 *
 * Author: Enrico Fucile <enrico.fucile@ecmwf.int>
 *
 *
 */
#include "grib_tools.h"

grib_option grib_options[] = {
    /*  {id, args, help}, on, command_line, value*/
    { "f", 0, 0, 1, 0, 0 },
    { "p:", 0, 0, 0, 1, 0 },
    { "F:", 0, 0, 1, 1, "%g" },
    { "P:", 0, 0, 0, 1, 0 },
    { "w:", 0, 0, 0, 1, 0 },
    { "j", 0, "json output\n", 0, 1, 0 },
    { "B:", 0, 0, 0, 1, 0 },
    /* {"l:",0,0,0,1,0}, */
    { "s:", 0, 0, 0, 1, 0 },
    /* {"i:",0,0,0,1,0}, */
    { "n:", 0, 0, 1, 1, "ls" },
    { "m", 0, 0, 0, 1, 0 },
    { "V", 0, 0, 0, 1, 0 },
    { "W:", 0, 0, 1, 1, "10" },
    { "S", 0, 0, 1, 0, 0 },
    /* {"M",0,0,0,1,0}, */
    { "H", 0, 0, 1, 0, 0 },
    { "g", 0, 0, 0, 1, 0 },
    { "P", 0, 0, 1, 0, 0 },
    { "T:", 0, 0, 1, 0, "M" },
    { "7", 0, 0, 0, 1, 0 },
    { "v", 0, 0, 1, 0, 0 }
    /*{"x",0,0,0,1,0}*/
};

const char* tool_description =
    "List content of METAR files printing values of "
    "some keys.\n\tIt does not fail when a key is not found.";
const char* tool_name  = "metar_ls";
const char* tool_usage = "[options] file file ...";

int grib_options_count = sizeof(grib_options) / sizeof(grib_option);
double lat             = 0;
double lon             = 0;
int mode               = 0;
static int json        = 0;

int main(int argc, char* argv[])
{
    return grib_tool(argc, argv);
}

/*
This is executed before processing the options with i
getopt and therfore it is the right place for hacking 
the arguments if needed
 */
int grib_tool_before_getopt(grib_runtime_options* options)
{
    return 0;
}

/*
The options have been parsed and the structure 
grib_runtime_options* options has been loaded. 
Initialization and startup can be done here
 */
int grib_tool_init(grib_runtime_options* options)
{
    if (grib_options_on("j")) {
        options->verbose = 0;
        json             = 1;
    }

    if (json)
        printf("[\n");

    return 0;
}

/*
A new file is being parsed. The file name is file. This function is called every time
a new input file name is processed, before opening the file.
 */
int grib_tool_new_filename_action(grib_runtime_options* options, const char* file)
{
    return 0;
}

int grib_tool_new_file_action(grib_runtime_options* options, grib_tools_file* file)
{
    return 0;
}

/* A new handle is available from the current input file and can be processed here.
The handle available in this function is in the set of messages satisfying the constrant of the
-w option. They are not to be skipped.
 */
int grib_tool_new_handle_action(grib_runtime_options* options, grib_handle* h)
{
    int err = 0;

    if (!options->skip) {
        if (options->set_values_count != 0)
            err = grib_set_values(h, options->set_values, options->set_values_count);

        if (err != GRIB_SUCCESS && options->fail)
            exit(err);
    }

    return 0;
}

/* A new handle to skip is available. At this point something can be done
with the message to be skipped before deleting the handle. */
int grib_tool_skip_handle(grib_runtime_options* options, grib_handle* h)
{
    grib_handle_delete(h);
    return 0;
}

/* key values can be printed in this function. Headers are already printed if requested.*/
void grib_tool_print_key_values(grib_runtime_options* options, grib_handle* h)
{
    grib_print_key_values(options, h);
}

/* this is executed after the last message in the last file is processed */
int grib_tool_finalise_action(grib_runtime_options* options)
{
    if (json)
        printf("\n]\n");

    return 0;
}

int grib_no_handle_action(grib_runtime_options* options, int err)
{
    fprintf(dump_file, "\t\t\"ERROR: unreadable message\"\n");
    return 0;
}
