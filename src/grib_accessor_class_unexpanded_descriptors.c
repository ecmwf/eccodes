/*
 * Copyright 2005-2015 ECMWF.
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
   IMPLEMENTS = unpack_long;pack_long
   IMPLEMENTS = init;dump
   IMPLEMENTS = next_offset
   IMPLEMENTS = byte_count
   IMPLEMENTS = value_count
   IMPLEMENTS = byte_offset
   IMPLEMENTS = update_size
   MEMBERS    = const char* numberOfUnexpandedDescriptors

   END_CLASS_DEF

 */

/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "accessor.class" and rerun ./make_class.pl

*/

static int pack_long(grib_accessor*, const long* val,size_t *len);
static int unpack_long(grib_accessor*, long* val,size_t *len);
static long byte_count(grib_accessor*);
static long byte_offset(grib_accessor*);
static long next_offset(grib_accessor*);
static int value_count(grib_accessor*,long*);
static void dump(grib_accessor*, grib_dumper*);
static void init(grib_accessor*,const long, grib_arguments* );
static void init_class(grib_accessor_class*);
static void update_size(grib_accessor*,size_t);

typedef struct grib_accessor_unexpanded_descriptors {
    grib_accessor          att;
/* Members defined in gen */
/* Members defined in long */
/* Members defined in unexpanded_descriptors */
	const char* numberOfUnexpandedDescriptors;
} grib_accessor_unexpanded_descriptors;

extern grib_accessor_class* grib_accessor_class_long;

static grib_accessor_class _grib_accessor_class_unexpanded_descriptors = {
    &grib_accessor_class_long,                      /* super                     */
    "unexpanded_descriptors",                      /* name                      */
    sizeof(grib_accessor_unexpanded_descriptors),  /* size                      */
    0,                           /* inited */
    &init_class,                 /* init_class */
    &init,                       /* init                      */
    0,                  /* post_init                      */
    0,                    /* free mem                       */
    &dump,                       /* describes himself         */
    &next_offset,                /* get length of section     */
    0,              /* get length of string      */
    &value_count,                /* get number of values      */
    &byte_count,                 /* get number of bytes      */
    &byte_offset,                /* get offset to bytes           */
    0,            /* get native type               */
    0,                /* get sub_section                */
    0,               /* grib_pack procedures long      */
    0,                 /* grib_pack procedures long      */
    &pack_long,                  /* grib_pack procedures long      */
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
    &update_size,                /* update_size   */
    0,            /* preferred_size   */
    0,                    /* resize   */
    0,      /* nearest_smaller_value */
    0,                       /* next accessor    */
    0,                    /* compare vs. another accessor   */
    0,     /* unpack only ith value          */
    0,     /* unpack a subarray         */
    0,             		/* clear          */
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
};


grib_accessor_class* grib_accessor_class_unexpanded_descriptors = &_grib_accessor_class_unexpanded_descriptors;


static void init_class(grib_accessor_class* c)
{
	c->string_length	=	(*(c->super))->string_length;
	c->get_native_type	=	(*(c->super))->get_native_type;
	c->sub_section	=	(*(c->super))->sub_section;
	c->pack_missing	=	(*(c->super))->pack_missing;
	c->is_missing	=	(*(c->super))->is_missing;
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
	c->next	=	(*(c->super))->next;
	c->compare	=	(*(c->super))->compare;
	c->unpack_double_element	=	(*(c->super))->unpack_double_element;
	c->unpack_double_subarray	=	(*(c->super))->unpack_double_subarray;
	c->clear	=	(*(c->super))->clear;
	c->add_attribute	=	(*(c->super))->add_attribute;
	c->replace_attribute	=	(*(c->super))->replace_attribute;
	c->delete_attribute	=	(*(c->super))->delete_attribute;
	c->get_attribute	=	(*(c->super))->get_attribute;
	c->pack_attribute_bytes	=	(*(c->super))->pack_attribute_bytes;
	c->pack_attribute_double	=	(*(c->super))->pack_attribute_double;
	c->pack_attribute_long	=	(*(c->super))->pack_attribute_long;
	c->pack_attribute_string	=	(*(c->super))->pack_attribute_string;
	c->unpack_attribute_bytes	=	(*(c->super))->unpack_attribute_bytes;
	c->unpack_attribute_double	=	(*(c->super))->unpack_attribute_double;
	c->unpack_attribute_long	=	(*(c->super))->unpack_attribute_long;
	c->unpack_attribute_string	=	(*(c->super))->unpack_attribute_string;
}

/* END_CLASS_IMP */


static long compute_byte_count(grib_accessor* a){
  grib_accessor_unexpanded_descriptors* self = (grib_accessor_unexpanded_descriptors*)a;
  long numberOfUnexpandedDescriptors;
  int ret=0;

  ret=grib_get_long(a->parent->h,self->numberOfUnexpandedDescriptors,&numberOfUnexpandedDescriptors);
  if (ret) {
		grib_context_log(a->parent->h->context,GRIB_LOG_ERROR,
			"%s unable to get %s to compute size",a->name,self->numberOfUnexpandedDescriptors);
		return 0;
  }

  return 2*numberOfUnexpandedDescriptors;
}


static void init(grib_accessor* a, const long len , grib_arguments* args )
{
	grib_accessor_unexpanded_descriptors* self = (grib_accessor_unexpanded_descriptors*)a;
	int n=0;
	self->numberOfUnexpandedDescriptors=grib_arguments_get_name(a->parent->h,args,n++);
	a->length = compute_byte_count(a);
}

static void dump(grib_accessor* a, grib_dumper* dumper)
{
  grib_dump_long(dumper,a,NULL);
}

static int    unpack_long   (grib_accessor* a, long* val, size_t *len)
{
	grib_accessor_unexpanded_descriptors* self = (grib_accessor_unexpanded_descriptors*)a;
	int ret=0;
	long pos = a->offset*8;
	long rlen =0;
	long numberOfUnexpandedDescriptors = 0;
	long f,x,y;
  long *v=val;
  int i;

  ret=value_count(a,&rlen);
  if (ret) return ret;

  if (rlen==0) {
    grib_context_log(a->parent->h->context,GRIB_LOG_FATAL,
        "No descriptors in section 3. Malformed message.");
    return GRIB_MESSAGE_MALFORMED;
  }

	if(*len < rlen)
	{
		grib_context_log(a->parent->h->context, GRIB_LOG_ERROR, 
			" wrong size (%ld) for %s it contains %d values ",*len, a->name , rlen);
		*len = 0;
		return GRIB_ARRAY_TOO_SMALL;
	}

	ret=grib_get_long(a->parent->h,self->numberOfUnexpandedDescriptors,&numberOfUnexpandedDescriptors);
	if (ret) return ret;

  for (i=0;i<rlen;i++) {
    f=grib_decode_unsigned_long(a->parent->h->buffer->data,&pos,2);
    x=grib_decode_unsigned_long(a->parent->h->buffer->data,&pos,6);
    y=grib_decode_unsigned_long(a->parent->h->buffer->data,&pos,8);
    *v++=f*100000+x*1000+y;
  }

	*len = rlen;

	return GRIB_SUCCESS;
}

static int    pack_long   (grib_accessor* a, const long* val, size_t *len)
{
	int ret=0,i;
	long pos = a->offset*8;
	unsigned long f,x,y;

        for (i=0;i<*len;i++) {
	    f=val[i]/100000;
	    x=(val[i]%100000)/1000;
	    y=(val[i]%100000)%1000;
            printf("f=%ld x=%ld y=%ld new=%ld\n",f,x,y,*val);
            grib_encode_unsigned_longb(a->parent->h->buffer->data,f,&pos,2);
            grib_encode_unsigned_longb(a->parent->h->buffer->data,x,&pos,6);
            grib_encode_unsigned_longb(a->parent->h->buffer->data,y,&pos,8);
        }

	return ret;

}

static long byte_count(grib_accessor* a){
	return a->length;
}

static int value_count(grib_accessor* a,long* numberOfUnexpandedDescriptors)
{
  grib_accessor_unexpanded_descriptors* self = (grib_accessor_unexpanded_descriptors*)a;
  *numberOfUnexpandedDescriptors=0;

  return grib_get_long(a->parent->h,self->numberOfUnexpandedDescriptors,numberOfUnexpandedDescriptors);
}

static long byte_offset(grib_accessor* a){
  return a->offset;
}

static void update_size(grib_accessor* a,size_t s)
{
  a->length = s;
}

static long next_offset(grib_accessor* a){
  return byte_offset(a)+a->length;
}

