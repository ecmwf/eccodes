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
   SUPER      = grib_accessor_class_ascii
   IMPLEMENTS = pack_string;unpack_string;value_count;string_length
   IMPLEMENTS = pack_long; unpack_long
   IMPLEMENTS = get_native_type
   IMPLEMENTS = init
   MEMBERS    = const char* stepRange
   MEMBERS    = const char* stepType
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
static int pack_string(grib_accessor*, const char*, size_t *len);
static int unpack_long(grib_accessor*, long* val,size_t *len);
static int unpack_string (grib_accessor*, char*, size_t *len);
static size_t string_length(grib_accessor*);
static int value_count(grib_accessor*,long*);
static void init(grib_accessor*,const long, grib_arguments* );
static void init_class(grib_accessor_class*);

typedef struct grib_accessor_mars_step {
    grib_accessor          att;
/* Members defined in gen */
/* Members defined in ascii */
/* Members defined in mars_step */
	const char* stepRange;
	const char* stepType;
} grib_accessor_mars_step;

extern grib_accessor_class* grib_accessor_class_ascii;

static grib_accessor_class _grib_accessor_class_mars_step = {
    &grib_accessor_class_ascii,                      /* super                     */
    "mars_step",                      /* name                      */
    sizeof(grib_accessor_mars_step),  /* size                      */
    0,                           /* inited */
    &init_class,                 /* init_class */
    &init,                       /* init                      */
    0,                  /* post_init                      */
    0,                    /* free mem                       */
    0,                       /* describes himself         */
    0,                /* get length of section     */
    &string_length,              /* get length of string      */
    &value_count,                /* get number of values      */
    0,                 /* get number of bytes      */
    0,                /* get offset to bytes           */
    &get_native_type,            /* get native type               */
    0,                /* get sub_section                */
    0,               /* grib_pack procedures long      */
    0,                 /* grib_pack procedures long      */
    &pack_long,                  /* grib_pack procedures long      */
    &unpack_long,                /* grib_unpack procedures long    */
    0,                /* grib_pack procedures double    */
    0,              /* grib_unpack procedures double  */
    &pack_string,                /* grib_pack procedures string    */
    &unpack_string,              /* grib_unpack procedures string  */
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


grib_accessor_class* grib_accessor_class_mars_step = &_grib_accessor_class_mars_step;


static void init_class(grib_accessor_class* c)
{
	c->dump	=	(*(c->super))->dump;
	c->next_offset	=	(*(c->super))->next_offset;
	c->byte_count	=	(*(c->super))->byte_count;
	c->byte_offset	=	(*(c->super))->byte_offset;
	c->sub_section	=	(*(c->super))->sub_section;
	c->pack_missing	=	(*(c->super))->pack_missing;
	c->is_missing	=	(*(c->super))->is_missing;
	c->pack_double	=	(*(c->super))->pack_double;
	c->unpack_double	=	(*(c->super))->unpack_double;
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
    int n=0;
    grib_accessor_mars_step* self = (grib_accessor_mars_step*)a;
    self->stepRange = grib_arguments_get_name(grib_handle_of_accessor(a),c,n++);
    self->stepType = grib_arguments_get_name(grib_handle_of_accessor(a),c,n++);
}

static int pack_string(grib_accessor* a, const char* val, size_t *len)
{
    char stepType[100];
    size_t stepTypeLen=100;
    char buf[100]={0,};
    int ret;
    grib_accessor_mars_step* self = (grib_accessor_mars_step*)a;
    grib_accessor* stepRangeAcc=grib_find_accessor(grib_handle_of_accessor(a),self->stepRange);

    if (!stepRangeAcc) {
        grib_context_log(a->context,GRIB_LOG_ERROR,"%s not found",self->stepRange);
        return GRIB_NOT_FOUND;
    }

    if ((ret=grib_get_string(grib_handle_of_accessor(a),self->stepType,stepType,&stepTypeLen))!=GRIB_SUCCESS)
        return ret;

    if (!strcmp(stepType,"instant")) sprintf(buf,"%s",val) ;
    else sprintf(buf,"0-%s",val);

    return grib_pack_string(stepRangeAcc,buf,len);
}

static int unpack_string(grib_accessor* a, char* val, size_t *len)
{
    int ret=0;
    grib_accessor_mars_step* self = (grib_accessor_mars_step*)a;
    char buf[100]={0,};
    char* p=NULL;
    size_t buflen=100;
    long step;
    grib_accessor* stepRangeAcc=grib_find_accessor(grib_handle_of_accessor(a),self->stepRange);

    if (!stepRangeAcc) {
        grib_context_log(a->context,GRIB_LOG_ERROR,"%s not found",self->stepRange);
        return GRIB_NOT_FOUND;
    }

    if ((ret=grib_unpack_string(stepRangeAcc,buf,&buflen))!=GRIB_SUCCESS)
        return ret;

    if (*len < buflen) {
        grib_context_log(a->context, GRIB_LOG_ERROR,
                "grib_accessor_class_mars_step: Buffer too small for %s. It is %ld bytes long (len=%ld)\n",
                a->name, buflen, *len);
        *len = buflen;
        return GRIB_BUFFER_TOO_SMALL;
    }

    strcpy(val,buf);
    step=strtol(buf, &p,10);

    if ( p!=NULL && *p=='-' && step==0 ) strcpy(val,++p);

    *len=strlen(val);

    return ret;
}

static int pack_long(grib_accessor* a, const long* val, size_t *len)
{
    char buff[100]={0,};
    size_t bufflen=100;

    sprintf(buff,"%ld",*val);

    return pack_string(a,buff,&bufflen);
}

static int unpack_long(grib_accessor* a, long* val, size_t *len)
{
    grib_accessor_mars_step* self = (grib_accessor_mars_step*)a;
    grib_accessor* stepRangeAcc=grib_find_accessor(grib_handle_of_accessor(a),self->stepRange);

    if (!stepRangeAcc) return GRIB_NOT_FOUND;

    return grib_unpack_long(stepRangeAcc,val,len);
}

static int value_count(grib_accessor* a,long* count)
{
    *count=1;
    return 0;
}

static size_t string_length(grib_accessor* a)
{
    return 16;
}

static int  get_native_type(grib_accessor* a)
{
    return GRIB_TYPE_LONG;
}
