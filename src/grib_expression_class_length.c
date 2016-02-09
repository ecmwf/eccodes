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
   IMPLEMENTS = print
   IMPLEMENTS = compile
   IMPLEMENTS = add_dependency
   MEMBERS    = char *name
   MEMBERS    = size_t start
   MEMBERS    = size_t length
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
static void        compile(grib_expression*,grib_compiler*);
static void        add_dependency(grib_expression* e, grib_accessor* observer);
static string get_name(grib_expression* e);

static int        native_type(grib_expression*,grib_handle*);

static int        evaluate_long(grib_expression*,grib_handle*,long*);
static int      evaluate_double(grib_expression*,grib_handle*,double*);
static string evaluate_string(grib_expression*,grib_handle*,char*,size_t*,int*);

typedef struct grib_expression_length{
  grib_expression base;
/* Members defined in length */
	char *name;
	size_t start;
	size_t length;
} grib_expression_length;


static grib_expression_class _grib_expression_class_length = {
    0,                    /* super                     */
    "length",                    /* name                      */
    sizeof(grib_expression_length),/* size of instance          */
    0,                           /* inited */
    &init_class,                 /* init_class */
    0,                     /* constructor               */
    &destroy,                  /* destructor                */
    &print,                 
    &compile,                 
    &add_dependency,       

	&native_type,
	&get_name,

	&evaluate_long,
	&evaluate_double,
	&evaluate_string,
};

grib_expression_class* grib_expression_class_length = &_grib_expression_class_length;


static void init_class(grib_expression_class* c)
{
}
/* END_CLASS_IMP */

static const char* get_name(grib_expression* g)
{
    grib_expression_length* e = (grib_expression_length*)g;
    return e->name;
}

static int evaluate_long(grib_expression* g,grib_handle *h,long* result)
{
    grib_expression_length* e = (grib_expression_length*)g;
    int err=0;
    char mybuf[1024]={0,};
    size_t size=1024;
    if((err=grib_get_string_internal(h,e->name,mybuf,&size)) != GRIB_SUCCESS)
        return err;

    *result=strlen(mybuf);
    return err;
}

static int evaluate_double(grib_expression *g,grib_handle *h,double* result)
{
    grib_expression_length* e = (grib_expression_length*)g;
    char mybuf[1024]={0,};
    size_t size=1024;
    int err=0;
    if((err=grib_get_string_internal(h,e->name,mybuf,&size)) != GRIB_SUCCESS)
        return err;

    *result=strlen(mybuf);
    return err;
}

static string evaluate_string(grib_expression* g,grib_handle* h,char* buf,size_t* size,int* err)
{
    grib_expression_length* e = (grib_expression_length*)g;
    char mybuf[1024]={0,};
    Assert(buf);
    if((*err=grib_get_string_internal(h,e->name,mybuf,size)) != GRIB_SUCCESS)
        return NULL;

    sprintf(buf,"%ld",(long)strlen(mybuf));
    return buf;
}

static void print(grib_context* c,grib_expression* g,grib_handle* f)
{
    grib_expression_length* e = (grib_expression_length*)g;
    printf("access('%s",e->name);
    if(f)
    {
        long s = 0;
        grib_get_long(f,e->name,&s);
        printf("=%ld",s);
    }
    printf("')");
}

static void destroy(grib_context* c,grib_expression* g)
{
    grib_expression_length* e = (grib_expression_length*)g;
    grib_context_free_persistent(c,e->name);
}

static void  add_dependency(grib_expression* g, grib_accessor* observer)
{
    grib_expression_length* e = (grib_expression_length*)g;
    grib_accessor *observed = grib_find_accessor(grib_handle_of_accessor(observer),e->name);

    if(!observed)
    {
        /* grib_context_log(observer->context, GRIB_LOG_ERROR, */
        /* "Error in accessor_add_dependency: cannot find [%s]", e->name); */
        /* Assert(observed); */
        return;
    }

    grib_dependency_add(observer,observed);
}

grib_expression* new_length_expression(grib_context* c,const char *name)
{
    grib_expression_length* e = (grib_expression_length*)grib_context_malloc_clear_persistent(c,sizeof(grib_expression_length));
    e->base.cclass            = grib_expression_class_length;
    e->name                   = grib_context_strdup_persistent(c,name);
    return (grib_expression*)e;
}

static void compile(grib_expression* g,grib_compiler* c)
{
    grib_expression_length* e = (grib_expression_length*)g;
    fprintf(c->out,"new_length_expression(ctx,\"%s\")",e->name);
}

static int native_type(grib_expression* g,grib_handle *h)
{
    grib_expression_length* e = (grib_expression_length*)g;
    int type = 0;
    int err;
    if((err=grib_get_native_type(h,e->name,&type)) != GRIB_SUCCESS)
        grib_context_log(h->context, GRIB_LOG_ERROR,
                "Error in native_type %s : %s", e->name,grib_get_error_message(err));
    return type;
}
