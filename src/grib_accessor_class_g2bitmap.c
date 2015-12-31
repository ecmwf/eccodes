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
   SUPER      = grib_accessor_class_bitmap
   IMPLEMENTS = pack_double
   IMPLEMENTS = value_count
   IMPLEMENTS = init;
   MEMBERS=const char* numberOfValues
   END_CLASS_DEF

 */

/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "accessor.class" and rerun ./make_class.pl

*/

static int pack_double(grib_accessor*, const double* val,size_t *len);
static int value_count(grib_accessor*,long*);
static void init(grib_accessor*,const long, grib_arguments* );
static void init_class(grib_accessor_class*);

typedef struct grib_accessor_g2bitmap {
    grib_accessor          att;
/* Members defined in gen */
/* Members defined in bytes */
/* Members defined in bitmap */
	const char* tableReference;
	const char* missing_value;
	const char* offsetbsec;
	const char* sLength;
/* Members defined in g2bitmap */
	const char* numberOfValues;
} grib_accessor_g2bitmap;

extern grib_accessor_class* grib_accessor_class_bitmap;

static grib_accessor_class _grib_accessor_class_g2bitmap = {
    &grib_accessor_class_bitmap,                      /* super                     */
    "g2bitmap",                      /* name                      */
    sizeof(grib_accessor_g2bitmap),  /* size                      */
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
    0,                /* grib_unpack procedures long    */
    &pack_double,                /* grib_pack procedures double    */
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


grib_accessor_class* grib_accessor_class_g2bitmap = &_grib_accessor_class_g2bitmap;


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
	c->unpack_long	=	(*(c->super))->unpack_long;
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



static void init(grib_accessor* a, const long len , grib_arguments* arg )
{

	grib_accessor_g2bitmap* self = (grib_accessor_g2bitmap*)a;
  
	self->numberOfValues     = grib_arguments_get_name(grib_handle_of_accessor(a),arg,4);
}


static void grib_set_bit_on( unsigned char* p, long *bitp){
	unsigned char o =  1; 
	p += (*bitp >> 3); 
	o <<= 7-((*bitp)%8);
	*p |= o;
	(*bitp)+=1;
}

static int pack_double(grib_accessor* a, const double* val,size_t *len){
	grib_accessor_g2bitmap* self = (grib_accessor_g2bitmap*)a;

	size_t tlen;

	unsigned char* buf = NULL;
	size_t i;
	int err = 0;
	long pos = 0;
	long bmaplen = 0;
	double miss_values = 0;
	tlen = (*len+7)/8;

	if((err = grib_get_double_internal(grib_handle_of_accessor(a), self->missing_value, &miss_values))
       != GRIB_SUCCESS)
		return err;

	buf = (unsigned char*)grib_context_malloc_clear(a->context,tlen);
	if(!buf) return GRIB_OUT_OF_MEMORY;
	pos=0;
	for(i=0;i<*len;i++)
	{     
		if (val[i] == miss_values)
			pos++;
		else{
			bmaplen++;
			grib_set_bit_on(buf, &pos);
		}
	}

	if((err = grib_set_long_internal(grib_handle_of_accessor(a), self->numberOfValues,*len )) != GRIB_SUCCESS) {
		grib_context_free(a->context,buf);
		return err;
	}

	grib_buffer_replace(a, buf, tlen,1,1); 

	grib_context_free(a->context,buf);

	return GRIB_SUCCESS;
}


static int value_count(grib_accessor* a,long* tlen)
{
	grib_accessor_g2bitmap* self = (grib_accessor_g2bitmap*)a;
	int err;
	*tlen=0;
	
	err=grib_get_long_internal(grib_handle_of_accessor(a), self->numberOfValues, tlen);
	return err;
}

