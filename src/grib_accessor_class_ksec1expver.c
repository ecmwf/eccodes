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
   SUPER      = grib_accessor_class_ascii
   IMPLEMENTS = unpack_long;pack_string;
   IMPLEMENTS = pack_long
   IMPLEMENTS = init
   END_CLASS_DEF

 */

/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "accessor.class" and rerun ./make_class.pl

*/

static int pack_long(grib_accessor*, const long* val,size_t *len);
static int pack_string(grib_accessor*, const char*, size_t *len);
static int unpack_long(grib_accessor*, long* val,size_t *len);
static void init(grib_accessor*,const long, grib_arguments* );
static void init_class(grib_accessor_class*);

typedef struct grib_accessor_ksec1expver {
    grib_accessor          att;
/* Members defined in gen */
/* Members defined in ascii */
/* Members defined in ksec1expver */
} grib_accessor_ksec1expver;

extern grib_accessor_class* grib_accessor_class_ascii;

static grib_accessor_class _grib_accessor_class_ksec1expver = {
    &grib_accessor_class_ascii,                      /* super                     */
    "ksec1expver",                      /* name                      */
    sizeof(grib_accessor_ksec1expver),  /* size                      */
    0,                           /* inited */
    &init_class,                 /* init_class */
    &init,                       /* init                      */
    0,                  /* post_init                      */
    0,                    /* free mem                       */
    0,                       /* describes himself         */
    0,                /* get length of section     */
    0,              /* get length of string      */
    0,                /* get number of values      */
    0,                 /* get number of bytes      */
    0,                /* get offset to bytes           */
    0,            /* get native type               */
    0,                /* get sub_section                */
    0,               /* grib_pack procedures long      */
    0,                 /* grib_pack procedures long      */
    &pack_long,                  /* grib_pack procedures long      */
    &unpack_long,                /* grib_unpack procedures long    */
    0,                /* grib_pack procedures double    */
    0,              /* grib_unpack procedures double  */
    &pack_string,                /* grib_pack procedures string    */
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


grib_accessor_class* grib_accessor_class_ksec1expver = &_grib_accessor_class_ksec1expver;


static void init_class(grib_accessor_class* c)
{
	c->dump	=	(*(c->super))->dump;
	c->next_offset	=	(*(c->super))->next_offset;
	c->string_length	=	(*(c->super))->string_length;
	c->value_count	=	(*(c->super))->value_count;
	c->byte_count	=	(*(c->super))->byte_count;
	c->byte_offset	=	(*(c->super))->byte_offset;
	c->get_native_type	=	(*(c->super))->get_native_type;
	c->sub_section	=	(*(c->super))->sub_section;
	c->pack_missing	=	(*(c->super))->pack_missing;
	c->is_missing	=	(*(c->super))->is_missing;
	c->pack_double	=	(*(c->super))->pack_double;
	c->unpack_double	=	(*(c->super))->unpack_double;
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

static void init(grib_accessor* a,  const long len,  grib_arguments* arg )
{
  a->length = len;
  Assert(a->length>=0);
}

static int unpack_long(grib_accessor* a, long* val, size_t *len)
{

  long value = 0;
  long pos = a->offset*8;
  char *intc = NULL;
  char  expver[5];
  char  refexpver[5];
  size_t llen = a->length+1;
  Assert(a->length == 4);

  if(*len < 1)
  {
    grib_context_log(a->context, GRIB_LOG_ERROR, "Wrong size for %s it contains %d values ", a->name , 1 );
    *len = 0;
    return GRIB_ARRAY_TOO_SMALL;
  }
  value = grib_decode_unsigned_long(grib_handle_of_accessor(a)->buffer->data , &pos, a->length*8);

  grib_unpack_string(a, refexpver , &llen);

  /* test for endiannes */
  intc = (char*)&value;

  expver[0] = intc[0];
  expver[1] = intc[1];
  expver[2] = intc[2];
  expver[3] = intc[3];
  expver[4] = 0;

  /* if there is a difference, have to reverse*/
  if(strcmp(refexpver, expver)){
    intc[0] = expver[3];
    intc[1] = expver[2];
    intc[2] = expver[1];
    intc[3] = expver[0];
  }

  *val = value;
  *len = 1;
  return GRIB_SUCCESS;
}

static int pack_string(grib_accessor* a, const char* val, size_t *len)
{
  int i = 0;
  if (len[0] != 4 ) {
  	grib_context_log(a->context, GRIB_LOG_ERROR, "Wrong length for %s. It has to be 4",a->name);
	return GRIB_INVALID_KEY_VALUE;
  }
  if(len[0] > (a->length)+1)
  {
    grib_context_log(a->context, GRIB_LOG_ERROR, "pack_string: Wrong size (%d) for %s it contains %d values ", len[0], a->name , a->length+1 );
    len[0] = 0;
    return GRIB_BUFFER_TOO_SMALL;
  }

  for ( i = 0; i < a->length; i++)
        grib_handle_of_accessor(a)->buffer->data[a->offset+i] = val[i];

  return GRIB_SUCCESS;
}

static int pack_long(grib_accessor* a, const long* val, size_t *len) {
	char sval[5]={0,};
	size_t slen=4;
	sprintf(sval,"%04d",(int)(*val));
	return pack_string(a,sval,&slen);
}

