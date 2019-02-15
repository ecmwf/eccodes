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

#if GRIB_PTHREADS
static pthread_once_t once  = PTHREAD_ONCE_INIT;
static pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;

static void init_mutex() {
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr,PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&mutex1,&attr);
    pthread_mutexattr_destroy(&attr);

}
#elif GRIB_OMP_THREADS
static int once = 0;
static omp_nest_lock_t mutex1;

static void init_mutex()
{
    GRIB_OMP_CRITICAL(lock_action_c)
    {
        if (once == 0)
        {
            omp_init_nest_lock(&mutex1);
            once = 1;
        }
    }
}
#endif


static void init(grib_action_class *c)
{
    GRIB_MUTEX_INIT_ONCE(&once,&init_mutex);
    GRIB_MUTEX_LOCK(&mutex1);
    if(c && !c->inited)
    {
        init(c->super ? *(c->super) : NULL);
        c->init_class(c);
        c->inited = 1;
    }
    GRIB_MUTEX_UNLOCK(&mutex1);
}

void grib_dump(grib_action* a, FILE* f, int l)
{
    grib_action_class *c = a->cclass;
    init(c);

    while(c)
    {
        if(c->dump)
        {
            c->dump(a, f, l);
            return;
        }
        c = c->super ? *(c->super) : NULL;
    }
    Assert(0);
}

void grib_xref(grib_action* a, FILE* f,const char* path)
{
    grib_action_class *c = a->cclass;
    init(c);

    while(c)
    {
        if(c->xref)
        {
            c->xref(a, f,path);
            return;
        }
        c = c->super ? *(c->super) : NULL;
    }
    printf("xref not implemented for %s\n",a->cclass->name);
    Assert(0);
}


void grib_action_delete(grib_context* context,grib_action* a)
{
    grib_action_class *c = a->cclass;
    init(c);
    while(c)
    {
        if(c->destroy)
            c->destroy(context, a);
        c = c->super ? *(c->super) : NULL;
    }
    grib_context_free_persistent(context, a);
}

int grib_create_accessor(grib_section* p, grib_action* a,  grib_loader* h)
{
    grib_action_class *c = a->cclass;
    init(c);
    while(c)
    {
        if(c->create_accessor) {
            int ret;
            GRIB_MUTEX_INIT_ONCE(&once,&init_mutex);
            GRIB_MUTEX_LOCK(&mutex1);
            ret=c->create_accessor(p, a, h);
            GRIB_MUTEX_UNLOCK(&mutex1);
            return ret;
        }
        c = c->super ? *(c->super) : NULL;
    }
    fprintf(stderr,"Cannot create accessor %s %s\n",a->name,a->cclass->name);
    Assert(0);
    return 0;
}

int grib_action_notify_change( grib_action* a, grib_accessor *observer, grib_accessor *observed)
{
    grib_action_class *c = a->cclass;

    GRIB_MUTEX_INIT_ONCE(&once,&init_mutex);
    GRIB_MUTEX_LOCK(&mutex1);

    init(c);
    while(c)
    {
        if(c->notify_change) {
            int result = c->notify_change(a,observer,observed);
            GRIB_MUTEX_UNLOCK(&mutex1);
            return result;
        }
        c = c->super ? *(c->super) : NULL;
    }
    GRIB_MUTEX_UNLOCK(&mutex1);
    Assert(0);
    return 0;
}

grib_action* grib_action_reparse( grib_action* a, grib_accessor* acc,int* doit)
{
    grib_action_class *c = a->cclass;
    init(c);
    while(c)
    {
        if(c->reparse)
            return c->reparse(a,acc,doit);
        c = c->super ? *(c->super) : NULL;
    }
    Assert(0);
    return 0;
}

int grib_action_execute( grib_action* a, grib_handle* h)
{
    grib_action_class *c = a->cclass;
    init(c);
    while(c)
    {
        if(c->execute)
            return c->execute(a,h);
        c = c->super ? *(c->super) : NULL;
    }
    Assert(0);
    return 0;
}

void grib_dump_action_branch(FILE* out,grib_action* a, int decay)
{
    while(a)
    {
        grib_dump(a,out,decay);
        a=a->next;
    }
}

void grib_dump_action_tree( grib_context* ctx,FILE* out)
{
    grib_dump_action_branch( out, ctx->grib_reader->first->root ,0);
}

void grib_xref_action_branch(FILE* out,grib_action* a,const char* path)
{
    while(a)
    {
        grib_xref(a,out,path);
        a=a->next;
    }
}

