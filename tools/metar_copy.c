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

char* grib_tool_description="Copies the content of METAR files printing"
        " values of some keys.";
char* grib_tool_name="metar_copy";
char* grib_tool_usage="[options] file file ... output_file";

grib_option grib_options[]={
        /*  {id, args, help}, on, command_line, value */
        {"f",0,0,0,1,0},
        {"c",0,0,1,0,0},
        {"q",0,0,1,0,0},
        {"p:",0,0,1,1,0},
        {"P:",0,0,0,1,0},
        {"w:","key[:{s|d|i}]=value,key[:{s|d|i}]=value,...","\n\t\tWhere clause."
                "\n\t\tOnly messages matching the key/value constraints are "
                "copied to the\n\t\toutput_grib_file.\n\t\tFor each key a string (key:s), a "
                "double (key:d) or an integer (key:i)\n\t\ttype can be defined. Default type "
                "is string.\n",0,1,0},
       {"B:",0,0,0,1,0},
       {"V",0,0,0,1,0},
       {"W:",0,0,0,1,0},
       {"U",0,0,1,0,0},
       {"H",0,0,1,0,0},
       {"T:",0,0,1,0,"M"},  /* METAR */
       {"S",0,0,1,0,0},
       {"g",0,0,0,1,0},
       {"7",0,0,0,1,0},
       {"v",0,0,0,1,0}
};

int grib_options_count=sizeof(grib_options)/sizeof(grib_option);

int main(int argc, char *argv[])
{
    int ret=grib_tool(argc,argv);
    return ret;
}

int grib_tool_before_getopt(grib_runtime_options* options)
{
    return 0;
}

int grib_tool_init(grib_runtime_options* options)
{
#if 0
    if (options->outfile && options->outfile->name) {
        options->outfile->file = fopen(options->outfile->name,"w");
        if(!options->outfile->file) {
            perror(options->outfile->name);
            exit(1);
        }
    }
#endif
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
    if (!options->skip) {
        if (options->set_values_count != 0)
            err=grib_set_values(h,options->set_values,options->set_values_count);

        if( err != GRIB_SUCCESS && options->fail) exit(err);
    }

    grib_tools_write_message(options,h);
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
    /*
  if (options->outfile->file) {
    fclose(options->outfile->file);
  }
     */
    return 0;
}

int grib_no_handle_action(grib_runtime_options* options, int err)
{
    fprintf(dump_file,"\t\t\"ERROR: unreadable message\"\n");
    return 0;
}
