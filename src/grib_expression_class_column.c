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

/*
   This is used by make_class.pl

   START_CLASS_DEF
   CLASS      = expression
   IMPLEMENTS = init_class
   IMPLEMENTS = destroy
   IMPLEMENTS = native_type
   IMPLEMENTS = get_name
   IMPLEMENTS = evaluate_long
   IMPLEMENTS = evaluate_double
   IMPLEMENTS = evaluate_string
   MEMBERS    = grib_column* column
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

static string get_name(grib_expression* e);

static int        native_type(grib_expression*,grib_handle*);

static int        evaluate_long(grib_expression*,grib_handle*,long*);
static int      evaluate_double(grib_expression*,grib_handle*,double*);
static string evaluate_string(grib_expression*,grib_handle*,char*,size_t*,int*);

typedef struct grib_expression_column{
  grib_expression base;
/* Members defined in column */
	grib_column* column;
} grib_expression_column;


static grib_expression_class _grib_expression_class_column = {
    0,                    /* super                     */
    "column",                    /* name                      */
    sizeof(grib_expression_column),/* size of instance          */
    0,                           /* inited */
    &init_class,                 /* init_class */
    0,                     /* constructor               */
    &destroy,                  /* destructor                */
    0,                 
    0,                 
    0,       

	&native_type,
	&get_name,

	&evaluate_long,
	&evaluate_double,
	&evaluate_string,
};

grib_expression_class* grib_expression_class_column = &_grib_expression_class_column;


static void init_class(grib_expression_class* c)
{
}
/* END_CLASS_IMP */

static const char* get_name(grib_expression* g)
{
  grib_expression_column* e = (grib_expression_column*)g;
  return e->name;
}

static int evaluate_long(grib_expression* g,grib_handle *h,long* result)
{
  grib_expression_column* e = (grib_expression_column*)g;
  int err;
  if((err=grib_get_long_internal(h,e->name,result)) != GRIB_SUCCESS)
    grib_context_log(h->context, GRIB_LOG_DEBUG,
    "Error in evaluate_long %s : %s", e->name,grib_get_error_message(err));
  return err;
}

static int evaluate_double(grib_expression *g,grib_handle *h,double* result)
{
  grib_expression_column* e = (grib_expression_column*)g;
  int err;
  if((err=grib_get_double_internal(h,e->name,result)) != GRIB_SUCCESS)
    grib_context_log(h->context, GRIB_LOG_DEBUG,
    "Error in evaluate_double %s : %s", e->name,grib_get_error_message(err));
  return err;
}

static string evaluate_string(grib_expression* g,grib_handle* h,char* buf,size_t* size,int* err)
{
  grib_expression_column* e = (grib_expression_column*)g;
  Assert(buf);
  if((err=grib_get_string_internal(h,e->name,buf,size)) != GRIB_SUCCESS)
  {
    grib_context_log(h->context, GRIB_LOG_DEBUG,
    "Error in evaluate_string %s : %s", e->name,grib_get_error_message(err));
    return NULL;
    }
  return buf;
}

static void destroy(grib_context* c,grib_expression* g)
{
  grib_expression_column* e = (grib_expression_column*)g;
  grib_context_free_persistent(c,e->name);
}


grib_expression* new_column_expression(grib_context* c,const char *name)
{
  grib_expression_column* e = grib_context_malloc_clear_persistent(c,sizeof(grib_expression_column));
  e->base.cclass                 = grib_expression_class_column;
  e->name                   = grib_context_strdup_persistent(c,name);
  return (grib_expression*)e;
}

static int native_type(grib_expression* g,grib_handle *h) {
  grib_expression_column* e = (grib_expression_column*)g;
  return e->column->type;
}

