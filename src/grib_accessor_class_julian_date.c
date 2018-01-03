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
   IMPLEMENTS = unpack_double; pack_double
   IMPLEMENTS = unpack_long; pack_long
   IMPLEMENTS = unpack_string; pack_string
   IMPLEMENTS = dump
   IMPLEMENTS = init
   IMPLEMENTS = pack_expression
   MEMBERS    = const char *year
   MEMBERS    = const char *month
   MEMBERS    = const char *day
   MEMBERS    = const char *hour
   MEMBERS    = const char *minute
   MEMBERS    = const char *second
   MEMBERS    = const char *ymd
   MEMBERS    = const char *hms
   MEMBERS    = char sep[5]
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
static int pack_string(grib_accessor*, const char*, size_t *len);
static int pack_expression(grib_accessor*, grib_expression*);
static int unpack_double(grib_accessor*, double* val,size_t *len);
static int unpack_long(grib_accessor*, long* val,size_t *len);
static int unpack_string (grib_accessor*, char*, size_t *len);
static void dump(grib_accessor*, grib_dumper*);
static void init(grib_accessor*,const long, grib_arguments* );
static void init_class(grib_accessor_class*);

typedef struct grib_accessor_julian_date {
    grib_accessor          att;
/* Members defined in gen */
/* Members defined in double */
/* Members defined in julian_date */
	const char *year;
	const char *month;
	const char *day;
	const char *hour;
	const char *minute;
	const char *second;
	const char *ymd;
	const char *hms;
	char sep[5];
} grib_accessor_julian_date;

extern grib_accessor_class* grib_accessor_class_double;

static grib_accessor_class _grib_accessor_class_julian_date = {
    &grib_accessor_class_double,                      /* super                     */
    "julian_date",                      /* name                      */
    sizeof(grib_accessor_julian_date),  /* size                      */
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
    &pack_string,                /* grib_pack procedures string    */
    &unpack_string,              /* grib_unpack procedures string  */
    0,          /* grib_pack array procedures string    */
    0,        /* grib_unpack array procedures string  */
    0,                 /* grib_pack procedures bytes     */
    0,               /* grib_unpack procedures bytes   */
    &pack_expression,            /* pack_expression */
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


grib_accessor_class* grib_accessor_class_julian_date = &_grib_accessor_class_julian_date;


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
	c->pack_string_array	=	(*(c->super))->pack_string_array;
	c->unpack_string_array	=	(*(c->super))->unpack_string_array;
	c->pack_bytes	=	(*(c->super))->pack_bytes;
	c->unpack_bytes	=	(*(c->super))->unpack_bytes;
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
    grib_accessor_julian_date* self = (grib_accessor_julian_date*)a;
    int n=0;
    grib_handle* h=grib_handle_of_accessor(a);

    self->year = grib_arguments_get_name(h,c,n++);
    self->month = grib_arguments_get_name(h,c,n++);

    self->day = grib_arguments_get_name(h,c,n++);
    if (self->day ==NULL) {
        self->hour = 0;
        self->minute = 0;
        self->second = 0;
        self->ymd=self->year;
        self->hms=self->month;
        self->year=0;
        self->month=0;
    } else {
        self->ymd=0;
        self->hms=0;
        self->hour = grib_arguments_get_name(h,c,n++);
        self->minute = grib_arguments_get_name(h,c,n++);
        self->second = grib_arguments_get_name(h,c,n++);
    }
    self->sep[0]=' ';
    self->sep[1]=0;
    self->sep[2]=0;
    self->sep[3]=0;
    self->sep[4]=0;

    a->length=0;
}

static void dump(grib_accessor* a, grib_dumper* dumper)
{
    grib_dump_string(dumper,a,NULL);
}

static int unpack_double(grib_accessor* a, double* val, size_t *len)
{
    int ret=0;
    long hour,minute,second;
    long year,month,day,ymd,hms;
    grib_handle* h=grib_handle_of_accessor(a);
    grib_accessor_julian_date* self = (grib_accessor_julian_date*)a;

    if (self->ymd==NULL) {
        ret=grib_get_long(h,self->year,&year);
        if (ret!=GRIB_SUCCESS) return ret;
        ret=grib_get_long(h,self->month,&month);
        if (ret!=GRIB_SUCCESS) return ret;
        ret=grib_get_long(h,self->day,&day);
        if (ret!=GRIB_SUCCESS) return ret;

        ret=grib_get_long(h,self->hour,&hour);
        if (ret!=GRIB_SUCCESS) return ret;
        ret=grib_get_long(h,self->minute,&minute);
        if (ret!=GRIB_SUCCESS) return ret;
        ret=grib_get_long(h,self->second,&second);
        if (ret!=GRIB_SUCCESS) return ret;
    } else {
        ret=grib_get_long(h,self->ymd,&ymd);
        if (ret!=GRIB_SUCCESS) return ret;
        year = ymd / 10000;
        ymd %= 10000;
        month  = ymd / 100;
        ymd %= 100;
        day = ymd;

        ret=grib_get_long(h,self->hms,&hms);
        if (ret!=GRIB_SUCCESS) return ret;
        hour = hms / 10000;
        hms %= 10000;
        minute  = hms / 100;
        hms %= 100;
        second = hms;
    }

    ret=grib_datetime_to_julian(year,month,day,hour,minute,second,val);

    return ret;
}

static int pack_double(grib_accessor* a, const double* val, size_t *len)
{
    grib_accessor_julian_date* self = (grib_accessor_julian_date*)a;
    int ret=0;
    long hour=0;
    long minute=0;
    long second=0;
    long ymd=0,hms=0;
    long year,month,day;
    grib_handle* h=grib_handle_of_accessor(a);

    ret=grib_julian_to_datetime(*val,&year,&month,&day,&hour,&minute,&second);
    if (ret!=0) return ret;

    if (self->ymd==NULL) {
        ret=grib_set_long(h,self->year,year);
        if (ret!=0) return ret;
        ret=grib_set_long(h,self->month,month);
        if (ret!=0) return ret;
        ret=grib_set_long(h,self->day,day);
        if (ret!=0) return ret;
        ret=grib_set_long(h,self->hour,hour);
        if (ret!=0) return ret;
        ret=grib_set_long(h,self->minute,minute);
        if (ret!=0) return ret;
        ret=grib_set_long(h,self->second,second);
        if (ret!=0) return ret;
    } else {
        ymd=year * 10000 + month * 100 + day;
        ret=grib_set_long(h,self->ymd,ymd);
        if (ret!=0) return ret;

        hms=hour * 10000 + minute * 100 + second;
        ret=grib_set_long(h,self->hms,hms);
        if (ret!=0) return ret;
    }

    return ret;
}

static int unpack_string(grib_accessor* a, char* val, size_t *len)
{
    int ret=0;
    long hour,minute,second;
    long year,month,day,ymd,hms;
    grib_accessor_julian_date* self = (grib_accessor_julian_date*)a;
    char* sep=self->sep;
    grib_handle* h=grib_handle_of_accessor(a);

    if (*len <  15) return GRIB_ARRAY_TOO_SMALL;

    if (self->ymd==NULL) {
        ret=grib_get_long(h,self->year,&year);
        if (ret!=GRIB_SUCCESS) return ret;
        ret=grib_get_long(h,self->month,&month);
        if (ret!=GRIB_SUCCESS) return ret;
        ret=grib_get_long(h,self->day,&day);
        if (ret!=GRIB_SUCCESS) return ret;

        ret=grib_get_long(h,self->hour,&hour);
        if (ret!=GRIB_SUCCESS) return ret;
        ret=grib_get_long(h,self->minute,&minute);
        if (ret!=GRIB_SUCCESS) return ret;
        ret=grib_get_long(h,self->second,&second);
        if (ret!=GRIB_SUCCESS) return ret;
    } else {
        ret=grib_get_long(h,self->ymd,&ymd);
        if (ret!=GRIB_SUCCESS) return ret;
        year = ymd / 10000;
        ymd %= 10000;
        month  = ymd / 100;
        ymd %= 100;
        day = ymd;

        ret=grib_get_long(h,self->hms,&hms);
        if (ret!=GRIB_SUCCESS) return ret;
        hour = hms / 10000;
        hms %= 10000;
        minute  = hms / 100;
        hms %= 100;
        second = hms;
    }

    if (sep[1]!=0 && sep[2]!=0 && sep[3]!=0 && sep[4]!=0) {
        sprintf(val,"%04ld%c%02ld%c%02ld%c%02ld%c%02ld%c%02ld",year,sep[0],month,sep[1],day,sep[2],hour,sep[3],minute,sep[4],second);
    } else if (sep[0]!=0) {
        sprintf(val,"%04ld%02ld%02ld%c%02ld%02ld%02ld",year,month,day,sep[0],hour,minute,second);
    } else {
        sprintf(val,"%04ld%02ld%02ld%02ld%02ld%02ld",year,month,day,hour,minute,second);
    }
    return ret;
}

static int pack_string(grib_accessor* a, const char* val, size_t *len)
{
    int ret=0;
    long hour,minute,second;
    long year,month,day,ymd,hms;
    grib_accessor_julian_date* self = (grib_accessor_julian_date*)a;
    char* sep=self->sep;
    grib_handle* h=grib_handle_of_accessor(a);

    ret=sscanf(val,"%04ld%c%02ld%c%02ld%c%02ld%c%02ld%c%02ld",&year,&sep[0],&month,&sep[1],&day,&sep[2],&hour,&sep[3],&minute,&sep[4],&second);
    if (ret!=11) {
        if (strlen(val)==15) {
            ret=sscanf(val,"%04ld%02ld%02ld%c%02ld%02ld%02ld",&year,&month,&day,&sep[0],&hour,&minute,&second);
            if (ret!=7) {
                grib_context_log(h->context,GRIB_LOG_ERROR," Wrong date time format. Please use \"YYYY-MM-DD hh:mm:ss\"");
                return GRIB_INVALID_KEY_VALUE;
            }
            sep[1]=0;
            sep[2]=0;
            sep[3]=0;
            sep[4]=0;
        } else {
            ret=sscanf(val,"%04ld%02ld%02ld%02ld%02ld%02ld",&year,&month,&day,&hour,&minute,&second);
            if (ret!=6) {
                grib_context_log(h->context,GRIB_LOG_ERROR," Wrong date time format. Please use \"YYYY-MM-DD hh:mm:ss\"");
                return GRIB_INVALID_KEY_VALUE;
            }
            sep[0]=0;
            sep[1]=0;
            sep[2]=0;
            sep[3]=0;
            sep[4]=0;
        }
    }

    if (self->ymd==NULL) {
        ret=grib_set_long(h,self->year,year);
        if (ret!=0) return ret;
        ret=grib_set_long(h,self->month,month);
        if (ret!=0) return ret;
        ret=grib_set_long(h,self->day,day);
        if (ret!=0) return ret;
        ret=grib_set_long(h,self->hour,hour);
        if (ret!=0) return ret;
        ret=grib_set_long(h,self->minute,minute);
        if (ret!=0) return ret;
        ret=grib_set_long(h,self->second,second);
        if (ret!=0) return ret;
    } else {
        ymd=year * 10000 + month * 100 + day;
        ret=grib_set_long(h,self->ymd,ymd);
        if (ret!=0) return ret;

        hms=hour * 10000 + minute * 100 + second;
        ret=grib_set_long(h,self->hms,hms);
        if (ret!=0) return ret;
    }

    return ret;
}

static int unpack_long(grib_accessor* a, long* val, size_t *len)
{
    grib_context_log(a->context,GRIB_LOG_ERROR, " Cannot unpack %s as long", a->name);
    return GRIB_NOT_IMPLEMENTED;
}
static int pack_long(grib_accessor* a, const long*  v, size_t *len)
{
    grib_context_log(a->context,GRIB_LOG_ERROR, " Cannot pack %s as long", a->name);
    return GRIB_NOT_IMPLEMENTED;
}

static int pack_expression(grib_accessor* a, grib_expression *e)
{
    size_t len = 1;
    long   lval=0;
    double dval=0;
    const char *cval=NULL;
    int ret=0;
    grib_handle* hand = grib_handle_of_accessor(a);

    switch(grib_expression_native_type(hand,e)) {
        case GRIB_TYPE_LONG: {
            len = 1;
            ret = grib_expression_evaluate_long(hand,e,&lval);
            if (ret != GRIB_SUCCESS) {
                grib_context_log(a->context,GRIB_LOG_ERROR,"unable to set %s as long",a->name);
                return ret;
            }
            /*if (hand->context->debug)
                    printf("ECCODES DEBUG grib_accessor_class_gen::pack_expression %s %ld\n", a->name,lval);*/
            return grib_pack_long(a,&lval,&len);
        }

        case GRIB_TYPE_DOUBLE: {
            len = 1;
            ret = grib_expression_evaluate_double(hand,e,&dval);
            /*if (hand->context->debug)
                    printf("ECCODES DEBUG grib_accessor_class_gen::pack_expression %s %g\n", a->name, dval);*/
            return grib_pack_double(a,&dval,&len);
        }

        case GRIB_TYPE_STRING: {
            char tmp[1024];
            len = sizeof(tmp);
            cval = grib_expression_evaluate_string(hand,e,tmp,&len,&ret);
            if (ret != GRIB_SUCCESS) {
                grib_context_log(a->context,GRIB_LOG_ERROR,"unable to set %s as string",a->name);
                return ret;
            }
            len = strlen(cval);
            /*if (hand->context->debug)
                    printf("ECCODES DEBUG grib_accessor_class_gen::pack_expression %s %s\n", a->name, cval);*/
            return grib_pack_string(a,cval,&len);
        }
    }

    return GRIB_NOT_IMPLEMENTED;
}
