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
   IMPLEMENTS = init
   IMPLEMENTS = unpack_double
   IMPLEMENTS = dump;get_native_type
   MEMBERS=const char*  primary_bitmap
   MEMBERS=const char*  secondary_bitmap
   MEMBERS=const char*  missing_value
   MEMBERS=const char*  expand_by
   END_CLASS_DEF
 */

/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "accessor.class" and rerun ./make_class.pl

*/

static int  get_native_type(grib_accessor*);
static int unpack_double(grib_accessor*, double* val,size_t *len);
static void dump(grib_accessor*, grib_dumper*);
static void init(grib_accessor*,const long, grib_arguments* );
static void init_class(grib_accessor_class*);

typedef struct grib_accessor_data_secondary_bitmap {
    grib_accessor          att;
/* Members defined in gen */
/* Members defined in data_secondary_bitmap */
	const char*  primary_bitmap;
	const char*  secondary_bitmap;
	const char*  missing_value;
	const char*  expand_by;
} grib_accessor_data_secondary_bitmap;

extern grib_accessor_class* grib_accessor_class_gen;

static grib_accessor_class _grib_accessor_class_data_secondary_bitmap = {
    &grib_accessor_class_gen,                      /* super                     */
    "data_secondary_bitmap",                      /* name                      */
    sizeof(grib_accessor_data_secondary_bitmap),  /* size                      */
    0,                           /* inited */
    &init_class,                 /* init_class */
    &init,                       /* init                      */
    0,                  /* post_init                      */
    0,                    /* free mem                       */
    &dump,                       /* describes himself         */
    0,                /* get length of section     */
    0,              /* get length of string      */
    0,                /* get number of values      */
    0,                 /* get number of bytes      */
    0,                /* get offset to bytes           */
    &get_native_type,            /* get native type               */
    0,                /* get sub_section                */
    0,               /* grib_pack procedures long      */
    0,                 /* grib_pack procedures long      */
    0,                  /* grib_pack procedures long      */
    0,                /* grib_unpack procedures long    */
    0,                /* grib_pack procedures double    */
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


grib_accessor_class* grib_accessor_class_data_secondary_bitmap = &_grib_accessor_class_data_secondary_bitmap;


static void init_class(grib_accessor_class* c)
{
	c->next_offset	=	(*(c->super))->next_offset;
	c->string_length	=	(*(c->super))->string_length;
	c->value_count	=	(*(c->super))->value_count;
	c->byte_count	=	(*(c->super))->byte_count;
	c->byte_offset	=	(*(c->super))->byte_offset;
	c->sub_section	=	(*(c->super))->sub_section;
	c->pack_missing	=	(*(c->super))->pack_missing;
	c->is_missing	=	(*(c->super))->is_missing;
	c->pack_long	=	(*(c->super))->pack_long;
	c->unpack_long	=	(*(c->super))->unpack_long;
	c->pack_double	=	(*(c->super))->pack_double;
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
  grib_accessor_data_secondary_bitmap *self =(grib_accessor_data_secondary_bitmap*)a;


  self->primary_bitmap   = grib_arguments_get_name(grib_handle_of_accessor(a),args,0);
  self->secondary_bitmap = grib_arguments_get_name(grib_handle_of_accessor(a),args,1);
  self->missing_value    = grib_arguments_get_name(grib_handle_of_accessor(a),args,2);
  self->expand_by        = grib_arguments_get_name(grib_handle_of_accessor(a),args,3);

  a->length              = 0;
}
static void dump(grib_accessor* a, grib_dumper* dumper)
{
  grib_dump_values(dumper,a);
}




static int  unpack_double(grib_accessor* a, double* val, size_t *len)
{
  grib_accessor_data_secondary_bitmap* self =  (grib_accessor_data_secondary_bitmap*)a;

  size_t i = 0;
  size_t j = 0;
  size_t k = 0;
  size_t m = 0;
  size_t n_vals = 0;
  long nn=0;
  long expand_by =0;
  int err = 0;
  size_t primary_len;
  size_t secondary_len;
  double* primary_vals;
  double* secondary_vals;
  err=grib_value_count(a,&nn);
  n_vals=nn;
  if (err) return err;

  if(*len < n_vals)
  {
    *len = n_vals;
    return GRIB_ARRAY_TOO_SMALL;
  }

  if((err = grib_get_long(grib_handle_of_accessor(a),self->expand_by,&expand_by)) != GRIB_SUCCESS)
    return err;

  if((err = grib_get_size(grib_handle_of_accessor(a),self->primary_bitmap,&primary_len)) != GRIB_SUCCESS)
    return err;

  if((err = grib_get_size(grib_handle_of_accessor(a),self->secondary_bitmap,&secondary_len)) != GRIB_SUCCESS)
    return err;

  primary_vals = (double*)grib_context_malloc(a->context,primary_len*sizeof(double));
  if(!primary_vals)
    return GRIB_OUT_OF_MEMORY;

  secondary_vals = (double*)grib_context_malloc(a->context,secondary_len*sizeof(double));
  if(!secondary_vals)
  {
    grib_context_free(a->context,primary_vals);
    return GRIB_OUT_OF_MEMORY;
  }

  if((err = grib_get_double_array_internal(grib_handle_of_accessor(a),self->primary_bitmap,primary_vals,&primary_len)) != GRIB_SUCCESS)
  {
    grib_context_free(a->context,secondary_vals);
    grib_context_free(a->context,primary_vals);
    return err;
  }

  if((err = grib_get_double_array_internal(grib_handle_of_accessor(a),self->secondary_bitmap,secondary_vals,&secondary_len)) != GRIB_SUCCESS)
  {
    grib_context_free(a->context,secondary_vals);
    grib_context_free(a->context,primary_vals);
    return err;
  }

  k = 0;
  m = 0;
  for(i=0;i < primary_len;i++)
  {
    /* if(primary_vals[i]) f++; */
    if(primary_vals[i])
    {
      for(j = 0; j < expand_by; j++)
        val[k++] = secondary_vals[m++];
    }
    else {
      for(j = 0; j < expand_by; j++)
        val[k++] = 0;
    }

  }

  Assert(k <= *len);
  Assert(m <= secondary_len);

  /*printf("FOOBAR %d %d %ld %d\n",f,primary_len,expand_by,n_vals);*/

  *len =  n_vals;

  grib_context_free(a->context,primary_vals);
  grib_context_free(a->context,secondary_vals);
  return err;
}


static int  get_native_type(grib_accessor* a)
{
  /*  grib_accessor_data_secondary_bitmap* self =  (grib_accessor_data_secondary_bitmap*)a;
    return grib_accessor_get_native_type(grib_find_accessor(grib_handle_of_accessor(a),self->coded_values));*/

  return GRIB_TYPE_DOUBLE;
}
