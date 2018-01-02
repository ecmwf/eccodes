/*
 * Copyright 2005-2018 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

/******************************
 *  Enrico Fucile
 *****************************/

#include "grib_api_internal.h"

/*
   This is used by make_class.pl

   START_CLASS_DEF
   CLASS      = accessor
   SUPER      = grib_accessor_class_gen
   IMPLEMENTS = init
   IMPLEMENTS = unpack_double
   IMPLEMENTS = pack_double
   IMPLEMENTS = value_count
   MEMBERS=const char*  missing_value
   MEMBERS=const char*  number_of_values
   MEMBERS=const char*  number_of_points
   MEMBERS=const char*  latitude_of_first_point
   MEMBERS=const char*  ni

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

typedef struct grib_accessor_gds_not_present_bitmap {
    grib_accessor          att;
/* Members defined in gen */
/* Members defined in gds_not_present_bitmap */
	const char*  missing_value;
	const char*  number_of_values;
	const char*  number_of_points;
	const char*  latitude_of_first_point;
	const char*  ni;
} grib_accessor_gds_not_present_bitmap;

extern grib_accessor_class* grib_accessor_class_gen;

static grib_accessor_class _grib_accessor_class_gds_not_present_bitmap = {
    &grib_accessor_class_gen,                      /* super                     */
    "gds_not_present_bitmap",                      /* name                      */
    sizeof(grib_accessor_gds_not_present_bitmap),  /* size                      */
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


grib_accessor_class* grib_accessor_class_gds_not_present_bitmap = &_grib_accessor_class_gds_not_present_bitmap;


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
  int n=0;
  grib_accessor_gds_not_present_bitmap *self =(grib_accessor_gds_not_present_bitmap*)a;

  self->missing_value = grib_arguments_get_name(grib_handle_of_accessor(a),args,n++);
  self->number_of_values        = grib_arguments_get_name(grib_handle_of_accessor(a),args,n++);
  self->number_of_points = grib_arguments_get_name(grib_handle_of_accessor(a),args,n++);
  self->latitude_of_first_point = grib_arguments_get_name(grib_handle_of_accessor(a),args,n++);
  self->ni = grib_arguments_get_name(grib_handle_of_accessor(a),args,n++);


  a->length = 0;
}


static int value_count(grib_accessor* a,long* number_of_points)
{
  grib_accessor_gds_not_present_bitmap* self = (grib_accessor_gds_not_present_bitmap*)a;

  *number_of_points=0;
  return grib_get_long_internal(grib_handle_of_accessor(a), self->number_of_points, number_of_points);
}



static int  unpack_double(grib_accessor* a, double* val, size_t *len)
{
  grib_accessor_gds_not_present_bitmap* self =  (grib_accessor_gds_not_present_bitmap*)a;

  long number_of_points=0,number_of_values=0,ni=0;
  long latitude_of_first_point=0;
  size_t i = 0;
  size_t  n_vals = 0;
  long nn=0;
  long missing_value;

  int err = 0;
  double* coded_vals = NULL;
  err=grib_value_count(a,&nn);
  n_vals=nn;
  if (err) return err;

  if((err = grib_get_long(grib_handle_of_accessor(a),self->number_of_points,&number_of_points))
       !=  GRIB_SUCCESS) return err;

  if((err = grib_get_long(grib_handle_of_accessor(a),self->number_of_values,&number_of_values))
       !=  GRIB_SUCCESS) return err;

  if((err = grib_get_long(grib_handle_of_accessor(a),self->latitude_of_first_point,&latitude_of_first_point))
       !=  GRIB_SUCCESS) return err;

  if((err = grib_get_long(grib_handle_of_accessor(a),self->missing_value,&missing_value))
       !=  GRIB_SUCCESS) return err;

  if((err = grib_get_long(grib_handle_of_accessor(a),self->ni,&ni))
       !=  GRIB_SUCCESS) return err;

  if(*len < number_of_points)
  {
    *len = n_vals;
    return GRIB_ARRAY_TOO_SMALL;
  }

  if(number_of_values > 0){
    coded_vals = (double*)grib_context_malloc(a->context,number_of_values*sizeof(double));

    if(coded_vals == NULL)
      return GRIB_OUT_OF_MEMORY;
  }

  if (latitude_of_first_point == 0) {
    for (i=0;i < number_of_values;i++) val[i]=1;
    for (i=number_of_values;i<number_of_points;i++)
      val[i]=0;
  } else {
    for(i=0;i<ni-1;i++) val[i]=0;
    for(i=ni-1;i<number_of_points;i++) val[i]=1;
  }

  *len =  number_of_points;

  grib_context_free(a->context,coded_vals);
  return err;
}

#if 0

static void grib_set_bit_on( unsigned char* p, long *bitp){
  unsigned char o =  1;
  p += (*bitp >> 3);
  o <<= 7-((*bitp)%8);
  *p |= o;
  (*bitp)+=1;
}

static int pack_double(grib_accessor* a, const double* val,size_t *len){
  grib_accessor_gds_not_present_bitmap* self = (grib_accessor_gds_not_present_bitmap*)a;

  size_t tlen;

  unsigned char* buf = NULL;
  long i;
  int err = 0;
  long pos = 0;
  long bmaplen = 0;
  const int bit_padding = 16;
  double miss_values = 0;
  tlen = ((*len+bit_padding-1)/bit_padding*bit_padding)/8;

  if((err = grib_get_double_internal(grib_handle_of_accessor(a), self->missing_value, &miss_values)) != GRIB_SUCCESS)
  {
    grib_context_log(a->context, GRIB_LOG_ERROR, "grib_accessor_class_bitmap : pack_double : Cannot unpack %s err=%d ",self->missing_value,err);
    return err;
  }

  buf = grib_context_malloc_clear(a->context,tlen);
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

  if((err = grib_set_long_internal(grib_handle_of_accessor(a), self->unusedBits,tlen*8 - *len )) != GRIB_SUCCESS) {
    grib_context_log(a->context, GRIB_LOG_ERROR, "grib_accessor_class_bitmap : pack_double : Cannot pack %s err=%d ",self->unusedBits,err);
    grib_context_free(a->context,buf);
    return err;
  }


  grib_buffer_replace(a, buf, tlen,1,1);

  grib_context_free(a->context,buf);

  return GRIB_SUCCESS;
}

#else

static int pack_double(grib_accessor* a, const double* val,size_t *len){
  return GRIB_NOT_IMPLEMENTED;
}

#endif
