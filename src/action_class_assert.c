/*
 * Copyright 2005-2018 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

/***************************************************************************
 *  Enrico Fucile                                                                         *
 ***************************************************************************/
#include "grib_api_internal.h"

/*
   This is used by make_class.pl

   START_CLASS_DEF
   CLASS      = action
   IMPLEMENTS = create_accessor
   IMPLEMENTS = dump;notify_change
   IMPLEMENTS = destroy; execute
   MEMBERS    = grib_expression *expression
   END_CLASS_DEF

 */

/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "action.class" and rerun ./make_class.pl

*/

static void init_class      (grib_action_class*);
static void dump            (grib_action* d, FILE*,int);
static void destroy         (grib_context*,grib_action*);
static int create_accessor(grib_section*,grib_action*,grib_loader*);
static int notify_change(grib_action* a, grib_accessor* observer,grib_accessor* observed);
static int execute(grib_action* a,grib_handle* h);


typedef struct grib_action_assert {
    grib_action          act;  
/* Members defined in assert */
	grib_expression *expression;
} grib_action_assert;


static grib_action_class _grib_action_class_assert = {
    0,                              /* super                     */
    "action_class_assert",                              /* name                      */
    sizeof(grib_action_assert),            /* size                      */
    0,                                   /* inited */
    &init_class,                         /* init_class */
    0,                               /* init                      */
    &destroy,                            /* destroy */

    &dump,                               /* dump                      */
    0,                               /* xref                      */

    &create_accessor,             /* create_accessor*/

    &notify_change,                            /* notify_change */
    0,                            /* reparse */
    &execute,                            /* execute */
};

grib_action_class* grib_action_class_assert = &_grib_action_class_assert;

static void init_class(grib_action_class* c)
{
}
/* END_CLASS_IMP */

grib_action* grib_action_create_assert( grib_context* context, grib_expression* expression)
{
  grib_action_assert* a     =  NULL;
  grib_action_class* c   =  grib_action_class_assert;
  grib_action* act       =  (grib_action*)grib_context_malloc_clear_persistent(context,c->size);
  act->next              =  NULL;
  act->name              =  grib_context_strdup_persistent(context, "assertion");
  act->op                =  grib_context_strdup_persistent(context, "evaluate");
  act->cclass            =  c;
  act->context = context;
  a                      =  (grib_action_assert*)act;
  a->expression              =  expression;
  return act;
}

static int create_accessor(grib_section* p, grib_action* act,grib_loader *h)
{
  grib_action_assert* self = (grib_action_assert*)act;
  grib_accessor*  as = grib_accessor_factory(p, act,0,NULL);
  if(!as)return GRIB_INTERNAL_ERROR;
  grib_dependency_observe_expression(as,self->expression);

  grib_push_accessor(as,p->block);


  return GRIB_SUCCESS;
}

static void dump( grib_action* act, FILE* f, int lvl)
{
  int i =0;
  grib_action_assert* self=(grib_action_assert*)act;
  for (i=0;i<lvl;i++)
    grib_context_print(act->context,f,"     ");
  grib_expression_print(act->context,self->expression,0);
  printf("\n");
}

static void destroy(grib_context* context,grib_action* act)
{
  grib_action_assert* a = ( grib_action_assert*)act;
  grib_expression_free(context,a->expression);
  grib_context_free_persistent(context, act->name);
  grib_context_free_persistent(context, act->op);
}

static int execute(grib_action* a,grib_handle* h) {
  int ret=0;
  double res=0;
  grib_action_assert* self= (grib_action_assert*)a;

  if ((ret=grib_expression_evaluate_double(h,self->expression,&res)) != GRIB_SUCCESS)
    return ret;

  if (res != 0)
    return GRIB_SUCCESS;
  else {
    grib_context_log(h->context,GRIB_LOG_ERROR,"Assertion failure: ");
    grib_expression_print(h->context,self->expression,h);
    printf("\n");
    return GRIB_ASSERTION_FAILURE;
  }
}

static int notify_change(grib_action* a, grib_accessor* observer,grib_accessor* observed)
{
  grib_action_assert* self = (grib_action_assert*) a;

  int ret = GRIB_SUCCESS;
  long lres;

  if ((ret = grib_expression_evaluate_long(grib_handle_of_accessor(observed), self->expression,&lres))
       != GRIB_SUCCESS) return ret;


  if (lres != 0)
    return GRIB_SUCCESS;
  else
    return GRIB_ASSERTION_FAILURE;
}
