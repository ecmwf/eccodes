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

/* For debugging purposes */
void grib_vdarray_print(const char* title, const grib_vdarray* vdarray)
{
    size_t i;
    char text[100]={0,};
    Assert(vdarray);
    printf("%s: vdarray.n=%lu\n", title, (unsigned long)vdarray->n);
    for (i=0; i<vdarray->n; i++) {
        sprintf(text, " vdarray->v[%lu]", (unsigned long)i);
        grib_darray_print(text, vdarray->v[i]);
    }
    printf("\n");
}

grib_vdarray* grib_vdarray_new(grib_context* c,size_t size,size_t incsize)
{
    grib_vdarray* v=NULL;
    if (!c) c=grib_context_get_default();
    v=(grib_vdarray*)grib_context_malloc_clear(c,sizeof(grib_vdarray));
    if (!v) {
        grib_context_log(c,GRIB_LOG_ERROR,
                "grib_vdarray_new unable to allocate %d bytes\n",sizeof(grib_vdarray));
        return NULL;
    }
    v->size=size;
    v->n=0;
    v->incsize=incsize;
    v->v=(grib_darray**)grib_context_malloc_clear(c,sizeof(grib_darray*)*size);
    if (!v->v) {
        grib_context_log(c,GRIB_LOG_ERROR,
                "grib_vdarray_new unable to allocate %d bytes\n",sizeof(grib_darray*)*size);
        return NULL;
    }
    return v;
}

grib_vdarray* grib_vdarray_resize(grib_context* c,grib_vdarray* v)
{
    int newsize=v->incsize+v->size;

    if (!c) c=grib_context_get_default();

    v->v=(grib_darray**)grib_context_realloc(c,v->v,newsize*sizeof(grib_darray*));
    v->size=newsize;
    if (!v->v) {
        grib_context_log(c,GRIB_LOG_ERROR,
                "grib_vdarray_resize unable to allocate %d bytes\n",sizeof(grib_darray*)*newsize);
        return NULL;
    }
    return v;
}

grib_vdarray* grib_vdarray_push(grib_context* c,grib_vdarray* v,grib_darray* val)
{
    size_t start_size=100;
    size_t start_incsize=100;
    if (!v) v=grib_vdarray_new(c,start_size,start_incsize);

    if (v->n >= v->size) v=grib_vdarray_resize(c,v);
    v->v[v->n]=val;
    v->n++;
    return v;
}

void grib_vdarray_delete(grib_context* c,grib_vdarray* v)
{
    if (!v) return;
    if (!c) grib_context_get_default();
    if (v->v) grib_context_free(c,v->v);
    grib_context_free(c,v);
}

void grib_vdarray_delete_content(grib_context* c,grib_vdarray* v)
{
    int i;
    if (!v || !v->v) return;
    if (!c) grib_context_get_default();
    for (i=0;i<v->n;i++) {
        grib_darray_delete(c,v->v[i]);
        v->v[i]=0;
    }
    v->n=0;
}

grib_darray** grib_vdarray_get_array(grib_context* c,grib_vdarray* v)
{
    grib_darray** ret;
    int i;
    if (!v) return NULL;
    ret=(grib_darray**)grib_context_malloc_clear(c,sizeof(grib_darray*)*v->n);
    for (i=0;i<v->n;i++) ret[i]=v->v[i];
    return ret;
}

size_t grib_vdarray_used_size(grib_vdarray* v) { return v->n;}
