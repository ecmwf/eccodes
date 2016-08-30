/*
 * Copyright 2005-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_api_internal.h"
#include <stdio.h>
#ifndef ECCODES_ON_WINDOWS
#  include <unistd.h>
#endif
#include <string.h>

#ifndef GRIB_TOOLS_H
#define GRIB_TOOLS_H

#ifndef S_IFMT
#define S_IFMT        0170000 /*  type of file */
#endif

#ifndef S_IFLNK
#define S_IFLNK  0120000 /*  symbolic link */
#endif

#ifndef S_ISLNK
#define S_ISLNK(mode) (((mode) & (S_IFMT)) == (S_IFLNK))
#endif

#define MAX_KEYS 1000
#define MAX_STRING_LEN  512
#define MAX_FAILED  1024
#define MAX_CONSTRAINT_VALUES 500

#define MODE_GRIB 	      0
#define MODE_GTS          1
#define MODE_BUFR         2
#define MODE_METAR        3
#define MODE_TAF          5
#define MODE_ANY          6

typedef union grib_typed_value {
  long*        long_value;
  double*      double_value;
  const char* string_value;
} grib_typed_value;

/*
typedef struct grib_constraints {
  const char* name;
  int         type;
  grib_typed_value values[MAX_CONSTRAINT_VALUES];
  int         count;
  int         error;
  int         equal;
} grib_constraints;
*/

typedef struct grib_options_help {
  char* id;
  char* args;
  char* help;
} grib_options_help;


typedef struct grib_option {
  char* id;
  char* args;
  char* help;
  int on;
  int command_line;
  char* value;
} grib_option;

typedef struct grib_failed grib_failed;

struct grib_failed {
   int count;
   int error;
   grib_failed* next;
};

typedef struct grib_tools_file grib_tools_file;

struct grib_tools_file {
   FILE* file;
   char* name;
   int handle_count;
   int filter_handle_count;
   grib_failed* failed;
   grib_tools_file* next;
};

typedef struct grib_runtime_options {
  int verbose;
  int fail;
  int skip;
  int default_print_width;
  int print_header;
  char* name_space;
  int print_number;
  int print_statistics;
  grib_values requested_print_keys[MAX_KEYS];
  int requested_print_keys_count;
  grib_values print_keys[MAX_KEYS];
  int print_keys_count;
  int strict;
  int multi_support;
  int set_values_count;
  grib_values set_values[MAX_KEYS];
  grib_values constraints[MAX_KEYS];
  int constraints_count;
  grib_values compare[MAX_KEYS];
  int compare_count;
  int handle_count;
  int filter_handle_count;
  int file_count;
  grib_tools_file* infile_extra;
  grib_tools_file* current_infile;
  grib_tools_file* infile;
  grib_tools_file* outfile;
  grib_action* action;
  grib_rule* rules;
  int dump_flags;
  char* dump_mode;
  int repack;
  int error;
  int gts;
  char* orderby;
  char* latlon;
  double lats[4];
  double lons[4];
  double values[4];
  double distances[4];
  int indexes[4];
  int latlon_mode;
  char* latlon_mask;
  int latlon_idx;
  double mask_values[4];
  int index;
  int index_on;
  double constant;
  char* dump_filename;
  grib_fieldset* idx;
  int random;
  char* format;
  int onlyfiles;
  int tolerance_count;
  int through_index;
  grib_index* index1;
  grib_index* index2;
  grib_context* context;
  int stop;
  int mode;
  int headers_only;
  int skip_all;
  grib_values tolerance[MAX_KEYS];
  off_t infile_offset;
} grib_runtime_options;

extern grib_option grib_options[];
extern int grib_options_count;
extern char* grib_tool_name;
extern char* grib_tool_description;
extern char* grib_tool_usage;

extern FILE* dump_file;

char* grib_options_get_option(const char* id);
int grib_options_on(const char* id);
int grib_options_get(int argc,char** argv);
int grib_options_get_values(int argc,char** argv,int values_required,int default_type, grib_values values[],int* n,int* optind);
int grib_tool(int argc, char **argv);
char* grib_options_get_help(char* id);
char* grib_options_get_args(char* id);
int grib_options_command_line(const char* id);
void usage();
void usage_doxygen();
int grib_tool_before_getopt(grib_runtime_options* options);
int grib_tool_init(grib_runtime_options* options);
int grib_tool_new_file_action(grib_runtime_options* options,grib_tools_file* file);
int grib_tool_new_handle_action(grib_runtime_options* options, grib_handle* h);
int grib_tool_skip_handle(grib_runtime_options* options, grib_handle* h);
void grib_tool_print_key_values(grib_runtime_options* options,grib_handle* h);
int grib_tool_finalise_action(grib_runtime_options* options);
void grib_skip_check(grib_runtime_options* options,grib_handle* h);
void grib_print_key_values(grib_runtime_options* options,grib_handle* h);
void grib_print_file_statistics(grib_runtime_options* options,grib_tools_file* file);
void grib_print_full_statistics(grib_runtime_options* options);
int grib_get_runtime_options(int argc,char** argv,grib_runtime_options* options);
int grib_process_runtime_options(grib_context* c,int argc,char** argv,grib_runtime_options* options);
void grib_tools_write_message(grib_runtime_options* options, grib_handle* h);
int grib_tool_new_filename_action(grib_runtime_options* options,const char* file); 
int grib_no_handle_action(grib_runtime_options* options,int err);

#endif

