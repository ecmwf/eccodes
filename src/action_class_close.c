/*
 * Copyright 2005-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_api_internal.h"
/*
   This is used by make_class.pl

   START_CLASS_DEF
   CLASS      = action
   IMPLEMENTS = dump
   IMPLEMENTS = destroy;execute
   MEMBERS    = char *filename
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


typedef struct grib_action_close {
    grib_action          act;  
/* Members defined in close */
	char *filename;
} grib_action_close;


static grib_action_class _grib_action_class_close = {
    0,                              /* super                     */
    "action_class_close",                              /* name                      */
    sizeof(grib_action_close),            /* size                      */
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
    0,                            /* compile */
};

grib_action_class* grib_action_class_close = &_grib_action_class_close;

static void init_class(grib_action_class* c)
{
}
/* END_CLASS_IMP */

grib_action* grib_action_create_close( grib_context* context, char* filename)
{
  char buf[1024];

  grib_action_close* a ;
  grib_action_class* c   = grib_action_class_close;
  grib_action* act       = (grib_action*)grib_context_malloc_clear_persistent(context,c->size);
  act->op              =  grib_context_strdup_persistent(context,"section");

  act->cclass            = c;
  a                      = (grib_action_close*)act;
  act->context           = context;

  a->filename            = grib_context_strdup_persistent(context,filename);

  sprintf(buf,"close_%p",(void*)a->filename);

  act->name      = grib_context_strdup_persistent(context,buf);

  return act;
}

static int execute(grib_action* act, grib_handle *h)
{
  char filename[2048]={0,};
  size_t len=2048;
  grib_action_close* self = (grib_action_close*) act;
  int err =0;
  grib_file* file=0;

  err=grib_get_string(h,self->filename,filename,&len);
  /* fprintf(stderr,"++++ name %s\n",filename); */
  if (err) return err;
  /* grib_file_close(filename,1,&err); */
  file=grib_get_file(filename,&err);
  if (err) return err;
  if (file) grib_file_pool_delete_file(file);

  return err;
}


static void dump(grib_action* act, FILE* f, int lvl)
{
}


static void destroy(grib_context* context,grib_action* act)
{
  grib_action_close* a = (grib_action_close*) act;

  grib_context_free_persistent(context, a->filename);
  grib_context_free_persistent(context, act->name);
  grib_context_free_persistent(context, act->op);

}
