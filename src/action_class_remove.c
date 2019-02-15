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
   IMPLEMENTS = destroy; xref
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
static void xref            (grib_action* d, FILE* f,const char* path);
static void destroy         (grib_context*,grib_action*);
static int create_accessor(grib_section*,grib_action*,grib_loader*);


typedef struct grib_action_remove {
    grib_action          act;  
/* Members defined in remove */
	grib_arguments* args;
} grib_action_remove;


static grib_action_class _grib_action_class_remove = {
    0,                              /* super                     */
    "action_class_remove",                              /* name                      */
    sizeof(grib_action_remove),            /* size                      */
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

grib_action_class* grib_action_class_remove = &_grib_action_class_remove;

static void init_class(grib_action_class* c)
{
}
/* END_CLASS_IMP */

grib_action* grib_action_create_remove( grib_context* context, grib_arguments *args )
{
    grib_action_remove*  a    =  NULL;
    grib_action_class* c   =  grib_action_class_remove;
    grib_action* act       =  (grib_action*)grib_context_malloc_clear_persistent(context,c->size);
    act->next              =  NULL;
    act->name              =  grib_context_strdup_persistent(context,"DELETE");
    act->op                =  grib_context_strdup_persistent(context,"remove");
    act->cclass            =  c;
    act->context           =  context;
    a                      =  (grib_action_remove*)act;
    a->args                =  args;
    return act;
}

static void remove_accessor(grib_accessor *a)
{
    grib_section* s = NULL;
    int id;

    if (!a || !a->previous) return;
    s = a->parent;

    if (grib_handle_of_accessor(a)->use_trie && *(a->all_names[0]) != '_') {
        id=grib_hash_keys_get_id(a->context->keys,a->all_names[0]);
        grib_handle_of_accessor(a)->accessors[id]=NULL;
    }

    if (a->next) a->previous->next = a->next;
    else return;

    a->next->previous = a->previous;

    grib_accessor_delete(s->h->context,a);

    return;
}

static int create_accessor(grib_section* p, grib_action* act,grib_loader*h)
{
    grib_action_remove* a = ( grib_action_remove*)act;


    grib_accessor* ga = NULL;


    ga = grib_find_accessor(p->h, grib_arguments_get_name(p->h,a->args,0));

    if(ga)
        remove_accessor(ga);


    else{
        grib_context_log(act->context, GRIB_LOG_DEBUG, "Action_class_remove  : create_accessor_buffer : No accessor named %s to remove ", grib_arguments_get_name(p->h,a->args,0));
    }
    return GRIB_SUCCESS;
}

static void dump( grib_action* act, FILE* f, int lvl)
{
    grib_action_remove* a = ( grib_action_remove*)act;

    int i = 0;

    for (i=0;i<lvl;i++) grib_context_print(act->context,f,"     ");

    grib_context_print(act->context,f,"remove %s as %s in %s\n",grib_arguments_get_name(0,a->args,0),act->name,grib_arguments_get_name(0,a->args,1));

}
static void destroy(grib_context* context, grib_action* act)
{
    grib_action_remove* a = ( grib_action_remove*)act;

    grib_arguments_free(context, a->args);
    grib_context_free_persistent(context, act->name);
    grib_context_free_persistent(context, act->op);
}

static void xref(grib_action* d, FILE* f,const char* path)
{
}
