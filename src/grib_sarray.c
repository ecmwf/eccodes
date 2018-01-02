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

grib_sarray* grib_sarray_new(grib_context* c,size_t size,size_t incsize)
{
    grib_sarray* v=NULL;
    if (!c) c=grib_context_get_default();
    v=(grib_sarray*)grib_context_malloc_clear(c,sizeof(grib_sarray));
    if (!v) {
        grib_context_log(c,GRIB_LOG_ERROR,
                "grib_sarray_new unable to allocate %d bytes\n",sizeof(grib_sarray));
        return NULL;
    }
    v->size=size;
    v->n=0;
    v->incsize=incsize;
    v->v=(char**)grib_context_malloc_clear(c,sizeof(char*)*size);
    if (!v->v) {
        grib_context_log(c,GRIB_LOG_ERROR,
                "grib_sarray_new unable to allocate %d bytes\n",sizeof(char*)*size);
        return NULL;
    }
    return v;
}

grib_sarray* grib_sarray_resize(grib_context* c,grib_sarray* v)
{
    int newsize=v->incsize+v->size;

    if (!c) c=grib_context_get_default();

    v->v=(char**)grib_context_realloc(c,v->v,newsize*sizeof(char*));
    v->size=newsize;
    if (!v->v) {
        grib_context_log(c,GRIB_LOG_ERROR,
                "grib_sarray_resize unable to allocate %d bytes\n",sizeof(char*)*newsize);
        return NULL;
    }
    return v;
}

grib_sarray* grib_sarray_push(grib_context* c,grib_sarray* v,char* val)
{
    size_t start_size=100;
    size_t start_incsize=100;
    if (!v) v=grib_sarray_new(c,start_size,start_incsize);

    if (v->n >= v->size) v=grib_sarray_resize(c,v);
    v->v[v->n]=val;
    v->n++;
    return v;
}

void grib_sarray_delete(grib_context* c,grib_sarray* v)
{
    if (!v) return;
    if (!c) grib_context_get_default();
    if (v->v) grib_context_free(c,v->v);
    grib_context_free(c,v);
}

void grib_sarray_delete_content(grib_context* c,grib_sarray* v)
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

char** grib_sarray_get_array(grib_context* c,grib_sarray* v)
{
    char** ret;
    int i;
    if (!v) return NULL;
    ret=(char**)grib_context_malloc_clear(c,sizeof(char*)*v->n);
    for (i=0;i<v->n;i++) ret[i]=v->v[i];
    return ret;
}

size_t grib_sarray_used_size(grib_sarray* v) { return v->n;}
