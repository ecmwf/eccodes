/*
 * Copyright 2005-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include <stdarg.h>
#include "grib_api_internal.h"

int grib_expression_native_type(grib_handle* h,grib_expression* g)
{
    grib_expression_class *c = g->cclass;
    while(c)
    {
        if(c->native_type)
            return c->native_type(g,h);
        c = c->super ? *(c->super) : NULL;
    }
    grib_context_log(h->context,GRIB_LOG_ERROR, "No native_type() in %s\n",g->cclass->name);
    Assert(1==0);
    return 0;
}

int grib_expression_evaluate_long(grib_handle* h,grib_expression* g,long* result)
{
    grib_expression_class *c = g->cclass;
    while(c)
    {
        if(c->evaluate_long)
            return c->evaluate_long(g,h,result);
        c = c->super ? *(c->super) : NULL;
    }
    return GRIB_INVALID_TYPE;
}

int grib_expression_evaluate_double(grib_handle* h,grib_expression* g,double* result)
{
    grib_expression_class *c = g->cclass;
    while(c)
    {
        if(c->evaluate_double)
            return c->evaluate_double(g,h,result);
        c = c->super ? *(c->super) : NULL;
    }
    return GRIB_INVALID_TYPE;
}

const char* grib_expression_evaluate_string(grib_handle* h,grib_expression* g,char* buf, size_t* size, int *err)
{
    grib_expression_class *c = g->cclass;
    while(c)
    {
        if(c->evaluate_string)
            return c->evaluate_string(g,h,buf,size,err);
        c = c->super ? *(c->super) : NULL;
    }
    grib_context_log(h->context,GRIB_LOG_ERROR, "No evaluate_string() in %s\n",g->cclass->name);
    *err=GRIB_INVALID_TYPE;

    return 0;
}

const char* grib_expression_get_name(grib_expression* g)
{
    grib_expression_class *c = g->cclass;
    while(c)
    {
        if(c->get_name)
            return c->get_name(g);
        c = c->super ? *(c->super) : NULL;
    }
    printf("No expression_get_name() in %s\n",g->cclass->name);
    Assert(1==0);
    return 0;
}

void grib_expression_print(grib_context* ctx,grib_expression* g,grib_handle* f)
{
    grib_expression_class *c = g->cclass;
    while(c)
    {
        if(c->print)
        {
            c->print(ctx,g,f);
            return;
        }
        c = c->super ? *(c->super) : NULL;
    }
    Assert(1==0);
}

void grib_expression_compile(grib_expression* g,grib_compiler* f)
{
    grib_expression_class *c = g->cclass;
    if(!c->compile)
    {
        fprintf(stderr, "NO COMPILE METHOD %s\n", c->name);
        Assert(0);
    }
    while(c)
    {
        if(c->compile)
        {
            c->compile(g,f);
            return;
        }
        c = c->super ? *(c->super) : NULL;
    }
    Assert(1==0);

}

void grib_expression_free(grib_context* ctx,grib_expression* g)
{
    grib_expression_class *c = g->cclass;
    while(c)
    {
        if(c->destroy)
            c->destroy(ctx,g);
        c = c->super ? *(c->super) : NULL;
    }
    grib_context_free_persistent(ctx,g);
}

void grib_expression_add_dependency(grib_expression* e, grib_accessor* observer)
{
    grib_expression_class *c = e->cclass;
    while(c)
    {
        if(c->add_dependency)
        {
            c->add_dependency(e,observer);
            return ;
        }
        c = c->super ? *(c->super) : NULL;
    }
    Assert(1==0);
}

/*----------------------------------------*/
int grib_expression_set_value(grib_handle* h,grib_expression* g,grib_values* v)
{
    char buffer[1024];
    int ret=0;
    size_t size = sizeof(buffer);

    switch(v->type = grib_expression_native_type(h,g))
    {
    case GRIB_TYPE_LONG:
        return grib_expression_evaluate_long(h,g,&v->long_value);
        break;

    case GRIB_TYPE_DOUBLE:
        return grib_expression_evaluate_double(h,g,&v->double_value);
        break;

    case GRIB_TYPE_STRING:
        v->string_value = grib_expression_evaluate_string(h,g,buffer,&size,&ret);
        if (ret != GRIB_SUCCESS) {
            grib_context_log(h->context,GRIB_LOG_ERROR,
                    "grib_expression_set_value: unable to evaluate %s as string",
                    grib_expression_get_name( g));
            return ret;
        }
        Assert(v->string_value != buffer);
        Assert(v->string_value);
        break;

    default:
        Assert(1 == 0);
        break;
    }
    return 0;
}

/*----------------------------------------*/
grib_arguments *grib_arguments_new(grib_context* c,grib_expression* g, grib_arguments* n)
{
    grib_arguments* l = (grib_arguments*)grib_context_malloc_clear_persistent(c,sizeof(grib_arguments));
    l->expression = g;
    l->next       = n;
    return l;
}

void grib_arguments_free(grib_context* c,grib_arguments* g)
{
    if(g) {
        grib_arguments_free(c,g->next);
        grib_expression_free(c,g->expression);
        grib_context_free_persistent(c,g);
    }
}

void grib_arguments_print(grib_context* c,grib_arguments* g,grib_handle* f)
{
    if(g) {
        if(g->expression)
            grib_expression_print(c,g->expression,f);
        if(g->next)
        {
            printf(",");
            grib_arguments_print(c,g->next,f);
        }
    }
}

const char *grib_arguments_get_name(grib_handle *h, grib_arguments *args, int n)
{
    grib_expression *e = NULL;
    while(args && n-->0) {args = args->next;}

    if(!args) return NULL;

    e = args->expression;
    return e ? grib_expression_get_name(e) : NULL;
}

const char* grib_arguments_get_string(grib_handle* h,grib_arguments* args,int n)
{
    grib_expression *e = NULL;
    int ret=0;
    while(args && n-->0) {args = args->next;}

    if(!args) return NULL;

    e = args->expression;
    return grib_expression_evaluate_string(h,e,NULL,NULL,&ret);
}

long grib_arguments_get_long(grib_handle* h,grib_arguments* args,int n)
{
    int ret=0;
    long lres=0;
    grib_expression *e = NULL;
    while(args && n-->0) {args = args->next;}

    if(!args) return 0;

    e = args->expression;
    ret = grib_expression_evaluate_long(h,e,&lres);
    (void)ret;
    return lres;
}

double grib_arguments_get_double(grib_handle* h,grib_arguments* args,int n)
{
    int ret=0;
    double dres=0.0;

    grib_expression *e = NULL;
    while(args && n-->0) {args = args->next;}

    if(!args) return 0;

    e = args->expression;
    ret = grib_expression_evaluate_double(h,e,&dres);
    (void)ret;
    return dres;
}

grib_expression* grib_arguments_get_expression(grib_handle* h,grib_arguments* args,int n)
{
    while(args && n-->0) {args = args->next;}

    if(!args) return 0;

    return args->expression;
}
