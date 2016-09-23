/*
 * Copyright 2005-2016 ECMWF.
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
   SUPER      = action_class_gen
   IMPLEMENTS = dump;xref
   IMPLEMENTS = destroy;execute
   MEMBERS    = grib_darray *darray
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
static void xref            (grib_action* d, FILE* f,const char* path);
static void destroy         (grib_context*,grib_action*);
static int execute(grib_action* a,grib_handle* h);


typedef struct grib_action_transient_darray {
    grib_action          act;  
/* Members defined in gen */
	long            len;
	grib_arguments* params;
/* Members defined in transient_darray */
	grib_darray *darray;
	char *name;
} grib_action_transient_darray;

extern grib_action_class* grib_action_class_gen;

static grib_action_class _grib_action_class_transient_darray = {
    &grib_action_class_gen,                              /* super                     */
    "action_class_transient_darray",                              /* name                      */
    sizeof(grib_action_transient_darray),            /* size                      */
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
    0,                            /* compile */
};

grib_action_class* grib_action_class_transient_darray = &_grib_action_class_transient_darray;

static void init_class(grib_action_class* c)
{
	c->create_accessor	=	(*(c->super))->create_accessor;
	c->notify_change	=	(*(c->super))->notify_change;
	c->reparse	=	(*(c->super))->reparse;
	c->compile	=	(*(c->super))->compile;
}
/* END_CLASS_IMP */

grib_action* grib_action_create_transient_darray( grib_context* context, const char* name, grib_darray* darray,int flags)
{
    grib_action_transient_darray* a = NULL;
    grib_action_class* c   = grib_action_class_transient_darray;
    grib_action* act       = (grib_action*)grib_context_malloc_clear_persistent(context,c->size);
    act->op              = grib_context_strdup_persistent(context,"transient_darray");

    act->cclass       = c;
    a                 = (grib_action_transient_darray*)act;
    act->context      = context;
    act->flags        = flags;

    a->darray  = darray;
    a->name        = grib_context_strdup_persistent(context,name);

    act->name      = grib_context_strdup_persistent(context,name);

    return act;
}

static int execute(grib_action* act, grib_handle *h)
{
    grib_action_transient_darray* self = (grib_action_transient_darray*) act;
    size_t len=grib_darray_used_size(self->darray);
    grib_accessor* a=NULL;
    grib_section* p=h->root;

    a = grib_accessor_factory( p, act,self->len,self->params);
    if(!a) return GRIB_INTERNAL_ERROR;

    grib_push_accessor(a,p->block);

    if(a->flags & GRIB_ACCESSOR_FLAG_CONSTRAINT)
        grib_dependency_observe_arguments(a,act->default_value);

    return grib_pack_double(a,self->darray->v,&len);
}

static void dump(grib_action* act, FILE* f, int lvl)
{
    int i =0;
    grib_action_transient_darray* self=(grib_action_transient_darray*)act;
    for (i=0;i<lvl;i++)
        grib_context_print(act->context,f,"     ");
    grib_context_print(act->context,f,self->name);
    printf("\n");
}

static void destroy(grib_context* context,grib_action* act)
{
    grib_action_transient_darray* a = (grib_action_transient_darray*) act;

    grib_context_free_persistent(context, a->name);
    grib_darray_delete(context,a->darray);
    grib_context_free_persistent(context, act->name);
    grib_context_free_persistent(context, act->op);
}

static void xref(grib_action* d, FILE* f,const char *path)
{
}
