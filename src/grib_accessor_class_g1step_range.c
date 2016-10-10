/*
 * Copyright 2005-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

/*********************************************
 *   Enrico Fucile
 *******************************************/

#include "grib_api_internal.h"
/*
   This is used by make_class.pl

   START_CLASS_DEF
   CLASS      = accessor
   SUPER      = grib_accessor_class_abstract_long_vector
   IMPLEMENTS = pack_string;unpack_string;value_count
   IMPLEMENTS = pack_long;unpack_long;dump
   IMPLEMENTS = get_native_type;string_length
   IMPLEMENTS = init; destroy
   MEMBERS    = const char* p1
   MEMBERS    = const char* p2
   MEMBERS    = const char* timeRangeIndicator
   MEMBERS    = const char *unit
   MEMBERS    = const char *step_unit
   MEMBERS    = const char *stepType
   MEMBERS    = const char *patch_fp_precip
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
static void destroy(grib_context*,grib_accessor*);
static void dump(grib_accessor*, grib_dumper*);
static void init(grib_accessor*,const long, grib_arguments* );
static void init_class(grib_accessor_class*);

typedef struct grib_accessor_g1step_range {
    grib_accessor          att;
/* Members defined in gen */
/* Members defined in abstract_long_vector */
	long* v;
	long pack_index;
	int number_of_elements;
/* Members defined in g1step_range */
	const char* p1;
	const char* p2;
	const char* timeRangeIndicator;
	const char *unit;
	const char *step_unit;
	const char *stepType;
	const char *patch_fp_precip;
} grib_accessor_g1step_range;

extern grib_accessor_class* grib_accessor_class_abstract_long_vector;

static grib_accessor_class _grib_accessor_class_g1step_range = {
    &grib_accessor_class_abstract_long_vector,                      /* super                     */
    "g1step_range",                      /* name                      */
    sizeof(grib_accessor_g1step_range),  /* size                      */
    0,                           /* inited */
    &init_class,                 /* init_class */
    &init,                       /* init                      */
    0,                  /* post_init                      */
    &destroy,                    /* free mem                       */
    &dump,                       /* describes himself         */
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


grib_accessor_class* grib_accessor_class_g1step_range = &_grib_accessor_class_g1step_range;


static void init_class(grib_accessor_class* c)
{
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
    grib_accessor_g1step_range* self = (grib_accessor_g1step_range*)a;
    grib_handle* h=grib_handle_of_accessor(a);
    int n = 0;
    self->p1                  = grib_arguments_get_name(h,c,n++);
    self->p2                  = grib_arguments_get_name(h,c,n++);
    self->timeRangeIndicator  = grib_arguments_get_name(h,c,n++);
    self->unit                = grib_arguments_get_name(h,c,n++);
    self->step_unit           = grib_arguments_get_name(h,c,n++);
    self->stepType            = grib_arguments_get_name(h,c,n++);
    self->patch_fp_precip     = grib_arguments_get_name(h,c,n++);

    self->number_of_elements=2;
    self->v=(long*)grib_context_malloc_clear(h->context,
            sizeof(long)*self->number_of_elements);
    self->pack_index=-1;
    a->dirty=1;

    a->length=0;
}

static void dump(grib_accessor* a, grib_dumper* dumper)
{
    grib_dump_string(dumper,a,NULL);

}

static int error_on_units=1;

static int u2s1[] =  {
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
        900,     /* (13) 15 minutes  */
        1800,    /* (14) 30 minutes */
        1        /* (15) seconds  */   /* See ECC-316 */
};

static int units_index[] = {
        1,0,10,11,12,2,0,13,14,15
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


int grib_g1_step_get_steps(grib_accessor* a,long* start,long* theEnd)
{
    grib_accessor_g1step_range* self = (grib_accessor_g1step_range*)a;
    int err = 0;
    long p1 = 0,p2 = 0,unit = 0,timeRangeIndicator=0,timeRangeIndicatorFromStepRange=0;
    long step_unit=1;
    char stepType[20]={0,};
    size_t stepTypeLen=20;
    long newstart,newend;
    int factor=1;
    long u2sf,u2sf_step_unit;

    if (self->step_unit != NULL)
        grib_get_long_internal(grib_handle_of_accessor(a),self->step_unit,&step_unit);

    if (err!=GRIB_SUCCESS) return err;

    err = grib_get_long_internal(grib_handle_of_accessor(a),self->unit,&unit);
    if(err)           return err;
    if (unit == 254) {
        unit = 15; /* See ECC-316: WMO says 254 is for 'seconds' but we use 15! */
    }

    err = grib_get_long_internal(grib_handle_of_accessor(a),self->p1,&p1);
    if(err)               return err;

    err = grib_get_long_internal(grib_handle_of_accessor(a),self->p2,&p2);
    if(err)               return err;

    err = grib_get_long_internal(grib_handle_of_accessor(a),self->timeRangeIndicator,&timeRangeIndicator);
    if(err)  return err;

    /* TODO move to the def file */
    err = grib_get_long(grib_handle_of_accessor(a),"timeRangeIndicatorFromStepRange",&timeRangeIndicatorFromStepRange);

    if (timeRangeIndicatorFromStepRange==10) timeRangeIndicator=timeRangeIndicatorFromStepRange;

    if (self->stepType) {
        err = grib_get_string_internal(grib_handle_of_accessor(a),self->stepType,stepType,&stepTypeLen);
        if(err)  return err;
    } else sprintf(stepType,"unknown");

    *start = p1;
    *theEnd = p2;

    if (timeRangeIndicator==10) *start = *theEnd = (p1<<8) | (p2<<0);
    else if (!strcmp(stepType,"instant")) *start = *theEnd = p1;
    else if (!strcmp(stepType,"accum") && timeRangeIndicator==0 ) {*start =0; *theEnd = p1;}

    if (u2s1[unit] == u2s[step_unit] || (*start==0 && *theEnd==0) ) return 0;

    newstart = (*start) * u2s1[unit];
    newend   = (*theEnd) * u2s1[unit];

    if (newstart<0 || newend<0) {
        factor=60;
        u2sf=u2s1[unit]/factor;
        if (u2s1[unit] % factor) return GRIB_DECODING_ERROR;
        newstart = (*start) * u2sf;
        newend   = (*theEnd)   * u2sf;
        u2sf_step_unit=u2s[step_unit]/factor;
        if (u2s[step_unit] % factor) return GRIB_DECODING_ERROR;
    } else {
        u2sf_step_unit=u2s[step_unit];
    }

    if (newstart % u2sf_step_unit != 0 || newend % u2sf_step_unit != 0) {
        return GRIB_DECODING_ERROR;
    } else {
        *start = newstart/u2sf_step_unit;
        *theEnd = newend/u2sf_step_unit;
    }

    return 0;
}

static int unpack_string(grib_accessor* a, char* val, size_t *len)
{
    grib_accessor_g1step_range* self = (grib_accessor_g1step_range*)a;
    char buf[100];
    size_t size=0;
    long start=0,theEnd=0;
    long timeRangeIndicator=0;
    long unit;
    int err=0;
    char stepType[20]={0,};
    size_t stepTypeLen=20;

    if ((err=grib_g1_step_get_steps(a,&start,&theEnd))!=GRIB_SUCCESS) {
        size_t step_unit_string_len=10;
        char step_unit_string[10];

        if (self->step_unit != NULL)
            grib_get_string(grib_handle_of_accessor(a),self->step_unit,step_unit_string,&step_unit_string_len);
        else
            sprintf(step_unit_string,"h");

        if (error_on_units) {
            grib_get_long_internal(grib_handle_of_accessor(a),self->unit,&unit);
            if (unit==254) {
                unit=15; /* See ECC-316 */
            }
            grib_set_long_internal(grib_handle_of_accessor(a),self->step_unit,unit);
            grib_context_log(a->context,GRIB_LOG_ERROR,
                    "unable to represent the step in %s\n                    Hint: try changing the step units",
                    step_unit_string);
        }
        return err;
    }

    err = grib_get_long_internal(grib_handle_of_accessor(a),self->timeRangeIndicator,&timeRangeIndicator);
    if(err)  return err;

    if (self->stepType) {
        err = grib_get_string_internal(grib_handle_of_accessor(a),self->stepType,stepType,&stepTypeLen);
        if(err)  return err;
    } else sprintf(stepType,"unknown");

    /* Patch for old forecast probabilities */
    if (self->patch_fp_precip)
    {
        start += 24;
    }

    if (strcmp(stepType,"instant") == 0) {
        sprintf(buf,"%ld",start);
    }
    else if ( (strcmp(stepType,"avgfc") == 0)  ||
              (strcmp(stepType,"avgua") == 0)  ||
              (strcmp(stepType,"avgia") == 0)  ||
              (strcmp(stepType,"varins") == 0) )
    {
        sprintf(buf,"%ld",start);
    }
    else if (
            (strcmp(stepType,"accum") == 0) ||
            (strcmp(stepType,"avg") == 0)   ||
            (strcmp(stepType,"min") == 0)   ||
            (strcmp(stepType,"max") == 0)   ||
            (strcmp(stepType,"rms") == 0)   ||
            (strcmp(stepType,"diff") == 0)  ||
            (strcmp(stepType,"avgas") == 0) ||
            (strcmp(stepType,"avgad") == 0) ||
            (strcmp(stepType,"avgid") == 0) ||
            (strcmp(stepType,"varas") == 0) ||
            (strcmp(stepType,"varad") == 0) )
    {
        if (start == theEnd) {
            sprintf(buf,"%ld",theEnd);
        }
        else
        {
            sprintf(buf,"%ld-%ld",start, theEnd);
        }
    }
    else {
        grib_context_log(a->context,GRIB_LOG_ERROR, "Unknown stepType=[%s] timeRangeIndicator=[%ld]",stepType,timeRangeIndicator);
        return GRIB_NOT_IMPLEMENTED;
    }

    size=strlen(buf)+1;

    if (*len<size) return GRIB_ARRAY_TOO_SMALL;

    *len=size;

    memcpy(val,buf,size);

    return GRIB_SUCCESS;
}

int grib_g1_step_apply_units(long *start,long *theEnd,long* step_unit,
        long *P1,long *P2,long* unit,
        const int max,const int instant)
{
    int j=0;
    long start_sec,end_sec;
    int index=0;
    int max_index=sizeof(units_index)/sizeof(*units_index);

    while (*unit != units_index[index] && index!=max_index)
        index++;

    start_sec=*start*u2s[*step_unit];
    *P2=0;

    if (instant) {
        *unit=units_index[0];
        for (j=index;j<max_index;j++) {
            if ( start_sec % u2s1[*unit]==0 &&
                    (*P1=start_sec/u2s1[*unit])<=max)
                return 0;
            *unit=units_index[j];
        }
        for (j=0;j<index;j++) {
            if ( start_sec%u2s1[*unit]==0 &&
                    (*P1=start_sec/u2s1[*unit])<=max)
                return 0;
            *unit=units_index[j];
        }

    } else {
        end_sec=*theEnd*u2s[*step_unit];
        *unit=units_index[0];
        for (j=index;j<max_index;j++) {
            if ( start_sec%u2s1[*unit]==0 &&
                    end_sec%u2s1[*unit]==0 &&
                    (*P1=start_sec/u2s1[*unit])<=max &&
                    (*P2=end_sec/u2s1[*unit])<=max )
                return 0;
            *unit=units_index[j];
        }
        for (j=0;j<index;j++) {
            if ( start_sec%u2s1[*unit]==0 &&
                    end_sec%u2s1[*unit]==0 &&
                    (*P1=start_sec/u2s1[*unit])<=max &&
                    (*P2=end_sec/u2s1[*unit])<=max )
                return 0;
            *unit=units_index[j];
        }
    }

    return GRIB_WRONG_STEP;
}

static int pack_string(grib_accessor* a, const char* val, size_t *len)
{
    grib_accessor_g1step_range* self = (grib_accessor_g1step_range*)a;
    grib_handle* h=grib_handle_of_accessor(a);
    long timeRangeIndicator=0,P1=0,P2=0;
    long start=0,theEnd=-1,unit=0,ounit=0,step_unit=1;
    int ret=0;
    long end_sec,start_sec;
    char *p=NULL,*q=NULL;
    int instant=0;
    char stepType[20]={0,};
    size_t stepTypeLen=20;

    if (self->stepType) {
        ret = grib_get_string_internal(grib_handle_of_accessor(a),self->stepType,stepType,&stepTypeLen);
        if(ret)  return ret;
    } else sprintf(stepType,"unknown");

    if((ret = grib_set_long_internal(h,"timeRangeIndicatorFromStepRange",-1)))
        return ret;

    /* don't change timeRangeIndicator when setting step EXCEPT IF instant*/
    if((ret = grib_get_long_internal(h,self->timeRangeIndicator,&timeRangeIndicator)))
        return ret;

    instant= ( strcmp(stepType,"instant") == 0 ) ? 1 : 0;

    if((ret = grib_get_long_internal(h,self->unit,&unit)))
        return ret;
    if (unit == 254) {
        unit=15; /* See ECC-316 */
    }

    if(self->step_unit!=NULL && (ret = grib_get_long_internal(h,self->step_unit,&step_unit)))
        return ret;

    ounit=unit;

    start=strtol(val, &p,10);
    theEnd=start;
    if ( *p!=0 ) theEnd=strtol(++p, &q,10);

    if (start==0 && theEnd==0) {
        if((ret = grib_set_long_internal(h,self->p1,start)) != GRIB_SUCCESS)
            return ret;
        ret = grib_set_long_internal(h,self->p2,theEnd);
        return ret;
    }
    end_sec=theEnd*u2s[step_unit];
    start_sec=start*u2s[step_unit];

    if ( ( end_sec > 918000 || start_sec > 918000 ) &&
            h->context->gribex_mode_on && instant ) {
        timeRangeIndicator = 10;
        if((ret = grib_set_long_internal(h,self->timeRangeIndicator,10)))
            return ret;
        /* TODO move to the def file*/
        if((ret = grib_set_long_internal(h,"timeRangeIndicatorFromStepRange",10)))
            return ret;
    }

    if (timeRangeIndicator == 10) {
        long off=0;
        grib_accessor* p1_accessor=NULL;
        if ( theEnd != start && !h->context->gribex_mode_on) {
            if ( h->context->gribex_mode_on==0 ) {
                grib_context_log(h->context,GRIB_LOG_ERROR,
                        "Unable to set %s: end must be equal to start when timeRangeIndicator=10",
                        a->name);
                return GRIB_WRONG_STEP;
            } else start = theEnd;
        }
        if ((ret=grib_g1_step_apply_units(&start,&theEnd,&step_unit,&P1,&P2,&unit,65535,instant))
                !=GRIB_SUCCESS) {
            grib_context_log(h->context,GRIB_LOG_ERROR,"unable to find units to set %s=%s",a->name,val);
            return ret;
        }

        p1_accessor=grib_find_accessor( grib_handle_of_accessor(a),self->p1);
        if (p1_accessor==NULL) {
            grib_context_log(h->context,GRIB_LOG_ERROR,"unable to find accessor %s",self->p1);
            return GRIB_NOT_FOUND;
        }
        off = p1_accessor->offset*8;
        ret = grib_encode_unsigned_long(grib_handle_of_accessor(a)->buffer->data, P1,&off,16);
        if (ret!=0) return ret;

        if (ounit != unit)
            ret = grib_set_long_internal(h,self->unit,unit);

        return ret;
    }

    if ( (ret=grib_g1_step_apply_units(&start,&theEnd,&step_unit,&P1,&P2,&unit,255,instant))
            !=GRIB_SUCCESS ) {

        if (instant || h->context->gribex_mode_on) {
            long off=0;
            grib_accessor* p1_accessor=NULL;
            if((ret = grib_set_long_internal(h,self->timeRangeIndicator,10)))
                return ret;
            /* TODO move to the def file*/
            if((ret = grib_set_long_internal(h,"timeRangeIndicatorFromStepRange",10)))
                return ret;
            if (theEnd != start && !h->context->gribex_mode_on) {
                grib_context_log(h->context,GRIB_LOG_ERROR,
                        "Unable to set %s: end must be equal to start when timeRangeIndicator=10",
                        a->name);
                return GRIB_WRONG_STEP;
            } else start=theEnd;

            if ((ret=grib_g1_step_apply_units(&start,&theEnd,&step_unit,&P1,&P2,&unit,65535,instant))
                    !=GRIB_SUCCESS) {
                grib_context_log(h->context,GRIB_LOG_ERROR,"unable to find units to set %s=%s",a->name,val);
                return ret;
            }

            p1_accessor=grib_find_accessor( grib_handle_of_accessor(a),self->p1);
            if (p1_accessor==NULL) {
                grib_context_log(h->context,GRIB_LOG_ERROR,"unable to find accessor %s",self->p1);
                return GRIB_NOT_FOUND;
            }
            off = p1_accessor->offset*8;
            ret = grib_encode_unsigned_long(grib_handle_of_accessor(a)->buffer->data, P1,&off,16);
            if (ret!=0) return ret;

            if (ounit != unit)
                ret = grib_set_long_internal(h,self->unit,unit);
        }

        return ret;
    }

    if (ounit != unit)
        if((ret = grib_set_long_internal(h,self->unit,unit)) != GRIB_SUCCESS)
            return ret;

    if((ret = grib_set_long_internal(h,self->p1,P1)) != GRIB_SUCCESS)
        return ret;
    if((ret = grib_set_long_internal(h,self->p2,P2)) != GRIB_SUCCESS)
        return ret;

    self->v[0]=start;
    self->v[1]=theEnd;
    a->dirty=0;

    return 0;
}

static int value_count(grib_accessor* a,long* count)
{
    *count=1;
    return 0;
}

static size_t string_length(grib_accessor* a)
{
    return 255;
}

static int pack_long(grib_accessor* a, const long* val, size_t *len)
{
    char buff[100];
    size_t bufflen=100;
    char sval[100];
    char* p=sval;
    size_t svallen=100;
    grib_accessor_g1step_range* self = (grib_accessor_g1step_range*)a;
    char stepType[20]={0,};
    size_t stepTypeLen=20;
    long step_unit=0;
    int err=0;

    if (self->stepType) {
        err = grib_get_string_internal(grib_handle_of_accessor(a),self->stepType,stepType,&stepTypeLen);
        if(err)  return err;
    } else sprintf(stepType,"unknown");

    if(self->step_unit!=NULL && (err = grib_get_long_internal(grib_handle_of_accessor(a),self->step_unit,&step_unit)))
        return err;

    switch (self->pack_index) {
    case  -1 :
        self->pack_index=-1;
        sprintf(buff,"%ld",*val);
        return pack_string( a,buff,&bufflen);
    case  0 :
        self->pack_index=-1;
        error_on_units=0;
        unpack_string(a,sval,&svallen);
        error_on_units=1;
        while (*p != '-' && *p != '\0' ) p++;
        if (*p=='-') {
            sprintf(buff,"%ld-%s",*val,++p);
        } else {
            if (strcmp(stepType,"instant") && strcmp(stepType,"avgd")) {
                sprintf(buff,"%ld-%s",*val,sval);
            } else {
                sprintf(buff,"%ld",*val);
            }
        }
        return pack_string( a,buff,&bufflen);
    case  1 :
        self->pack_index=-1;
        error_on_units=0;
        unpack_string(a,sval,&svallen);
        error_on_units=1;
        while (*p != '-' && *p != '\0' ) p++;
        if (*p=='-') {
            *p='\0';
            sprintf(buff,"%s-%ld",sval,*val);
        } else {
            if (strcmp(stepType,"instant") && strcmp(stepType,"avgd")) {
                sprintf(buff,"%s-%ld",sval,*val);
            } else {
                sprintf(buff,"%ld",*val);
            }
        }
        return pack_string( a,buff,&bufflen);
    default :
        Assert(self->pack_index<2);
        break;
    }

    return GRIB_INTERNAL_ERROR;
}

static int unpack_long(grib_accessor* a, long* val, size_t *len)
{
    grib_accessor_g1step_range* self = (grib_accessor_g1step_range*)a;
    char buff[100];
    size_t bufflen=100;
    long start,theEnd;
    char* p=buff;
    char* q=NULL;
    int err=0;

    /*TODO implement dirty*/
    if ((err=unpack_string( a,buff,&bufflen))!=GRIB_SUCCESS)
        return err;

    start=strtol(buff, &p,10);
    theEnd=start;
    if ( *p!=0 ) theEnd=strtol(++p, &q,10);

    if (self->pack_index==1) *val=start;
    else *val=theEnd;

    self->v[0]=start;
    self->v[1]=theEnd;
    a->dirty=0;

    return 0;
}

static int  get_native_type(grib_accessor* a)
{
    return GRIB_TYPE_STRING;
}

static void destroy(grib_context* c,grib_accessor* a)
{
    grib_accessor_g1step_range *self = (grib_accessor_g1step_range*)a;
    grib_context_free(c,self->v);
}
