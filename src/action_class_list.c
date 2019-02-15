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
 *   Jean Baptiste Filippi - 01.11.2005                                                           *
 *   Enrico Fucile
 *                                                                         *
 ***************************************************************************/
#include "grib_api_internal.h"
/*
   This is used by make_class.pl

   START_CLASS_DEF
   CLASS      = action
   SUPER      = action_class_section
   IMPLEMENTS = create_accessor
   IMPLEMENTS = dump
   IMPLEMENTS = destroy
   IMPLEMENTS = reparse
   MEMBERS    = grib_expression *expression
   MEMBERS    = grib_action     *block_list
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
static grib_action* reparse(grib_action* a,grib_accessor* acc,int *doit);


typedef struct grib_action_list {
    grib_action          act;  
/* Members defined in section */
/* Members defined in list */
	grib_expression *expression;
	grib_action     *block_list;
} grib_action_list;

extern grib_action_class* grib_action_class_section;

static grib_action_class _grib_action_class_list = {
    &grib_action_class_section,                              /* super                     */
    "action_class_list",                              /* name                      */
    sizeof(grib_action_list),            /* size                      */
    0,                                   /* inited */
    &init_class,                         /* init_class */
    0,                               /* init                      */
    &destroy,                            /* destroy */

    &dump,                               /* dump                      */
    0,                               /* xref                      */

    &create_accessor,             /* create_accessor*/

    0,                            /* notify_change */
    &reparse,                            /* reparse */
    0,                            /* execute */
};

grib_action_class* grib_action_class_list = &_grib_action_class_list;

static void init_class(grib_action_class* c)
{
	c->xref	=	(*(c->super))->xref;
	c->notify_change	=	(*(c->super))->notify_change;
	c->execute	=	(*(c->super))->execute;
}
/* END_CLASS_IMP */

static void dump( grib_action* act, FILE* f, int lvl)
{
    grib_action_list* a = ( grib_action_list*)act;
    int i = 0;
    for (i=0;i<lvl;i++)   grib_context_print(act->context,f,"     ");
    grib_context_print(act->context,f,"Loop   %s\n", act->name );
    grib_dump_action_branch(f,a->block_list,lvl+1);
}

static int create_accessor(grib_section* p, grib_action* act,grib_loader* h)
{
    grib_action_list* a = ( grib_action_list*)act;

    grib_accessor* ga = NULL;
    grib_section*  gs = NULL;
    grib_action*  la = NULL;
    grib_action*  next = NULL;
    int ret = 0;
    long val = 0;

    if ((ret=grib_expression_evaluate_long(p->h,a->expression,&val)) != GRIB_SUCCESS){
        grib_context_log(p->h->context, GRIB_LOG_DEBUG, " List %s creating %d values unable to evaluate long \n", act->name, val );
        return ret;
    }

    grib_context_log(p->h->context, GRIB_LOG_DEBUG, " List %s creating %d values \n", act->name, val );

    ga = grib_accessor_factory(p, act,0,NULL);
    if(!ga)  return GRIB_BUFFER_TOO_SMALL;
    gs = ga->sub_section;
    ga->loop = val;

    grib_push_accessor(ga,p->block);

    la = a->block_list;

    gs->branch = la;
    grib_dependency_observe_expression(ga,a->expression);

    while(val--){
        next = la;
        while(next){
            ret =   grib_create_accessor(gs, next,h);
            if(ret != GRIB_SUCCESS) return ret;
            next= next->next;
        }
    }
    return GRIB_SUCCESS;
}

grib_action* grib_action_create_list( grib_context* context, const char* name, grib_expression* expression, grib_action* block)
{
    grib_action_list* a ;
    grib_action_class* c   = grib_action_class_list;
    grib_action* act       = ( grib_action*)grib_context_malloc_clear_persistent(context,c->size);
    act->cclass       = c;
    act->context = context;
    a                 = ( grib_action_list*)act;
    act->next = NULL;
    act->name =   grib_context_strdup_persistent(context,name);
    act->op =   grib_context_strdup_persistent(context,"section");
    a->expression =   expression;

    a->block_list = block;

    grib_context_log(context, GRIB_LOG_DEBUG, " Action List %s is created  \n",act->name);
    return act;
}

static grib_action* reparse(grib_action* a,grib_accessor* acc,int *doit)
{
    grib_action_list* self = ( grib_action_list*)a;

    int ret = 0;
    long val = 0;

    if ((ret=grib_expression_evaluate_long(grib_handle_of_accessor(acc),self->expression,&val)) != GRIB_SUCCESS){
        grib_context_log(acc->context, GRIB_LOG_ERROR, " List %s creating %d values unable to evaluate long \n", acc->name, val );
    }

    *doit = (val != acc->loop);

    return self->block_list;
}

static void destroy(grib_context* context,grib_action* act)
{
    grib_action_list* self = ( grib_action_list*)act;
    grib_action *a = self->block_list;

    while(a)
    {
        grib_action *na = a->next;
        grib_action_delete(context,a);
        a = na;
    }

    grib_context_free_persistent(context, act->name);
    grib_context_free_persistent(context, act->op);
    grib_expression_free(context, self->expression);
}
