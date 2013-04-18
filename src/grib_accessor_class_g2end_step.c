/*
 * Copyright 2005-2013 ECMWF.
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
  MEMBERS = const char* start_step
  MEMBERS = const char* unit
  
  MEMBERS = const char* year
  MEMBERS = const char* month
  MEMBERS = const char* day
  MEMBERS = const char* hour
  MEMBERS = const char* minute
  MEMBERS = const char* second
  
  MEMBERS = const char* year_of_end_of_interval
  MEMBERS = const char* month_of_end_of_interval
  MEMBERS = const char* day_of_end_of_interval
  MEMBERS = const char* hour_of_end_of_interval
  MEMBERS = const char* minute_of_end_of_interval
  MEMBERS = const char* second_of_end_of_interval

  MEMBERS = const char* coded_unit
  MEMBERS = const char* coded_time_range

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
static void dump(grib_accessor*, grib_dumper*);
static void init(grib_accessor*,const long, grib_arguments* );
static void init_class(grib_accessor_class*);

typedef struct grib_accessor_g2end_step {
    grib_accessor          att;
/* Members defined in gen */
/* Members defined in long */
/* Members defined in g2end_step */
	const char* start_step;
	const char* unit;
	const char* year;
	const char* month;
	const char* day;
	const char* hour;
	const char* minute;
	const char* second;
	const char* year_of_end_of_interval;
	const char* month_of_end_of_interval;
	const char* day_of_end_of_interval;
	const char* hour_of_end_of_interval;
	const char* minute_of_end_of_interval;
	const char* second_of_end_of_interval;
	const char* coded_unit;
	const char* coded_time_range;
} grib_accessor_g2end_step;

extern grib_accessor_class* grib_accessor_class_long;

static grib_accessor_class _grib_accessor_class_g2end_step = {
    &grib_accessor_class_long,                      /* super                     */
    "g2end_step",                      /* name                      */
    sizeof(grib_accessor_g2end_step),  /* size                      */
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
    0,               /* grib_pack procedures long      */
    &pack_long,                  /* grib_pack procedures long      */
    &unpack_long,                /* grib_unpack procedures long    */
    0,                /* grib_pack procedures double    */
    0,              /* grib_unpack procedures double  */
    0,                /* grib_pack procedures string    */
    0,              /* grib_unpack procedures string  */
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
    0,             		/* clear          */
};


grib_accessor_class* grib_accessor_class_g2end_step = &_grib_accessor_class_g2end_step;


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
	c->pack_double	=	(*(c->super))->pack_double;
	c->unpack_double	=	(*(c->super))->unpack_double;
	c->pack_string	=	(*(c->super))->pack_string;
	c->unpack_string	=	(*(c->super))->unpack_string;
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
}

/* END_CLASS_IMP */

static void init(grib_accessor* a,const long l, grib_arguments* c)
{
	grib_accessor_g2end_step* self = (grib_accessor_g2end_step*)a;
	int n = 0;

	self->start_step = grib_arguments_get_name(a->parent->h,c,n++);
	self->unit  = grib_arguments_get_name(a->parent->h,c,n++);

	self->year = grib_arguments_get_name(a->parent->h,c,n++);
	self->month = grib_arguments_get_name(a->parent->h,c,n++);
	self->day = grib_arguments_get_name(a->parent->h,c,n++);
	self->hour  = grib_arguments_get_name(a->parent->h,c,n++);
	self->minute  = grib_arguments_get_name(a->parent->h,c,n++);
	self->second  = grib_arguments_get_name(a->parent->h,c,n++);

	self->year_of_end_of_interval = grib_arguments_get_name(a->parent->h,c,n++);
	self->month_of_end_of_interval  = grib_arguments_get_name(a->parent->h,c,n++);
	self->day_of_end_of_interval  = grib_arguments_get_name(a->parent->h,c,n++);
	self->hour_of_end_of_interval = grib_arguments_get_name(a->parent->h,c,n++);
	self->minute_of_end_of_interval = grib_arguments_get_name(a->parent->h,c,n++);
	self->second_of_end_of_interval = grib_arguments_get_name(a->parent->h,c,n++);

	self->coded_unit  = grib_arguments_get_name(a->parent->h,c,n++);
	self->coded_time_range  = grib_arguments_get_name(a->parent->h,c,n++);
}

static void dump(grib_accessor* a, grib_dumper* dumper)
{
	grib_dump_double(dumper,a,NULL);
}

static int u2s2[] =  {
  60,      /* (0) minutes */
  3600,    /* (1) hour    */
  86400,   /* (2) day     */
  2592000, /* (3) month     */
  -1,      /* (4) */
  -1,      /* (5) */
  -1,      /* (6) */
  -1,      /* (7) */
  -1,      /* (8) */
  -1,      /* (9) */
  10800,   /* (10) 3 hours */
  21600,   /* (11) 6 hours */
  43200,   /* (12) 12 hours */
  1        /* (13) seconds  */
};

static int u2s[] =  {
  60,      /* (0) minutes */
  3600,    /* (1) hour    */
  86400,   /* (2) day     */
  2592000, /* (3) month     */
  -1,      /* (4) */
  -1,      /* (5) */
  -1,      /* (6) */
  -1,      /* (7) */
  -1,      /* (8) */
  -1,      /* (9) */
  10800,   /* (10) 3 hours */
  21600,   /* (11) 6 hours */
  43200,   /* (12) 12 hours */
  1,       /* (13) seconds  */
  900,     /* (14) 15 minutes  */
  1800     /* (15) 30 minutes  */
};

static int unpack_long(grib_accessor* a, long* val, size_t *len)
{
	grib_accessor_g2end_step* self = (grib_accessor_g2end_step*)a;
	int err = 0;
	long start_step;
	long unit;
	long coded_unit;
	long coded_time_range;
	long coded_time_range_sec=0;
	int factor;
	long u2sf,u2sf_step_unit;

	grib_handle* h=a->parent->h;

	if((err = grib_get_long_internal(h,self->start_step,&start_step))) return err;

	/*point in time */
	if (self->year == NULL) {
		*val=start_step;
		return 0;
	}

	if((err = grib_get_long_internal(h,self->unit,&unit))) return err;

	if((err = grib_get_long_internal(h,self->coded_unit,&coded_unit))) return err;
	if((err = grib_get_long_internal(h,self->coded_time_range,
			&coded_time_range))) return err;

	if (coded_unit!=unit) {
		coded_time_range_sec=coded_time_range*u2s2[coded_unit];
		if (coded_time_range_sec<0) {
			factor=60;
			if (u2s2[coded_unit] % factor) return GRIB_DECODING_ERROR;
			if (u2s[unit] % factor) return GRIB_DECODING_ERROR;
			u2sf=u2s2[coded_unit]/factor;
			coded_time_range_sec=coded_time_range*u2sf;
			u2sf_step_unit=u2s[unit]/factor;
		} else {
			u2sf_step_unit=u2s[unit];
		}
		if (coded_time_range_sec % u2sf_step_unit!=0) {
			grib_context_log(h->context,GRIB_LOG_ERROR,"unable to convert endStep in stepUnits");
			return GRIB_WRONG_STEP_UNIT;
		}
		coded_time_range = coded_time_range_sec / u2sf_step_unit;
	}

	*val = start_step + coded_time_range;

	return GRIB_SUCCESS;
}

static int pack_long(grib_accessor* a, const long* val, size_t *len)
{
	grib_accessor_g2end_step* self = (grib_accessor_g2end_step*)a;
	grib_handle* h=a->parent->h;

	int err = 0;

	long year;
	long month;
	long day;
	long hour;
	long minute;
	long second;

	long start_step;
	long unit,coded_unit;
	long year_of_end_of_interval;
	long month_of_end_of_interval;
	long day_of_end_of_interval;
	long hour_of_end_of_interval;
	long minute_of_end_of_interval = 0;
	long second_of_end_of_interval = 0;

	long coded_time_range,time_range;

	double dend, dstep;

	/*point in time */
	if (self->year == NULL) {
		err = grib_set_long_internal(h,self->start_step,*val);
		return err;
	}

	if((err = grib_get_long_internal(h,self->coded_unit,&coded_unit))) return err;
	if((err = grib_get_long_internal(h,self->unit,&unit))) return err;
	if((err = grib_get_long_internal(h,self->year,&year))) return err;
	if((err = grib_get_long_internal(h,self->month,&month))) return err;
	if((err = grib_get_long_internal(h,self->day,&day))) return err;
	if((err = grib_get_long_internal(h,self->hour,&hour))) return err;
	if((err = grib_get_long_internal(h,self->minute,&minute))) return err;
	if((err = grib_get_long_internal(h,self->second,&second))) return err;

	if((err = grib_get_long_internal(h,self->start_step,&start_step)))
		return err;

	time_range  = *val-start_step;

	if (time_range<0){
		grib_context_log(h->context,GRIB_LOG_ERROR,
				"endStep < startStep (%ld < %ld)",*val,start_step);
		return GRIB_WRONG_STEP;
	}

	err=grib_datetime_to_julian(year,month,day,hour,minute,second,&dend);
	if (err!=GRIB_SUCCESS) return err;

	dstep=(((double)(*val))*u2s[unit])/u2s[2];
	dend+=dstep;

	err=grib_julian_to_datetime(dend,&year_of_end_of_interval,&month_of_end_of_interval,
			&day_of_end_of_interval,&hour_of_end_of_interval,
			&minute_of_end_of_interval,&second_of_end_of_interval);
	if (err!=GRIB_SUCCESS) return err;

	if((err = grib_set_long_internal(a->parent->h,self->year_of_end_of_interval,
			year_of_end_of_interval))) return err;
	if((err = grib_set_long_internal(a->parent->h,self->month_of_end_of_interval,
			month_of_end_of_interval))) return err;
	if((err = grib_set_long_internal(a->parent->h,self->day_of_end_of_interval,
			day_of_end_of_interval))) return err;
	if((err = grib_set_long_internal(a->parent->h,self->hour_of_end_of_interval,
			hour_of_end_of_interval))) return err;
	if((err = grib_set_long_internal(a->parent->h,self->minute_of_end_of_interval,
			minute_of_end_of_interval))) return err;
	if((err = grib_set_long_internal(a->parent->h,self->second_of_end_of_interval,
			second_of_end_of_interval))) return err;

	if (time_range*u2s[unit]%u2s2[coded_unit]) {
		coded_unit=unit;
		if((err = grib_set_long_internal(a->parent->h,self->coded_unit,
				coded_unit))) return err;
		coded_time_range=time_range;
	} else
		coded_time_range=(time_range*u2s[unit])/u2s2[coded_unit];

	if((err = grib_set_long_internal(a->parent->h,self->coded_time_range,
			coded_time_range))) return err;

	return GRIB_SUCCESS;
}

