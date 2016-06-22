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
   IMPLEMENTS = get_native_type;pack_missing;
   IMPLEMENTS = unpack_string; pack_string
   IMPLEMENTS = unpack_double
   IMPLEMENTS = dump
   IMPLEMENTS = compare
   END_CLASS_DEF

 */

/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "accessor.class" and rerun ./make_class.pl

*/

static int  get_native_type(grib_accessor*);
static int pack_missing(grib_accessor*);
static int pack_string(grib_accessor*, const char*, size_t *len);
static int unpack_double(grib_accessor*, double* val,size_t *len);
static int unpack_string (grib_accessor*, char*, size_t *len);
static void dump(grib_accessor*, grib_dumper*);
static void init_class(grib_accessor_class*);
static int compare(grib_accessor*, grib_accessor*);

typedef struct grib_accessor_long {
    grib_accessor          att;
/* Members defined in gen */
/* Members defined in long */
} grib_accessor_long;

extern grib_accessor_class* grib_accessor_class_gen;

static grib_accessor_class _grib_accessor_class_long = {
    &grib_accessor_class_gen,                      /* super                     */
    "long",                      /* name                      */
    sizeof(grib_accessor_long),  /* size                      */
    0,                           /* inited */
    &init_class,                 /* init_class */
    0,                       /* init                      */
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
    &pack_missing,               /* grib_pack procedures long      */
    0,                 /* grib_pack procedures long      */
    0,                  /* grib_pack procedures long      */
    0,                /* grib_unpack procedures long    */
    0,                /* grib_pack procedures double    */
    &unpack_double,              /* grib_unpack procedures double  */
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
    &compare,                    /* compare vs. another accessor   */
    0,     /* unpack only ith value          */
    0,     /* unpack a subarray         */
    0,              		/* clear          */
    0,               		/* clone accessor          */
};


grib_accessor_class* grib_accessor_class_long = &_grib_accessor_class_long;


static void init_class(grib_accessor_class* c)
{
	c->next_offset	=	(*(c->super))->next_offset;
	c->string_length	=	(*(c->super))->string_length;
	c->value_count	=	(*(c->super))->value_count;
	c->byte_count	=	(*(c->super))->byte_count;
	c->byte_offset	=	(*(c->super))->byte_offset;
	c->sub_section	=	(*(c->super))->sub_section;
	c->is_missing	=	(*(c->super))->is_missing;
	c->pack_long	=	(*(c->super))->pack_long;
	c->unpack_long	=	(*(c->super))->unpack_long;
	c->pack_double	=	(*(c->super))->pack_double;
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
	c->unpack_double_element	=	(*(c->super))->unpack_double_element;
	c->unpack_double_subarray	=	(*(c->super))->unpack_double_subarray;
	c->clear	=	(*(c->super))->clear;
	c->make_clone	=	(*(c->super))->make_clone;
}

/* END_CLASS_IMP */



static int  get_native_type(grib_accessor* a)
{
    return GRIB_TYPE_LONG;
}

static void dump(grib_accessor* a,grib_dumper* dumper)
{
    grib_dump_long(dumper,a,NULL);
}

static int unpack_string(grib_accessor*a , char*  v, size_t *len)
{
    long val = 0;
    size_t l = 1;
    char repres[1024];

    grib_unpack_long (a , &val, &l);

    if ((val == GRIB_MISSING_LONG) && ((a->flags & GRIB_ACCESSOR_FLAG_CAN_BE_MISSING) != 0) )
        sprintf(repres,"MISSING");
    else
        sprintf(repres,"%ld", val);

    l = strlen(repres)+1;

    if(l >*len ){
        grib_context_log(a->context, GRIB_LOG_ERROR, "grib_accessor_long : unpack_string : Buffer too small for %s ", a->name );

        *len = l;
        return GRIB_BUFFER_TOO_SMALL;
    }
    grib_context_log(a->context,GRIB_LOG_DEBUG, "grib_accessor_long: Casting long %s to string ", a->name);

    *len = l;

    strcpy(v,repres);
    return GRIB_SUCCESS;
}

static int pack_missing(grib_accessor* a)
{

    size_t one = 1;
    long value = GRIB_MISSING_LONG;

    if(a->flags & GRIB_ACCESSOR_FLAG_CAN_BE_MISSING)
        return grib_pack_long(a,&value,&one);

    return GRIB_VALUE_CANNOT_BE_MISSING;
}


/*
static int is_missing(grib_accessor* a){

  size_t one = 1;
  long value = GRIB_MISSING_LONG;
  long ret=0;

  if(a->flags & GRIB_ACCESSOR_FLAG_CAN_BE_MISSING)
  {
    ret = grib_unpack_long(a,&value,&one);
    Assert( ret == 0);
    return value == GRIB_MISSING_LONG;
  }

  return 0;
}
 */

static int unpack_double(grib_accessor* a, double* val,size_t *len)
{
    size_t rlen = 0;
    long count=0;
    unsigned long i = 0;
    long   *values = NULL;
    long   oneval = 0;
    int ret = GRIB_SUCCESS;

    ret=grib_value_count(a,&count);
    if (ret) return ret;
    rlen=count;

    if(*len < rlen)
    {
        grib_context_log(a->context, GRIB_LOG_ERROR, " wrong size for %s it contains %d values ", a->name , rlen);
        *len = 0;
        return GRIB_ARRAY_TOO_SMALL;
    }

    if(rlen == 1){
        ret = grib_unpack_long(a,&oneval,&rlen);
        if(ret != GRIB_SUCCESS) return ret;
        *val =  oneval;
        *len = 1;
        return GRIB_SUCCESS;
    }

    values = (long*)grib_context_malloc(a->context,rlen*sizeof(long));
    if(!values) return GRIB_INTERNAL_ERROR;


    ret = grib_unpack_long(a,values,&rlen);
    if(ret != GRIB_SUCCESS){
        grib_context_free(a->context,values);
        return ret;
    }
    for(i=0; i< rlen;i++)
        val[i] = values[i];

    grib_context_free(a->context,values);

    *len = rlen;
    return GRIB_SUCCESS;
}

static int compare(grib_accessor* a,grib_accessor* b)
{
    int retval=0;
    long *aval=0;
    long *bval=0;
    long count=0;

    size_t alen = 0;
    size_t blen = 0;
    int err=0;

    err=grib_value_count(a,&count);
    if (err) return err;
    alen=count;

    err=grib_value_count(b,&count);
    if (err) return err;
    blen=count;

    if (alen != blen) return GRIB_COUNT_MISMATCH;

    aval=(long*)grib_context_malloc(a->context,alen*sizeof(long));
    bval=(long*)grib_context_malloc(b->context,blen*sizeof(long));

    grib_unpack_long(a,aval,&alen);
    grib_unpack_long(b,bval,&blen);

    retval = GRIB_SUCCESS;
    while (alen != 0) {
        if (*bval != *aval) retval = GRIB_LONG_VALUE_MISMATCH;
        alen--;
    }

    grib_context_free(a->context,aval);
    grib_context_free(b->context,bval);

    return retval;
}

static int pack_string(grib_accessor* a, const char* val, size_t *len)
{
    char* theEnd=NULL;
    long v=strtol(val,&theEnd,10);
    if (theEnd) {
        grib_context_log(a->context,GRIB_LOG_ERROR,"trying to pack \"%s\" as long",val);
        return GRIB_WRONG_TYPE;
    }
    return grib_pack_long( a,&v,len);
}
