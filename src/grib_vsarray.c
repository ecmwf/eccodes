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

grib_vsarray* grib_vsarray_new(grib_context* c,size_t size,size_t incsize)
{
    grib_vsarray* v=NULL;
    if (!c) c=grib_context_get_default();
    v=(grib_vsarray*)grib_context_malloc_clear(c,sizeof(grib_vsarray));
    if (!v) {
        grib_context_log(c,GRIB_LOG_ERROR,
                "grib_vsarray_new unable to allocate %d bytes\n",sizeof(grib_vsarray));
        return NULL;
    }
    v->size=size;
    v->n=0;
    v->incsize=incsize;
    v->v=(grib_sarray**)grib_context_malloc_clear(c,sizeof(grib_sarray*)*size);
    if (!v->v) {
        grib_context_log(c,GRIB_LOG_ERROR,
                "grib_vsarray_new unable to allocate %d bytes\n",sizeof(grib_sarray*)*size);
        return NULL;
    }
    return v;
}

grib_vsarray* grib_vsarray_resize(grib_context* c,grib_vsarray* v)
{
    int newsize=v->incsize+v->size;

    if (!c) c=grib_context_get_default();

    v->v=(grib_sarray**)grib_context_realloc(c,v->v,newsize*sizeof(grib_sarray*));
    v->size=newsize;
    if (!v->v) {
        grib_context_log(c,GRIB_LOG_ERROR,
                "grib_vsarray_resize unable to allocate %d bytes\n",sizeof(grib_sarray*)*newsize);
        return NULL;
    }
    return v;
}

grib_vsarray* grib_vsarray_push(grib_context* c,grib_vsarray* v,grib_sarray* val)
{
    size_t start_size=100;
    size_t start_incsize=100;
    if (!v) v=grib_vsarray_new(c,start_size,start_incsize);

    if (v->n >= v->size) v=grib_vsarray_resize(c,v);
    v->v[v->n]=val;
    v->n++;
    return v;
}

void grib_vsarray_delete(grib_context* c,grib_vsarray* v)
{
    if (!v) return;
    if (!c) grib_context_get_default();
    if (v->v) grib_context_free(c,v->v);
    grib_context_free(c,v);
}

void grib_vsarray_delete_content(grib_context* c,grib_vsarray* v)
{
    int i;
    if (!v || !v->v) return;
    if (!c) grib_context_get_default();
    for (i=0;i<v->n;i++) {
        grib_sarray_delete_content(c,v->v[i]);
        grib_sarray_delete(c,v->v[i]);
        v->v[i]=0;
    }
    v->n=0;
}

grib_sarray** grib_vsarray_get_array(grib_context* c,grib_vsarray* v)
{
    grib_sarray** ret;
    int i;
    if (!v) return NULL;
    ret=(grib_sarray**)grib_context_malloc_clear(c,sizeof(grib_sarray*)*v->n);
    for (i=0;i<v->n;i++) ret[i]=v->v[i];
    return ret;
}

size_t grib_vsarray_used_size(grib_vsarray* v) { return v->n;}
