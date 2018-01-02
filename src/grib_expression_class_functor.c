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
   IMPLEMENTS = evaluate_long
   IMPLEMENTS = print
   IMPLEMENTS = add_dependency
   MEMBERS    = char *name
   MEMBERS    = grib_arguments *args
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

static int        evaluate_long(grib_expression*,grib_handle*,long*);

typedef struct grib_expression_functor{
  grib_expression base;
/* Members defined in functor */
	char *name;
	grib_arguments *args;
} grib_expression_functor;


static grib_expression_class _grib_expression_class_functor = {
    0,                    /* super                     */
    "functor",                    /* name                      */
    sizeof(grib_expression_functor),/* size of instance          */
    0,                           /* inited */
    &init_class,                 /* init_class */
    0,                     /* constructor               */
    &destroy,                  /* destructor                */
    &print,                 
    &add_dependency,       

	&native_type,
	0,

	&evaluate_long,
	0,
	0,
};

grib_expression_class* grib_expression_class_functor = &_grib_expression_class_functor;


static void init_class(grib_expression_class* c)
{
}
/* END_CLASS_IMP */

static int evaluate_long(grib_expression* g,grib_handle* h,long* lres)
{
    grib_expression_functor* e = (grib_expression_functor*)g;

    /*
	TODO: needs OO code here
     */
    if(strcmp(e->name,"lookup") == 0) {
        return GRIB_SUCCESS;
    }

    if(strcmp(e->name,"new") == 0) {
        *lres=h->loader != NULL;
        return GRIB_SUCCESS;
    }

    if(strcmp(e->name,"missing") == 0)
    {
        const char *p = grib_arguments_get_name(h,e->args,0);
        if(p)
        {
            long val = 0;
            int err = 0;
            err = grib_get_long_internal(h,p,&val);
            if (err) return err;
            /* Note: This does not cope with keys like typeOfSecondFixedSurface
             * which are codetable entries with values like 255: this value is
             * not classed as 'missing'!
             * (See ECC-594)
             */
            *lres = (val == GRIB_MISSING_LONG);
            return GRIB_SUCCESS;
        }
        else
            *lres=GRIB_MISSING_LONG;
        return GRIB_SUCCESS;
    }

    if(strcmp(e->name,"defined") == 0)
    {
        const char *p = grib_arguments_get_name(h,e->args,0);

        if(p)  {
            grib_accessor* a=grib_find_accessor(h,p);
            *lres=a!=NULL ? 1 : 0;
            return GRIB_SUCCESS;
        }
        *lres=0;
        return GRIB_SUCCESS;
    }

    if(strcmp(e->name,"changed") == 0)
    {
        *lres=1;
        return GRIB_SUCCESS;
    }

    if(strcmp(e->name,"gribex_mode_on") == 0)
    {
        *lres= h->context->gribex_mode_on ? 1 : 0;
        return GRIB_SUCCESS;
    }

    return GRIB_NOT_IMPLEMENTED;
}

static void print(grib_context* c,grib_expression* g,grib_handle* f)
{
    grib_expression_functor* e = (grib_expression_functor*)g;
    printf("%s(",e->name);
    /*grib_expression_print(c,e->args,f);*/
    printf(")");
}

static void destroy(grib_context* c,grib_expression* g)
{
    grib_expression_functor* e = (grib_expression_functor*)g;
    grib_context_free_persistent(c,e->name);
    grib_arguments_free(c,e->args);
}

static void  add_dependency(grib_expression* g, grib_accessor* observer)
{
    grib_expression_functor* e = (grib_expression_functor*)g;
    if (strcmp(e->name,"defined"))
        grib_dependency_observe_arguments(observer,e->args);
}

grib_expression* new_func_expression(grib_context* c,const char *name,grib_arguments* args)
{
    grib_expression_functor* e = (grib_expression_functor*)grib_context_malloc_clear_persistent(c,sizeof(grib_expression_functor));
    e->base.cclass                 = grib_expression_class_functor;
    e->name                   = grib_context_strdup_persistent(c,name);
    e->args                  = args;
    return (grib_expression*)e;
}

static int native_type(grib_expression* g,grib_handle *h)
{
    return GRIB_TYPE_LONG;
}
