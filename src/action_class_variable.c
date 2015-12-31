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
   SUPER      = action_class_gen
   IMPLEMENTS = execute
   IMPLEMENTS = compile
   END_CLASS_DEF

 */

/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "action.class" and rerun ./make_class.pl

*/

static void init_class      (grib_action_class*);
static void compile         (grib_action* a, grib_compiler* compiler);
static int execute(grib_action* a,grib_handle* h);


typedef struct grib_action_variable {
    grib_action          act;  
/* Members defined in gen */
	long            len;
	grib_arguments* params;
/* Members defined in variable */
} grib_action_variable;

extern grib_action_class* grib_action_class_gen;

static grib_action_class _grib_action_class_variable = {
    &grib_action_class_gen,                              /* super                     */
    "action_class_variable",                              /* name                      */
    sizeof(grib_action_variable),            /* size                      */
    0,                                   /* inited */
    &init_class,                         /* init_class */
    0,                               /* init                      */
    0,                            /* destroy */

    0,                               /* dump                      */
    0,                               /* xref                      */

    0,             /* create_accessor*/

    0,                            /* notify_change */
    0,                            /* reparse */
    &execute,                            /* execute */
    &compile,                            /* compile */
};

grib_action_class* grib_action_class_variable = &_grib_action_class_variable;

static void init_class(grib_action_class* c)
{
	c->dump	=	(*(c->super))->dump;
	c->xref	=	(*(c->super))->xref;
	c->create_accessor	=	(*(c->super))->create_accessor;
	c->notify_change	=	(*(c->super))->notify_change;
	c->reparse	=	(*(c->super))->reparse;
}
/* END_CLASS_IMP */

grib_action* grib_action_create_variable( grib_context* context, const char* name, const char* op, const long len,  grib_arguments* params,  grib_arguments* default_value,int flags,const char* name_space)
{
	grib_action_variable* a     =  NULL;
	grib_action_class* c   =  grib_action_class_variable;
	grib_action* act       =  (grib_action*)grib_context_malloc_clear_persistent(context,c->size);
	act->next              =  NULL;
	act->name              =  grib_context_strdup_persistent(context, name);
	if(name_space)
	act->name_space              =  grib_context_strdup_persistent(context, name_space);
	act->op                =  grib_context_strdup_persistent(context, op);
	act->cclass            =  c;
	act->context           =  context;
	act->flags             =  flags;
	a                      =  (grib_action_variable*)act;
	a->len                 =  len;
	a->params              =  params;
	act->default_value       =  default_value;

	/* printf("CREATE %s\n",name); */

	return act;
}

static int execute(grib_action* a, grib_handle *h)
{
	return grib_create_accessor(h->root, a, NULL );
}

static void compile(grib_action* act, grib_compiler *compiler)
{
    grib_action_variable* a  = (grib_action_variable*)act;
    fprintf(compiler->out,"%s = grib_action_create_variable(ctx,",compiler->var);
    fprintf(compiler->out,"\"%s\",",act->name);
    fprintf(compiler->out,"\"%s\",",act->op);
    fprintf(compiler->out,"%ld,",a->len);
    fprintf(compiler->out,"NULL,"); /* a->params */
    fprintf(compiler->out,"NULL,"); /* a->default_value */
    grib_compile_flags(compiler, act->flags);
    fprintf(compiler->out,",");
    if(act->name_space) {
        fprintf(compiler->out,"\"%s\");",act->name_space);
    }
    else
    {
        fprintf(compiler->out,"NULL);");
    }    
    fprintf(compiler->out,"\n");
}



