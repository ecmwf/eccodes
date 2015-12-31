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
   SUPER      = action_class_section
   IMPLEMENTS = create_accessor
   IMPLEMENTS = dump
   IMPLEMENTS = destroy
   MEMBERS    = grib_expression *expression
   MEMBERS    = grib_action     *block_while
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


typedef struct grib_action_while {
    grib_action          act;  
/* Members defined in section */
/* Members defined in while */
	grib_expression *expression;
	grib_action     *block_while;
} grib_action_while;

extern grib_action_class* grib_action_class_section;

static grib_action_class _grib_action_class_while = {
    &grib_action_class_section,                              /* super                     */
    "action_class_while",                              /* name                      */
    sizeof(grib_action_while),            /* size                      */
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
    0,                            /* compile */
};

grib_action_class* grib_action_class_while = &_grib_action_class_while;

static void init_class(grib_action_class* c)
{
	c->xref	=	(*(c->super))->xref;
	c->notify_change	=	(*(c->super))->notify_change;
	c->reparse	=	(*(c->super))->reparse;
	c->execute	=	(*(c->super))->execute;
	c->compile	=	(*(c->super))->compile;
}
/* END_CLASS_IMP */

static void dump( grib_action* act, FILE* f, int lvl)
{
	grib_action_while* a = ( grib_action_while*)act;
	int i = 0;
	for (i=0;i<lvl;i++)   grib_context_print(act->context,f,"     ");
	grib_context_print(act->context,f,"Loop   %s\n", act->name );
	grib_dump_action_branch(f,a->block_while,lvl+1);
}

static int create_accessor(grib_section* p, grib_action* act,grib_loader* h)
{
	grib_action_while* a = ( grib_action_while*)act;

	grib_accessor* ga = NULL;
	grib_section*  gs = NULL;
	grib_action*  la = NULL;
	grib_action*  next = NULL;
	int ret = 0;
	/* long n = 0; */

	ga = grib_accessor_factory(p, act,0,NULL);
	if(!ga)  return GRIB_BUFFER_TOO_SMALL;
	gs = ga->sub_section;

	grib_push_accessor(ga,p->block);   

	la = a->block_while;

	for(;;)
	{
		long val = 0;

		if ((ret=grib_expression_evaluate_long(p->h,a->expression,&val)) != GRIB_SUCCESS){
			grib_context_log(p->h->context, GRIB_LOG_DEBUG, " List %s creating %d values unable to evaluate long \n", act->name, val );
			return ret;
		}

		/* printf("val=%ld %ld\n",val,n++); */

		if(!val)
			break;


		next = la;
		while(next){
			ret =   grib_create_accessor(gs, next,h);
			if(ret != GRIB_SUCCESS) return ret;
			next= next->next;
		}
	}
	return GRIB_SUCCESS;

}

grib_action* grib_action_create_while( grib_context* context, grib_expression* expression, grib_action* block)
{
	char name[80];
	grib_action_while* a ;    
	grib_action_class* c   = grib_action_class_while;
	grib_action* act       = ( grib_action*)grib_context_malloc_clear_persistent(context,c->size);
	act->cclass       = c;
	act->context = context;
	a                 = ( grib_action_while*)act;
	act->next = NULL;


	sprintf(name,"_while%p",(void*)a);
	act->name =   grib_context_strdup_persistent(context,name);
	act->op =   grib_context_strdup_persistent(context,"section");
	a->expression =   expression;

	a->block_while = block;

	grib_context_log(context, GRIB_LOG_DEBUG, " Action List %s is created  \n",act->name);
	return act;
}

static void destroy(grib_context* context,grib_action* act)
{
	grib_action_while* self = ( grib_action_while*)act;
	grib_action *a = self->block_while;

	while(a)
	{
		grib_action *na = a->next;
		grib_action_delete(context,a);
		a = na;
	}

	grib_context_free_persistent(context, act->name);
	grib_context_free_persistent(context, act->op);
	grib_expression_free(context, self->expression);

}    

