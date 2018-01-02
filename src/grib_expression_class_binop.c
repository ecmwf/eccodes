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
   IMPLEMENTS = evaluate_double
   IMPLEMENTS = print
   IMPLEMENTS = add_dependency
   MEMBERS    = grib_expression *left
   MEMBERS = grib_expression *right
   MEMBERS = grib_binop_long_proc    long_func
   MEMBERS = grib_binop_double_proc  double_func
   MEMBERS = grib_binop_string_proc  string_func
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

typedef struct grib_expression_binop{
  grib_expression base;
/* Members defined in binop */
	grib_expression *left;
	grib_expression *right;
	grib_binop_long_proc    long_func;
	grib_binop_double_proc  double_func;
	grib_binop_string_proc  string_func;
} grib_expression_binop;


static grib_expression_class _grib_expression_class_binop = {
    0,                    /* super                     */
    "binop",                    /* name                      */
    sizeof(grib_expression_binop),/* size of instance          */
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

grib_expression_class* grib_expression_class_binop = &_grib_expression_class_binop;


static void init_class(grib_expression_class* c)
{
}
/* END_CLASS_IMP */

static int evaluate_long(grib_expression *g,grib_handle* h,long* lres)
{
    long v1=0;
    long v2=0;
    int ret;
    grib_expression_binop* e = (grib_expression_binop*)g;

#if 0 /* DEBUGGING */
    {
        int typeLeft, typeRight;
        const char* nameLeft;
        const char* nameRight;
        typeLeft = grib_expression_native_type(h, e->left);
        typeRight = grib_expression_native_type(h, e->right);
        nameLeft = grib_expression_get_name(e->left);
        nameRight= grib_expression_get_name(e->right);
        printf("eval_long nameLeft=%s (type=%d), nameRight=%s (type=%d)\n",nameLeft,typeLeft, nameRight,typeRight);
        grib_expression_print(h->context, g, h);
        printf("\n");
    }
#endif

    ret = grib_expression_evaluate_long(h,e->left,&v1);
    if (ret != GRIB_SUCCESS) return ret;

    ret = grib_expression_evaluate_long(h,e->right,&v2);
    if (ret != GRIB_SUCCESS) return ret;

    *lres=e->long_func(v1,v2);
    return GRIB_SUCCESS;
}

static int evaluate_double(grib_expression *g,grib_handle* h,double* dres)
{
    double v1=0.0;
    double v2=0.0;
    int ret;

    grib_expression_binop* e = (grib_expression_binop*)g;

#if 0 /* DEBUGGING */
    {
        int typeLeft, typeRight;
        const char* nameLeft;
        const char* nameRight;
        typeLeft = grib_expression_native_type(h, e->left);
        typeRight = grib_expression_native_type(h, e->right);
        nameLeft = grib_expression_get_name(e->left);
        nameRight= grib_expression_get_name(e->right);
        printf("eval_dbl nameLeft=%s (type=%d), nameRight=%s (type=%d)\n",nameLeft,typeLeft, nameRight,typeRight);
        grib_expression_print(h->context, g, h);
        printf("\n");
    }
#endif

    ret = grib_expression_evaluate_double(h,e->left,&v1);
    if (ret != GRIB_SUCCESS) return ret;

    ret = grib_expression_evaluate_double(h,e->right,&v2);
    if (ret != GRIB_SUCCESS) return ret;

    *dres = e->double_func ? e->double_func(v1,v2) : e->long_func(v1,v2);

    return GRIB_SUCCESS;
}

static void print(grib_context* c,grib_expression* g,grib_handle* f)
{
    grib_expression_binop* e = (grib_expression_binop*)g;
    printf("binop(");
    grib_expression_print(c,e->left,f);
    printf(",");
    grib_expression_print(c,e->right,f);
    printf(")");
}

static void destroy(grib_context* c,grib_expression* g)
{
    grib_expression_binop* e = (grib_expression_binop*)g;
    grib_expression_free(c,e->left);
    grib_expression_free(c,e->right);
}

static void  add_dependency(grib_expression* g, grib_accessor* observer)
{
    grib_expression_binop* e = (grib_expression_binop*)g;
    grib_dependency_observe_expression(observer,e->left);
    grib_dependency_observe_expression(observer,e->right);
}

grib_expression* new_binop_expression(grib_context* c,
        grib_binop_long_proc  long_func,
        grib_binop_double_proc double_func,
        grib_expression* left,grib_expression* right)
{
    grib_expression_binop* e = (grib_expression_binop*)grib_context_malloc_clear_persistent(c,sizeof(grib_expression_binop));
    e->base.cclass                 = grib_expression_class_binop;
    e->left                = left;
    e->right               = right;
    e->long_func            = long_func;
    e->double_func          = double_func;
    return (grib_expression*)e;
}

static int native_type(grib_expression* g,grib_handle *h)
{
    grib_expression_binop* e = (grib_expression_binop*)g;
    /* See GRIB-394 : The type of this binary expression will be double if any of its operands are double */
    if (grib_expression_native_type(h, e->left)  == GRIB_TYPE_DOUBLE ||
        grib_expression_native_type(h, e->right) == GRIB_TYPE_DOUBLE)
    {
        return GRIB_TYPE_DOUBLE;
    }
    return e->long_func ? GRIB_TYPE_LONG : GRIB_TYPE_DOUBLE;
}
