/*
 * Copyright 2005-2019 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

/**************************************
 *  Enrico Fucile
 **************************************/
#include "grib_api_internal.h"

#if GRIB_PTHREADS
static pthread_once_t once  = PTHREAD_ONCE_INIT;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

static void init_mutex() {
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr,PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&mutex,&attr);
    pthread_mutexattr_destroy(&attr);
}
#elif GRIB_OMP_THREADS
static int once = 0;
static omp_nest_lock_t mutex;

static void init_mutex()
{
    GRIB_OMP_CRITICAL(lock_dumper_c)
    {
        if (once == 0)
        {
            omp_init_nest_lock(&mutex);
            once = 1;
        }
    }
}
#endif

static void init_dumpers(grib_dumper_class* c,grib_dumper* d)
{
    GRIB_MUTEX_INIT_ONCE(&once,&init_mutex);
    GRIB_MUTEX_LOCK(&mutex);
    if(c) {
        grib_dumper_class *s = c->super ? *(c->super) : NULL;
        if(!c->inited)
        {
            if(c->init_class) c->init_class(c);
            c->inited = 1;
        }
        init_dumpers(s,d);
        if(c->init) c->init(d);
    }
    GRIB_MUTEX_UNLOCK(&mutex);
}

void grib_init_dumper(grib_dumper* d)
{
    init_dumpers(d->cclass,d);
}

void grib_dumper_delete(grib_dumper* d)
{
    grib_dumper_class *c = d->cclass;
    grib_context *ctx = d->handle->context;
    while(c)
    {
        grib_dumper_class *s = c->super ? *(c->super) : NULL;
        if(c->destroy) c->destroy(d);
        c = s;
    }
    grib_context_free(ctx,d);
}

void  grib_dump_long(grib_dumper* d, grib_accessor* a, const char* comment)
{
    grib_dumper_class *c = d->cclass;
    while(c)
    {
        if(c->dump_long)
        {
            c->dump_long(d, a, comment);
            return;
        }
        c = c->super ? *(c->super) : NULL;
    }
    Assert(0);
}

void  grib_dump_double(grib_dumper* d,grib_accessor* a,const char* comment)
{
    grib_dumper_class *c = d->cclass;
    while(c)
    {
        if(c->dump_double)
        {
            c->dump_double(d, a, comment);
            return;
        }
        c = c->super ? *(c->super) : NULL;
    }
    Assert(0);
}

void  grib_dump_string(grib_dumper* d,grib_accessor* a,const char* comment)
{
    grib_dumper_class *c = d->cclass;
    while(c)
    {
        if(c->dump_string)
        {
            c->dump_string(d, a, comment);
            return;
        }
        c = c->super ? *(c->super) : NULL;
    }
    Assert(0);
}

void  grib_dump_string_array(grib_dumper* d,grib_accessor* a,const char* comment)
{
    grib_dumper_class *c = d->cclass;
    while(c)
    {
        if(c->dump_string_array)
        {
            c->dump_string_array(d, a, comment);
            return;
        }
        c = c->super ? *(c->super) : NULL;
    }
    Assert(0);
}

void  grib_dump_label(grib_dumper* d,grib_accessor* a,const char* comment)
{
    grib_dumper_class *c = d->cclass;
    while(c)
    {
        if(c->dump_label)
        {
            c->dump_label(d, a, comment);
            return;
        }
        c = c->super ? *(c->super) : NULL;
    }
    Assert(0);
}

void  grib_dump_bytes(grib_dumper* d,grib_accessor* a,const char* comment)
{
    grib_dumper_class *c = d->cclass;
    while(c)
    {
        if(c->dump_bytes)
        {
            c->dump_bytes(d, a, comment);
            return;
        }
        c = c->super ? *(c->super) : NULL;
    }
    Assert(0);
}

void  grib_dump_bits(grib_dumper* d,grib_accessor* a,const char* comment)
{
    grib_dumper_class *c = d->cclass;
    while(c)
    {
        if(c->dump_bits)
        {
            c->dump_bits(d, a, comment);
            return;
        }
        c = c->super ? *(c->super) : NULL;
    }
    Assert(0);
}

void  grib_dump_section(grib_dumper* d,grib_accessor* a,grib_block_of_accessors* block)
{
    grib_dumper_class *c = d->cclass;
    while(c)
    {
        if(c->dump_section)
        {
            c->dump_section(d, a, block);
            return;
        }
        c = c->super ? *(c->super) : NULL;
    }
    Assert(0);
}

void  grib_dump_values(grib_dumper* d,grib_accessor* a)
{
    grib_dumper_class *c = d->cclass;
    while(c)
    {
        if(c->dump_values)
        {
            c->dump_values(d, a);
            return;
        }
        c = c->super ? *(c->super) : NULL;
    }
    Assert(0);
}

void  grib_dump_header(grib_dumper* d, const grib_handle* ch)
{
    grib_handle* h = (grib_handle*)ch;
    grib_dumper_class *c = d->cclass;
    while(c)
    {
        if(c->header)
        {
            c->header(d,h);
            return;
        }
        c = c->super ? *(c->super) : NULL;
    }
}

void  grib_dump_footer(grib_dumper* d, const grib_handle* ch)
{
    grib_handle* h = (grib_handle*)ch;
    grib_dumper_class *c = d->cclass;
    while(c)
    {
        if(c->footer)
        {
            c->footer(d,h);
            return;
        }
        c = c->super ? *(c->super) : NULL;
    }
}
