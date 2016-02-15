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
   IMPLEMENTS = unpack_double;pack_double
   IMPLEMENTS = unpack_long;pack_long;init;dump;is_missing
   MEMBERS=const char* type_first
   MEMBERS=const char* scale_first
   MEMBERS=const char* value_first
   MEMBERS=const char* pressure_units
   END_CLASS_DEF

 */

/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "accessor.class" and rerun ./make_class.pl

*/

static int is_missing(grib_accessor*);
static int pack_double(grib_accessor*, const double* val,size_t *len);
static int pack_long(grib_accessor*, const long* val,size_t *len);
static int unpack_double(grib_accessor*, double* val,size_t *len);
static int unpack_long(grib_accessor*, long* val,size_t *len);
static void dump(grib_accessor*, grib_dumper*);
static void init(grib_accessor*,const long, grib_arguments* );
static void init_class(grib_accessor_class*);

typedef struct grib_accessor_g2level {
    grib_accessor          att;
/* Members defined in gen */
/* Members defined in long */
/* Members defined in g2level */
	const char* type_first;
	const char* scale_first;
	const char* value_first;
	const char* pressure_units;
} grib_accessor_g2level;

extern grib_accessor_class* grib_accessor_class_long;

static grib_accessor_class _grib_accessor_class_g2level = {
    &grib_accessor_class_long,                      /* super                     */
    "g2level",                      /* name                      */
    sizeof(grib_accessor_g2level),  /* size                      */
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
    0,            /* get native type               */
    0,                /* get sub_section                */
    0,               /* grib_pack procedures long      */
    &is_missing,                 /* grib_pack procedures long      */
    &pack_long,                  /* grib_pack procedures long      */
    &unpack_long,                /* grib_unpack procedures long    */
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


grib_accessor_class* grib_accessor_class_g2level = &_grib_accessor_class_g2level;


static void init_class(grib_accessor_class* c)
{
	c->next_offset	=	(*(c->super))->next_offset;
	c->string_length	=	(*(c->super))->string_length;
	c->value_count	=	(*(c->super))->value_count;
	c->byte_count	=	(*(c->super))->byte_count;
	c->byte_offset	=	(*(c->super))->byte_offset;
	c->get_native_type	=	(*(c->super))->get_native_type;
	c->sub_section	=	(*(c->super))->sub_section;
	c->pack_missing	=	(*(c->super))->pack_missing;
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
  grib_accessor_g2level* self = (grib_accessor_g2level*)a;
  int n = 0;

  self->type_first   = grib_arguments_get_name(grib_handle_of_accessor(a),c,n++);
  self->scale_first  = grib_arguments_get_name(grib_handle_of_accessor(a),c,n++);
  self->value_first  = grib_arguments_get_name(grib_handle_of_accessor(a),c,n++);
  self->pressure_units = grib_arguments_get_name(grib_handle_of_accessor(a),c,n++);
}

static void dump(grib_accessor* a, grib_dumper* dumper)
{
  grib_dump_long(dumper,a,NULL);
}

static int unpack_double(grib_accessor* a, double* val, size_t *len)
{
	int ret=0;
	grib_accessor_g2level* self = (grib_accessor_g2level*)a;

	long type_first   = 0;
	long scale_first  = 0;
	long value_first  = 0;
	char pressure_units[10]={0,};
	size_t pressure_units_len=10;

	double v;

	if((ret = grib_get_long_internal(grib_handle_of_accessor(a), self->type_first,&type_first))
			!=GRIB_SUCCESS) return ret;
	if((ret = grib_get_long_internal(grib_handle_of_accessor(a), self->scale_first,&scale_first))
			!=GRIB_SUCCESS) return ret;
	if((ret = grib_get_long_internal(grib_handle_of_accessor(a), self->value_first,&value_first))
			!=GRIB_SUCCESS) return ret;
	if((ret = grib_get_string_internal(grib_handle_of_accessor(a), self->pressure_units,pressure_units,&pressure_units_len))
			!=GRIB_SUCCESS) return ret;

	if (value_first == GRIB_MISSING_LONG) {
		*val=0;
		return GRIB_SUCCESS;
	}
	/* value = value_first * 10 ^ -scale_first */

	if(*len < 1)
		return GRIB_WRONG_ARRAY_SIZE;

	v = value_first;

	if(scale_first != GRIB_MISSING_LONG)
	{
        /* GRIB-637 Potential vorticity surface */
        if (type_first == 109) {
            scale_first -= 6;
        }

		while(scale_first<0 && v!=0)  { v *= 10.0; scale_first ++; }
		while(scale_first>0 && v!=0)  { v /= 10.0; scale_first --; }
	}

	switch(type_first)
	{
	case 100: /* Isobaric surface (Pa) */
		if (!strcmp(pressure_units,"hPa"))
		{
			long x= v/100.0;  /* 1 hPa = 100 Pa */
			if (scale_first == 0 && x==0)
			{
				/* Switch to Pa instead of hPa as the value is less than a hectoPascal */
				char pa[]="Pa";
				size_t lpa=strlen(pa);
				if((ret = grib_set_string_internal(grib_handle_of_accessor(a), self->pressure_units,pa,&lpa)) !=GRIB_SUCCESS)
					return ret;
			}
			else
			{
				v=x;
			}
		}
		break;
	}

    *val=v;

	return GRIB_SUCCESS;
}

static int unpack_long(grib_accessor* a, long* val, size_t *len)
{
    double dval = 0;
    int ret = unpack_double(a, &dval, len);
    if (ret == GRIB_SUCCESS) {
        *val = (long)(dval+0.5); /* round up */
    }
    return ret;
}

static int pack_double(grib_accessor* a, const double* val, size_t *len)
{
    grib_accessor_g2level* self = (grib_accessor_g2level*)a;
    int ret=0;
    double value_first = *val;
    long scale_first = 0;
    long type_first   = 0;
    char pressure_units[10]={0,};
    size_t pressure_units_len=10;

    if(*len !=  1)
        return GRIB_WRONG_ARRAY_SIZE;

    if((ret = grib_get_long_internal(grib_handle_of_accessor(a), self->type_first,&type_first))
            !=GRIB_SUCCESS) return ret;

    if((ret = grib_get_string_internal(grib_handle_of_accessor(a), self->pressure_units,pressure_units,&pressure_units_len))
            !=GRIB_SUCCESS) return ret;

    switch(type_first)
    {
    case 100: /* Pa */
        scale_first  = 0;
        if (!strcmp(pressure_units,"hPa"))
            value_first *= 100;
        break;

    default:
        break;
    }
    /*
     * final = scaled_value * 10 ^ -scale_factor
     *      = scaled_value / (10^scale_factor)
     *
     * Choose 2 decimal places
     */
    scale_first = 2;
    value_first *= 100;
    value_first = value_first+0.5; /* round up */

    if ( type_first>9 ) {
        if((ret = grib_set_long_internal(grib_handle_of_accessor(a), self->scale_first,scale_first))
                !=GRIB_SUCCESS) return ret;
        if((ret = grib_set_long_internal(grib_handle_of_accessor(a), self->value_first,(long)value_first))
                !=GRIB_SUCCESS) return ret;
    }

    return GRIB_SUCCESS;
}

static int pack_long(grib_accessor* a, const long* val, size_t *len)
{
  int ret=0;
  long value_first = *val;
  long scale_first = 0;
  long type_first   = 0;
  char pressure_units[10]={0,};
  size_t pressure_units_len=10;

  grib_accessor_g2level* self = (grib_accessor_g2level*)a;

  if(*len !=  1)
    return GRIB_WRONG_ARRAY_SIZE;

/*Not sure if this is necessary
 *     if (value_first == GRIB_MISSING_LONG) {
 *         if ((ret=grib_set_missing_internal(grib_handle_of_accessor(a), self->scale_first)) != GRIB_SUCCESS)
 *             return ret;
 *         if ((ret=grib_set_missing_internal(grib_handle_of_accessor(a), self->value_first)) != GRIB_SUCCESS)
 *                 return ret;
 *         return GRIB_SUCCESS;
 *     }
 */

  if((ret = grib_get_long_internal(grib_handle_of_accessor(a), self->type_first,&type_first))
      !=GRIB_SUCCESS) return ret;

  if((ret = grib_get_string_internal(grib_handle_of_accessor(a), self->pressure_units,pressure_units,&pressure_units_len))
      !=GRIB_SUCCESS) return ret;
  
  switch(type_first)
  {
    case 100: /* Pa */
      scale_first  = 0;
      if (!strcmp(pressure_units,"hPa")) 
        value_first *= 100;
      break;

    default:
      break;
  }

  if ( type_first>9 ) {
	if((ret = grib_set_long_internal(grib_handle_of_accessor(a), self->scale_first,scale_first))
		!=GRIB_SUCCESS) return ret;
	if((ret = grib_set_long_internal(grib_handle_of_accessor(a), self->value_first,value_first))
		!=GRIB_SUCCESS) return ret;
  }
  return GRIB_SUCCESS;
}

static int is_missing(grib_accessor* a){

  grib_accessor_g2level* self = (grib_accessor_g2level*)a;
  int err=0;
  int ret=0;

  ret=grib_is_missing(grib_handle_of_accessor(a), self->scale_first,&err) +
      grib_is_missing(grib_handle_of_accessor(a), self->value_first,&err);
  return ret;
}
