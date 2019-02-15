/*
 * Copyright 2005-2018 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_api_internal.h"

/* 
   This is used by make_class.pl

   START_CLASS_DEF
   CLASS      = expression
   IMPLEMENTS = native_type
   IMPLEMENTS = destroy
   IMPLEMENTS = evaluate_string
   IMPLEMENTS = print
   IMPLEMENTS = add_dependency
   MEMBERS    = char* value
   END_CLASS_DEF

 */
/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "expression.class" and rerun ./make_class.pl

*/

typedef const char* string; /* to keep make_class.pl happy */


static void init_class              (grib_expression_class*);

static void        destroy(grib_context*,grib_expression* e);

static void        print(grib_context*,grib_expression*,grib_handle*);
static void        add_dependency(grib_expression* e, grib_accessor* observer);

static int        native_type(grib_expression*,grib_handle*);

static string evaluate_string(grib_expression*,grib_handle*,char*,size_t*,int*);

typedef struct grib_expression_sub_string{
  grib_expression base;
/* Members defined in sub_string */
	char* value;
} grib_expression_sub_string;


static grib_expression_class _grib_expression_class_sub_string = {
    0,                    /* super                     */
    "sub_string",                    /* name                      */
    sizeof(grib_expression_sub_string),/* size of instance          */
    0,                           /* inited */
    &init_class,                 /* init_class */
    0,                     /* constructor               */
    &destroy,                  /* destructor                */
    &print,                 
    &add_dependency,       

	&native_type,
	0,

	0,
	0,
	&evaluate_string,
};

grib_expression_class* grib_expression_class_sub_string = &_grib_expression_class_sub_string;


static void init_class(grib_expression_class* c)
{
}
/* END_CLASS_IMP */

static const char* evaluate_string(grib_expression* g,grib_handle *h,char* buf,size_t* size,int* err)
{
	grib_expression_sub_string* e = (grib_expression_sub_string*)g;
	*err=0;
	return e->value;
}

static void print(grib_context* c,grib_expression* g,grib_handle* f)
{
	grib_expression_sub_string* e = (grib_expression_sub_string*)g;
	printf("string('%s')",e->value);
}

static void destroy(grib_context* c,grib_expression* g)
{
	grib_expression_sub_string* e = (grib_expression_sub_string*)g;
	grib_context_free_persistent(c,e->value);
}


static void  add_dependency(grib_expression* g, grib_accessor* observer){
	/* grib_expression_sub_string* e = (grib_expression_sub_string*)g; */
}


grib_expression* new_sub_string_expression(grib_context* c,const char* value,size_t start,size_t length)
{
	char v[1024]={0,};
	grib_expression_sub_string* e = (grib_expression_sub_string*)grib_context_malloc_clear_persistent(c,sizeof(grib_expression_sub_string));

/* 	if (start<0) start+=strlen(value);  */

	memcpy(v,value+start,length);
	e->base.cclass                 = grib_expression_class_sub_string;
	e->value               = grib_context_strdup_persistent(c,v);
	return (grib_expression*)e;
}

static int native_type(grib_expression* g,grib_handle *h)
{
	return GRIB_TYPE_STRING;
}
