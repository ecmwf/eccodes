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
 * C Implementation: grib_ls
 *
 * Author: Enrico Fucile <enrico.fucile@ecmwf.int>
 *
 *
 */
#include "grib_tools.h"

grib_option grib_options[]={
/*  {id, args, help}, on, command_line, value*/
    {"f",0,0,1,0,0},
    {"p:",0,0,0,1,0},
    {"F:",0,0,1,1,"%g"},
    {"P:",0,0,0,1,0},
    {"w:",0,0,0,1,0},
	{"j",0,"json output\n",0,1,0},
    {"B:",0,0,0,1,0},
    {"l:",0,0,0,1,0},
	{"s:",0,0,0,1,0},
    {"i:",0,0,0,1,0},
    {"n:",0,0,1,1,"ls"},
    {"m",0,0,0,1,0},
    {"V",0,0,0,1,0},
    {"W:",0,0,1,1,"10"},
    {"S",0,0,1,0,0},
    {"M",0,0,0,1,0},
    {"H",0,0,1,0,0},
    {"g",0,0,0,1,0},
    {"P",0,0,1,0,0},
    {"T:",0,0,1,0,"M"},
    {"7",0,0,0,1,0},
    {"v",0,0,1,0,0},
    {"x",0,0,0,1,0}
};

char* grib_tool_description="List content of metar files printing values of "
                  "some keys.\n\tIt does not fail when a key is not found.";
char* grib_tool_name="metar_ls";
char* grib_tool_usage="[options] file file ...";
static char* new_handle="";

int grib_options_count=sizeof(grib_options)/sizeof(grib_option);
double lat=0;
double lon=0;
int mode=0;
static int json=0;

grib_nearest* n=NULL;
/*double *outlats,*outlons,*values,*lsm_values,*distances;*/

int main(int argc, char *argv[]) { return grib_tool(argc,argv);}

/*
This is executed before processing the options with i
getopt and therfore it is the right place for hacking 
the arguments if needed
*/
int grib_tool_before_getopt(grib_runtime_options* options) {
  return 0;
}

/*
The options have been parsed and the structure 
grib_runtime_options* options has been loaded. 
Initialization and startup can be done here
*/
int grib_tool_init(grib_runtime_options* options) {
  char  *end = NULL, *end1=NULL;
  size_t size=4;
  int ret=0;
  double min=0,max=0;
  int i=0;
  char* p=NULL;
  if (grib_options_on("j")) {
	options->verbose=0;
	json=1;
  }
  
  if (options->latlon) {
	  
    lat = strtod(options->latlon,&end);
    if (*end != ',') {
      printf("ERROR: wrong latitude value\n");
      exit(1);
    }
    lon= strtod(++end,&end1);
    
    mode=GRIB_NEAREST_SAME_POINT | GRIB_NEAREST_SAME_GRID;

    if (end1 && *end1 == ',') {
      end1++;
      if (*end1 != '0') {
        p=end1;
        while (*p != ',' && *p !='\0') p++;
        if (*end1 == '4') {
            options->latlon_mode=4;
        } else if (*end1 == '1') {
            options->latlon_mode=1;
        } else {
          printf("ERROR %s: wrong mode given in option -l\n",grib_tool_name);
          exit(1);
        }
      }
      if (*p == ',') {
        p++;
        options->latlon_mask=strdup(p);
      }
    }

  }

  if (options->latlon && options->latlon_mask) {
    FILE* f=NULL;
    grib_handle* hh;
    f=fopen(options->latlon_mask,"r");
    if(!f) {
      perror(options->latlon_mask);
      exit(1);
    }
    hh=grib_handle_new_from_file(0,f,&ret);
    fclose(f);
    GRIB_CHECK_NOLINE(ret,0);
    n=grib_nearest_new(hh,&ret);
    GRIB_CHECK_NOLINE(ret,0);
    GRIB_CHECK_NOLINE(grib_nearest_find(n,hh,lat,lon,mode,
        options->lats,options->lons,options->mask_values,options->distances,options->indexes,&size),0);
    grib_nearest_delete(n);
    n=NULL;
    grib_handle_delete( hh);

    options->latlon_idx=-1;
    max=options->distances[0];
    for (i=0;i<4;i++)
      if (max<options->distances[i]) {max=options->distances[i];}
    min=max;
    for (i=0;i<4;i++) {
      if ((min >= options->distances[i]) && (options->mask_values[i] >= 0.5)) {
        options->latlon_idx=i;
        min = options->distances[i];
      }
    }

    if (options->latlon_idx<0){
      min=0;
      options->latlon_idx=0;
      for (i=1;i<4;i++)
        if (min>options->distances[i]) {
          min = options->distances[i];
          options->latlon_idx=i;
        }
    }
  }
  if (json) printf("[\n");

  return 0;
}

/*
A new file is being parsed. The file name is file. This function is called every time
a new input file name is processed, before opening the file.
*/
int grib_tool_new_filename_action(grib_runtime_options* options,const char* file) {
   return 0;
   }


int grib_tool_new_file_action(grib_runtime_options* options,grib_tools_file* file) {
   return 0;
}

static void print_key_values(grib_runtime_options* options,grib_handle* h) {
	int i;
	int ret=0;
	char* s="\"keys\" : {";
	double dvalue=0;
	long lvalue=0;
	char value[MAX_STRING_LEN];
	size_t len=MAX_STRING_LEN;
	for (i=0;i<options->print_keys_count;i++) {
		ret=GRIB_SUCCESS;
		printf("%s",s);
		len=MAX_STRING_LEN;
		printf("\"%s\" : ",options->print_keys[i].name);
		if (grib_is_missing(h,options->print_keys[i].name,&ret) && ret==GRIB_SUCCESS)
			printf("\"missing\"");
		else if ( ret == GRIB_SUCCESS ) {
			if (options->print_keys[i].type == GRIB_TYPE_UNDEFINED)
				grib_get_native_type(h,options->print_keys[i].name,&(options->print_keys[i].type));
			switch (options->print_keys[i].type) {
				case GRIB_TYPE_STRING:
					ret=grib_get_string( h,options->print_keys[i].name,value,&len);
					printf("\"%s\"",value);
					break;
				case GRIB_TYPE_DOUBLE:
					ret=grib_get_double( h,options->print_keys[i].name,&dvalue);
					printf("%g",dvalue);
					break;
				case GRIB_TYPE_LONG:
					ret=grib_get_long( h,options->print_keys[i].name,&lvalue);
					printf("%ld",lvalue);
					break;
				default:
					printf("invalid_type");
			}
		}
		if (ret == GRIB_NOT_FOUND) printf("null");
		s=", ";
	}
	printf("}");
}

/* A new handle is available from the current input file and can be processed here.
The handle available in this function is in the set of messages satisfying the constrant of the
-w option. They are not to be skipped.
*/
int grib_tool_new_handle_action(grib_runtime_options* options, grib_handle* h) {
  size_t size=4;
  double v=0;
  int err=0;
  int i;
  
  if (!options->skip) {

	  if (options->set_values_count != 0)
		  err=grib_set_values(h,options->set_values,options->set_values_count);

	  if( err != GRIB_SUCCESS && options->fail) exit(err);
  }
  
  if (options->latlon) {
    err=0;
	double min;
    if (!n) n=grib_nearest_new(h,&err);
    GRIB_CHECK_NOLINE(err,0);
    GRIB_CHECK_NOLINE(grib_nearest_find(n,h,lat,lon,0,
        options->lats,options->lons,options->values,
        options->distances,options->indexes,&size),0);

	if (!options->latlon_mask) {
		min=options->distances[0];
		options->latlon_idx=0;
		i=0;
		for (i=1;i<4;i++) {
			if (min>options->distances[i]) {
					min=options->distances[i];
					options->latlon_idx=i;
			}
		}
	}
	
	if (json) {
		char* s="\n[\n";
		double missingValue=9999;
		char value[MAX_STRING_LEN];
		size_t len=MAX_STRING_LEN;
		printf("%s",new_handle);
		printf(	"{\n");
		print_key_values(options,h);
		printf("\n, \"selected\" : %d",options->latlon_idx);
		printf(", \"method\" : ");
		if (options->latlon_mask) printf("\"nearest_land\"");
		else printf("\"nearest\"");
		printf("\n, \"neighbours\" : ");
		for (i=0;i<4;i++) {
			printf("%s",s);
			len=MAX_STRING_LEN;
			printf("{\"index\" : %d, \"latitude\" : %g, \"longitude\" : %g, \"distance\" : %g, "
				   "\"distance_unit\" : \"km\", ",
	   				(int)options->indexes[i],options->lats[i],options->lons[i],
					options->distances[i]);
			if (grib_get_double_element(h,"values",options->indexes[i],&v) == GRIB_SUCCESS) {
				if (v==missingValue) printf("\"value\" : null ");
				else printf("\"value\" : %g ",v);
			}

			if (grib_get_string( h,"units",value,&len)==GRIB_SUCCESS)
				printf(", \"unit\" : \"%s\"",value);
			
			if (options->latlon_mask) 
				printf(", \"mask_value\"=%.2f",options->mask_values[i]);
			printf("}");
			s="\n,";
		}

		printf("\n]");
		printf("\n}");
	}

  }
  new_handle="\n,";
  return 0;
}

/* A new handle to skip is available. At this point something can be done
with the message to be skipped before deleting the handle. */
int grib_tool_skip_handle(grib_runtime_options* options, grib_handle* h) {
  grib_handle_delete(h);
  return 0;
}

/* key values can be printed in this function. Headers are already printed if requested.*/
void grib_tool_print_key_values(grib_runtime_options* options,grib_handle* h) {
  grib_print_key_values(options,h);
}

/* this is executed after the last message in the last file is processed */
int grib_tool_finalise_action(grib_runtime_options* options) {
  int i=0;
  if (options->latlon && options->verbose) {
 
		printf("Input Point: latitude=%.2f  longitude=%.2f\n",lat,lon);
		printf("Grid Point chosen #%d index=%d latitude=%.2f longitude=%.2f distance=%.2f (Km)\n",
			   options->latlon_idx+1,(int)options->indexes[options->latlon_idx],
									  options->lats[options->latlon_idx],
		   	options->lons[options->latlon_idx],
	 		options->distances[options->latlon_idx]);

		if (options->latlon_mask) {
			printf("Mask values:\n");
			for (i=0;i<4;i++) {
				printf("- %d - index=%d latitude=%.2f longitude=%.2f distance=%.2f (Km) value=%.2f\n",
					   i+1,(int)options->indexes[i],options->lats[i],options->lons[i],
							options->distances[i],options->mask_values[i]);
			}
		} else {
			printf("Other grid Points\n");
			for (i=0;i<4;i++) {
				printf("- %d - index=%d latitude=%.2f longitude=%.2f distance=%.2f (Km)\n",
					   i+1,(int)options->indexes[i],options->lats[i],options->lons[i],
							options->distances[i]);
			}
		}
	}

    if (n) grib_nearest_delete(n);
	if (json) printf("\n]\n");
  
  return 0;
}

int grib_no_handle_action(grib_runtime_options* options, int err)
{
    fprintf(dump_file,"\t\t\"ERROR: unreadable message\"\n");
    return 0;
}
