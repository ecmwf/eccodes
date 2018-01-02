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
   IMPLEMENTS = init_class
   IMPLEMENTS = destroy
   IMPLEMENTS = native_type
   IMPLEMENTS = evaluate_long
   IMPLEMENTS = evaluate_double
   IMPLEMENTS = print
   IMPLEMENTS = add_dependency
   MEMBERS    = grib_expression *left
   MEMBERS = grib_expression *right
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
static int      evaluate_double(grib_expression*,grib_handle*,double*);

typedef struct grib_expression_logical_and{
  grib_expression base;
/* Members defined in logical_and */
	grib_expression *left;
	grib_expression *right;
} grib_expression_logical_and;


static grib_expression_class _grib_expression_class_logical_and = {
    0,                    /* super                     */
    "logical_and",                    /* name                      */
    sizeof(grib_expression_logical_and),/* size of instance          */
    0,                           /* inited */
    &init_class,                 /* init_class */
    0,                     /* constructor               */
    &destroy,                  /* destructor                */
    &print,                 
    &add_dependency,       

	&native_type,
	0,

	&evaluate_long,
	&evaluate_double,
	0,
};

grib_expression_class* grib_expression_class_logical_and = &_grib_expression_class_logical_and;


static void init_class(grib_expression_class* c)
{
}
/* END_CLASS_IMP */

static int evaluate_long(grib_expression *g,grib_handle* h,long* lres)
{
    long v1=0;
    long v2=0;
    double dv1=0;
    double dv2=0;
    int ret;
    grib_expression_logical_and* e = (grib_expression_logical_and*)g;


    switch (grib_expression_native_type(h, e->left)) {
      case GRIB_TYPE_LONG:
        ret = grib_expression_evaluate_long(h,e->left,&v1);
        if (ret != GRIB_SUCCESS) return ret;
        if (v1 == 0) {
          *lres=0;
          return ret;
        }
        break;
      case GRIB_TYPE_DOUBLE:
        ret = grib_expression_evaluate_double(h,e->left,&dv1);
        if (ret != GRIB_SUCCESS) return ret;
        if (dv1 == 0) {
          *lres=0;
          return ret;
        }
        break;
      default :
        return GRIB_INVALID_TYPE;
    }

    switch (grib_expression_native_type(h, e->right)) {
      case GRIB_TYPE_LONG:
        ret = grib_expression_evaluate_long(h,e->right,&v2);
        if (ret != GRIB_SUCCESS) return ret;
        *lres = v2 ? 1 : 0;
        break;
      case GRIB_TYPE_DOUBLE:
        ret = grib_expression_evaluate_double(h,e->right,&dv2);
        if (ret != GRIB_SUCCESS) return ret;
        *lres = dv2 ? 1 : 0;
        break;
      default :
        return GRIB_INVALID_TYPE;
    }

    return GRIB_SUCCESS;
}

static int evaluate_double(grib_expression *g,grib_handle* h,double* dres)
{
    long lres=0;
    int ret=0;

    ret=evaluate_long(g,h,&lres);
    *dres=(double)lres;

    return ret;
}

static void print(grib_context* c,grib_expression* g,grib_handle* f)
{
    grib_expression_logical_and* e = (grib_expression_logical_and*)g;
    printf("(");
    grib_expression_print(c,e->left,f);
    printf(" && ");
    grib_expression_print(c,e->right,f);
    printf(")");
}

static void destroy(grib_context* c,grib_expression* g)
{
    grib_expression_logical_and* e = (grib_expression_logical_and*)g;
    grib_expression_free(c,e->left);
    grib_expression_free(c,e->right);
}

static void  add_dependency(grib_expression* g, grib_accessor* observer)
{
    grib_expression_logical_and* e = (grib_expression_logical_and*)g;
    grib_dependency_observe_expression(observer,e->left);
    grib_dependency_observe_expression(observer,e->right);
}

grib_expression* new_logical_and_expression(grib_context* c, grib_expression* left,grib_expression* right)
{
    grib_expression_logical_and* e = (grib_expression_logical_and*)grib_context_malloc_clear_persistent(c,sizeof(grib_expression_logical_and));
    e->base.cclass                 = grib_expression_class_logical_and;
    e->left                = left;
    e->right               = right;
    return (grib_expression*)e;
}

static int native_type(grib_expression* g,grib_handle *h)
{
    return GRIB_TYPE_LONG;
}
