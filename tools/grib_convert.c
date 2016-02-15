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
 * C Implementation: grib_convert
 *
 */

#include "grib_tools.h"


grib_option grib_options[]={
  /*  {id, args, help}, on, command_line, value*/
  {"f",0,0,0,1,0},
  {"F",0,0,1,0,0},
  {"q",0,0,1,0,0},
  {"M",0,0,0,1,0},
  {"U",0,0,1,0,0},
  {"I",0,0,1,0,0},
  {"g",0,0,0,1,0},
  {"G",0,0,0,1,0},
  {"T",0,0,0,1,0},
  {"V",0,0,0,1,0},
  {"7",0,0,0,1,0},
  {"v",0,0,0,1,0}
};

char* grib_tool_description=
"It converts grib messages applying the rules from a conversion_rules file."
"\n\tThe rules are of the type \"keyname = value;\" and if blocks are allowed as"
"\n\tif ( keyname1 == value1 || keyname2 != value2 && keyname3 == value3 ) {"
"\n\t keyname4 = value4;"
"\n\t}";
char* grib_tool_name="grib_convert";
char* grib_tool_usage="[options] conversion_rules "
"grib_file grib_file ... output_grib_file";

int grib_options_count=sizeof(grib_options)/sizeof(grib_option);

int main(int argc, char *argv[]) { return grib_tool(argc,argv);}

int grib_tool_before_getopt(grib_runtime_options* options) {
  return 0;
}

int grib_tool_init(grib_runtime_options* options) {
  int set=0;
  grib_rule* r=NULL;

  options->rules = grib_parse_rules_file(NULL,options->infile_extra->name);

  if(!options->rules) {
    printf("Unable to build rules from %s\n",options->infile_extra->name);
    exit(1);
  }
  r = options->rules;
  set = 0;
  while(r)  {
    if(!r->condition) {
      grib_rule_entry* e = r->entries;
      while(e)  {
        Assert(set < (sizeof(options->set_values)/sizeof(options->set_values[0])));
        options->set_values[set].name = e->name;
        GRIB_CHECK_NOLINE(grib_expression_set_value(NULL,e->value,&(options->set_values[set])),NULL);
        set++;
        e = e->next;
      }
    }
    r = r->next;
  }
  options->set_values_count=set;

  if(options->verbose)
  {
    printf("Fixed rules:\n");
    printf("------------\n");
    grib_print_values(options->set_values,
        options->set_values_count);
    printf("\n");
  }
/*
  if (options->outfile && options->outfile->name) {
    options->outfile->file = fopen(options->outfile->name,"w");
    if(!options->outfile->file) {
      perror(options->outfile->name);
      exit(1);
    }
  }
*/
  return 0;
}

int grib_tool_new_filename_action(grib_runtime_options* options,const char* file) {
   return 0;
}

int grib_tool_new_file_action(grib_runtime_options* options,grib_tools_file* file) {
  return 0;
}

int grib_tool_new_handle_action(grib_runtime_options* options, grib_handle* h) {
  grib_rule* r = options->rules;
  int i;
  int err=0;
  double scale = 1;
  double offset = 0;

  int set = options->set_values_count;

  while(r) {
    if(r->condition)  {
      long ok = 0;
      GRIB_CHECK_NOLINE(grib_expression_evaluate_long(h,r->condition,&ok),NULL);
      if(ok) {
        grib_rule_entry* e = r->entries;
        while(e) {
          if (!strcmp(e->name,"skip")) return 0;
          Assert(set < (sizeof(options->set_values)/sizeof(options->set_values[0])));
          options->set_values[set].name = e->name;
          options->set_values[set].error=grib_expression_set_value(h,
                                              e->value,&(options->set_values[set]));
          set++;
          e = e->next;
        }

      }
    }
    r = r->next;
  }

  if(options->verbose)
  {
    printf("Applying %d:\n",options->infile->handle_count);
    grib_print_values(options->set_values, set);
    printf("\n");
  }

  err=grib_set_values(h,options->set_values, set);

  if( err != GRIB_SUCCESS) {
      for(i = 0; i < options->set_values_count ; i++) {
        if(options->set_values[i].error != GRIB_SUCCESS && options->fail)
           grib_context_log(h->context,GRIB_LOG_ERROR,"unable to set key \"%s\" (%s)",
              options->set_values[i].name,grib_get_error_message( options->set_values[i].error));
      }
	  grib_context_log(h->context,GRIB_LOG_ERROR,"error processing message %d\n",options->infile->handle_count);
	  if (options->fail) exit(err);
  }

  grib_get_double(h,"scaleValuesBy",&scale);
  grib_get_double(h,"offsetValuesBy",&offset);

  if(scale != 1 || offset != 0)
  {
    double *v;
    size_t size;
    int i;

    GRIB_CHECK_NOLINE(grib_get_size(h,"values",&size),0);

    v    = (double*)calloc(size,sizeof(double));
    if(!v) {
      fprintf(stderr,"failed to allocate %ld bytes\n",(long)(size*sizeof(double)));
      exit(1);
    }

    GRIB_CHECK_NOLINE(grib_get_double_array(h,"values",v,&size),0);

    if(options->verbose)
    {
      printf("Applying scale=%g/offset=%g\n",scale,offset);
    }

    for(i = 0; i< size; i++)
      v[i] =  v[i] * scale + offset;

    GRIB_CHECK_NOLINE(grib_set_double_array(h,"values",v,size),0);
    free(v);
  }

  grib_tools_write_message(options,h);

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

