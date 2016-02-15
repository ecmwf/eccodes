/*
 * Copyright 2005-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#ifndef grib_expression_H
#define grib_expression_H

#include "grib_api_internal.h"


typedef int        (*expression_evaluate_long_proc)(grib_expression*,grib_handle*,long*);
typedef int      (*expression_evaluate_double_proc)(grib_expression*,grib_handle*,double*);
typedef const char* (*expression_evaluate_string_proc)(grib_expression*,grib_handle*,char*,size_t*,int*);
typedef const char* (*expression_get_name_proc)(grib_expression*);

typedef void        (*expression_print_proc)(grib_context*,grib_expression*,grib_handle*);
typedef void        (*expression_compile_proc)(grib_expression*,grib_compiler*);
typedef void        (*expression_add_dependency_proc) (grib_expression*e, grib_accessor* observer );

typedef struct grib_expression_class grib_expression_class;

typedef void        (*expression_class_init_proc) (grib_expression_class*e);
typedef void        (*expression_init_proc) (grib_expression*e);
typedef void        (*expression_destroy_proc) (grib_context*,grib_expression*e);

typedef int        (*expression_native_type_proc)(grib_expression*,grib_handle*);

struct grib_expression {
	grib_expression_class* cclass;
};

struct grib_expression_class {
	
	grib_expression_class **super;
	const char *name;
	size_t size;
	int inited;

	expression_class_init_proc init_class;
	expression_init_proc init;
	expression_destroy_proc destroy;


	expression_print_proc              print;
    expression_compile_proc            compile;
	expression_add_dependency_proc     add_dependency;

	expression_native_type_proc         native_type;
	expression_get_name_proc			get_name;

	expression_evaluate_long_proc      evaluate_long;
	expression_evaluate_double_proc    evaluate_double;
	expression_evaluate_string_proc    evaluate_string;
};


struct grib_arguments {
	struct grib_arguments *next;
	grib_expression       *expression;
	char value[80];
};


long grib_expression_evaluate(grib_handle*,grib_expression*);
void grib_expression_free(grib_context*,grib_expression*);

grib_arguments* grib_arguments_new(grib_context*,grib_expression*,grib_arguments*);
void grib_arguments_free(grib_context*,grib_arguments*);

const char* grib_argument_next(grib_arguments**);

#endif
