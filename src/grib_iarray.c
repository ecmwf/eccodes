/*
 * Copyright 2005-2013 ECMWF.
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

grib_iarray* grib_iarray_new(grib_context* c,size_t size,size_t incsize) {
  grib_iarray* v=NULL;
  if (!c) c=grib_context_get_default();
  v=(grib_iarray*)grib_context_malloc(c,sizeof(grib_iarray));
  if (!v) {
    grib_context_log(c,GRIB_LOG_ERROR,
          "grib_iarray_new unable to allocate %d bytes\n",sizeof(grib_iarray));
    return NULL;
  }
  v->size=size;
  v->n=0;
  v->incsize=incsize;
  v->v=(long*)grib_context_malloc(c,sizeof(long)*size);
  if (!v->v) {
    grib_context_log(c,GRIB_LOG_ERROR,
          "grib_iarray_new unable to allocate %d bytes\n",sizeof(long)*size);
    return NULL;
  }
  return v;
}

grib_iarray* grib_iarray_resize(grib_context* c,grib_iarray* v) {
  int newsize=v->incsize+v->size;

  if (!c) c=grib_context_get_default();

  v->v=grib_context_realloc(c,v->v,newsize*sizeof(long));
  v->size=newsize;
  if (!v->v) {
    grib_context_log(c,GRIB_LOG_ERROR,
          "grib_iarray_resize unable to allocate %d bytes\n",sizeof(long)*newsize);
    return NULL;
  }
  return v;
}

grib_iarray* grib_iarray_push(grib_context* c,grib_iarray* v,long val) {
  size_t start_size=100;
  size_t start_incsize=100;
  if (!v) v=grib_iarray_new(c,start_size,start_incsize);

  if (v->n >= v->size) v=grib_iarray_resize(c,v);
  v->v[v->n]=val;
  v->n++;
  return v;
}

void grib_iarray_delete(grib_context* c,grib_iarray* v) {
  if (!v) return;
  if (!c) grib_context_get_default();
  if (v->v) grib_context_free(c,v->v);
  grib_context_free(c,v);
}

