/*
 * Copyright 2005-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

/********************************************
 *  Enrico Fucile
 *********************************************/

#include "grib_api_internal.h"
/*
   This is used by make_class.pl

   START_CLASS_DEF
   CLASS      = accessor
   SUPER      = grib_accessor_class_double
   IMPLEMENTS = unpack_double;pack_double
   IMPLEMENTS = is_missing;pack_missing
   IMPLEMENTS = init
   MEMBERS=const char*                  grid
   MEMBERS=int                          index
   MEMBERS=const char*                  given
   END_CLASS_DEF

 */

/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "accessor.class" and rerun ./make_class.pl

*/

static int pack_missing(grib_accessor*);
static int is_missing(grib_accessor*);
static int pack_double(grib_accessor*, const double* val,size_t *len);
static int unpack_double(grib_accessor*, double* val,size_t *len);
static void init(grib_accessor*,const long, grib_arguments* );
static void init_class(grib_accessor_class*);

typedef struct grib_accessor_g2latlon {
    grib_accessor          att;
/* Members defined in gen */
/* Members defined in double */
/* Members defined in g2latlon */
	const char*                  grid;
	int                          index;
	const char*                  given;
} grib_accessor_g2latlon;

extern grib_accessor_class* grib_accessor_class_double;

static grib_accessor_class _grib_accessor_class_g2latlon = {
    &grib_accessor_class_double,                      /* super                     */
    "g2latlon",                      /* name                      */
    sizeof(grib_accessor_g2latlon),  /* size                      */
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
    &pack_missing,               /* grib_pack procedures long      */
    &is_missing,                 /* grib_pack procedures long      */
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


grib_accessor_class* grib_accessor_class_g2latlon = &_grib_accessor_class_g2latlon;


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

static void init(grib_accessor* a,const long l, grib_arguments* c)
{
  grib_accessor_g2latlon* self = (grib_accessor_g2latlon*)a;
  int n = 0;

  self->grid     = grib_arguments_get_name(grib_handle_of_accessor(a),c,n++);
  self->index    = grib_arguments_get_long(grib_handle_of_accessor(a),c,n++);
  self->given    = grib_arguments_get_name(grib_handle_of_accessor(a),c,n++);
}

static int unpack_double   (grib_accessor* a, double* val, size_t *len)
{
  grib_accessor_g2latlon* self = (grib_accessor_g2latlon*)a;
  int ret = 0;

  long given        = 1;
  double grid[6];
  size_t size = 6;

  if(*len < 1){
    ret = GRIB_ARRAY_TOO_SMALL;
    return ret;
  }

  if(self->given)
    if((ret = grib_get_long_internal(grib_handle_of_accessor(a), self->given,&given)) != GRIB_SUCCESS)
      return ret;

  if(!given)
  {
    *val =  GRIB_MISSING_DOUBLE;
    return GRIB_SUCCESS;
  }


  if((ret = grib_get_double_array_internal(grib_handle_of_accessor(a), self->grid,grid,&size)) != GRIB_SUCCESS)
    return ret;

  *val = grid[self->index];

  return GRIB_SUCCESS;
}


static int pack_double(grib_accessor* a, const double* val, size_t *len)
{
  grib_accessor_g2latlon* self = (grib_accessor_g2latlon*)a;
  int ret = 0;

  double grid[6];
  size_t size = 6;

  if(*len < 1){
    ret = GRIB_ARRAY_TOO_SMALL;
    return ret;
  }

  if(self->given)
  {
    long given        = *val != GRIB_MISSING_DOUBLE;
    if((ret = grib_set_long_internal(grib_handle_of_accessor(a), self->given,given)) != GRIB_SUCCESS)
      return ret;
  }


  if((ret = grib_get_double_array_internal(grib_handle_of_accessor(a), self->grid,grid,&size)) != GRIB_SUCCESS)
    return ret;

  if ( (self->index == 1 || self->index == 3) && *val < 0 ) grid[self->index] = 360+*val;
  else grid[self->index] = *val;

  return grib_set_double_array_internal(grib_handle_of_accessor(a), self->grid,grid,size);
}

static int pack_missing(grib_accessor* a)
{
  grib_accessor_g2latlon* self = (grib_accessor_g2latlon*)a;
  double missing = GRIB_MISSING_DOUBLE;
  size_t size = 1;

  if(!self->given)
    return GRIB_NOT_IMPLEMENTED;

  return pack_double(a,&missing,&size);
}

static int is_missing(grib_accessor* a)
{
  grib_accessor_g2latlon* self = (grib_accessor_g2latlon*)a;
  long given        = 1;


  if(self->given) grib_get_long_internal(grib_handle_of_accessor(a), self->given,&given);


  return !given;
}


