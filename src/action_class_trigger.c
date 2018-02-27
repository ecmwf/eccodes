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
   MEMBERS    = grib_arguments* trigger_on
   MEMBERS    = grib_action     *block
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


typedef struct grib_action_trigger {
    grib_action          act;  
/* Members defined in section */
/* Members defined in trigger */
	grib_arguments* trigger_on;
	grib_action     *block;
} grib_action_trigger;

extern grib_action_class* grib_action_class_section;

static grib_action_class _grib_action_class_trigger = {
    &grib_action_class_section,                              /* super                     */
    "action_class_trigger",                              /* name                      */
    sizeof(grib_action_trigger),            /* size                      */
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

grib_action_class* grib_action_class_trigger = &_grib_action_class_trigger;

static void init_class(grib_action_class* c)
{
	c->xref	=	(*(c->super))->xref;
	c->notify_change	=	(*(c->super))->notify_change;
	c->execute	=	(*(c->super))->execute;
}
/* END_CLASS_IMP */

grib_action *grib_action_create_trigger(grib_context *context, grib_arguments *args, grib_action *block)
{
    char name[1024];

    grib_action_trigger* a = 0;
    grib_action_class* c   =  grib_action_class_trigger;
    grib_action* act       =  (grib_action*)grib_context_malloc_clear_persistent(context,c->size);

    sprintf(name,"_trigger%p",(void*)act);

    act-> name             =  grib_context_strdup_persistent(context,name);
    act-> op               =  grib_context_strdup_persistent(context,"section");
    act-> cclass           =  c;
    act-> next             =  NULL;
    act->context           =  context;

    a = ( grib_action_trigger*)act;
    a->trigger_on        = args;
    a->block             = block;

    return act;
}

static void dump( grib_action* act, FILE* f, int lvl)
{
    /* grib_action_trigger* a = ( grib_action_trigger*)act; */
    int i = 0;
    for (i=0;i<lvl;i++) grib_context_print(act->context,f,"     ");
    grib_context_print(act->context,f,"Trigger\n");
}

static int  create_accessor(grib_section* p, grib_action* act, grib_loader *h )
{
    int ret = GRIB_SUCCESS;
    grib_action_trigger* a = ( grib_action_trigger*)act;
    grib_action* next = NULL;
    grib_accessor* as = NULL;
    grib_section*         gs = NULL;


    as = grib_accessor_factory(p, act,0,NULL);

    if(!as) return GRIB_INTERNAL_ERROR;

    gs = as->sub_section;
    gs->branch = 0; /* Force a reparse each time */

    grib_push_accessor(as,p->block);
    grib_dependency_observe_arguments(as,a->trigger_on);

    next = a->block;

    while(next){
        ret = grib_create_accessor(gs, next,h);
        if(ret != GRIB_SUCCESS) return ret;
        next= next->next;
    }

    return GRIB_SUCCESS;
}

static grib_action* reparse(grib_action* a,grib_accessor* acc,int* doit)
{
    grib_action_trigger* self = (grib_action_trigger*)a;
    return self->block;
}

/* COMEBACK */
static void destroy(grib_context* context,grib_action* act)
{
    grib_action_trigger* a = (grib_action_trigger*)act;

    grib_action *b = a->block;

    while(b)
    {
        grib_action *n = b->next;
        grib_action_delete(context,b);
        b = n;
    }

    grib_arguments_free(context, a->trigger_on);
    grib_context_free_persistent(context, act->name);
    grib_context_free_persistent(context, act->op);
}
