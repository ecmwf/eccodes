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
 *   Enrico Fucile
 *                                                                         *
 ***************************************************************************/
#include "grib_api_internal.h"
/*
   This is used by make_class.pl

   START_CLASS_DEF
   CLASS      = action
   SUPER      = action_class_section
   IMPLEMENTS = destroy
   IMPLEMENTS = xref   
   IMPLEMENTS = execute
   MEMBERS    = grib_arguments* args
   MEMBERS    = grib_case *Case
   MEMBERS    = grib_action *Default
   END_CLASS_DEF

 */

/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "action.class" and rerun ./make_class.pl

*/

static void init_class      (grib_action_class*);
static void xref            (grib_action* d, FILE* f,const char* path);
static void destroy         (grib_context*,grib_action*);
static int execute(grib_action* a,grib_handle* h);


typedef struct grib_action_switch {
    grib_action          act;  
/* Members defined in section */
/* Members defined in switch */
	grib_arguments* args;
	grib_case *Case;
	grib_action *Default;
} grib_action_switch;

extern grib_action_class* grib_action_class_section;

static grib_action_class _grib_action_class_switch = {
    &grib_action_class_section,                              /* super                     */
    "action_class_switch",                              /* name                      */
    sizeof(grib_action_switch),            /* size                      */
    0,                                   /* inited */
    &init_class,                         /* init_class */
    0,                               /* init                      */
    &destroy,                            /* destroy */

    0,                               /* dump                      */
    &xref,                               /* xref                      */

    0,             /* create_accessor*/

    0,                            /* notify_change */
    0,                            /* reparse */
    &execute,                            /* execute */
    0,                            /* compile */
};

grib_action_class* grib_action_class_switch = &_grib_action_class_switch;

static void init_class(grib_action_class* c)
{
	c->dump	=	(*(c->super))->dump;
	c->create_accessor	=	(*(c->super))->create_accessor;
	c->notify_change	=	(*(c->super))->notify_change;
	c->reparse	=	(*(c->super))->reparse;
	c->compile	=	(*(c->super))->compile;
}
/* END_CLASS_IMP */

grib_action* grib_action_create_switch( grib_context* context,
        grib_arguments* args,
        grib_case* Case,
        grib_action* Default)
{   char name[1024];
grib_action_switch* a ;
grib_action_class* c   = grib_action_class_switch;
grib_action* act       = (grib_action*)grib_context_malloc_clear_persistent(context,c->size);
act->op              = grib_context_strdup_persistent(context,"section");

act->cclass       = c;
a                 = (grib_action_switch*)act;
act->context      = context;

a->args = args;
a->Case = Case;
a->Default = Default;

sprintf(name,"_switch%p",(void*)a);

act->name      = grib_context_strdup_persistent(context,name);

return act;
}

grib_case* grib_case_new(grib_context* c,grib_arguments* values,grib_action* action)
{
    grib_case* Case = (grib_case*)grib_context_malloc_clear_persistent(c,sizeof(grib_case));

    Case->values=values;
    Case->action=action;

    return Case;
}

static int execute(grib_action* act, grib_handle *h) {
    grib_action_switch* a = (grib_action_switch*)act;
    grib_case* c=a->Case;
    grib_action* next = a->Default;
    grib_arguments* args=a->args;
    grib_arguments* values;
    grib_expression* e;
    grib_expression* value;
    int ret = 0;
    long lres=0;
    double dres=0;
    long lval=0;
    double dval=0;
    int type=0;
    int  ok=0;
    const char *cval;
    const char *cres;
    char buf[80];
    char tmp[80];
    size_t len = sizeof(buf);
    size_t size=sizeof(tmp);
    int err=0;

    Assert(args);

    while (c) {
        e=args->expression;
        values=c->values;
        value=values->expression;
        ok=0;
        while (e && value) {
            if (!strcmp(value->cclass->name,"true"))  ok=1;
            else {
                type=grib_expression_native_type(h,value);

                switch(type) {
                case GRIB_TYPE_LONG:
                    ok= ( grib_expression_evaluate_long(h,value,&lres) == GRIB_SUCCESS) &&
                    ( grib_expression_evaluate_long(h,e,&lval) == GRIB_SUCCESS) &&
                    (lval == lres);
                    break;

                case GRIB_TYPE_DOUBLE:
                    ok = ( grib_expression_evaluate_double(h,value,&dres) == GRIB_SUCCESS) &&
                    ( grib_expression_evaluate_double(h,e,&dval) == GRIB_SUCCESS) &&
                    (dval == dres);
                    break;

                case GRIB_TYPE_STRING:
                    len = sizeof(buf);
                    size=sizeof(tmp);
                    ok = ((cres=grib_expression_evaluate_string(h,e,buf,&len,&err)) != NULL ) &&
                            (err==0) && ((cval = grib_expression_evaluate_string(h,value,tmp,&size,&err)) != NULL) &&
                            (err==0) && ((strcmp(buf,cval) == 0) || (strcmp(cval,"*")==0));
                    break;

                default:
                    /* TODO: */
                    break;
                }
            }
            if (!ok) break;

            args=args->next;
            if (args) e=args->expression;
            else e=NULL;

            values=values->next;
            if (values) value=values->expression;
            else value=NULL;
        }

        if(ok) { next=c->action; break; }

        c=c->next;
    }

    if (!next) return GRIB_SWITCH_NO_MATCH;

    while(next){
        ret = grib_action_execute(next, h);
        if(ret != GRIB_SUCCESS) return ret;
        next= next->next;
    }

    return GRIB_SUCCESS;
}

static void destroy(grib_context* context,grib_action* act)
{
    grib_action_switch* a = (grib_action_switch*) act;
    grib_case *t = a->Case;

    while(t)
    {
        grib_case *nt = t->next;
        grib_action_delete(context,t->action);
        grib_arguments_free(context,t->values);
        grib_context_free(context,t);
        t = nt;
    }

    grib_action_delete(context,a->Default);

    grib_context_free_persistent(context, act->name);
    grib_context_free_persistent(context, act->op);
}

static void xref(grib_action* d, FILE* f,const char* path)
{
}
