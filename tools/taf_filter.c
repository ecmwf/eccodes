/*
 * Copyright 2005-2018 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_tools.h"

grib_option grib_options[]={
        /*  {id, args, help}, on, command_line, value */
        {"f",0,0,0,1,0},
        {"f",0,0,1,0,0},
        {"F",0,0,1,0,0},
        {"o:",0,0,1,1,"filter.out"},
        {"q",0,0,1,0,0},
        {"M",0,0,0,1,0},
        {"I",0,0,1,0,0},
        {"V",0,0,0,1,0},
        {"g",0,0,0,1,0},
        {"G",0,0,0,1,0},
        {"T:",0,0,1,0,"F"},
        {"7",0,0,0,1,0},
        {"v",0,0,0,1,0}
};
char* grib_tool_description="Apply the rules defined in rules_file to each TAF "
        "message\n\tin the files provided as arguments.";
char* grib_tool_name="taf_filter";
char* grib_tool_usage="[options] rules_file "
        "file file ...";
int fail=0;
int grib_options_count=sizeof(grib_options)/sizeof(grib_option);

int main(int argc, char *argv[])
{
    return grib_tool(argc,argv);
}

int grib_tool_before_getopt(grib_runtime_options* options)
{
    return 0;
}

int grib_tool_init(grib_runtime_options* options)
{
    options->action = grib_action_from_filter(options->infile_extra->name);
    if (!options->action) {
        fprintf(stderr,"%s: error unable to create action\n",options->infile_extra->name);
        exit(1);
    }

    if ( options->outfile && options->outfile->name )
        options->action->context->outfilename=options->outfile->name;

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

int grib_tool_new_handle_action(grib_runtime_options* options, grib_handle* h)
{
    int err=0;

    if (options->current_infile->name) {
        size_t len=strlen(options->current_infile->name);
        grib_set_string(h,"file",options->current_infile->name,&len);
    }

    err=grib_handle_apply_action(h,options->action);
    if (err != GRIB_SUCCESS && options->fail && err!=GRIB_NOT_FOUND) {
        printf("ERROR: %s\n",grib_get_error_message(err));
        exit(err);
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
    grib_file_pool_clean();
    return 0;
}

int grib_no_handle_action(grib_runtime_options* options, int err)
{
    fprintf(dump_file,"\t\t\"ERROR: unreadable message\"\n");
    return 0;
}
