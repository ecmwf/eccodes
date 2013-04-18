/*
 * Copyright 2005-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */


/*******************************************************
 *   Enrico Fucile
 ******************************************************/

#include "grib_api_internal.h"

/*
   This is used by make_class.pl

   START_CLASS_DEF
   CLASS      = accessor
   SUPER      = grib_accessor_class_gen
   IMPLEMENTS = unpack_double;pack_double
   IMPLEMENTS = unpack_string;pack_string;string_length
   IMPLEMENTS = unpack_long;pack_long;destroy
   IMPLEMENTS = init;dump;value_count;get_native_type
   IMPLEMENTS = compare
    END_CLASS_DEF

 */

/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "accessor.class" and rerun ./make_class.pl

*/

static int  get_native_type(grib_accessor*);
static int pack_double(grib_accessor*, const double* val,size_t *len);
static int pack_long(grib_accessor*, const long* val,size_t *len);
static int pack_string(grib_accessor*, const char*, size_t *len);
static int unpack_double(grib_accessor*, double* val,size_t *len);
static int unpack_long(grib_accessor*, long* val,size_t *len);
static int unpack_string (grib_accessor*, char*, size_t *len);
static size_t string_length(grib_accessor*);
static long value_count(grib_accessor*);
static void destroy(grib_context*,grib_accessor*);
static void dump(grib_accessor*, grib_dumper*);
static void init(grib_accessor*,const long, grib_arguments* );
static void init_class(grib_accessor_class*);
static int compare(grib_accessor*, grib_accessor*);

typedef struct grib_accessor_concept {
    grib_accessor          att;
/* Members defined in gen */
/* Members defined in concept */
} grib_accessor_concept;

extern grib_accessor_class* grib_accessor_class_gen;

static grib_accessor_class _grib_accessor_class_concept = {
    &grib_accessor_class_gen,                      /* super                     */
    "concept",                      /* name                      */
    sizeof(grib_accessor_concept),  /* size                      */
    0,                           /* inited */
    &init_class,                 /* init_class */
    &init,                       /* init                      */
    0,                  /* post_init                      */
    &destroy,                    /* free mem                       */
    &dump,                       /* describes himself         */
    0,                /* get length of section     */
    &string_length,              /* get length of string      */
    &value_count,                /* get number of values      */
    0,                 /* get number of bytes      */
    0,                /* get offset to bytes           */
    &get_native_type,            /* get native type               */
    0,                /* get sub_section                */
    0,               /* grib_pack procedures long      */
    0,               /* grib_pack procedures long      */
    &pack_long,                  /* grib_pack procedures long      */
    &unpack_long,                /* grib_unpack procedures long    */
    &pack_double,                /* grib_pack procedures double    */
    &unpack_double,              /* grib_unpack procedures double  */
    &pack_string,                /* grib_pack procedures string    */
    &unpack_string,              /* grib_unpack procedures string  */
    0,                 /* grib_pack procedures bytes     */
    0,               /* grib_unpack procedures bytes   */
    0,            /* pack_expression */
    0,              /* notify_change   */
    0,                /* update_size   */
    0,            /* preferred_size   */
    0,                    /* resize   */
    0,      /* nearest_smaller_value */
    0,                       /* next accessor    */
    &compare,                    /* compare vs. another accessor   */
    0,     /* unpack only ith value          */
    0,     /* unpack a subarray         */
    0,             		/* clear          */
};


grib_accessor_class* grib_accessor_class_concept = &_grib_accessor_class_concept;


static void init_class(grib_accessor_class* c)
{
	c->next_offset	=	(*(c->super))->next_offset;
	c->byte_count	=	(*(c->super))->byte_count;
	c->byte_offset	=	(*(c->super))->byte_offset;
	c->sub_section	=	(*(c->super))->sub_section;
	c->pack_missing	=	(*(c->super))->pack_missing;
	c->is_missing	=	(*(c->super))->is_missing;
	c->pack_bytes	=	(*(c->super))->pack_bytes;
	c->unpack_bytes	=	(*(c->super))->unpack_bytes;
	c->pack_expression	=	(*(c->super))->pack_expression;
	c->notify_change	=	(*(c->super))->notify_change;
	c->update_size	=	(*(c->super))->update_size;
	c->preferred_size	=	(*(c->super))->preferred_size;
	c->resize	=	(*(c->super))->resize;
	c->nearest_smaller_value	=	(*(c->super))->nearest_smaller_value;
	c->next	=	(*(c->super))->next;
	c->unpack_double_element	=	(*(c->super))->unpack_double_element;
	c->unpack_double_subarray	=	(*(c->super))->unpack_double_subarray;
	c->clear	=	(*(c->super))->clear;
}

/* END_CLASS_IMP */

#define MAX_CONCEPT_STRING_LENGTH 255

GRIB_INLINE static int grib_inline_strcmp(const char* a,const char* b) {
  if (*a != *b) return 1;
  while((*a!=0 && *b!=0) &&  *(a) == *(b) ) {a++;b++;}
  return (*a==0 && *b==0) ? 0 : 1;
}

static void init(grib_accessor* a, const long len , grib_arguments* args )
{
   a->length = 0;
}

static void dump(grib_accessor* a, grib_dumper* dumper)
{
  grib_dump_string(dumper,a,NULL);
}

static int pack_double(grib_accessor* a, const double* val, size_t *len)
{
  return GRIB_NOT_IMPLEMENTED;
}

static int pack_long(grib_accessor* a, const long* val, size_t *len)
{
  char buf[80];
  size_t s;
  sprintf(buf,"%ld",*val);
#if 0
  if(*len > 1)
    return GRIB_NOT_IMPLEMENTED;
#endif
  s = strlen(buf)+1;
  return pack_string(a,buf,&s);
}

static int unpack_double(grib_accessor* a, double* val, size_t *len)
{
  return GRIB_NOT_IMPLEMENTED;

}
static int unpack_long(grib_accessor* a, long* val, size_t *len)
{
  /* TODO properly caling grib_concept_evaluate_long ! */
  const char *p = grib_concept_evaluate(a->parent->h,a->creator);

  if(!p) {
    if (a->creator->defaultkey)
      return grib_get_long_internal(a->parent->h,a->creator->defaultkey,val);

    return GRIB_NOT_FOUND;
  }

  *val = atol(p);
  *len = 1;

  return GRIB_SUCCESS;

}

static int get_native_type(grib_accessor* a)
{
	int type=GRIB_TYPE_STRING;
	if (a->flags & GRIB_ACCESSOR_FLAG_LONG_TYPE)
		type=GRIB_TYPE_LONG;
	
  	return type;
}

static void destroy(grib_context* c,grib_accessor* a)
{
  /*
  grib_accessor_concept *self = (grib_accessor_concept*)a;
  grib_context_free(c,self->cval);
  */
}

static int unpack_string (grib_accessor* a, char* val, size_t *len){
  size_t slen ;
  const char *p = grib_concept_evaluate(a->parent->h,a->creator);

  if(!p) {
    if (a->creator->defaultkey)
      return grib_get_string_internal(a->parent->h,a->creator->defaultkey,val,len);

    return GRIB_NOT_FOUND;
  }

  slen = strlen(p) +1;
  if(*len < slen)
  {
    grib_context_log(a->parent->h->context, GRIB_LOG_ERROR, "Variable unpack_string Wrong size for %s it is %d bytes big (len=%d)", a->name , slen ,*len);
    *len = slen;
    return GRIB_BUFFER_TOO_SMALL;
  }
  strcpy(val,p);
  *len = slen;
  return GRIB_SUCCESS;
}


static int pack_string(grib_accessor* a, const char* val, size_t *len)
{
  return  grib_concept_apply(a->parent->h,a->creator,val);
}

static size_t string_length(grib_accessor* a)
{
	return MAX_CONCEPT_STRING_LENGTH;
}

static long value_count(grib_accessor* a)
{
	return 1;
}


static int compare(grib_accessor* a,grib_accessor* b) {
  int retval=0;
  char *aval=0;
  char *bval=0;

  size_t alen = (size_t)grib_value_count(a);
  size_t blen = (size_t)grib_value_count(b);

  if (alen != blen) return GRIB_COUNT_MISMATCH;

  aval=grib_context_malloc(a->parent->h->context,alen*sizeof(char));
  bval=grib_context_malloc(b->parent->h->context,blen*sizeof(char));

  grib_unpack_string(a,aval,&alen);
  grib_unpack_string(b,bval,&blen);

  retval = GRIB_SUCCESS;
  if (!aval || !bval || grib_inline_strcmp(aval,bval)) retval = GRIB_STRING_VALUE_MISMATCH;

  grib_context_free(a->parent->h->context,aval);
  grib_context_free(b->parent->h->context,bval);

  return retval;
}

