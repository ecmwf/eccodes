/*
 * Copyright 2005-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

/**************************************
 *  Enrico Fucile
 **************************************/


#include "grib_api_internal.h"
#include <ctype.h>

static int parse_in(grib_context* gc,FILE *in,const char* name,char** strvalue);

int grib_lookup_long_from_file(grib_context* gc,grib_loader* loader,const char* name,long* value)
{
	FILE       *in = (FILE*)loader->data;
  char* strvalue=0;
	int retval;

	if ((retval=parse_in(gc,in,name,&strvalue)) != GRIB_SUCCESS)
		return retval;

	if(isdigit(*strvalue))
	{
		*value = atol(strvalue);
		grib_context_log(gc,GRIB_LOG_DEBUG, "One digit string %s converted to %ld (long)", strvalue, *value);
	}
	else
	{

		*value = 0;

		while(*strvalue)
		{
			*value <<= 8;
			*value |=  *strvalue++;
		}
	  grib_context_log(gc,GRIB_LOG_DEBUG, "Value %s converted to %ld (long)", strvalue, *value);
	}
	/* TO DO free strvalue */
	return retval;
}

int grib_init_accessor_from_file(grib_loader* loader,grib_accessor* ga,grib_arguments* default_value)
{
	grib_handle* h = grib_handle_of_accessor(ga);

	FILE       *in = (FILE*)loader->data;
	int retval =GRIB_SUCCESS;
	char* strvalue=0;
	size_t len=0;
	long  lval;
	double dval;

	grib_context_log(h->context,GRIB_LOG_INFO, "Initialize  %s",   ga->name);

	if(default_value)
	{
		grib_context_log(h->context,GRIB_LOG_INFO, "Setting %s to default value", ga->name);
		grib_pack_expression(ga,grib_arguments_get_expression(h,default_value,0));
		return GRIB_SUCCESS;
	}

	if(ga->flags & GRIB_ACCESSOR_FLAG_READ_ONLY)
	{
		grib_context_log(h->context,GRIB_LOG_INFO, "Setting  %s  ignored (read only)",   ga->name);
		return GRIB_SUCCESS;
	}

	if ((retval=parse_in(h->context,in,ga->name,&strvalue)) != GRIB_SUCCESS)
		return retval;

	if (strvalue) {
		len=strlen(strvalue);

		switch(grib_accessor_get_native_type(ga))
		{
			case GRIB_TYPE_STRING:
				grib_context_log(h->context,GRIB_LOG_DEBUG, "Setting %s to string %s", ga->name, strvalue);
				grib_set_string_internal(h,ga->name,strvalue,&len);
				break;

			case GRIB_TYPE_LONG:
				lval = strtol(strvalue,NULL,10);
				grib_context_log(h->context,GRIB_LOG_DEBUG, "Setting %s to long %d", ga->name, lval);
				grib_set_long_internal(h,ga->name,lval);
				break;

			case GRIB_TYPE_DOUBLE:
				dval = atof(strvalue);
				grib_context_log(h->context,GRIB_LOG_DEBUG, "Setting %s to double %e", ga->name, dval);
				grib_set_double_internal(h,ga->name,dval);
				break;

			case GRIB_TYPE_BYTES:
				len=strlen(strvalue);
	      grib_set_bytes_internal(h,ga->name,(unsigned char*)strvalue,&len);
				break;

			case GRIB_TYPE_LABEL:
				break;

			default:
				grib_context_log(h->context,GRIB_LOG_ERROR, "Setting %s, cannot establish type %d [%s]"
						, ga->name,grib_accessor_get_native_type(ga),ga->creator->cclass->name);
				break;
		}


	}
  /* TO DO free strvalue */
	return retval;

}

static int parse_in(grib_context* gc,FILE *in,const char* name,char** strvalue) {

	char line[1024];
	char *tok=NULL;
	char escape[]="!!!";

	printf("Please enter [%s]\n",name);

	if (!fgets(line,sizeof(line)-1,in)) {
		grib_context_log(gc,GRIB_LOG_DEBUG, "End of file reached");
		return GRIB_END_OF_FILE;
	}

  line[strlen(line)-1] = 0;
	grib_context_log(gc,GRIB_LOG_DEBUG, "Parsing: \"%s\"",line);

	if (strncmp(line,escape,strlen(escape))==0) {
		grib_context_log(gc,GRIB_LOG_DEBUG, "Escape %s matched",escape);
		return GRIB_END_OF_FILE;
	}

	if (*line) {
		tok=strtok(line," =");
	}

	if (!tok || *tok == '#' || strcmp(tok,name)!=0) {
		parse_in(gc,in,name,strvalue);
	}
	else
	{
		tok=strtok(NULL," =[");

		if (!tok) {
			parse_in(gc,in,name,strvalue);
		} else {
			*strvalue=tok;
			grib_context_log(gc,GRIB_LOG_DEBUG, "Value parsed as string: \"%s\"",tok);
		}

	}

	return GRIB_SUCCESS;
}
