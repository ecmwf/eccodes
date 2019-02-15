/*
 * Copyright 2005-2018 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_api_internal.h"

/*
   This is used by make_class.pl

   START_CLASS_DEF
   CLASS      = accessor
   SUPER      = grib_accessor_class_gen
   IMPLEMENTS = next_offset;byte_count
   IMPLEMENTS = destroy;sub_section
   IMPLEMENTS = get_native_type
   IMPLEMENTS = init;dump;update_size
   IMPLEMENTS = next
   END_CLASS_DEF

 */

/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "accessor.class" and rerun ./make_class.pl

*/

static grib_section* sub_section(grib_accessor* a);
static int  get_native_type(grib_accessor*);
static long byte_count(grib_accessor*);
static long next_offset(grib_accessor*);
static void destroy(grib_context*,grib_accessor*);
static void dump(grib_accessor*, grib_dumper*);
static void init(grib_accessor*,const long, grib_arguments* );
static void init_class(grib_accessor_class*);
static void update_size(grib_accessor*,size_t);
static grib_accessor* next(grib_accessor*, int);

typedef struct grib_accessor_section {
    grib_accessor          att;
/* Members defined in gen */
/* Members defined in section */
} grib_accessor_section;

extern grib_accessor_class* grib_accessor_class_gen;

static grib_accessor_class _grib_accessor_class_section = {
    &grib_accessor_class_gen,                      /* super                     */
    "section",                      /* name                      */
    sizeof(grib_accessor_section),  /* size                      */
    0,                           /* inited */
    &init_class,                 /* init_class */
    &init,                       /* init                      */
    0,                  /* post_init                      */
    &destroy,                    /* free mem                       */
    &dump,                       /* describes himself         */
    &next_offset,                /* get length of section     */
    0,              /* get length of string      */
    0,                /* get number of values      */
    &byte_count,                 /* get number of bytes      */
    0,                /* get offset to bytes           */
    &get_native_type,            /* get native type               */
    &sub_section,                /* get sub_section                */
    0,               /* grib_pack procedures long      */
    0,                 /* grib_pack procedures long      */
    0,                  /* grib_pack procedures long      */
    0,                /* grib_unpack procedures long    */
    0,                /* grib_pack procedures double    */
    0,              /* grib_unpack procedures double  */
    0,                /* grib_pack procedures string    */
    0,              /* grib_unpack procedures string  */
    0,          /* grib_pack array procedures string    */
    0,        /* grib_unpack array procedures string  */
    0,                 /* grib_pack procedures bytes     */
    0,               /* grib_unpack procedures bytes   */
    0,            /* pack_expression */
    0,              /* notify_change   */
    &update_size,                /* update_size   */
    0,            /* preferred_size   */
    0,                    /* resize   */
    0,      /* nearest_smaller_value */
    &next,                       /* next accessor    */
    0,                    /* compare vs. another accessor   */
    0,     /* unpack only ith value          */
    0,     /* unpack a subarray         */
    0,              		/* clear          */
    0,               		/* clone accessor          */
};


grib_accessor_class* grib_accessor_class_section = &_grib_accessor_class_section;


static void init_class(grib_accessor_class* c)
{
	c->string_length	=	(*(c->super))->string_length;
	c->value_count	=	(*(c->super))->value_count;
	c->byte_offset	=	(*(c->super))->byte_offset;
	c->pack_missing	=	(*(c->super))->pack_missing;
	c->is_missing	=	(*(c->super))->is_missing;
	c->pack_long	=	(*(c->super))->pack_long;
	c->unpack_long	=	(*(c->super))->unpack_long;
	c->pack_double	=	(*(c->super))->pack_double;
	c->unpack_double	=	(*(c->super))->unpack_double;
	c->pack_string	=	(*(c->super))->pack_string;
	c->unpack_string	=	(*(c->super))->unpack_string;
	c->pack_string_array	=	(*(c->super))->pack_string_array;
	c->unpack_string_array	=	(*(c->super))->unpack_string_array;
	c->pack_bytes	=	(*(c->super))->pack_bytes;
	c->unpack_bytes	=	(*(c->super))->unpack_bytes;
	c->pack_expression	=	(*(c->super))->pack_expression;
	c->notify_change	=	(*(c->super))->notify_change;
	c->preferred_size	=	(*(c->super))->preferred_size;
	c->resize	=	(*(c->super))->resize;
	c->nearest_smaller_value	=	(*(c->super))->nearest_smaller_value;
	c->compare	=	(*(c->super))->compare;
	c->unpack_double_element	=	(*(c->super))->unpack_double_element;
	c->unpack_double_subarray	=	(*(c->super))->unpack_double_subarray;
	c->clear	=	(*(c->super))->clear;
	c->make_clone	=	(*(c->super))->make_clone;
}

/* END_CLASS_IMP */




static void init(grib_accessor* a, const long len , grib_arguments* arg )
{
  a->sub_section           = grib_section_create(grib_handle_of_accessor(a),a);
  a->length                   = 0;
  a->flags                   |= GRIB_ACCESSOR_FLAG_READ_ONLY;
}

static void dump(grib_accessor* a, grib_dumper* dumper)
{
  grib_dump_section(dumper,a,a->sub_section->block);
}

static long byte_count(grib_accessor* a)
{

 if( !a->length || grib_handle_of_accessor(a)->loader ) 
 {
	if(a->name[1]=='_') return 0;

	/* printf("adjusting sizes SECTION %s is %ld %ld\n",a->name,(long)a->offset,(long)a->length); */
	grib_section_adjust_sizes(a->sub_section,grib_handle_of_accessor(a)->loader != NULL,0);
	/* printf("                SECTION %s is %ld %ld\n",a->name,(long)a->offset,(long)a->length);  */
 }


   /* printf("SECTION %s is %ld %d\n",a->name,a->length,a->sub_section->aclength != NULL);  */

  return a->length;
}

static long next_offset(grib_accessor* a)
{
  return a->offset + byte_count(a);
}

static void destroy(grib_context* ct, grib_accessor* a)
{
  grib_section_delete(ct,a->sub_section);
}

static int  get_native_type(grib_accessor* a){
  return GRIB_TYPE_SECTION;
}

static grib_section* sub_section(grib_accessor* a){
  /* grib_accessor_section* self = (grib_accessor_section*)a; */
  return a->sub_section;
}


static void update_size(grib_accessor* a,size_t length)
{

  size_t size = 1;
  long len = length;
  Assert(length <= 0x7fffffff);
  if(a->sub_section->aclength)
  {
    int e=grib_pack_long(a->sub_section->aclength,&len,&size);
    Assert( e == GRIB_SUCCESS);
    printf("update_length %s %ld %ld\n",a->sub_section->aclength->name,
      (long)a->sub_section->aclength->offset,
      (long)a->sub_section->aclength->length

      );
  }

  a->sub_section->length = a->length = length;
  a->sub_section->padding  = 0;

  printf("update_size %s %ld\n",a->name,a->length);

  Assert(a->length>=0);
}

static grib_accessor* next(grib_accessor* a,int explore) {
  grib_accessor* next=NULL;
  if (explore) {
    next=a->sub_section->block->first;
    if (!next) next=a->next;
  } else {
      next=a->next;
  }
  if (!next) {
    if (a->parent->owner)
      next=a->parent->owner->cclass->next(a->parent->owner,0);
  }
  return next;
}

