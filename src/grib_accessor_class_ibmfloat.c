/*
 * Copyright 2005-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

/********************************
 *   Enrico Fucile
 *******************************/

#include "grib_api_internal.h"

/*
   This is used by make_class.pl

   START_CLASS_DEF
   CLASS      = accessor
   SUPER      = grib_accessor_class_double
   IMPLEMENTS = unpack_double;pack_double
   IMPLEMENTS = init
   IMPLEMENTS = next_offset
   IMPLEMENTS = byte_count
   IMPLEMENTS = value_count
   IMPLEMENTS = byte_offset
   MEMBERS    = grib_arguments* arg
   IMPLEMENTS = update_size
   IMPLEMENTS = nearest_smaller_value
   END_CLASS_DEF

 */

/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "accessor.class" and rerun ./make_class.pl

*/

static int pack_double(grib_accessor*, const double* val,size_t *len);
static int unpack_double(grib_accessor*, double* val,size_t *len);
static long byte_count(grib_accessor*);
static long byte_offset(grib_accessor*);
static long next_offset(grib_accessor*);
static int value_count(grib_accessor*,long*);
static void init(grib_accessor*,const long, grib_arguments* );
static void init_class(grib_accessor_class*);
static void update_size(grib_accessor*,size_t);
static int nearest_smaller_value (grib_accessor*, double,double*);

typedef struct grib_accessor_ibmfloat {
    grib_accessor          att;
/* Members defined in gen */
/* Members defined in double */
/* Members defined in ibmfloat */
	grib_arguments* arg;
} grib_accessor_ibmfloat;

extern grib_accessor_class* grib_accessor_class_double;

static grib_accessor_class _grib_accessor_class_ibmfloat = {
    &grib_accessor_class_double,                      /* super                     */
    "ibmfloat",                      /* name                      */
    sizeof(grib_accessor_ibmfloat),  /* size                      */
    0,                           /* inited */
    &init_class,                 /* init_class */
    &init,                       /* init                      */
    0,                  /* post_init                      */
    0,                    /* free mem                       */
    0,                       /* describes himself         */
    &next_offset,                /* get length of section     */
    0,              /* get length of string      */
    &value_count,                /* get number of values      */
    &byte_count,                 /* get number of bytes      */
    &byte_offset,                /* get offset to bytes           */
    0,            /* get native type               */
    0,                /* get sub_section                */
    0,               /* grib_pack procedures long      */
    0,                 /* grib_pack procedures long      */
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
    &update_size,                /* update_size   */
    0,            /* preferred_size   */
    0,                    /* resize   */
    &nearest_smaller_value,      /* nearest_smaller_value */
    0,                       /* next accessor    */
    0,                    /* compare vs. another accessor   */
    0,     /* unpack only ith value          */
    0,     /* unpack a subarray         */
    0,              		/* clear          */
    0,               		/* clone accessor          */
};


grib_accessor_class* grib_accessor_class_ibmfloat = &_grib_accessor_class_ibmfloat;


static void init_class(grib_accessor_class* c)
{
	c->dump	=	(*(c->super))->dump;
	c->string_length	=	(*(c->super))->string_length;
	c->get_native_type	=	(*(c->super))->get_native_type;
	c->sub_section	=	(*(c->super))->sub_section;
	c->pack_missing	=	(*(c->super))->pack_missing;
	c->is_missing	=	(*(c->super))->is_missing;
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
	c->preferred_size	=	(*(c->super))->preferred_size;
	c->resize	=	(*(c->super))->resize;
	c->next	=	(*(c->super))->next;
	c->compare	=	(*(c->super))->compare;
	c->unpack_double_element	=	(*(c->super))->unpack_double_element;
	c->unpack_double_subarray	=	(*(c->super))->unpack_double_subarray;
	c->clear	=	(*(c->super))->clear;
	c->make_clone	=	(*(c->super))->make_clone;
}

/* END_CLASS_IMP */

static void init(grib_accessor* a,  const long len, grib_arguments* arg)
{
    grib_accessor_ibmfloat* self = (grib_accessor_ibmfloat*)a;
    long count=0;

    self->arg = arg;
    grib_value_count(a,&count);
    a->length = 4*count;
    Assert(a->length>=0);
}

static int unpack_double   (grib_accessor* a, double* val, size_t *len)
{
    unsigned long rlen = 0;
    long count=0;
    int err=0;
    unsigned long i = 0;
    long bitp = a->offset*8;

    err=grib_value_count(a,&count);
    if (err) return err;
    rlen=count;

    if(*len < rlen)
    {
        grib_context_log(a->context, GRIB_LOG_ERROR, " wrong size (%ld) for %s it contains %d values ", *len,a->name , rlen);
        *len = 0;
        return GRIB_ARRAY_TOO_SMALL;
    }

    for(i=0; i< rlen; i++)
        val[i] = grib_long_to_ibm(grib_decode_unsigned_long(grib_handle_of_accessor(a)->buffer->data,&bitp,32));

    *len = rlen;
    return GRIB_SUCCESS;
}

static int pack_double   (grib_accessor* a, const double* val, size_t *len)
{
    grib_accessor_ibmfloat* self = (grib_accessor_ibmfloat*)a;
    int ret = 0;
    unsigned long i = 0;
    unsigned long rlen = *len;
    size_t buflen  = 0;
    unsigned char *buf = NULL;
    long off = 0;

    if(*len < 1)
    {
        grib_context_log(a->context, GRIB_LOG_ERROR, " wrong size for %s it pack at least 1 values ", a->name , rlen );
        *len = 0;
        return GRIB_ARRAY_TOO_SMALL;
    }

    if (rlen == 1){
        /*
    double x = 0;
    grib_nearest_smaller_ibm_float(val[0],&x);
    double y = grib_long_to_ibm(grib_ibm_to_long(val[0]));
    printf("IBMFLOAT val=%.20f nearest_smaller_ibm_float=%.20f long_to_ibm=%.20f\n",val[0],x ,y);
         */
        off = byte_offset(a)*8;
        ret =  grib_encode_unsigned_long(grib_handle_of_accessor(a)->buffer->data,grib_ibm_to_long(val[0]), &off,  32);
        if (*len > 1)  grib_context_log(a->context, GRIB_LOG_WARNING, "grib_accessor_unsigned : Trying to pack %d values in a scalar %s, packing first value",  *len, a->name  );
        if (ret == GRIB_SUCCESS) len[0] = 1;
        return ret;
    }

    buflen = rlen*4;

    buf = (unsigned char*)grib_context_malloc(a->context,buflen);

    for(i=0; i < rlen;i++){
        grib_encode_unsigned_longb(buf,grib_ibm_to_long(val[i]), &off,  32);
    }
    ret = grib_set_long_internal(grib_handle_of_accessor(a),grib_arguments_get_name(a->parent->h,self->arg,0),rlen);

    if(ret == GRIB_SUCCESS)
        grib_buffer_replace(a, buf, buflen,1,1);
    else
        *len = 0;

    grib_context_free(a->context,buf);

    a->length = byte_count(a);

    return ret;
}

static long byte_count(grib_accessor* a)
{
    return a->length;
}

static int value_count(grib_accessor* a,long* len)
{
    grib_accessor_ibmfloat* self = (grib_accessor_ibmfloat*)a;
    *len = 0;
    if(!self->arg) {*len=1;return 0;}
    return grib_get_long_internal(grib_handle_of_accessor(a),grib_arguments_get_name(a->parent->h,self->arg,0),len);
}

static long byte_offset(grib_accessor* a)
{
    return a->offset;
}
static void update_size(grib_accessor* a,size_t s)
{
    a->length = s;
    Assert(a->length>=0);
}
static long next_offset(grib_accessor* a)
{
    return grib_byte_offset(a)+grib_byte_count(a);
}

static int nearest_smaller_value(grib_accessor* a, double val,double* nearest)
{
    int ret=0;
    if (grib_nearest_smaller_ibm_float(val,nearest)==GRIB_INTERNAL_ERROR) {
        grib_context_log(a->context,GRIB_LOG_ERROR,"grib_nearest_smaller_ibm_float overflow value=%g\n",val);
        grib_dump_content(grib_handle_of_accessor(a),stderr,"wmo",GRIB_DUMP_FLAG_HEXADECIMAL,0);
        ret=GRIB_INTERNAL_ERROR;
    }
    return ret;
}
