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
 *   maf@agnoman                                                           *
 *                                                                         *
 ***************************************************************************/
#include "grib_api_internal.h"

int  grib_compressor_pack  (grib_compressor* cmp, const double* vals, size_t valslen, void*  buf, size_t* buflen){
  grib_compressor_class *c = cmp->cclass;
  while(c)
  {
    grib_compressor_class *s = c->super ? *(c->super) : NULL;
    if(c->pack) return  c->pack(cmp,vals,valslen,buf,buflen);
    c = s;
  }
  Assert(0);
  return 0;
}
int   grib_compressor_unpack(grib_compressor* cmp, const void*   buf, size_t buflen, double* vals, size_t* valslen){

  grib_compressor_class *c = cmp->cclass;
  while(c)
  {
    grib_compressor_class *s = c->super ? *(c->super) : NULL;
    if(c->unpack) return  c->unpack(cmp,buf,buflen,vals,valslen);
    c = s;
  }
  Assert(0);
  return 0;

}

/* For this one, ALL init are called */

static int init_compressor(grib_compressor_class* c,grib_compressor* cmp, grib_handle *h, grib_arguments* args)
{

  if(c) {
    int ret = GRIB_SUCCESS;
    grib_compressor_class *s = c->super ? *(c->super) : NULL;
    if(!c->inited)
    {
      if(c->init_class) c->init_class(c);
      c->inited = 1;
    }
    if(s) ret = init_compressor(s,cmp,h,args);

    if(ret != GRIB_SUCCESS) return ret;

    if(c->init) return c->init(cmp,h, args);
  }
  return GRIB_INTERNAL_ERROR;
}

int grib_compressor_init(grib_compressor* i, grib_handle *h, grib_arguments* args)
{
  return init_compressor(i->cclass,i,h,args);
}

/* For this one, ALL destroy are called */

int grib_compressor_delete(grib_compressor *cmp)
{
  grib_compressor_class *c = cmp->cclass;
  while(c)
  {
    grib_compressor_class *s = c->super ? *(c->super) : NULL;
    if(c->destroy) c->destroy(cmp);
    c = s;
  }
  /* This should go in a top class */
  grib_context_free(cmp->h->context,cmp);
  return 0;
}


