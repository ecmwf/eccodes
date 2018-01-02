/*
 * Copyright 2005-2018 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

/*********************************************
 *   Enrico Fucile
 *******************************************/

#include "grib_api_internal.h"
/*
   This is used by make_class.pl

   START_CLASS_DEF
   CLASS      = accessor
   SUPER      = grib_accessor_class_gen
   IMPLEMENTS = pack_string;unpack_string;value_count
   IMPLEMENTS = pack_long;unpack_long;dump
   IMPLEMENTS = get_native_type;string_length
   IMPLEMENTS = init
   MEMBERS    = const char* startStep
   MEMBERS    = const char* endStep
   END_CLASS_DEF

 */

/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "accessor.class" and rerun ./make_class.pl

*/

static int  get_native_type(grib_accessor*);
static int pack_long(grib_accessor*, const long* val,size_t *len);
static int pack_string(grib_accessor*, const char*, size_t *len);
static int unpack_long(grib_accessor*, long* val,size_t *len);
static int unpack_string (grib_accessor*, char*, size_t *len);
static size_t string_length(grib_accessor*);
static int value_count(grib_accessor*,long*);
static void dump(grib_accessor*, grib_dumper*);
static void init(grib_accessor*,const long, grib_arguments* );
static void init_class(grib_accessor_class*);

typedef struct grib_accessor_g2step_range {
    grib_accessor          att;
/* Members defined in gen */
/* Members defined in g2step_range */
	const char* startStep;
	const char* endStep;
} grib_accessor_g2step_range;

extern grib_accessor_class* grib_accessor_class_gen;

static grib_accessor_class _grib_accessor_class_g2step_range = {
    &grib_accessor_class_gen,                      /* super                     */
    "g2step_range",                      /* name                      */
    sizeof(grib_accessor_g2step_range),  /* size                      */
    0,                           /* inited */
    &init_class,                 /* init_class */
    &init,                       /* init                      */
    0,                  /* post_init                      */
    0,                    /* free mem                       */
    &dump,                       /* describes himself         */
    0,                /* get length of section     */
    &string_length,              /* get length of string      */
    &value_count,                /* get number of values      */
    0,                 /* get number of bytes      */
    0,                /* get offset to bytes           */
    &get_native_type,            /* get native type               */
    0,                /* get sub_section                */
    0,               /* grib_pack procedures long      */
    0,                 /* grib_pack procedures long      */
    &pack_long,                  /* grib_pack procedures long      */
    &unpack_long,                /* grib_unpack procedures long    */
    0,                /* grib_pack procedures double    */
    0,              /* grib_unpack procedures double  */
    &pack_string,                /* grib_pack procedures string    */
    &unpack_string,              /* grib_unpack procedures string  */
    0,          /* grib_pack array procedures string    */
    0,        /* grib_unpack array procedures string  */
    0,                 /* grib_pack procedures bytes     */
    0,               /* grib_unpack procedures bytes   */
    0,            /* pack_expression */
    0,              /* notify_change   */
    0,                /* update_size   */
    0,            /* preferred_size   */
    0,                    /* resize   */
    0,      /* nearest_smaller_value */
    0,                       /* next accessor    */
    0,                    /* compare vs. another accessor   */
    0,     /* unpack only ith value          */
    0,     /* unpack a subarray         */
    0,              		/* clear          */
    0,               		/* clone accessor          */
};


grib_accessor_class* grib_accessor_class_g2step_range = &_grib_accessor_class_g2step_range;


static void init_class(grib_accessor_class* c)
{
	c->next_offset	=	(*(c->super))->next_offset;
	c->byte_count	=	(*(c->super))->byte_count;
	c->byte_offset	=	(*(c->super))->byte_offset;
	c->sub_section	=	(*(c->super))->sub_section;
	c->pack_missing	=	(*(c->super))->pack_missing;
	c->is_missing	=	(*(c->super))->is_missing;
	c->pack_double	=	(*(c->super))->pack_double;
	c->unpack_double	=	(*(c->super))->unpack_double;
	c->pack_string_array	=	(*(c->super))->pack_string_array;
	c->unpack_string_array	=	(*(c->super))->unpack_string_array;
	c->pack_bytes	=	(*(c->super))->pack_bytes;
	c->unpack_bytes	=	(*(c->super))->unpack_bytes;
	c->pack_expression	=	(*(c->super))->pack_expression;
	c->notify_change	=	(*(c->super))->notify_change;
	c->update_size	=	(*(c->super))->update_size;
	c->preferred_size	=	(*(c->super))->preferred_size;
	c->resize	=	(*(c->super))->resize;
	c->nearest_smaller_value	=	(*(c->super))->nearest_smaller_value;
	c->next	=	(*(c->super))->next;
	c->compare	=	(*(c->super))->compare;
	c->unpack_double_element	=	(*(c->super))->unpack_double_element;
	c->unpack_double_subarray	=	(*(c->super))->unpack_double_subarray;
	c->clear	=	(*(c->super))->clear;
	c->make_clone	=	(*(c->super))->make_clone;
}

/* END_CLASS_IMP */

static void init(grib_accessor* a,const long l, grib_arguments* c)
{
  grib_accessor_g2step_range* self = (grib_accessor_g2step_range*)a;

  int n = 0;

  self->startStep   = grib_arguments_get_name(grib_handle_of_accessor(a),c,n++);
  self->endStep     = grib_arguments_get_name(grib_handle_of_accessor(a),c,n++);
  
  a->length=0;
}

static void dump(grib_accessor* a, grib_dumper* dumper)
{
  grib_dump_string(dumper,a,NULL);

}

static int unpack_string(grib_accessor* a, char* val, size_t *len) {
  grib_accessor_g2step_range* self = (grib_accessor_g2step_range*)a;
  grib_handle* h=grib_handle_of_accessor(a);
  char buf[100];
  int ret=0;
  size_t size=0;
  long start=0,theEnd=0;

  ret = grib_get_long_internal(h,self->startStep,&start);
  if (ret) return ret;
  
  if (self->endStep==NULL) {
    sprintf(buf,"%ld",start);
  } else {
    ret = grib_get_long_internal(h,self->endStep,&theEnd);
    if (ret) return ret;

	if(start == theEnd)
	{
		sprintf(buf,"%ld",theEnd);
	}
	else
	{
		sprintf(buf,"%ld-%ld",start,theEnd);
	}
  }

  size=strlen(buf)+1;

  if (*len<size) return GRIB_ARRAY_TOO_SMALL;

  *len=size;

  memcpy(val,buf,size);

  return GRIB_SUCCESS;
}

static int pack_string(grib_accessor* a, const char* val, size_t *len){
  grib_accessor_g2step_range* self = (grib_accessor_g2step_range*)a;
  grib_handle* h=grib_handle_of_accessor(a);

  long start=0,theEnd=-1;
  int ret=0;
  char *p=NULL,*q=NULL;

  start=strtol(val, &p,10);
  theEnd=start;
  
  if ( *p!=0 ) theEnd=strtol(++p, &q,10);
  ret=grib_set_long_internal(h,self->startStep,start);
  if (ret) return ret;

  if(self->endStep!=NULL) {
    ret=grib_set_long_internal(h,self->endStep,theEnd);
  }

  return 0;
}

static int value_count(grib_accessor* a,long* count)
{
    *count=1;
    return 0;
}

static size_t string_length(grib_accessor* a)
{
  return 255;
}

static int pack_long(grib_accessor* a, const long* val, size_t *len)
{
  char buff[100];
  size_t bufflen=100;

  sprintf(buff,"%ld",*val);
  return pack_string( a,buff,&bufflen);
}

static int unpack_long(grib_accessor* a, long* val, size_t *len) {
  char buff[100];
  size_t bufflen=100;
  long start,theEnd;
  char* p=buff;
  char* q=NULL;
  int err=0;

  
  if ((err=unpack_string( a,buff,&bufflen))!=GRIB_SUCCESS)
    return err;
  
  start=strtol(buff, &p,10);
  theEnd=start;
  if ( *p!=0 ) theEnd=strtol(++p, &q,10);

  *val=theEnd;

  return 0;
}

static int  get_native_type(grib_accessor* a){
  return GRIB_TYPE_STRING;
}

