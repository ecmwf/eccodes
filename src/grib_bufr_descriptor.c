/*
 * Copyright 2005-2014 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */


#include "grib_api_internal.h"

bufr_descriptor* grib_bufr_descriptor_new(grib_context* c,int code) {
  bufr_descriptor* v=NULL;

  if (!c) c=grib_context_get_default();

  v=(bufr_descriptor*)grib_context_malloc_clear(c,sizeof(bufr_descriptor));
  if (!v) {
    grib_context_log(c,GRIB_LOG_ERROR,
          "grib_bufr_descriptor_new unable to allocate %d bytes\n",sizeof(bufr_descriptor));
    return NULL;
  }

  grib_bufr_descriptor_set_code(v,code);

  return v;
}

bufr_descriptor* grib_bufr_descriptor_clone(grib_context* c,bufr_descriptor* d) {

  if (!d) return NULL;

  return grib_bufr_descriptor_new(c,d->code);
}

void grib_bufr_descriptor_set_code(bufr_descriptor* v,int code) {
  if (!v) return;

  v->code=code;
  v->F=code/100000;
  v->X=(code-v->F*100000)/1000;
  v->Y=(code-v->F*100000)%1000;
}

void grib_bufr_descriptor_set_values(bufr_descriptor* v,int scale,int reference,int width) {
  if (!v) return;
  v->scale=scale;
  v->reference=reference;
  v->width=width;
  v->flags=BUFR_DESCRIPTOR_FLAG_HAS_VALUES;
  v->factor=grib_power(-scale,10);
}

void grib_bufr_descriptor_set_scale(bufr_descriptor* v,int scale) {
  if (!v) return;
  v->scale=scale;
  v->factor=grib_power(-scale,10);
}

void grib_bufr_descriptor_delete(grib_context* c,bufr_descriptor* v) {

  if (!v) return;

  grib_context_free(c,v);
}

