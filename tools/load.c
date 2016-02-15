/*
 * Copyright 2005-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>


#include "grib_api_internal.h"
#include "loady.h"
#include "load.h"

FILE* fout;

extern FILE* load_in;
extern int load_parse();
extern int yylineno;

static grib_handle *h = 0;

static double* double_value = 0;
static size_t double_value_size = 0;
static size_t double_value_count = 0;

static long* long_value = 0;
static size_t long_value_size = 0;
static size_t long_value_count = 0;

static int err;

const char *out_file;

static void check(int code,const char* name)
{
	if(code) {
		printf("%s: cannot set %s: %s\n",prog,name,grib_get_error_message(code));
		err++;
	}
}

static grib_handle* handle()
{
	if(h == 0)
		h = grib_handle_new_from_samples(NULL,"GRIB1");
	/* grib_dump_content(h,stdout,"debug",~0,NULL); */
	return h;
}

void load_finish() { 
	const void* buffer;
	size_t size;
	assert(h);
	GRIB_CHECK(grib_get_message(h,&buffer,&size),"");

	printf("%s: writing %ld bytes message\n",prog,(long)size);

	if(fwrite(buffer,1,size,fout) != size)
	{
		perror(out_file);
		exit(1);
	}
	grib_handle_delete(h);
	h = NULL;
}

void load_missing(const char* name)
{ 
	printf("grib_set_missing(%s)\n",name);
	check(grib_set_missing(handle(),name),name);
}

void load_long(const char* name,long value) 
{ 
	printf("grib_set_long(%s,%ld)\n",name,value);
	check(grib_set_long(handle(),name,value),name);  
}

void load_string(const char* name,const char* value) 
{
	size_t s = strlen(value);
	printf("grib_set_string(%s,%s)\n",name,value);
	check(grib_set_string(handle(),name,value,&s),name);
}

void load_double(const char* name,double value) 
{
	printf("grib_set_double(%s,%g)\n",name,value);
	check(grib_set_double(handle(),name,value),name);  
}

void load_start_array() {

	if(double_value == NULL)
	{
		double_value_size = 100000;
		double_value = malloc(double_value_size*sizeof(double));
	}

	if(long_value == NULL)
	{
		long_value_size = 100000;
		long_value = malloc(long_value_size*sizeof(long));
	}

	double_value_count = 0;
	long_value_count   = 0;

}


void load_end_array(const char* name) 
{
	if(double_value_count == long_value_count)
	{
		printf("grib_set_long_array(%s,%ld)\n",name,(long)long_value_count);
		check(grib_set_long_array(handle(),name,long_value,long_value_count),name);
	}
	else
	{
		printf("grib_set_double_array(%s,%ld)\n",name,(long)double_value_count);
		check(grib_set_double_array(handle(),name,double_value,double_value_count),name);  
	}
}


void load_double_value(double value) {

	if(double_value_count == double_value_size)
	{
		double_value_size += 100000;
		double_value  = realloc(double_value,double_value_size*sizeof(double));
	}

	double_value[double_value_count++] = value;

}

void load_long_value(long value) {

	if(long_value_count == long_value_size)
	{
		long_value_size += 100000;
		long_value  = realloc(long_value,long_value_size*sizeof(long));
	}

	long_value[long_value_count++] = value;

	load_double_value(value);


}



int load_wrap() { return 1; }
int load_error(const char* msg) { printf("%s line %d\n",msg,yylineno); err = 1; return 1; }



int load_file(const char* in,const char* out)
{
	err = 0;
	fout = fopen(out,"w");
	if(!fout) {
		perror(out);
		return 1;
	}

	load_in = fopen(in,"r");
	if(!load_in) {
		perror(in);
		return 1;
	}

	load_parse();

	fclose(load_in);
	if(fclose(fout))
	{
		perror(out);
		return 1;
	}

	return err;
}
