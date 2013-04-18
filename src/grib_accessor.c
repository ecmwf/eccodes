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
 *   Jean Baptiste Filippi - 01.11.2005
 *   Enrico Fucile
 *                                                                         *
 ***************************************************************************/

#include "grib_api_internal.h"

void  grib_print_accessor(grib_accessor* a, grib_dumper* f)
{
  grib_accessor_class *c = a->cclass;
  while(c)
  {

    if(c->dump)
    {
      c->dump(a, f);
      return;
    }
    c = c->super ? *(c->super) : NULL;
  }
  Assert(0);
}


int grib_pack_missing(grib_accessor* a)
{
  grib_accessor_class *c = a->cclass;
  /*grib_context_log(a->parent->h->context, GRIB_LOG_DEBUG, "(%s)%s is packing (double) %g",(a->parent->owner)?(a->parent->owner->name):"root", a->name ,v?(*v):0); */
  while(c)
  {
    if(c->pack_missing)
    {
      return c->pack_missing(a);
    }
    c = c->super ? *(c->super) : NULL;
  }
  Assert(0);
  return 0;
}

int grib_pack_zero(grib_accessor* a)
{
  grib_accessor_class *c = a->cclass;
  /*grib_context_log(a->parent->h->context, GRIB_LOG_DEBUG, "(%s)%s is packing (double) %g",(a->parent->owner)?(a->parent->owner->name):"root", a->name ,v?(*v):0); */
  while(c)
  {
    if(c->clear)
    {
      return c->clear(a);
    }
    c = c->super ? *(c->super) : NULL;
  }
  Assert(0);
  return 0;
}



int grib_is_missing_internal(grib_accessor* a)
{
  grib_accessor_class *c = a->cclass;
  /*grib_context_log(a->parent->h->context, GRIB_LOG_DEBUG, "(%s)%s is packing (double) %g",(a->parent->owner)?(a->parent->owner->name):"root", a->name ,v?(*v):0); */
  while(c)
  {
    if(c->is_missing)
    {
      return c->is_missing(a);
    }
    c = c->super ? *(c->super) : NULL;
  }
  Assert(0);
  return 0;
}
int grib_pack_double(grib_accessor* a, const double* v, size_t *len )
{
  grib_accessor_class *c = a->cclass;
  /*grib_context_log(a->parent->h->context, GRIB_LOG_DEBUG, "(%s)%s is packing (double) %g",(a->parent->owner)?(a->parent->owner->name):"root", a->name ,v?(*v):0); */
  while(c)
  {
    if(c->pack_double)
    {
      return c->pack_double(a,v,len);
    }
    c = c->super ? *(c->super) : NULL;
  }
  Assert(0);
  return 0;
}

int grib_pack_expression(grib_accessor* a, grib_expression *e )
{
  grib_accessor_class *c = a->cclass;
  /*grib_context_log(a->parent->h->context, GRIB_LOG_DEBUG, "(%s)%s is packing (double) %g",(a->parent->owner)?(a->parent->owner->name):"root", a->name ,v?(*v):0); */
  while(c)
  {
    if(c->pack_expression)
    {
      return c->pack_expression(a,e);
    }
    c = c->super ? *(c->super) : NULL;
  }
  Assert(0);
  return 0;
}


int grib_pack_string(grib_accessor* a, const char* v, size_t *len )
{
  grib_accessor_class *c = a->cclass;
  /*grib_context_log(a->parent->h->context, GRIB_LOG_DEBUG, "(%s)%s is packing (string) %s",(a->parent->owner)?(a->parent->owner->name):"root", a->name ,v?v:"(null)");*/
  while(c)
  {

    if(c->pack_string)
    {
      return c->pack_string(a,v,len);
    }
    c = c->super ? *(c->super) : NULL;
  }
  Assert(0);
  return 0;
}

int grib_pack_long(grib_accessor* a,const long* v, size_t *len )
{
  grib_accessor_class *c = a->cclass;
  /*grib_context_log(a->parent->h->context, GRIB_LOG_DEBUG, "(%s)%s is packing (long) %d",(a->parent->owner)?(a->parent->owner->name):"root", a->name ,v?(*v):0); */
  while(c)
  {
    if(c->pack_long)
    {
      return c->pack_long(a,v,len);
    }
    c = c->super ? *(c->super) : NULL;
  }
  Assert(0);
  return 0;
}

int grib_pack_bytes(grib_accessor* a, const unsigned char* v, size_t *len )
{
  grib_accessor_class *c = a->cclass;
  /*grib_context_log(a->parent->h->context, GRIB_LOG_DEBUG, "(%s)%s is packing (bytes) %d",(a->parent->owner)?(a->parent->owner->name):"root", a->name ,v?(*v):0); */
  while(c)
  {
    if(c->pack_bytes)
    {
      return c->pack_bytes(a,v,len);
    }
    c = c->super ? *(c->super) : NULL;
  }
  Assert(0);
  return 0;
}

int grib_unpack_bytes(grib_accessor* a, unsigned char* v, size_t *len )
{
  grib_accessor_class *c = a->cclass;
  /*grib_context_log(a->parent->h->context, GRIB_LOG_DEBUG, "(%s)%s is unpacking (bytes)",(a->parent->owner)?(a->parent->owner->name):"root", a->name ); */
  while(c)
  {
    if(c->unpack_bytes)
    {
      return c->unpack_bytes(a,v,len);
    }
    c = c->super ? *(c->super) : NULL;
  }
  Assert(0);
  return 0;
}

int grib_unpack_double_subarray(grib_accessor* a, double* v, size_t start, size_t len )
{
  grib_accessor_class *c = a->cclass;
  while(c)
  {
    if(c->unpack_double_subarray)
    {
      return c->unpack_double_subarray(a,v,start,len);
    }
    c = c->super ? *(c->super) : NULL;
  }
  Assert(0);
  return 0;
}

int grib_unpack_double(grib_accessor* a, double* v, size_t *len )
{
  grib_accessor_class *c = a->cclass;
  /*grib_context_log(a->parent->h->context, GRIB_LOG_DEBUG, "(%s)%s is unpacking (double)",(a->parent->owner)?(a->parent->owner->name):"root", a->name ); */
  while(c)
  {
    if(c->unpack_double)
    {
      return c->unpack_double(a,v,len);
    }
    c = c->super ? *(c->super) : NULL;
  }
  Assert(0);
  return 0;
}

int grib_unpack_double_element(grib_accessor* a, size_t i ,double* v )
{
  grib_accessor_class *c = a->cclass;
  while(c)
  {
    if(c->unpack_double_element)
    {
      return c->unpack_double_element(a,i,v);
    }
    c = c->super ? *(c->super) : NULL;
  }
  return GRIB_NOT_IMPLEMENTED;
}

int grib_unpack_string(grib_accessor* a, char* v, size_t *len )
{
  grib_accessor_class *c = a->cclass;
  /* grib_context_log(a->parent->h->context, GRIB_LOG_DEBUG, "(%s)%s is unpacking (string)",(a->parent->owner)?(a->parent->owner->name):"root", a->name ); */
  while(c)
  {
    if(c->unpack_string)
    {
      return c->unpack_string(a,v,len);
    }
    c = c->super ? *(c->super) : NULL;
  }
  Assert(0);
  return 0;
}

int grib_unpack_long(grib_accessor* a,long* v, size_t *len )
{
  grib_accessor_class *c = a->cclass;
  /*grib_context_log(a->parent->h->context, GRIB_LOG_DEBUG, "(%s)%s is unpacking (long)",(a->parent->owner)?(a->parent->owner->name):"root", a->name ); */
  while(c)
  {
    if(c->unpack_long)
    {
      return c->unpack_long(a,v,len);
    }
    c = c->super ? *(c->super) : NULL;
  }
  Assert(0);
  return 0;
}

long grib_accessor_get_native_type(grib_accessor* a)
{
  grib_accessor_class *c = NULL;
  if (a)  c = a->cclass;

  while(c)
  {
    if(c->get_native_type)
      return c->get_native_type(a);
    c = c->super ? *(c->super) : NULL;
  }
  Assert(0);
  return 0;
}

long grib_get_next_position_offset(grib_accessor* a)
{
  grib_accessor_class *c = NULL;
  /*grib_context_log(a->parent->h->context, GRIB_LOG_DEBUG, "(%s)%s is checking next (long)",(a->parent->owner)?(a->parent->owner->name):"root", a->name ); */
  if (a)  c = a->cclass;

  while(c)
  {
    if(c->next_offset)
      return c->next_offset(a);
    c = c->super ? *(c->super) : NULL;
  }
  Assert(0);
  return 0;
}

long grib_string_length(grib_accessor* a)
{
  grib_accessor_class *c = NULL;
  if (a)  c = a->cclass;

  while(c)
  {
    if(c->string_length)
      return c->string_length(a);
    c = c->super ? *(c->super) : NULL;
  }
  Assert(0);
  return 0;
}

long grib_byte_offset(grib_accessor* a)
{
  grib_accessor_class *c = NULL;
  if (a)  c = a->cclass;

  while(c)
  {
    if(c->byte_offset)
      return c->byte_offset(a);
    c = c->super ? *(c->super) : NULL;
  }
  Assert(0);
  return 0;
}

long grib_byte_count(grib_accessor* a)
{
  grib_accessor_class *c = NULL;
  if (a)  c = a->cclass;

  while(c)
  {
    if(c->byte_count)
      return c->byte_count(a);
    c = c->super ? *(c->super) : NULL;
  }
  Assert(0);
  return 0;
}

long grib_value_count(grib_accessor* a)
{
  grib_accessor_class *c = NULL;
  if (a)  c = a->cclass;

  while(c)
  {
    if(c->value_count)
      return c->value_count(a);
    c = c->super ? *(c->super) : NULL;
  }
  Assert(0);
  return 0;
}

int grib_accessor_notify_change(grib_accessor* a,grib_accessor* changed)
{
  grib_accessor_class *c = NULL;
  if (a)  c = a->cclass;

  while(c)
  {
    if(c->notify_change)
      return c->notify_change(a,changed);
    c = c->super ? *(c->super) : NULL;
  }
  printf("notify_change not implemented for %s %s\n",a->cclass->name,a->name);
  Assert(0);
  return 0;
}


/* For this one, ALL init are called */

static void init_accessor(grib_accessor_class* c,grib_accessor* a, const long len, grib_arguments* args)
{
  if(c) {
    grib_accessor_class *s = c->super ? *(c->super) : NULL;
    if(!c->inited)
    {
      if(c->init_class) c->init_class(c);
      c->inited = 1;
    }
    init_accessor(s,a,len,args);
    if(c->init) c->init(a,len, args);
  }
}

void grib_init_accessor(grib_accessor* a, const long len, grib_arguments* args)
{
  init_accessor(a->cclass,a,len,args);
}

#if 0
static void post_init_accessor(grib_accessor_class* c,grib_accessor* a)
{
  if(c) {
    grib_accessor_class *s = c->super ? *(c->super) : NULL;
    post_init_accessor(s,a);
    if(c->post_init) c->post_init(a);
  }
}
#endif

/* For this one, ALL destroy are called */

void grib_free_accessor(grib_context *ct, grib_accessor* a)
{
  grib_accessor_class *c = a->cclass;
  while(c)
  {
    grib_accessor_class *s = c->super ? *(c->super) : NULL;
    if(c->destroy) c->destroy(ct,a);
    c = s;
  }
  grib_context_free(ct,a);
}


void grib_update_size(grib_accessor* a, size_t len )
{
  grib_accessor_class *c = a->cclass;
  /*grib_context_log(a->parent->h->context, GRIB_LOG_DEBUG, "(%s)%s is packing (double) %g",(a->parent->owner)?(a->parent->owner->name):"root", a->name ,v?(*v):0); */
  while(c)
  {
    if(c->update_size)
    {
      c->update_size(a,len);
      return;
    }
    c = c->super ? *(c->super) : NULL;
  }
  Assert(0);
}

int grib_nearest_smaller_value(grib_accessor* a,double val,double* nearest)
{
  grib_accessor_class *c = a->cclass;
  while(c)
  {
    if(c->nearest_smaller_value)
    {
      return c->nearest_smaller_value(a,val,nearest);
    }
    c = c->super ? *(c->super) : NULL;
  }
  Assert(0);
  return 0;
}


size_t grib_preferred_size(grib_accessor* a,int from_handle)
{
  grib_accessor_class *c = a->cclass;
  /*grib_context_log(a->parent->h->context, GRIB_LOG_DEBUG, "(%s)%s is packing (long) %d",(a->parent->owner)?(a->parent->owner->name):"root", a->name ,v?(*v):0); */
  while(c)
  {
    if(c->preferred_size)
    {
      return c->preferred_size(a,from_handle);
    }
    c = c->super ? *(c->super) : NULL;
  }
  Assert(0);
  return 0;
}

grib_accessor* grib_next_accessor(grib_accessor* a)
{
  grib_accessor_class *c = a->cclass;
  while(c)
  {
    if(c->next)
    {
      return c->next(a,1);
    }
    c = c->super ? *(c->super) : NULL;
  }
  Assert(0);
  return 0;
}

void grib_resize(grib_accessor* a,size_t new_size)
{
  grib_accessor_class *c = a->cclass;
  /*grib_context_log(a->parent->h->context, GRIB_LOG_DEBUG, "(%s)%s is packing (long) %d",(a->parent->owner)?(a->parent->owner->name):"root", a->name ,v?(*v):0); */
  while(c)
  {
    if(c->resize)
    {
    c->resize(a,new_size);
      return ;
    }
    c = c->super ? *(c->super) : NULL;
  }
  Assert(0);
  return;
}

int grib_compare_accessors(grib_accessor* a1,grib_accessor* a2,int compare_flags) {
  int ret=0;
  long type1=0;
  long type2=0;
  int type_mismatch=0;
  grib_accessor_class *c1=NULL;

  if ((compare_flags & GRIB_COMPARE_NAMES) && strcmp(a1->name,a2->name))
      return GRIB_NAME_MISMATCH;

  if ( compare_flags & GRIB_COMPARE_TYPES ) {
    type1=grib_accessor_get_native_type( a1 );
    type2=grib_accessor_get_native_type( a2 );

    type_mismatch = type1 != type2 ? 1 : 0;
  }

  ret=GRIB_UNABLE_TO_COMPARE_ACCESSORS;
  c1 = a1->cclass;
  while(c1)
  {

    if(c1->compare)
    {
      ret = c1->compare(a1, a2);
      break;
    }
    c1 = c1->super ? *(c1->super) : NULL;
  }

  if (ret == GRIB_VALUE_MISMATCH && type_mismatch)
    ret = GRIB_TYPE_AND_VALUE_MISMATCH;

  return ret;
}

const char* grib_get_type_name(int type)
{
  switch(type)
  {
    case GRIB_TYPE_LONG:    return "long"; break;
    case GRIB_TYPE_STRING:  return "string"; break;
    case GRIB_TYPE_BYTES:   return "bytes"; break;
    case GRIB_TYPE_DOUBLE:  return "double"; break;
    case GRIB_TYPE_LABEL:   return "label"; break;
    case GRIB_TYPE_SECTION: return "section"; break;
  }

  return "unknown";
}
