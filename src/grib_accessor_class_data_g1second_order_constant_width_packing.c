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
   SUPER      = grib_accessor_class_data_simple_packing
   IMPLEMENTS = init
   IMPLEMENTS = pack_double
   IMPLEMENTS = unpack_double
   IMPLEMENTS = value_count
   MEMBERS=const char* half_byte
   MEMBERS=const char* packingType
   MEMBERS=const char* ieee_packing
   MEMBERS=const char* precision
   MEMBERS=const char* widthOfFirstOrderValues
   MEMBERS=const char* N1
   MEMBERS=const char* N2
   MEMBERS=const char* numberOfGroups
   MEMBERS=const char* numberOfSecondOrderPackedValues
   MEMBERS=const char* extraValues
   MEMBERS=const char* pl
   MEMBERS=const char* Ni
   MEMBERS=const char* Nj
   MEMBERS=const char* jPointsAreConsecutive
   MEMBERS=const char* bitmap
   MEMBERS=const char* groupWidth

   END_CLASS_DEF

 */

/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "accessor.class" and rerun ./make_class.pl

*/

static int pack_double(grib_accessor*, const double* val,size_t *len);
static int unpack_double(grib_accessor*, double* val,size_t *len);
static int value_count(grib_accessor*,long*);
static void init(grib_accessor*,const long, grib_arguments* );
static void init_class(grib_accessor_class*);

typedef struct grib_accessor_data_g1second_order_constant_width_packing {
    grib_accessor          att;
/* Members defined in gen */
/* Members defined in values */
	int  carg;
	const char* seclen;
	const char* offsetdata;
	const char* offsetsection;
	int dirty;
/* Members defined in data_simple_packing */
	int edition;
	const char*  units_factor;
	const char*  units_bias;
	const char*  changing_precision;
	const char*  number_of_values;
	const char*  bits_per_value;
	const char*  reference_value;
	const char*  binary_scale_factor;
	const char*  decimal_scale_factor;
/* Members defined in data_g1second_order_constant_width_packing */
	const char* half_byte;
	const char* packingType;
	const char* ieee_packing;
	const char* precision;
	const char* widthOfFirstOrderValues;
	const char* N1;
	const char* N2;
	const char* numberOfGroups;
	const char* numberOfSecondOrderPackedValues;
	const char* extraValues;
	const char* pl;
	const char* Ni;
	const char* Nj;
	const char* jPointsAreConsecutive;
	const char* bitmap;
	const char* groupWidth;
} grib_accessor_data_g1second_order_constant_width_packing;

extern grib_accessor_class* grib_accessor_class_data_simple_packing;

static grib_accessor_class _grib_accessor_class_data_g1second_order_constant_width_packing = {
    &grib_accessor_class_data_simple_packing,                      /* super                     */
    "data_g1second_order_constant_width_packing",                      /* name                      */
    sizeof(grib_accessor_data_g1second_order_constant_width_packing),  /* size                      */
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
    &unpack_double,              /* grib_unpack procedures double  */
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


grib_accessor_class* grib_accessor_class_data_g1second_order_constant_width_packing = &_grib_accessor_class_data_g1second_order_constant_width_packing;


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

static void init(grib_accessor* a,const long v, grib_arguments* args)
{
  grib_accessor_data_g1second_order_constant_width_packing *self =(grib_accessor_data_g1second_order_constant_width_packing*)a;

  self->half_byte    = grib_arguments_get_name(grib_handle_of_accessor(a),args,self->carg++);
  self->packingType    = grib_arguments_get_name(grib_handle_of_accessor(a),args,self->carg++);
  self->ieee_packing    = grib_arguments_get_name(grib_handle_of_accessor(a),args,self->carg++);
  self->precision    = grib_arguments_get_name(grib_handle_of_accessor(a),args,self->carg++);
  self->widthOfFirstOrderValues    = grib_arguments_get_name(grib_handle_of_accessor(a),args,self->carg++);
  self->N1 = grib_arguments_get_name(grib_handle_of_accessor(a),args,self->carg++);
  self->N2 = grib_arguments_get_name(grib_handle_of_accessor(a),args,self->carg++);
  self->numberOfGroups = grib_arguments_get_name(grib_handle_of_accessor(a),args,self->carg++);
  self->numberOfSecondOrderPackedValues = grib_arguments_get_name(grib_handle_of_accessor(a),args,self->carg++);
  self->extraValues = grib_arguments_get_name(grib_handle_of_accessor(a),args,self->carg++);
  self->Ni = grib_arguments_get_name(grib_handle_of_accessor(a),args,self->carg++);
  self->Nj = grib_arguments_get_name(grib_handle_of_accessor(a),args,self->carg++);
  self->pl = grib_arguments_get_name(grib_handle_of_accessor(a),args,self->carg++);
  self->jPointsAreConsecutive = grib_arguments_get_name(grib_handle_of_accessor(a),args,self->carg++);
  self->bitmap = grib_arguments_get_name(grib_handle_of_accessor(a),args,self->carg++);
  self->groupWidth = grib_arguments_get_name(grib_handle_of_accessor(a),args,self->carg++);
  self->edition=1;
  a->flags |= GRIB_ACCESSOR_FLAG_DATA;

}

static int value_count(grib_accessor* a,long* numberOfSecondOrderPackedValues)
{
  int err=0;
  grib_accessor_data_g1second_order_constant_width_packing *self =(grib_accessor_data_g1second_order_constant_width_packing*)a;
  *numberOfSecondOrderPackedValues=0;

  err=grib_get_long_internal(grib_handle_of_accessor(a),self->numberOfSecondOrderPackedValues,numberOfSecondOrderPackedValues);

  return err;

}

static int unpack_double(grib_accessor* a, double* values, size_t *len)
{
  grib_accessor_data_g1second_order_constant_width_packing* self =  (grib_accessor_data_g1second_order_constant_width_packing*)a;
  int ret=0;
  long numberOfGroups,numberOfSecondOrderPackedValues;
  long groupWidth=0;
  long* firstOrderValues=0;
  long* X=0;
  long numberPerRow=0;
  long pos=0;
  long widthOfFirstOrderValues=0;
  long jPointsAreConsecutive;
  unsigned char* buf = (unsigned char*)grib_handle_of_accessor(a)->buffer->data;
  long i,n;
  double reference_value;
  long binary_scale_factor;
  long decimal_scale_factor;
  double s,d;
  long* secondaryBitmap;

  buf += grib_byte_offset(a);

  if((ret=grib_get_long_internal(grib_handle_of_accessor(a),self->numberOfGroups,&numberOfGroups)) != GRIB_SUCCESS)
         return ret;

  if((ret=grib_get_long_internal(grib_handle_of_accessor(a),self->jPointsAreConsecutive,&jPointsAreConsecutive)) != GRIB_SUCCESS)
         return ret;

	if (jPointsAreConsecutive) {
	  if((ret=grib_get_long_internal(grib_handle_of_accessor(a),self->Ni,&numberPerRow)) != GRIB_SUCCESS)
         return ret;
	} else {
	  if((ret=grib_get_long_internal(grib_handle_of_accessor(a),self->Nj,&numberPerRow)) != GRIB_SUCCESS)
         return ret;
	}

  if((ret=grib_get_long_internal(grib_handle_of_accessor(a),self->widthOfFirstOrderValues,&widthOfFirstOrderValues)) != GRIB_SUCCESS)
         return ret;

  if((ret=grib_get_long_internal(grib_handle_of_accessor(a),self->binary_scale_factor,&binary_scale_factor)) != GRIB_SUCCESS)
         return ret;

  if((ret=grib_get_long_internal(grib_handle_of_accessor(a),self->decimal_scale_factor,&decimal_scale_factor)) != GRIB_SUCCESS)
         return ret;

  if((ret=grib_get_double_internal(grib_handle_of_accessor(a),self->reference_value,&reference_value)) != GRIB_SUCCESS)
         return ret;

  if((ret=grib_get_long_internal(grib_handle_of_accessor(a),self->numberOfSecondOrderPackedValues,
  					&numberOfSecondOrderPackedValues)) != GRIB_SUCCESS)
         return ret;

  if((ret=grib_get_long_internal(grib_handle_of_accessor(a),self->groupWidth, &groupWidth)) != GRIB_SUCCESS)
         return ret;

	secondaryBitmap=(long*)grib_context_malloc_clear(a->context,sizeof(long)*numberOfSecondOrderPackedValues);
	grib_decode_long_array(buf,&pos,1,numberOfSecondOrderPackedValues,secondaryBitmap);
	pos = 8 * ( (pos + 7 ) / 8);

	firstOrderValues=(long*)grib_context_malloc_clear(a->context,sizeof(long)*numberOfGroups);
	grib_decode_long_array(buf,&pos,widthOfFirstOrderValues,numberOfGroups,firstOrderValues);
	pos = 8 * ( (pos + 7 ) / 8);

	X=(long*)grib_context_malloc_clear(a->context,sizeof(long)*numberOfSecondOrderPackedValues);

	if (groupWidth>0) {
		grib_decode_long_array(buf,&pos,groupWidth,numberOfSecondOrderPackedValues,X);
		n=0;
		i=-1;
		while (n<numberOfSecondOrderPackedValues) {
			i+=secondaryBitmap[n];
			X[n]=firstOrderValues[i]+X[n];
			n++;
		}
	} else {
		n=0;
		i=-1;
		while (n<numberOfSecondOrderPackedValues) {
			i+=secondaryBitmap[n];
			X[n]=firstOrderValues[i];
			n++;
		}
	}


	/*
	{
	long extrabits = 16 * ( (pos + 15 ) / 16) - pos;
	printf("XXXXXXX extrabits=%ld pos=%ld\n",extrabits,pos);
	}
	*/

	s = grib_power(binary_scale_factor,2);
	d = grib_power(-decimal_scale_factor,10) ;
	for (i=0; i<numberOfSecondOrderPackedValues; i++) {
		values[i] = (double) (((X[i]*s)+reference_value)*d);
	}

	*len=numberOfSecondOrderPackedValues;
	grib_context_free(a->context,secondaryBitmap);
	grib_context_free(a->context,firstOrderValues);
	grib_context_free(a->context,X);
  
  return ret;
}

static int pack_double(grib_accessor* a, const double* cval, size_t *len)
{
  grib_context_log(a->context,GRIB_LOG_ERROR,"constant width packing not implemented");
  return GRIB_NOT_IMPLEMENTED;

}

