/*
 * Copyright 2005-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

/*
 * C Implementation: bufr_get
 *
 */

#include "grib_tools.h"

grib_option grib_options[]={
        /*  {id, args, help}, on, command_line, value*/
        {"f",0,0,0,1,0},
        {"p:",0,0,0,1,0},
        {"F:",0,0,1,1,"%g"},
        {"P:",0,0,0,1,0},
        {"w:",0,0,0,1,0},
        {"n:",0,0,1,1,0},
        {"s:",0,0,0,1,0},
        {"V",0,0,0,1,0},
        {"W:",0,0,1,1,"8"},
        {"m",0,0,0,1,0},
        {"S",0,0,1,0,0},
        {"g",0,0,0,1,0},
/*      {"G",0,0,0,1,0}, */
        {"T:",0,0,1,0,"B"},
        {"7",0,0,0,1,0},
        {"v",0,0,1,0,0}
};

char* grib_tool_description="Get values of some keys from a BUFR file."
        "\n\tIt is similar to bufr_ls, but fails returning an error code "
        "\n\twhen an error occurs (e.g. key not found).";
char* grib_tool_name="bufr_get";
char* grib_tool_usage="[options] file file ...";

int grib_options_count=sizeof(grib_options)/sizeof(grib_option);
grib_nearest* n=NULL;

int main(int argc, char *argv[])
{
    return grib_tool(argc,argv);
}

int grib_tool_before_getopt(grib_runtime_options* options)
{
    options->print_keys_count=-1;
    return 0;
}

int grib_tool_init(grib_runtime_options* options)
{
    options->print_header=0;
    options->print_statistics=0;
    options->default_print_width=-1;

    return 0;
}

int grib_tool_new_filename_action(grib_runtime_options* options,const char* file)
{
    return 0;
}

int grib_tool_new_file_action(grib_runtime_options* options,grib_tools_file* file)
{
    return 0;
}

int grib_tool_new_handle_action(grib_runtime_options* options,grib_handle* h)
{
    int err = 0;

    if (!options->skip) {
        if (options->set_values_count != 0) {
            err=grib_set_values(h,options->set_values,options->set_values_count);
        }

        if (err != GRIB_SUCCESS && options->fail) exit(err);
    }

    return 0;
}

int grib_tool_skip_handle(grib_runtime_options* options, grib_handle* h)
{
    grib_handle_delete(h);
    return 0;
}

void grib_tool_print_key_values(grib_runtime_options* options,grib_handle* h)
{
    grib_print_key_values(options,h);
}

int grib_tool_finalise_action(grib_runtime_options* options)
{
    return 0;
}

int grib_no_handle_action(grib_runtime_options* options, int err)
{
    fprintf(dump_file,"\t\t\"ERROR: unreadable message\"\n");
    return 0;
}
