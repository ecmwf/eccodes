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
   IMPLEMENTS = dump
   IMPLEMENTS = destroy;execute
   MEMBERS    = char *name
   MEMBERS    = char *outname
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


typedef struct grib_action_print {
    grib_action          act;  
/* Members defined in print */
	char *name;
	char *outname;
} grib_action_print;


static grib_action_class _grib_action_class_print = {
    0,                              /* super                     */
    "action_class_print",                              /* name                      */
    sizeof(grib_action_print),            /* size                      */
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

grib_action_class* grib_action_class_print = &_grib_action_class_print;

static void init_class(grib_action_class* c)
{
}
/* END_CLASS_IMP */

grib_action* grib_action_create_print( grib_context* context, const char* name,char* outname)
{
    char buf[1024];

    grib_action_print* a ;
    grib_action_class* c   = grib_action_class_print;
    grib_action* act       = (grib_action*)grib_context_malloc_clear_persistent(context,c->size);
    act->op              =  grib_context_strdup_persistent(context,"section");

    act->cclass            = c;
    a                      = (grib_action_print*)act;
    act->context           = context;

    a->name            = grib_context_strdup_persistent(context,name);

    if (outname) {
        FILE* out=NULL;
        int ioerr=0;
        a->outname      = grib_context_strdup_persistent(context,outname);
        out=fopen(outname,"w");
        ioerr=errno;
        if (!out)   {
            grib_context_log(act->context,(GRIB_LOG_ERROR)|(GRIB_LOG_PERROR),
                    "IO ERROR: %s: %s",strerror(ioerr),outname);
        }
        if (out) fclose(out);
    }

    sprintf(buf,"print%p",(void*)a->name);

    act->name      = grib_context_strdup_persistent(context,buf);

    return act;
}

static int execute(grib_action* act, grib_handle *h)
{
    grib_action_print* self = (grib_action_print*) act;
    int err =0;
    FILE* out=NULL;
    int ioerr=0;

    if (self->outname) {
        out=fopen(self->outname,"a");
        ioerr=errno;
        if (!out)   {
            grib_context_log(act->context,(GRIB_LOG_ERROR)|(GRIB_LOG_PERROR),
                    "IO ERROR: %s: %s",strerror(ioerr),self->outname);
            return GRIB_IO_PROBLEM;
        }
    } else {
        out=stdout;
    }

    err=grib_recompose_print(h,NULL,self->name,0,out);

    if (self->outname) fclose(out);

    return err;
}

static void dump(grib_action* act, FILE* f, int lvl)
{
}

static void destroy(grib_context* context,grib_action* act)
{
    grib_action_print* a = (grib_action_print*) act;

    grib_context_free_persistent(context, a->name);
    grib_context_free_persistent(context, act->name);
    grib_context_free_persistent(context, act->op);
}
