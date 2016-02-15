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
 *
 *   Enrico Fucile
 *
 ***************************************************************************/

#include "grib_api_internal.h"

grib_iarray* grib_iarray_new_from_array(grib_context* c,long* a,size_t size)
{
    size_t i;
    grib_iarray* v;

    if (!c) c=grib_context_get_default();

    v=grib_iarray_new(c,size,100);
    for (i=0;i<size;i++) v->v[i]=a[i];
    v->n=size;
    v->number_of_pop_front=0;
    v->context=c;
    return v;
}

grib_iarray* grib_iarray_new(grib_context* c,size_t size,size_t incsize)
{
    grib_iarray* v=NULL;

    if (!c) c=grib_context_get_default();

    v=(grib_iarray*)grib_context_malloc(c,sizeof(grib_iarray));
    if (!v) {
        grib_context_log(c,GRIB_LOG_ERROR,
                "grib_iarray_new unable to allocate %d bytes\n",sizeof(grib_iarray));
        return NULL;
    }
    v->context=c;
    v->size=size;
    v->n=0;
    v->incsize=incsize;
    v->v=(long*)grib_context_malloc(c,sizeof(long)*size);
    v->number_of_pop_front=0;
    if (!v->v) {
        grib_context_log(c,GRIB_LOG_ERROR,
                "grib_iarray_new unable to allocate %d bytes\n",sizeof(long)*size);
        return NULL;
    }
    return v;
}

long grib_iarray_pop(grib_iarray* a)
{
    a->n-=1;
    return a->v[a->n];
}

long grib_iarray_pop_front(grib_iarray* a)
{
    long v=a->v[0];
    /* size_t i=0; */
    if (a->n==0) Assert(0);
    a->n--;
    a->v++;
    a->number_of_pop_front++;
    /* for (i=0;i<a->n;i++) a->v[i]=a->v[i+1]; */

    return v;
}

grib_iarray* grib_iarray_resize_to(grib_iarray* v,size_t newsize)
{
    long* newv;
    size_t i;
    grib_context* c=v->context;

    if (newsize<v->size) return v;

    if (!c) c=grib_context_get_default();

    newv=(long*)grib_context_malloc_clear(c,newsize*sizeof(long));
    if (!newv) {
        grib_context_log(c,GRIB_LOG_ERROR,
                "grib_iarray_resize unable to allocate %d bytes\n",sizeof(long)*newsize);
        return NULL;
    }

    for (i=0;i<v->n;i++) newv[i]=v->v[i];

    v->v-=v->number_of_pop_front;
    grib_context_free(c,v->v);

    v->v=newv;
    v->size=newsize;
    v->number_of_pop_front=0;

    return v;
}

grib_iarray* grib_iarray_resize(grib_iarray* v)
{
    int newsize=v->incsize+v->size;

    return grib_iarray_resize_to(v,newsize);
}

grib_iarray* grib_iarray_push(grib_iarray* v,long val)
{
    size_t start_size=100;
    size_t start_incsize=100;

    if (!v) v=grib_iarray_new(0,start_size,start_incsize);

    if (v->n >= v->size-v->number_of_pop_front)
        v=grib_iarray_resize(v);

    v->v[v->n]=val;
    v->n++;
    return v;
}

grib_iarray* grib_iarray_push_front(grib_iarray* v,long val)
{
    size_t start_size=100;
    size_t start_incsize=100;
    int i;
    if (!v) v=grib_iarray_new(0,start_size,start_incsize);

    if (v->number_of_pop_front) {
        v->v--;
        v->number_of_pop_front--;
    } else {
        if (v->n >= v->size) v=grib_iarray_resize(v);
        for (i=v->n;i>0;i--) v[i]=v[i-1];
    }
    v->v[0]=val;
    v->n++;

    return v;
}

grib_iarray* grib_iarray_push_array(grib_iarray* v,long *val,size_t size)
{
    size_t start_size=size;
    size_t start_incsize=100;
    long* vp=0;
    long* valp=val;
    if (!v) v=grib_iarray_new(0,start_size,start_incsize);

    v=grib_iarray_resize_to(v,size+v->n);
    vp=v->v+v->n+v->number_of_pop_front;
    v->n+=size;
    while (size) {
        *(vp++)=*(valp++);
        size--;
    }
    return v;
}

long grib_iarray_get(grib_iarray* a,size_t i)
{
    return a->v[i];
}

void grib_iarray_set(grib_iarray* a,size_t i,long v)
{
    a->v[i]=v;
}

void grib_iarray_delete(grib_iarray* v)
{
    grib_context* c;

    if (!v) return;
    c=v->context;

    grib_iarray_delete_array(v);

    grib_context_free(c,v);
}

void grib_iarray_delete_array(grib_iarray* v)
{
    grib_context* c;

    if (!v) return;
    c=v->context;

    if (v->v) {
        long* vv = v->v - v->number_of_pop_front;
        grib_context_free(c,vv);
    }
}

long* grib_iarray_get_array(grib_iarray* v)
{
    long* vv;
    size_t i;
    grib_context* c=grib_context_get_default();

    vv=(long*)grib_context_malloc_clear(c,sizeof(long)*v->n);
    for (i=0;i<v->n;i++) vv[i]=v->v[i];

    return vv;
}

size_t grib_iarray_used_size(grib_iarray* v) {return v==NULL ? 0 : v->n;}
