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
 * C Implementation: bufr_index_build
 *
 */

#include "grib_tools.h"

char* grib_tool_description="Build an index file for a set of input BUFR files.";
char* grib_tool_name="bufr_index_build";
char* grib_tool_usage="[options] file file ... ";
grib_index* idx=NULL;
char* keys;
char* default_keys = "mars";

grib_option grib_options[]={
        /*  {id, args, help}, on, command_line, value */
        {"f",0,0,0,1,0},
        {"o:","output_index_file",
              "\n\t\tOutput is written to output_index_file."
              "\n\t\tIf an output index file is required and -o is not used, the"
              " output index is written to bufridx\n",
              1,1,"bufridx"},
        {"k:",0,0,0,1,0},
        {"V",0,0,0,1,0},
        {"T:",0,0,1,0,"B"},
        {"N",0,"Do not compress index."
             "\n\t\tBy default the index is compressed to remove keys with only one value.\n",0,1,0}
};

int compress_index;

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
    int ret=0;
    grib_context* c=grib_context_get_default();

    if (grib_options_on("N")) compress_index=0;
    else compress_index=1;

    if (grib_options_on("k:"))
        keys=grib_options_get_option("k:");
    else
        keys=default_keys;

    options->onlyfiles=1;

    idx=grib_index_new(c,keys,&ret);

    if (!idx || ret)
        grib_context_log(c,GRIB_LOG_FATAL,
                "Unable to create index %s",grib_get_error_message(ret));

    return 0;
}

int grib_tool_new_filename_action(grib_runtime_options* options,const char* file)
{
    int ret=0;
    printf("--- %s: processing %s\n",grib_tool_name,file);
    ret=_codes_index_add_file(idx,file,CODES_BUFR);
    if (ret) {printf("error: %s\n",grib_get_error_message(ret)); exit(ret);}
    return 0;
}

int grib_tool_new_file_action(grib_runtime_options* options,grib_tools_file* file)
{
    return 0;
}

int grib_tool_new_handle_action(grib_runtime_options* options, grib_handle* h)
{
    return 0;
}

int grib_tool_skip_handle(grib_runtime_options* options, grib_handle* h)
{
    return 0;
}

void grib_tool_print_key_values(grib_runtime_options* options,grib_handle* h)
{
}

int grib_tool_finalise_action(grib_runtime_options* options)
{
    grib_index_key* keys;
    grib_string_list* values;
    int first;


    if (compress_index) {
        grib_index_compress(idx);
    }
    printf("--- %s: keys included in the index file %s:\n",
            grib_tool_name,options->outfile->name);
    printf("--- ");
    first=1;
    keys=idx->keys;
    while (keys) {
        if (!first) printf(", ");
        printf("%s",keys->name);
        keys=keys->next;
        first=0;
    }
    printf("\n");
    keys=idx->keys;
    while (keys) {
        printf("--- %s = { ",keys->name);
        values=keys->values;
        first=1;
        while (values) {
            if (!first) printf(", ");
            printf("%s",values->value);
            first=0;
            values=values->next;
        }
        printf(" }\n");
        keys=keys->next;
    }
    printf("--- %d messages indexed\n",idx->count);

    if (idx->count)
        grib_index_write(idx,options->outfile->name);
    grib_index_delete(idx);
    return 0;
}

int grib_no_handle_action(grib_runtime_options* options, int err)
{
    fprintf(dump_file,"\t\t\"ERROR: unreadable message\"\n");
    return 0;
}
