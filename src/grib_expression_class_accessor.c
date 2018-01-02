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
   IMPLEMENTS = destroy
   IMPLEMENTS = native_type
   IMPLEMENTS = get_name
   IMPLEMENTS = evaluate_long
   IMPLEMENTS = evaluate_double
   IMPLEMENTS = evaluate_string
   IMPLEMENTS = print
   IMPLEMENTS = add_dependency
   MEMBERS    = char *name
   MEMBERS    = long start
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
static void        add_dependency(grib_expression* e, grib_accessor* observer);
static string get_name(grib_expression* e);

static int        native_type(grib_expression*,grib_handle*);

static int        evaluate_long(grib_expression*,grib_handle*,long*);
static int      evaluate_double(grib_expression*,grib_handle*,double*);
static string evaluate_string(grib_expression*,grib_handle*,char*,size_t*,int*);

typedef struct grib_expression_accessor{
  grib_expression base;
/* Members defined in accessor */
	char *name;
	long start;
	size_t length;
} grib_expression_accessor;


static grib_expression_class _grib_expression_class_accessor = {
    0,                    /* super                     */
    "accessor",                    /* name                      */
    sizeof(grib_expression_accessor),/* size of instance          */
    0,                           /* inited */
    &init_class,                 /* init_class */
    0,                     /* constructor               */
    &destroy,                  /* destructor                */
    &print,                 
    &add_dependency,       

	&native_type,
	&get_name,

	&evaluate_long,
	&evaluate_double,
	&evaluate_string,
};

grib_expression_class* grib_expression_class_accessor = &_grib_expression_class_accessor;


static void init_class(grib_expression_class* c)
{
}
/* END_CLASS_IMP */

static const char* get_name(grib_expression* g)
{
    grib_expression_accessor* e = (grib_expression_accessor*)g;
    return e->name;
}

static int evaluate_long(grib_expression* g,grib_handle *h,long* result)
{
    grib_expression_accessor* e = (grib_expression_accessor*)g;
    return grib_get_long_internal(h,e->name,result);
}

static int evaluate_double(grib_expression *g,grib_handle *h,double* result)
{
    grib_expression_accessor* e = (grib_expression_accessor*)g;
    return grib_get_double_internal(h,e->name,result);
}

static string evaluate_string(grib_expression* g,grib_handle* h,char* buf,size_t* size,int* err)
{
    grib_expression_accessor* e = (grib_expression_accessor*)g;
    char mybuf[1024]={0,};
    long start=e->start;

    Assert(buf);
    if((*err=grib_get_string_internal(h,e->name,mybuf,size)) != GRIB_SUCCESS)
        return NULL;

    if (e->start<0) start+=*size;

    if (e->length != 0) {
        memcpy(buf,mybuf+start,e->length);
        buf[e->length]=0;
    } else {
        memcpy(buf,mybuf,*size);
        if (*size==1024) *size = *size - 1;  /* ECC-336 */
        buf[*size]=0;
    }
    return buf;
}

static void print(grib_context* c,grib_expression* g,grib_handle* f)
{
    grib_expression_accessor* e = (grib_expression_accessor*)g;
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
    grib_expression_accessor* e = (grib_expression_accessor*)g;
    grib_context_free_persistent(c,e->name);
}

static void  add_dependency(grib_expression* g, grib_accessor* observer){
    grib_expression_accessor* e = (grib_expression_accessor*)g;
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

grib_expression* new_accessor_expression(grib_context* c,const char *name,long start, size_t length)
{
    grib_expression_accessor* e = (grib_expression_accessor*)grib_context_malloc_clear_persistent(c,sizeof(grib_expression_accessor));
    e->base.cclass                 = grib_expression_class_accessor;
    e->name                   = grib_context_strdup_persistent(c,name);
    e->start                  = start;
    e->length                 = length;
    return (grib_expression*)e;
}

static int native_type(grib_expression* g,grib_handle *h)
{
    grib_expression_accessor* e = (grib_expression_accessor*)g;
    int type = 0;
    int err;
    if((err=grib_get_native_type(h,e->name,&type)) != GRIB_SUCCESS)
        grib_context_log(h->context, GRIB_LOG_ERROR,
                "Error in native_type %s : %s", e->name,grib_get_error_message(err));
    return type;
}
