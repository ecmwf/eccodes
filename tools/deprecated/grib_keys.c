/*
 * Copyright 2005-2018 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

/*
 * C Implementation: grib_keys
 *
 */

#include "grib_tools.h"

grib_option grib_options[]={
/*  {id, args, help}, on, command_line, value*/
    {"T:","type","\n\t\tTo print the keys available in the given grib type."
        "\n\t\tFor a list of the available types see -L option.\n"
    ,0,1,0},
    {"F:","file","\n\t\tTo print the keys available in the grib file.\n",0,1,0},
    {"x",0,"Print the extended set of keys.\n",0,1,0},
    {"c",0,"Print only coded keys.\n",0,1,0},
    {"L",0,"List of available types.\n",0,1,0},
    {"t",0,0,0,1,0},
    {"T:",0,0,0,1,0},
    {"a",0,0,0,1,0}
};

char* grib_tool_description="Lists the keys available for a type of grib (-T option) or \n"
                 "\tin a grib message from a file (-F option).";
char* grib_tool_name="grib_keys";
char* grib_tool_usage="[options]";
char name[1024];

int grib_options_count=sizeof(grib_options)/sizeof(grib_option);

/* Print a sorted list of files in the directory 'dir' whose extension is 'extn' */
static void print_files_in_dir(const char* dir, const char* extension)
{
	int i, count = 0;
	struct dirent **files = NULL;

	/* Get a sorted directory list */
	count = scandir(dir, &files, 0, alphasort);
	if (count<0) {
		fprintf(stderr, "Could not read entries in the SAMPLES path. ");
		perror(dir);
		exit(1);
	}

	for (i=0; i<count; ++i) {
		if ( (strcmp(files[i]->d_name, ".") != 0) && (strcmp(files[i]->d_name, "..") != 0) ) {
			char buf[1024];
			char* extn = NULL; /* file extension */
			sprintf(buf,"%s", files[i]->d_name);
			extn = strrchr(buf, '.');
			if (extn && strcmp(extn, extension) == 0) {
				*extn = '\0';  /* Now buf holds the file name without the extension */
				if (strlen(buf) > 0) {
					printf("%s\n", buf);
				}
			}
		}
	}
}

int main(int argc, char *argv[]) { return grib_tool(argc,argv);}

/* Windows always has a colon in pathnames e.g. C:\temp\file. So instead we use semi-colons as delimiter */
/* in order to have multiple samples directories */
#ifdef ECCODES_ON_WINDOWS
#   define SAMPLES_PATH_DELIMITER_CHAR ';'
#   define SAMPLES_PATH_DELIMITER_STR  ";"
#else
#   define SAMPLES_PATH_DELIMITER_CHAR ':'
#   define SAMPLES_PATH_DELIMITER_STR  ":"
#endif

int grib_tool_before_getopt(grib_runtime_options* options) {
	grib_context *c=grib_context_get_default( );
	options->dump_mode=strdup("keys");
	if (grib_options_on("T:"))
	{
		grib_tools_file* infile=(grib_tools_file*)calloc(1,sizeof(grib_tools_file));
		sprintf(name,"%s",grib_options_get_option("T:"));
		infile->name=grib_external_template_path(c,name);
		infile->next=NULL;
		options->infile=infile;
	}
	else if (grib_options_on("F:"))
	{
		grib_tools_file* infile=(grib_tools_file*)calloc(1,sizeof(grib_tools_file));
		sprintf(name,"%s",grib_options_get_option("F:"));
		infile->name=name;
		infile->next=NULL;
		options->infile=infile;
	}

	if (grib_options_on("L"))
	{
		#define MAX_PLEN 8192 /* big enough to hold several dir paths */
		if (strchr(c->grib_samples_path, SAMPLES_PATH_DELIMITER_CHAR)) {
			/* Delimiter found so samples path contains multiple directories */
			char paths[MAX_PLEN];
			char* adir = NULL;
			/* Note: strtok modifies its first argument so we copy */
			strncpy(paths, c->grib_samples_path, MAX_PLEN);
			adir = strtok(paths, SAMPLES_PATH_DELIMITER_STR);
			while (adir != NULL) {
				printf("\nDirectory: %s:\n", adir);
				print_files_in_dir(adir, ".tmpl");
				adir=strtok(NULL, SAMPLES_PATH_DELIMITER_STR);
			}
		} else {
			/* No delimiter found so this is a single directory */
			print_files_in_dir(c->grib_samples_path, ".tmpl");
		}

		exit(0);
	}

	return 0;
}

int grib_tool_init(grib_runtime_options* options) {
  options->print_header=0;
  options->print_statistics=0;
  options->default_print_width=-1;

  options->dump_flags =GRIB_DUMP_FLAG_DUMP_OK;

  if (grib_options_on("x")) {
    options->dump_flags =0;
  } else if (grib_options_on("c")) {
    options->dump_flags =GRIB_DUMP_FLAG_CODED;
  }

  if (grib_options_on("a"))
      options->dump_flags |= GRIB_DUMP_FLAG_ALIASES;

  if (grib_options_on("t"))
      options->dump_flags |= GRIB_DUMP_FLAG_TYPE;

  if (!grib_options_on("T:") && !grib_options_on("F:") && !grib_options_on("A") ) {

  }

  return 0;
}

int grib_tool_new_filename_action(grib_runtime_options* options,const char* file) {
   return 0;
      }

int grib_tool_new_file_action(grib_runtime_options* options,grib_tools_file* file) {
  if (!options->current_infile->name) return 0;

  fprintf(stdout,"=================== %s \n",name);

  return 0;
}

int grib_tool_new_handle_action(grib_runtime_options* options,grib_handle* h) {
  if(grib_options_on("F:")) printf("------ %d --------\n",options->handle_count);
  grib_dump_content(h,stdout,options->dump_mode,options->dump_flags,0);
  return 0;
}

int grib_tool_skip_handle(grib_runtime_options* options, grib_handle* h) {
  grib_handle_delete(h);
  return 0;
}

void grib_tool_print_key_values(grib_runtime_options* options,grib_handle* h) {
  grib_print_key_values(options,h);
}

int grib_tool_finalise_action(grib_runtime_options* options) {
  return 0;
}

int grib_no_handle_action(int err) {
  fprintf(dump_file,"\t\t\"ERROR: unreadable message\"\n");
  return 0;
}

