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
   IMPLEMENTS = dump;xref
   IMPLEMENTS = destroy;execute
   MEMBERS    = grib_expression *expression
   MEMBERS    = char *name
   MEMBERS    = int nofail
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
static void xref            (grib_action* d, FILE* f,const char* path);
static void destroy         (grib_context*,grib_action*);
static int execute(grib_action* a,grib_handle* h);


typedef struct grib_action_set {
    grib_action          act;  
/* Members defined in set */
	grib_expression *expression;
	char *name;
	int nofail;
} grib_action_set;


static grib_action_class _grib_action_class_set = {
    0,                              /* super                     */
    "action_class_set",                              /* name                      */
    sizeof(grib_action_set),            /* size                      */
    0,                                   /* inited */
    &init_class,                         /* init_class */
    0,                               /* init                      */
    &destroy,                            /* destroy */

    &dump,                               /* dump                      */
    &xref,                               /* xref                      */

    0,             /* create_accessor*/

    0,                            /* notify_change */
    0,                            /* reparse */
    &execute,                            /* execute */
};

grib_action_class* grib_action_class_set = &_grib_action_class_set;

static void init_class(grib_action_class* c)
{
}
/* END_CLASS_IMP */

grib_action* grib_action_create_set( grib_context* context,
        const char* name,    grib_expression* expression,int nofail)
{
    char buf[1024];

    grib_action_set* a ;
    grib_action_class* c   = grib_action_class_set;
    grib_action* act       = (grib_action*)grib_context_malloc_clear_persistent(context,c->size);
    act->op              = grib_context_strdup_persistent(context,"section");

    act->cclass       = c;
    a                 = (grib_action_set*)act;
    act->context      = context;

    a->expression  = expression;
    a->name        = grib_context_strdup_persistent(context,name);
    a->nofail      = nofail;


    sprintf(buf,"set%p",(void*)expression);

    act->name      = grib_context_strdup_persistent(context,buf);

    return act;
}

static int execute(grib_action* a, grib_handle *h)
{
    int ret=0;
    grib_action_set* self = (grib_action_set*) a;
    ret=grib_set_expression(h,self->name,self->expression);
    if (self->nofail) return 0;
    if (ret != GRIB_SUCCESS) {
        grib_context_log(h->context,GRIB_LOG_ERROR,"Error while setting key %s (%s)",
                self->name,grib_get_error_message(ret));
    }
    return ret;
}

static void dump(grib_action* act, FILE* f, int lvl)
{
    int i =0;
    grib_action_set* self=(grib_action_set*)act;
    for (i=0;i<lvl;i++)
        grib_context_print(act->context,f,"     ");
    grib_context_print(act->context,f,self->name);
    printf("\n");
}

static void destroy(grib_context* context,grib_action* act)
{
    grib_action_set* a = (grib_action_set*) act;

    grib_context_free_persistent(context, a->name);
    grib_expression_free(context,a->expression);
    grib_context_free_persistent(context, act->name);
    grib_context_free_persistent(context, act->op);
}

static void xref(grib_action* d, FILE* f,const char *path)
{
}
