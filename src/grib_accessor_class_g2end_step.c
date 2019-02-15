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
  MEMBERS = const char* typeOfTimeIncrement
  MEMBERS = const char* numberOfTimeRange

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
	const char* typeOfTimeIncrement;
	const char* numberOfTimeRange;
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
    grib_accessor_g2end_step* self = (grib_accessor_g2end_step*)a;
    int n = 0;
    grib_handle* h = grib_handle_of_accessor(a);

    self->start_step = grib_arguments_get_name(h,c,n++);
    self->unit  = grib_arguments_get_name(h,c,n++);

    self->year = grib_arguments_get_name(h,c,n++);
    self->month = grib_arguments_get_name(h,c,n++);
    self->day = grib_arguments_get_name(h,c,n++);
    self->hour  = grib_arguments_get_name(h,c,n++);
    self->minute  = grib_arguments_get_name(h,c,n++);
    self->second  = grib_arguments_get_name(h,c,n++);

    self->year_of_end_of_interval = grib_arguments_get_name(h,c,n++);
    self->month_of_end_of_interval  = grib_arguments_get_name(h,c,n++);
    self->day_of_end_of_interval  = grib_arguments_get_name(h,c,n++);
    self->hour_of_end_of_interval = grib_arguments_get_name(h,c,n++);
    self->minute_of_end_of_interval = grib_arguments_get_name(h,c,n++);
    self->second_of_end_of_interval = grib_arguments_get_name(h,c,n++);

    self->coded_unit  = grib_arguments_get_name(h,c,n++);
    self->coded_time_range  = grib_arguments_get_name(h,c,n++);
    self->typeOfTimeIncrement = grib_arguments_get_name(h,c,n++);
    self->numberOfTimeRange = grib_arguments_get_name(h,c,n++);
}

static void dump(grib_accessor* a, grib_dumper* dumper)
{
    grib_dump_double(dumper,a,NULL);
}

static const int u2s2[] =  {
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

static const int u2s[] =  {
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

/* See GRIB-488 */
static int is_special_expver(grib_handle* h)
{
    int ret = 0;
    char strExpVer[50]={0,};
    size_t slen=50;
    ret = grib_get_string(h, "experimentVersionNumber", strExpVer, &slen);
    if (ret == GRIB_SUCCESS && !strcmp(strExpVer, "1605")) {
        return 1; /* Special case of expVer 1605! */
    }

    return 0;
}

static int convert_time_range(
        grib_handle* h,
        long stepUnits,                      /* unit */
        long indicatorOfUnitForTimeRange,    /* coded_unit */
        long* lengthOfTimeRange              /* coded_time_range */
        )
{
    Assert(lengthOfTimeRange != NULL);

    if (indicatorOfUnitForTimeRange != stepUnits) {
        long u2sf_step_unit;
        long coded_time_range_sec = (*lengthOfTimeRange)*u2s2[indicatorOfUnitForTimeRange];
        if (coded_time_range_sec < 0) {
            long u2sf;
            int factor = 60;
            if (u2s2[indicatorOfUnitForTimeRange] % factor)
                return GRIB_DECODING_ERROR;
            if (u2s[stepUnits] % factor)
                return GRIB_DECODING_ERROR;
            u2sf = u2s2[indicatorOfUnitForTimeRange]/factor;
            coded_time_range_sec = (*lengthOfTimeRange)*u2sf;
            u2sf_step_unit = u2s[stepUnits]/factor;
        } else {
            u2sf_step_unit = u2s[stepUnits];
        }
        if (coded_time_range_sec % u2sf_step_unit != 0) {
            grib_context_log(h->context,GRIB_LOG_ERROR,"unable to convert endStep in stepUnits");
            return GRIB_WRONG_STEP_UNIT;
        }
        *lengthOfTimeRange = coded_time_range_sec / u2sf_step_unit;
    }

    return GRIB_SUCCESS;
}

static int unpack_one_time_range(grib_accessor* a, long* val, size_t *len)
{
    grib_accessor_g2end_step* self = (grib_accessor_g2end_step*)a;
    int err = 0;
    long start_step;
    long unit;
    long coded_unit;
    long coded_time_range, typeOfTimeIncrement;
    int add_time_range = 1; /* whether we add lengthOfTimeRange */

    grib_handle* h = grib_handle_of_accessor(a);

    if((err = grib_get_long_internal(h,self->start_step,&start_step))) return err;
    if((err = grib_get_long_internal(h,self->unit,&unit))) return err;
    if((err = grib_get_long_internal(h,self->coded_unit,&coded_unit))) return err;
    if((err = grib_get_long_internal(h,self->coded_time_range, &coded_time_range))) return err;
    if((err = grib_get_long_internal(h,self->typeOfTimeIncrement, &typeOfTimeIncrement))) return err;

    err = convert_time_range(h, unit, coded_unit, &coded_time_range);
    if (err != GRIB_SUCCESS) return err;

    if (typeOfTimeIncrement == 1) {
        /* See GRIB-488 */
        /* Note: For this case, lengthOfTimeRange is not related to step and should not be used to calculate step */
        add_time_range = 0;
        if (is_special_expver(h)) {
            add_time_range = 1;
        }
    }
    if (add_time_range) {
        *val = start_step + coded_time_range;
    } else {
        *val = start_step;
    }

    return GRIB_SUCCESS;
}

#define MAX_NUM_TIME_RANGES 16 /* maximum number of time range specifications */
static int unpack_multiple_time_ranges(grib_accessor* a, long* val, size_t *len)
{
    grib_accessor_g2end_step* self = (grib_accessor_g2end_step*)a;
    int i = 0, err = 0;
    grib_handle* h = grib_handle_of_accessor(a);
    long numberOfTimeRange = 0, unit = 0, start_step = 0;

    size_t count = 0;
    long arr_typeOfTimeIncrement[MAX_NUM_TIME_RANGES] = {0,};
    long arr_coded_unit[MAX_NUM_TIME_RANGES]          = {0,};
    long arr_coded_time_range[MAX_NUM_TIME_RANGES]    = {0,};

    if((err = grib_get_long_internal(h,self->start_step,&start_step))) return err;
    if((err = grib_get_long_internal(h,self->unit,&unit))) return err;
    if((err = grib_get_long_internal(h,self->numberOfTimeRange, &numberOfTimeRange))) return err;
    if (numberOfTimeRange > MAX_NUM_TIME_RANGES) {
        grib_context_log(h->context, GRIB_LOG_ERROR, "Too many time range specifications!");
        return GRIB_DECODING_ERROR;
    }

    count = numberOfTimeRange;
    /* Get the arrays for the N time ranges */
    if ((err = grib_get_long_array(h, self->typeOfTimeIncrement, arr_typeOfTimeIncrement, &count))) return err;
    if ((err = grib_get_long_array(h, self->coded_unit,          arr_coded_unit,          &count))) return err;
    if ((err = grib_get_long_array(h, self->coded_time_range,    arr_coded_time_range,    &count))) return err;

    /* Look in the array of typeOfTimeIncrements for first entry whose typeOfTimeIncrement == 2 */
    for(i=0; i<count; i++) {
        if (arr_typeOfTimeIncrement[i] == 2) {
            /* Found the required time range. Get the other two keys from it */
            long the_coded_unit = arr_coded_unit[i];
            long the_coded_time_range = arr_coded_time_range[i];

            err = convert_time_range(h, unit, the_coded_unit, &the_coded_time_range);
            if (err != GRIB_SUCCESS) return err;

            *val = start_step + the_coded_time_range;
            return GRIB_SUCCESS;
        }
    }

    grib_context_log(h->context, GRIB_LOG_ERROR,
            "Cannot calculate endStep. No time range specification with typeOfTimeIncrement = 2");
    return GRIB_DECODING_ERROR;
}

static int unpack_long(grib_accessor* a, long* val, size_t *len)
{
    grib_accessor_g2end_step* self = (grib_accessor_g2end_step*)a;
    int err = 0;
    long start_step;
    long numberOfTimeRange;
    grib_handle* h=grib_handle_of_accessor(a);

    if((err = grib_get_long_internal(h,self->start_step,&start_step))) return err;

    /* point in time */
    if (self->year == NULL) {
        *val=start_step;
        return 0;
    }

    Assert(self->numberOfTimeRange);
    if((err = grib_get_long_internal(h,self->numberOfTimeRange, &numberOfTimeRange))) return err;
    Assert(numberOfTimeRange == 1 || numberOfTimeRange == 2);

    if (numberOfTimeRange == 1) {
        return unpack_one_time_range(a,val,len);
    } else {
        return unpack_multiple_time_ranges(a,val,len);
    }
}

#if 0
static int unpack_long(grib_accessor* a, long* val, size_t *len)
{
    grib_accessor_g2end_step* self = (grib_accessor_g2end_step*)a;
    int err = 0;
    long start_step;
    long unit;
    long coded_unit;
    long coded_time_range, typeOfTimeIncrement, numberOfTimeRange;
    long coded_time_range_sec=0;
    int factor;
    long u2sf,u2sf_step_unit;
    int add_time_range = 1; /* whether we add lengthOfTimeRange */

    grib_handle* h=grib_handle_of_accessor(a);

    if((err = grib_get_long_internal(h,self->start_step,&start_step))) return err;

    /*point in time */
    if (self->year == NULL) {
        *val=start_step;
        return 0;
    }

    if((err = grib_get_long_internal(h,self->unit,&unit))) return err;

    if((err = grib_get_long_internal(h,self->coded_unit,&coded_unit))) return err;
    if((err = grib_get_long_internal(h,self->coded_time_range, &coded_time_range))) return err;
    if((err = grib_get_long_internal(h,self->typeOfTimeIncrement, &typeOfTimeIncrement))) return err;
    if((err = grib_get_long_internal(h,self->numberOfTimeRange, &numberOfTimeRange))) return err;

    Assert(numberOfTimeRange == 1 || numberOfTimeRange == 2);

    err = convert_time_range(h, unit, coded_unit, &coded_time_range);
    if (err != GRIB_SUCCESS) return err;

#if 0
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
#endif
    if (typeOfTimeIncrement == 1) {
        /* See GRIB-488 */
        /* Note: For this case, lengthOfTimeRange is not related to step and should not be used to calculate step */
        add_time_range = 0;
        if (is_special_expver(h)) {
            add_time_range = 1;
        }
    }
    if (add_time_range) {
        *val = start_step + coded_time_range;
    } else {
        *val = start_step;
    }

    return GRIB_SUCCESS;
}
#endif

static int pack_long(grib_accessor* a, const long* val, size_t *len)
{
    grib_accessor_g2end_step* self = (grib_accessor_g2end_step*)a;
    grib_handle* h=grib_handle_of_accessor(a);

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

    long coded_time_range,time_range, typeOfTimeIncrement;

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

    if((err = grib_get_long_internal(h,self->start_step,&start_step))) return err;
    if((err = grib_get_long_internal(h,self->typeOfTimeIncrement, &typeOfTimeIncrement))) return err;

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

    if((err = grib_set_long_internal(h,self->year_of_end_of_interval,  year_of_end_of_interval))) return err;
    if((err = grib_set_long_internal(h,self->month_of_end_of_interval, month_of_end_of_interval))) return err;
    if((err = grib_set_long_internal(h,self->day_of_end_of_interval,   day_of_end_of_interval))) return err;
    if((err = grib_set_long_internal(h,self->hour_of_end_of_interval,  hour_of_end_of_interval))) return err;
    if((err = grib_set_long_internal(h,self->minute_of_end_of_interval, minute_of_end_of_interval))) return err;
    if((err = grib_set_long_internal(h,self->second_of_end_of_interval, second_of_end_of_interval))) return err;

    if (time_range*u2s[unit]%u2s2[coded_unit]) {
        coded_unit=unit;
        if((err = grib_set_long_internal(h,self->coded_unit, coded_unit))) return err;
        coded_time_range=time_range;
    } else
        coded_time_range=(time_range*u2s[unit])/u2s2[coded_unit];

    if (typeOfTimeIncrement != 1) {
        /* 1 means "Successive times processed have same forecast time, start time of forecast is incremented" */
        /* Note: For this case, length of timeRange is not related to step and so should NOT be used to calculate step */
        if((err = grib_set_long_internal(h,self->coded_time_range, coded_time_range))) return err;
    }

    return GRIB_SUCCESS;
}

