/*
 * Copyright 2005-2016 ECMWF.
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
   SUPER      = grib_accessor_class_long
   IMPLEMENTS = unpack_long
   IMPLEMENTS = init;
   IMPLEMENTS = value_count
   MEMBERS    = const char* bitmap
   MEMBERS    = const char* unusedBitsInBitmap
   MEMBERS    = const char* numberOfDataPoints
   END_CLASS_DEF

 */

/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "accessor.class" and rerun ./make_class.pl

*/

static int unpack_long(grib_accessor*, long* val,size_t *len);
static int value_count(grib_accessor*,long*);
static void init(grib_accessor*,const long, grib_arguments* );
static void init_class(grib_accessor_class*);

typedef struct grib_accessor_count_missing {
    grib_accessor          att;
/* Members defined in gen */
/* Members defined in long */
/* Members defined in count_missing */
	const char* bitmap;
	const char* unusedBitsInBitmap;
	const char* numberOfDataPoints;
} grib_accessor_count_missing;

extern grib_accessor_class* grib_accessor_class_long;

static grib_accessor_class _grib_accessor_class_count_missing = {
    &grib_accessor_class_long,                      /* super                     */
    "count_missing",                      /* name                      */
    sizeof(grib_accessor_count_missing),  /* size                      */
    0,                           /* inited */
    &init_class,                 /* init_class */
    &init,                       /* init                      */
    0,                  /* post_init                      */
    0,                    /* free mem                       */
    0,                       /* describes himself         */
    0,                /* get length of section     */
    0,              /* get length of string      */
    &value_count,                /* get number of values      */
    0,                 /* get number of bytes      */
    0,                /* get offset to bytes           */
    0,            /* get native type               */
    0,                /* get sub_section                */
    0,               /* grib_pack procedures long      */
    0,                 /* grib_pack procedures long      */
    0,                  /* grib_pack procedures long      */
    &unpack_long,                /* grib_unpack procedures long    */
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


grib_accessor_class* grib_accessor_class_count_missing = &_grib_accessor_class_count_missing;


static void init_class(grib_accessor_class* c)
{
	c->dump	=	(*(c->super))->dump;
	c->next_offset	=	(*(c->super))->next_offset;
	c->string_length	=	(*(c->super))->string_length;
	c->byte_count	=	(*(c->super))->byte_count;
	c->byte_offset	=	(*(c->super))->byte_offset;
	c->get_native_type	=	(*(c->super))->get_native_type;
	c->sub_section	=	(*(c->super))->sub_section;
	c->pack_missing	=	(*(c->super))->pack_missing;
	c->is_missing	=	(*(c->super))->is_missing;
	c->pack_long	=	(*(c->super))->pack_long;
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

static const unsigned char bitsoff[]={
8, 7, 7, 6, 7, 6, 6, 5, 7, 6, 6, 5, 6, 5, 5, 4, 7,
6, 6, 5, 6, 5, 5, 4, 6, 5, 5, 4, 5, 4, 4, 3, 7, 6,
6, 5, 6, 5, 5, 4, 6, 5, 5, 4, 5, 4, 4, 3, 6, 5, 5,
4, 5, 4, 4, 3, 5, 4, 4, 3, 4, 3, 3, 2, 7, 6, 6, 5,
6, 5, 5, 4, 6, 5, 5, 4, 5, 4, 4, 3, 6, 5, 5, 4, 5,
4, 4, 3, 5, 4, 4, 3, 4, 3, 3, 2, 6, 5, 5, 4, 5, 4,
4, 3, 5, 4, 4, 3, 4, 3, 3, 2, 5, 4, 4, 3, 4, 3, 3,
2, 4, 3, 3, 2, 3, 2, 2, 1, 7, 6, 6, 5, 6, 5, 5, 4,
6, 5, 5, 4, 5, 4, 4, 3, 6, 5, 5, 4, 5, 4, 4, 3, 5,
4, 4, 3, 4, 3, 3, 2, 6, 5, 5, 4, 5, 4, 4, 3, 5, 4,
4, 3, 4, 3, 3, 2, 5, 4, 4, 3, 4, 3, 3, 2, 4, 3, 3,
2, 3, 2, 2, 1, 6, 5, 5, 4, 5, 4, 4, 3, 5, 4, 4, 3,
4, 3, 3, 2, 5, 4, 4, 3, 4, 3, 3, 2, 4, 3, 3, 2, 3,
2, 2, 1, 5, 4, 4, 3, 4, 3, 3, 2, 4, 3, 3, 2, 3, 2,
2, 1, 4, 3, 3, 2, 3, 2, 2, 1, 3, 2, 2, 1, 2, 1, 1,
0 };

/*
static const unsigned char bitson[]={
0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4, 1,
2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 1, 2,
2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3,
4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 1, 2, 2, 3,
2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3,
4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 2, 3, 3, 4, 3, 4,
4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5,
6, 4, 5, 5, 6, 5, 6, 6, 7, 1, 2, 2, 3, 2, 3, 3, 4,
2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3,
4, 4, 5, 4, 5, 5, 6, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4,
4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5,
6, 5, 6, 6, 7, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5,
4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5,
6, 6, 7, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6,
6, 7, 4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7,
8 };
*/


static void init(grib_accessor* a, const long len , grib_arguments* arg )
{
  int n=0;
  grib_accessor_count_missing* self = (grib_accessor_count_missing*)a;
  a->length=0;
  a->flags|=GRIB_ACCESSOR_FLAG_READ_ONLY;
  self->bitmap = grib_arguments_get_name(grib_handle_of_accessor(a),arg,n++);
  self->unusedBitsInBitmap = grib_arguments_get_name(grib_handle_of_accessor(a),arg,n++);
  self->numberOfDataPoints = grib_arguments_get_name(grib_handle_of_accessor(a),arg,n++);
}

static int used[] ={ 0,1,3,7,15,31,63,127,255};

static int    unpack_long   (grib_accessor* a, long* val, size_t *len)
{
  grib_accessor_count_missing* self = (grib_accessor_count_missing*)a;
  unsigned char* p;
  int i;
  long size=0;
  long offset=0;
  long unusedBitsInBitmap=0;
  long numberOfDataPoints=0;
  grib_handle* h=grib_handle_of_accessor(a);
  grib_accessor* bitmap=grib_find_accessor(grib_handle_of_accessor(a),self->bitmap);

  *val=0;
  *len=1;
  if (!bitmap) return GRIB_SUCCESS;

  size=grib_byte_count(bitmap);
  offset=grib_byte_offset(bitmap);

  if (grib_get_long(h,self->unusedBitsInBitmap,&unusedBitsInBitmap) != GRIB_SUCCESS) { 
    if (grib_get_long(h,self->numberOfDataPoints,&numberOfDataPoints) != GRIB_SUCCESS) {
		grib_context_log(a->context,GRIB_LOG_ERROR,"unable to count missing values");
		return GRIB_INTERNAL_ERROR;
	} 
	unusedBitsInBitmap=size*8-numberOfDataPoints;
  }

  p=grib_handle_of_accessor(a)->buffer->data+offset;

  size-=unusedBitsInBitmap/8;
  unusedBitsInBitmap= unusedBitsInBitmap % 8;

  for (i=0;i<size-1;i++) *val += bitsoff[*(p++)];

  *val += bitsoff[(*p) | used[unusedBitsInBitmap]];

  return GRIB_SUCCESS;
}

static int value_count(grib_accessor* a,long* count)
{
    *count=1;
    return 0;
}

