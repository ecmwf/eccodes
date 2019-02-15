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
   IMPLEMENTS = dump
   IMPLEMENTS = destroy;execute
   MEMBERS    = char *name
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
static int execute(grib_action* a,grib_handle* h);


typedef struct grib_action_set_missing {
    grib_action          act;  
/* Members defined in set_missing */
	char *name;
} grib_action_set_missing;


static grib_action_class _grib_action_class_set_missing = {
    0,                              /* super                     */
    "action_class_set_missing",                              /* name                      */
    sizeof(grib_action_set_missing),            /* size                      */
    0,                                   /* inited */
    &init_class,                         /* init_class */
    0,                               /* init                      */
    &destroy,                            /* destroy */

    &dump,                               /* dump                      */
    0,                               /* xref                      */

    0,             /* create_accessor*/

    0,                            /* notify_change */
    0,                            /* reparse */
    &execute,                            /* execute */
};

grib_action_class* grib_action_class_set_missing = &_grib_action_class_set_missing;

static void init_class(grib_action_class* c)
{
}
/* END_CLASS_IMP */

grib_action* grib_action_create_set_missing( grib_context* context,
        const char* name)
{
    char buf[1024];

    grib_action_set_missing* a ;
    grib_action_class* c   = grib_action_class_set_missing;
    grib_action* act       = (grib_action*)grib_context_malloc_clear_persistent(context,c->size);
    act->op              = grib_context_strdup_persistent(context,"set_missing");

    act->cclass       = c;
    a                 = (grib_action_set_missing*)act;
    act->context      = context;

    a->name        = grib_context_strdup_persistent(context,name);

    sprintf(buf,"set_missing_%s",name);

    act->name      = grib_context_strdup_persistent(context,buf);

    return act;
}

static int execute(grib_action* a, grib_handle *h)
{
    grib_action_set_missing* self = (grib_action_set_missing*) a;

    return grib_set_missing(h,self->name);
}

static void dump(grib_action* act, FILE* f, int lvl)
{
    int i =0;
    grib_action_set_missing* self=(grib_action_set_missing*)act;
    for (i=0;i<lvl;i++)
        grib_context_print(act->context,f,"     ");
    grib_context_print(act->context,f,self->name);
    printf("\n");
}

static void destroy(grib_context* context,grib_action* act)
{
    grib_action_set_missing* a = (grib_action_set_missing*) act;

    grib_context_free_persistent(context, a->name);
    grib_context_free_persistent(context, act->name);
    grib_context_free_persistent(context, act->op);
}
