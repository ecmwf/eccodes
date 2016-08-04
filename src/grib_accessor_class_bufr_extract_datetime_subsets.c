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
   SUPER      = grib_accessor_class_gen
   IMPLEMENTS = init; get_native_type
   IMPLEMENTS = pack_long;
   MEMBERS    = const char* doExtractSubsets
   MEMBERS    = const char* numberOfSubsets
   MEMBERS    = const char* extractSubsetList
   END_CLASS_DEF

 */

/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "accessor.class" and rerun ./make_class.pl

*/

static int  get_native_type(grib_accessor*);
static int pack_long(grib_accessor*, const long* val,size_t *len);
static void init(grib_accessor*,const long, grib_arguments* );
static void init_class(grib_accessor_class*);

typedef struct grib_accessor_bufr_extract_datetime_subsets {
    grib_accessor          att;
/* Members defined in gen */
/* Members defined in bufr_extract_datetime_subsets */
	const char* doExtractSubsets;
	const char* numberOfSubsets;
	const char* extractSubsetList;
} grib_accessor_bufr_extract_datetime_subsets;

extern grib_accessor_class* grib_accessor_class_gen;

static grib_accessor_class _grib_accessor_class_bufr_extract_datetime_subsets = {
    &grib_accessor_class_gen,                      /* super                     */
    "bufr_extract_datetime_subsets",                      /* name                      */
    sizeof(grib_accessor_bufr_extract_datetime_subsets),  /* size                      */
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
    &get_native_type,            /* get native type               */
    0,                /* get sub_section                */
    0,               /* grib_pack procedures long      */
    0,                 /* grib_pack procedures long      */
    &pack_long,                  /* grib_pack procedures long      */
    0,                /* grib_unpack procedures long    */
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


grib_accessor_class* grib_accessor_class_bufr_extract_datetime_subsets = &_grib_accessor_class_bufr_extract_datetime_subsets;


static void init_class(grib_accessor_class* c)
{
	c->dump	=	(*(c->super))->dump;
	c->next_offset	=	(*(c->super))->next_offset;
	c->string_length	=	(*(c->super))->string_length;
	c->value_count	=	(*(c->super))->value_count;
	c->byte_count	=	(*(c->super))->byte_count;
	c->byte_offset	=	(*(c->super))->byte_offset;
	c->sub_section	=	(*(c->super))->sub_section;
	c->pack_missing	=	(*(c->super))->pack_missing;
	c->is_missing	=	(*(c->super))->is_missing;
	c->unpack_long	=	(*(c->super))->unpack_long;
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

static void init(grib_accessor* a, const long len , grib_arguments* arg )
{
    int n=0;
    grib_accessor_bufr_extract_datetime_subsets *self =(grib_accessor_bufr_extract_datetime_subsets*)a;

    a->length=0;
    self->doExtractSubsets = grib_arguments_get_name(grib_handle_of_accessor(a),arg,n++);
    self->numberOfSubsets = grib_arguments_get_name(grib_handle_of_accessor(a),arg,n++);
    self->extractSubsetList = grib_arguments_get_name(grib_handle_of_accessor(a),arg,n++);

    a->flags |= GRIB_ACCESSOR_FLAG_FUNCTION;
}

static int get_native_type(grib_accessor* a)
{
    return GRIB_TYPE_LONG;
}

static int select_datetime(grib_accessor* a) {
  int ret=0;
  long compressed=0;
  grib_accessor_bufr_extract_datetime_subsets *self =(grib_accessor_bufr_extract_datetime_subsets*)a;
  grib_handle* h=grib_handle_of_accessor(a);
  grib_context* c=h->context;

  ret=grib_get_long(h,"compressedData",&compressed);
  if (ret) return ret;

  if (compressed) {
    size_t n;
    long yearRank,monthRank,dayRank,hourRank,minuteRank,secondRank;
    long yearStart,monthStart,dayStart,hourStart,minuteStart,secondStart;
    long yearEnd,monthEnd,dayEnd,hourEnd,minuteEnd,secondEnd;
    long *year,*month,*hour,*day,*minute,*second;
    long numberOfSubsets,i;
    grib_iarray* subsets;
    long *subsets_ar=0;
    size_t nsubsets=0;
    char yearstr[20]={0,};
    char monthstr[20]={0,};
    char daystr[20]={0,};
    char hourstr[20]={0,};
    char minutestr[20]={0,};
    char secondstr[20]={0,};

    ret=grib_get_long(h,self->numberOfSubsets,&numberOfSubsets);
    if (ret) return ret;

    subsets=grib_iarray_new(c,numberOfSubsets,10);

    ret=grib_set_long(h,"unpack",1);
    if (ret) return ret;

    ret=grib_get_long(h,"extractDateTimeYearRank",&yearRank);
    if (ret) return ret;
    sprintf(yearstr,"#%ld#year",yearRank);

    ret=grib_get_long(h,"extractDateTimeMonthRank",&monthRank);
    if (ret) return ret;
    sprintf(monthstr,"#%ld#month",monthRank);

    ret=grib_get_long(h,"extractDateTimeDayRank",&dayRank);
    if (ret) return ret;
    sprintf(daystr,"#%ld#day",dayRank);

    ret=grib_get_long(h,"extractDateTimeHourRank",&hourRank);
    if (ret) return ret;
    sprintf(hourstr,"#%ld#hour",hourRank);

    ret=grib_get_long(h,"extractDateTimeMinuteRank",&minuteRank);
    if (ret) return ret;
    sprintf(minutestr,"#%ld#minute",minuteRank);

    ret=grib_get_long(h,"extractDateTimeSecondRank",&secondRank);
    if (ret) return ret;
    sprintf(secondstr,"#%ld#second",secondRank);

    n=numberOfSubsets;
    year=grib_context_malloc_clear(c,sizeof(long)*numberOfSubsets);
    ret=grib_get_long_array(h,yearstr,year,&n);
    if (ret) return ret;
    if (n!=numberOfSubsets) {
      if (n==1) {
        for (i=1;i<numberOfSubsets;i++) year[i]=year[0];
      } else return GRIB_INTERNAL_ERROR;
    }

    n=numberOfSubsets;
    month=grib_context_malloc_clear(c,sizeof(long)*numberOfSubsets);
    ret=grib_get_long_array(h,monthstr,month,&n);
    if (ret) return ret;
    if (n!=numberOfSubsets) {
      if (n==1) {
        for (i=1;i<numberOfSubsets;i++) month[i]=month[0];
      } else return GRIB_INTERNAL_ERROR;
    }

    n=numberOfSubsets;
    day=grib_context_malloc_clear(c,sizeof(long)*numberOfSubsets);
    ret=grib_get_long_array(h,daystr,day,&n);
    if (ret) return ret;
    if (n!=numberOfSubsets) {
      if (n==1) {
        for (i=1;i<numberOfSubsets;i++) day[i]=day[0];
      } else return GRIB_INTERNAL_ERROR;
    }

    n=numberOfSubsets;
    hour=grib_context_malloc_clear(c,sizeof(long)*numberOfSubsets);
    ret=grib_get_long_array(h,hourstr,hour,&n);
    if (ret) return ret;
    if (n!=numberOfSubsets) {
      if (n==1) {
        for (i=1;i<numberOfSubsets;i++) hour[i]=hour[0];
      } else return GRIB_INTERNAL_ERROR;
    }

    n=numberOfSubsets;
    minute=grib_context_malloc_clear(c,sizeof(long)*numberOfSubsets);
    ret=grib_get_long_array(h,minutestr,minute,&n);
    if (ret) {
      ret=0;
      minute[0]=0;
      n=1;
    }
    if (n!=numberOfSubsets) {
      if (n==1) {
        for (i=1;i<numberOfSubsets;i++) minute[i]=minute[0];
      } else return GRIB_INTERNAL_ERROR;
    }

    n=numberOfSubsets;
    second=grib_context_malloc_clear(c,sizeof(long)*numberOfSubsets);
    ret=grib_get_long_array(h,secondstr,second,&n);
    if (ret) {
      ret=0;
      second[0]=0;
      n=1;
    }
    if (n!=numberOfSubsets) {
      if (n==1) {
        for (i=1;i<numberOfSubsets;i++) second[i]=second[0];
      } else return GRIB_INTERNAL_ERROR;
    }

    ret=grib_get_long(h,"extractDateTimeYearStart",&yearStart);
    if (ret) return ret;
    ret=grib_get_long(h,"extractDateTimeMonthStart",&monthStart);
    if (ret) return ret;
    ret=grib_get_long(h,"extractDateTimeDayStart",&dayStart);
    if (ret) return ret;
    ret=grib_get_long(h,"extractDateTimeHourStart",&hourStart);
    if (ret) return ret;
    ret=grib_get_long(h,"extractDateTimeMinuteStart",&minuteStart);
    if (ret) minuteStart=0;
    ret=grib_get_long(h,"extractDateTimeSecondStart",&secondStart);
    if (ret) secondStart=0;

    ret=grib_get_long(h,"extractDateTimeYearEnd",&yearEnd);
    if (ret) return ret;
    ret=grib_get_long(h,"extractDateTimeMonthEnd",&monthEnd);
    if (ret) return ret;
    ret=grib_get_long(h,"extractDateTimeDayEnd",&dayEnd);
    if (ret) return ret;
    ret=grib_get_long(h,"extractDateTimeHourEnd",&hourEnd);
    if (ret) return ret;
    ret=grib_get_long(h,"extractDateTimeMinuteEnd",&minuteEnd);
    if (ret) minuteEnd=0;
    ret=grib_get_long(h,"extractDateTimeSecondEnd",&secondEnd);
    if (ret) secondEnd=0;

    for (i=0;i<numberOfSubsets;i++) {
      printf("++++++ day: %ld <= %ld <= %ld hour: %ld <= %ld <= %ld minute: %ld <= %ld <= %ld second: %ld <= %ld <= %ld \n",
              dayStart,day[i],dayEnd, hourStart,hour[i],hourEnd,
              minuteStart,minute[i],minuteEnd,secondStart,second[i],secondEnd);
      if (year[i]>=yearStart && year[i]<=yearEnd && month[i]>=monthStart && month[i]<=monthEnd
          && day[i]>=dayStart && day[i]<=dayEnd && hour[i]>=hourStart && hour[i]<=hourEnd
          && minute[i]>=minuteStart && minute[i]<=minuteEnd && second[i]>=secondStart && second[i]<=secondEnd
          ) {
        grib_iarray_push(subsets,i+1);
        printf("++++++++ %ld\n",i+1);
      }
    }

    subsets_ar=grib_iarray_get_array(subsets);
    nsubsets=grib_iarray_used_size(subsets);
    ret=grib_set_long_array(h,self->extractSubsetList,subsets_ar,nsubsets);
    if (ret) return ret;

    ret=grib_set_long(h,self->doExtractSubsets,1);
    if (ret) return ret;

    grib_context_free(c,year);
    grib_context_free(c,month);
    grib_context_free(c,day);
    grib_context_free(c,hour);
    grib_context_free(c,minute);
    grib_context_free(c,second);
    grib_iarray_delete(subsets);
    subsets=0;

  } else {
    return GRIB_NOT_IMPLEMENTED;
  }

  return ret;
}

static int pack_long(grib_accessor* a, const long* val, size_t *len)
{
    int err=0;
    grib_accessor_bufr_extract_datetime_subsets *self =(grib_accessor_bufr_extract_datetime_subsets*)a;
    size_t l=1;
    long v[1];

    if (*len==0) return GRIB_SUCCESS;
    err=select_datetime(a);
    if (err) return err;

    err=grib_set_long(a->parent->h,self->doExtractSubsets,1);
    if (err) return err;

    return err;
}
