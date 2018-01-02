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
 *
 *   Enrico Fucile
 *
 ***************************************************************************/

#include "grib_api_internal.h"

grib_oarray* grib_oarray_new(grib_context* c,size_t size,size_t incsize)
{
    grib_oarray* v=NULL;
    if (!c) c=grib_context_get_default();
    v=(grib_oarray*)grib_context_malloc_clear(c,sizeof(grib_oarray));
    if (!v) {
        grib_context_log(c,GRIB_LOG_ERROR,
                "grib_oarray_new unable to allocate %d bytes\n",sizeof(grib_oarray));
        return NULL;
    }
    v->size=size;
    v->n=0;
    v->incsize=incsize;
    v->v=(void**)grib_context_malloc_clear(c,sizeof(char*)*size);
    if (!v->v) {
        grib_context_log(c,GRIB_LOG_ERROR,
                "grib_oarray_new unable to allocate %d bytes\n",sizeof(char*)*size);
        return NULL;
    }
    return v;
}

grib_oarray* grib_oarray_resize(grib_context* c,grib_oarray* v)
{
    int newsize=v->incsize+v->size;

    if (!c) c=grib_context_get_default();

    v->v=(void**)grib_context_realloc(c,v->v,newsize*sizeof(char*));
    v->size=newsize;
    if (!v->v) {
        grib_context_log(c,GRIB_LOG_ERROR,
                "grib_oarray_resize unable to allocate %d bytes\n",sizeof(char*)*newsize);
        return NULL;
    }
    return v;
}

grib_oarray* grib_oarray_push(grib_context* c,grib_oarray* v,void* val)
{
    size_t start_size=100;
    size_t start_incsize=100;
    if (!v) v=grib_oarray_new(c,start_size,start_incsize);

    if (v->n >= v->size) v=grib_oarray_resize(c,v);
    v->v[v->n]=val;
    v->n++;
    return v;
}

void grib_oarray_delete(grib_context* c,grib_oarray* v)
{
    if (!v) return;
    if (!c) grib_context_get_default();
    if (v->v) grib_context_free(c,v->v);
    grib_context_free(c,v);
}

void grib_oarray_delete_content(grib_context* c,grib_oarray* v)
{
    int i;
    if (!v | !v->v) return;
    if (!c) grib_context_get_default();
    for (i=0;i<v->n;i++) {
        if (v->v[i]) grib_context_free(c,v->v[i]);
        v->v[i]=0;
    }
    v->n=0;
}

void** grib_oarray_get_array(grib_context* c,grib_oarray* v)
{
    void** ret;
    int i;
    if (!v) return NULL;
    ret=(void**)grib_context_malloc_clear(c,sizeof(char*)*v->n);
    for (i=0;i<v->n;i++) ret[i]=v->v[i];
    return ret;
}

void* grib_oarray_get(grib_oarray* v,int i)
{
    if (v==NULL || i>v->n-1) return NULL;
    return v->v[i];
}

size_t grib_oarray_used_size(grib_oarray* v)
{
    return v->n;
}
