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
 *                                                                         *
 ***************************************************************************/
#include "grib_api_internal.h"

/*
   This is used by make_class.pl

   START_CLASS_DEF
   CLASS      = action
   IMPLEMENTS = create_accessor
   IMPLEMENTS = dump
   IMPLEMENTS = destroy
   MEMBERS    = grib_arguments* args
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


typedef struct grib_action_put {
    grib_action          act;  
/* Members defined in put */
	grib_arguments* args;
} grib_action_put;


static grib_action_class _grib_action_class_put = {
    0,                              /* super                     */
    "action_class_put",                              /* name                      */
    sizeof(grib_action_put),            /* size                      */
    0,                                   /* inited */
    &init_class,                         /* init_class */
    0,                               /* init                      */
    &destroy,                            /* destroy */

    &dump,                               /* dump                      */
    0,                               /* xref                      */

    &create_accessor,             /* create_accessor*/

    0,                            /* notify_change */
    0,                            /* reparse */
    0,                            /* execute */
};

grib_action_class* grib_action_class_put = &_grib_action_class_put;

static void init_class(grib_action_class* c)
{
}
/* END_CLASS_IMP */

grib_action* grib_action_create_put( grib_context* context, const char* name ,grib_arguments *args )
{
    grib_action_put*  a    =  NULL;
    grib_action_class* c   =  grib_action_class_put;
    grib_action* act       =  (grib_action*)grib_context_malloc_clear_persistent(context,c->size);
    act->next              =  NULL;
    act->name              =  grib_context_strdup_persistent(context,name);
    act->op                =  grib_context_strdup_persistent(context,"forward");
    act->cclass            =  c;
    act->context           =  context;
    a                      =  (grib_action_put*)act;
    a->args                =  args;
    return act;
}

static int create_accessor(grib_section* p, grib_action* act,grib_loader *h)
{
    grib_action_put* a = ( grib_action_put*)act;

    grib_section* ts = NULL;

    grib_accessor* ga = NULL;

    ga = grib_find_accessor(p->h, grib_arguments_get_name(p->h,a->args,1));
    if(ga)
        ts = ga->sub_section;
    /* ts = grib_get_sub_section(ga); */
    else  return GRIB_BUFFER_TOO_SMALL;

    if(ts){
        ga = grib_accessor_factory( ts, act,0,a->args);
        if(ga)grib_push_accessor(ga,ts->block);
        else  return GRIB_BUFFER_TOO_SMALL;

    }
    else{
        grib_context_log(act->context, GRIB_LOG_ERROR, "Action_class_put  : create_accessor_buffer : No Section named %s to export %s ", grib_arguments_get_name(p->h,a->args,1), grib_arguments_get_name(p->h,a->args,0));
    }
    return GRIB_SUCCESS;
}

static void dump( grib_action* act, FILE* f, int lvl)
{
    grib_action_put* a = ( grib_action_put*)act;

    int i = 0;

    for (i=0;i<lvl;i++) grib_context_print(act->context,f,"     ");

    grib_context_print(act->context,f,"put %s as %s in %s\n",grib_arguments_get_name(0,a->args,0),act->name,grib_arguments_get_name(0,a->args,1));
}

static void destroy(grib_context* context, grib_action* act)
{
    grib_action_put* a = ( grib_action_put*)act;

    grib_arguments_free(context, a->args);
    grib_context_free_persistent(context, act->name);
    grib_context_free_persistent(context, act->op);
}
