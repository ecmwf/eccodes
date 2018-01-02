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
   SUPER      = grib_accessor_class_double
   IMPLEMENTS = unpack_long;pack_long
   IMPLEMENTS = unpack_double;pack_double
   IMPLEMENTS = dump
   IMPLEMENTS = init
   MEMBERS    = const char *date
   MEMBERS    = const char *hour
   MEMBERS    = const char *minute
   MEMBERS    = const char *second
   END_CLASS_DEF

 */

/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "accessor.class" and rerun ./make_class.pl

*/

static int pack_double(grib_accessor*, const double* val,size_t *len);
static int pack_long(grib_accessor*, const long* val,size_t *len);
static int unpack_double(grib_accessor*, double* val,size_t *len);
static int unpack_long(grib_accessor*, long* val,size_t *len);
static void dump(grib_accessor*, grib_dumper*);
static void init(grib_accessor*,const long, grib_arguments* );
static void init_class(grib_accessor_class*);

typedef struct grib_accessor_julian_day {
    grib_accessor          att;
/* Members defined in gen */
/* Members defined in double */
/* Members defined in julian_day */
	const char *date;
	const char *hour;
	const char *minute;
	const char *second;
} grib_accessor_julian_day;

extern grib_accessor_class* grib_accessor_class_double;

static grib_accessor_class _grib_accessor_class_julian_day = {
    &grib_accessor_class_double,                      /* super                     */
    "julian_day",                      /* name                      */
    sizeof(grib_accessor_julian_day),  /* size                      */
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
    0,                 /* grib_pack procedures long      */
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


grib_accessor_class* grib_accessor_class_julian_day = &_grib_accessor_class_julian_day;


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
	c->is_missing	=	(*(c->super))->is_missing;
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
  grib_accessor_julian_day* self = (grib_accessor_julian_day*)a;
  int n=0;

  self->date = grib_arguments_get_name(grib_handle_of_accessor(a),c,n++);
  self->hour = grib_arguments_get_name(grib_handle_of_accessor(a),c,n++);
  self->minute = grib_arguments_get_name(grib_handle_of_accessor(a),c,n++);
  self->second = grib_arguments_get_name(grib_handle_of_accessor(a),c,n++);

  a->length=0;
}


static void dump(grib_accessor* a, grib_dumper* dumper)
{
  grib_dump_string(dumper,a,NULL);
}

static int pack_long(grib_accessor* a, const long* val, size_t *len)
{
  const double v=*val;
  return pack_double(a,&v,len);
}

static int pack_double(grib_accessor* a, const double* val, size_t *len)
{
  grib_accessor_julian_day* self = (grib_accessor_julian_day*)a;
  int ret=0;
  long hour=0;
  long minute=0;
  long second=0;
  long date=0;
  long year,month,day;
  
  ret=grib_julian_to_datetime(*val,&year,&month,&day,&hour,&minute,&second);

  date=year * 10000 + month * 100 + day;

  ret=grib_set_long_internal(grib_handle_of_accessor(a),self->date,date);
  if (ret!=0) return ret;
  ret=grib_set_long_internal(grib_handle_of_accessor(a),self->hour,hour);
  if (ret!=0) return ret;
  ret=grib_set_long_internal(grib_handle_of_accessor(a),self->minute,minute);
  if (ret!=0) return ret;
  ret=grib_set_long_internal(grib_handle_of_accessor(a),self->second,second);

  return ret;
}

static int    unpack_long   (grib_accessor* a, long* val, size_t *len)
{
  int ret=0;
  double v=0;
  
  ret=unpack_double(a,&v,len);
  *val=(long)v;

  return ret;
}

static int    unpack_double   (grib_accessor* a, double* val, size_t *len)
{
  int ret=0;
  long date,hour,minute,second;
  long year,month,day;
  grib_accessor_julian_day* self = (grib_accessor_julian_day*)a;

  ret=grib_get_long_internal(grib_handle_of_accessor(a),self->date,&date);
  if (ret!=GRIB_SUCCESS) return ret;
  ret=grib_get_long_internal(grib_handle_of_accessor(a),self->hour,&hour);
  if (ret!=GRIB_SUCCESS) return ret;
  ret=grib_get_long_internal(grib_handle_of_accessor(a),self->minute,&minute);
  if (ret!=GRIB_SUCCESS) return ret;
  ret=grib_get_long_internal(grib_handle_of_accessor(a),self->second,&second);
  if (ret!=GRIB_SUCCESS) return ret;

  year = date / 10000;
  date %= 10000;
  month  = date / 100;
  date %= 100;
  day = date;

  ret=grib_datetime_to_julian(year,month,day,hour,minute,second,val);

  return ret;
}

