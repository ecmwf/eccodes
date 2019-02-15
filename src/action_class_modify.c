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
   IMPLEMENTS = dump;xref
   IMPLEMENTS = create_accessor
   IMPLEMENTS = destroy
   MEMBERS    = long flags
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
static int create_accessor(grib_section*,grib_action*,grib_loader*);


typedef struct grib_action_modify {
    grib_action          act;  
/* Members defined in modify */
	long flags;
	char *name;
} grib_action_modify;


static grib_action_class _grib_action_class_modify = {
    0,                              /* super                     */
    "action_class_modify",                              /* name                      */
    sizeof(grib_action_modify),            /* size                      */
    0,                                   /* inited */
    &init_class,                         /* init_class */
    0,                               /* init                      */
    &destroy,                            /* destroy */

    &dump,                               /* dump                      */
    &xref,                               /* xref                      */

    &create_accessor,             /* create_accessor*/

    0,                            /* notify_change */
    0,                            /* reparse */
    0,                            /* execute */
};

grib_action_class* grib_action_class_modify = &_grib_action_class_modify;

static void init_class(grib_action_class* c)
{
}
/* END_CLASS_IMP */

grib_action* grib_action_create_modify( grib_context* context, 
	const char* name,
		long flags)
{  
	
	grib_action_modify* a ;
	grib_action_class* c   = grib_action_class_modify;
	grib_action* act       = (grib_action*)grib_context_malloc_clear_persistent(context,c->size);
	act->op              = grib_context_strdup_persistent(context,"section");

	act->cclass       = c;
	a                 = (grib_action_modify*)act;
	act->context      = context;

	a->flags       = flags;
	a->name        = grib_context_strdup_persistent(context,name);


	act->name      = grib_context_strdup_persistent(context,"flags");

	return act;
}

static void dump(grib_action* act, FILE* f, int lvl)
{
}

static int create_accessor(grib_section* p, grib_action* act,grib_loader *h)
{
	grib_action_modify* a = ( grib_action_modify*)act;
	grib_accessor* ga = NULL;

	ga = grib_find_accessor(p->h, a->name);

	if(ga)
		ga->flags = a->flags;

	else{
		grib_context_log(act->context, GRIB_LOG_DEBUG, "action_class_modify: create_accessor_buffer : No accessor named %s to modify.", a->name);
	}
	return GRIB_SUCCESS;
}

static void destroy(grib_context* context,grib_action* act)
{
	grib_action_modify* a = (grib_action_modify*) act;

	grib_context_free_persistent(context, a->name);
	grib_context_free_persistent(context, act->name);
	grib_context_free_persistent(context, act->op);
}

static void xref(grib_action* d, FILE* f,const char* path)
{
}
