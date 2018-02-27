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
void grib_darray_print(const char* title, const grib_darray* darray)
{
    size_t i;
    Assert(darray);
    printf("%s: darray.n=%lu  \t", title, (unsigned long)darray->n);
    for (i=0; i<darray->n; i++) {
        printf("darray[%lu]=%g\t", (unsigned long)i, darray->v[i]);
    }
    printf("\n");
}

grib_darray* grib_darray_new_from_array(grib_context* c,double* a,size_t size)
{
  size_t i;
  grib_darray* v;

  if (!c) c=grib_context_get_default();

  v=grib_darray_new(c,size,100);
  for (i=0;i<size;i++) v->v[i]=a[i];
  v->n=size;
  v->context=c;
  return v;
}

grib_darray* grib_darray_new(grib_context* c,size_t size,size_t incsize) {
  grib_darray* v=NULL;
  if (!c) c=grib_context_get_default();
  v=(grib_darray*)grib_context_malloc_clear(c,sizeof(grib_darray));
  if (!v) {
    grib_context_log(c,GRIB_LOG_ERROR,
          "grib_darray_new unable to allocate %d bytes\n",sizeof(grib_darray));
    return NULL;
  }
  v->size=size;
  v->n=0;
  v->incsize=incsize;
  v->v=(double*)grib_context_malloc_clear(c,sizeof(double)*size);
  if (!v->v) {
    grib_context_log(c,GRIB_LOG_ERROR,
          "grib_darray_new unable to allocate %d bytes\n",sizeof(double)*size);
    return NULL;
  }
  return v;
}

grib_darray* grib_darray_resize(grib_context* c,grib_darray* v) {
  int newsize=v->incsize+v->size;

  if (!c) c=grib_context_get_default();

  v->v = (double*)grib_context_realloc(c,v->v,newsize*sizeof(double));
  v->size=newsize;
  if (!v->v) {
    grib_context_log(c,GRIB_LOG_ERROR,
          "grib_darray_resize unable to allocate %d bytes\n",sizeof(double)*newsize);
    return NULL;
  }
  return v;
}

grib_darray* grib_darray_push(grib_context* c,grib_darray* v,double val) {
  size_t start_size=100;
  size_t start_incsize=100;
  if (!v) v=grib_darray_new(c,start_size,start_incsize);

  if (v->n >= v->size) v=grib_darray_resize(c,v);
  v->v[v->n]=val;
  v->n++;
  return v;
}

void grib_darray_delete(grib_context* c,grib_darray* v) {
  if (!v) return;
  if (!c) grib_context_get_default();
  if (v->v) grib_context_free(c,v->v);
  grib_context_free(c,v);
}

double* grib_darray_get_array(grib_context* c,grib_darray* v) {
  double* ret;
  int i;
  if (!v) return NULL;
  ret=(double*)grib_context_malloc_clear(c,sizeof(double)*v->n);
  for (i=0;i<v->n;i++) ret[i]=v->v[i];
  return ret;
}

int grib_darray_is_constant(grib_darray* v,double eps) {
  int i;
  double val;
  if (v->n == 1) return 1;

  val=v->v[0];
  for (i=1;i<v->n;i++)  {
    if (fabs(val-v->v[i]) > eps) return 0;
  }
  return 1;
}

size_t grib_darray_used_size(grib_darray* v) { return v->n;}
