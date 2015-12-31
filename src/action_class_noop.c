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
 *   Jean Baptiste Filippi - 01.11.2005                                                           *
 *                                                                         *
 ***************************************************************************/
#include "grib_api_internal.h"
/*
   This is used by make_class.pl

   START_CLASS_DEF
   CLASS      = action
   IMPLEMENTS = dump;destroy;xref;execute;compile
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
static void compile         (grib_action* a, grib_compiler* compiler);
static void destroy         (grib_context*,grib_action*);
static int execute(grib_action* a,grib_handle* h);


typedef struct grib_action_noop {
    grib_action          act;  
/* Members defined in noop */
} grib_action_noop;


static grib_action_class _grib_action_class_noop = {
    0,                              /* super                     */
    "action_class_noop",                              /* name                      */
    sizeof(grib_action_noop),            /* size                      */
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
    &compile,                            /* compile */
};

grib_action_class* grib_action_class_noop = &_grib_action_class_noop;

static void init_class(grib_action_class* c)
{
}
/* END_CLASS_IMP */

grib_action* grib_action_create_noop( grib_context* context,const char* fname)
{
    char buf[1024];

    grib_action_noop* a ;
    grib_action_class* c   = grib_action_class_noop;
    grib_action* act       = (grib_action*)grib_context_malloc_clear_persistent(context,c->size);
    act->op                = grib_context_strdup_persistent(context,"section");

    act->cclass       = c;
    a                 = (grib_action_noop*)act;
    act->context      = context;

    sprintf(buf,"_noop%p",(void*)a);

    act->name      = grib_context_strdup_persistent(context,buf);

    return act;
}

static void compile(grib_action* act, grib_compiler *compiler)
{
    fprintf(compiler->out,"%s = grib_action_create_noop(ctx,",compiler->var);
    fprintf(compiler->out,"\"%s\"",act->name);
    fprintf(compiler->out,");");
    fprintf(compiler->out,"\n");
}

static void dump(grib_action* act, FILE* f, int lvl)
{
}

static void destroy(grib_context* context,grib_action* act)
{
    grib_context_free_persistent(context, act->name);
    grib_context_free_persistent(context, act->op);
}

static void xref(grib_action* d, FILE* f,const char* path)
{
}

static int execute(grib_action* act, grib_handle *h) {return 0;}
