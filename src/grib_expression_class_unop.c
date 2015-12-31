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
   IMPLEMENTS = evaluate_long
   IMPLEMENTS = evaluate_double
   IMPLEMENTS = print
   IMPLEMENTS = compile
   IMPLEMENTS = add_dependency
   MEMBERS    = grib_expression *exp
   MEMBERS = grib_unop_long_proc  long_func
   MEMBERS = grib_unop_double_proc  double_func
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

static int        native_type(grib_expression*,grib_handle*);

static int        evaluate_long(grib_expression*,grib_handle*,long*);
static int      evaluate_double(grib_expression*,grib_handle*,double*);

typedef struct grib_expression_unop{
  grib_expression base;
/* Members defined in unop */
	grib_expression *exp;
	grib_unop_long_proc  long_func;
	grib_unop_double_proc  double_func;
} grib_expression_unop;


static grib_expression_class _grib_expression_class_unop = {
    0,                    /* super                     */
    "unop",                    /* name                      */
    sizeof(grib_expression_unop),/* size of instance          */
    0,                           /* inited */
    &init_class,                 /* init_class */
    0,                     /* constructor               */
    &destroy,                  /* destructor                */
    &print,                 
    &compile,                 
    &add_dependency,       

	&native_type,
	0,

	&evaluate_long,
	&evaluate_double,
	0,
};

grib_expression_class* grib_expression_class_unop = &_grib_expression_class_unop;


static void init_class(grib_expression_class* c)
{
}
/* END_CLASS_IMP */

static int evaluate_long(grib_expression* g,grib_handle* h,long* lres)
{
    int ret;
    long v=0;
    grib_expression_unop* e = (grib_expression_unop*)g;
    ret = grib_expression_evaluate_long(h,e->exp,&v);
    if (ret != GRIB_SUCCESS) return ret;
    *lres=e->long_func(v);
    return GRIB_SUCCESS;
}

static int evaluate_double(grib_expression* g,grib_handle* h,double* dres)
{
    int ret;
    double v=0;
    grib_expression_unop* e = (grib_expression_unop*)g;
    ret = grib_expression_evaluate_double(h,e->exp,&v);
    if (ret != GRIB_SUCCESS) return ret;
    *dres = e->double_func ? e->double_func(v) :  e->long_func(v);
    return GRIB_SUCCESS;

}

static void print(grib_context* c,grib_expression* g,grib_handle* f)
{
    grib_expression_unop* e = (grib_expression_unop*)g;
    printf("unop(");
    grib_expression_print(c,e->exp,f);
    printf(")");
}

static void destroy(grib_context* c,grib_expression* g)
{
    grib_expression_unop* e = (grib_expression_unop*)g;
    grib_expression_free(c,e->exp);
}


static void  add_dependency(grib_expression* g, grib_accessor* observer){
    grib_expression_unop* e = (grib_expression_unop*)g;
    grib_dependency_observe_expression(observer,e->exp);
}

grib_expression* new_unop_expression(grib_context* c,
        grib_unop_long_proc long_func,
        grib_unop_double_proc double_func,
        grib_expression* exp)
{
	grib_expression_unop* e = (grib_expression_unop*)grib_context_malloc_clear_persistent(c,sizeof(grib_expression_unop));
    e->base.cclass                 = grib_expression_class_unop;
    e->exp                = exp;
    e->long_func          = long_func;
    e->double_func         = double_func;
    return (grib_expression*)e;
}

static void compile(grib_expression* g,grib_compiler* c)
{
    grib_expression_unop* e = (grib_expression_unop*)g;
    fprintf(c->out,"new_unop_expression(ctx,");
    fprintf(c->out,"%s,",grib_unop_long_proc_name(e->long_func));
    fprintf(c->out,"%s,",grib_unop_double_proc_name(e->double_func));
    grib_expression_compile(e->exp,c);
    fprintf(c->out,")");
}

static int native_type(grib_expression* g,grib_handle *h)
{
    grib_expression_unop* e = (grib_expression_unop*)g;
    return e->long_func ? GRIB_TYPE_LONG : GRIB_TYPE_DOUBLE;
}
