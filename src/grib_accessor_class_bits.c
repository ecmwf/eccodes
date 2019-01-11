/*
 * Copyright 2005-2018 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

/**************************************
 *  Enrico Fucile
 **************************************/


#include "grib_api_internal.h"


/*
   This is used by make_class.pl

   START_CLASS_DEF
   CLASS      = accessor
   SUPER      = grib_accessor_class_gen
   IMPLEMENTS = get_native_type
   IMPLEMENTS = unpack_long; pack_long
   IMPLEMENTS = unpack_bytes; byte_count
   IMPLEMENTS = unpack_double; pack_double
   IMPLEMENTS = unpack_string
   IMPLEMENTS = init
   MEMBERS=const char*    argument
   MEMBERS=long    start
   MEMBERS=long    len
   MEMBERS=double referenceValue
   MEMBERS=double referenceValuePresent
   MEMBERS=double scale
   END_CLASS_DEF

 */

/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "accessor.class" and rerun ./make_class.pl

*/

static int  get_native_type(grib_accessor*);
static int pack_double(grib_accessor*, const double* val,size_t *len);
static int pack_long(grib_accessor*, const long* val,size_t *len);
static int unpack_bytes (grib_accessor*,unsigned char*, size_t *len);
static int unpack_double(grib_accessor*, double* val,size_t *len);
static int unpack_long(grib_accessor*, long* val,size_t *len);
static int unpack_string (grib_accessor*, char*, size_t *len);
static long byte_count(grib_accessor*);
static void init(grib_accessor*,const long, grib_arguments* );
static void init_class(grib_accessor_class*);

typedef struct grib_accessor_bits {
    grib_accessor          att;
/* Members defined in gen */
/* Members defined in bits */
	const char*    argument;
	long    start;
	long    len;
	double referenceValue;
	double referenceValuePresent;
	double scale;
} grib_accessor_bits;

extern grib_accessor_class* grib_accessor_class_gen;

static grib_accessor_class _grib_accessor_class_bits = {
    &grib_accessor_class_gen,                      /* super                     */
    "bits",                      /* name                      */
    sizeof(grib_accessor_bits),  /* size                      */
    0,                           /* inited */
    &init_class,                 /* init_class */
    &init,                       /* init                      */
    0,                  /* post_init                      */
    0,                    /* free mem                       */
    0,                       /* describes himself         */
    0,                /* get length of section     */
    0,              /* get length of string      */
    0,                /* get number of values      */
    &byte_count,                 /* get number of bytes      */
    0,                /* get offset to bytes           */
    &get_native_type,            /* get native type               */
    0,                /* get sub_section                */
    0,               /* grib_pack procedures long      */
    0,                 /* grib_pack procedures long      */
    &pack_long,                  /* grib_pack procedures long      */
    &unpack_long,                /* grib_unpack procedures long    */
    &pack_double,                /* grib_pack procedures double    */
    &unpack_double,              /* grib_unpack procedures double  */
    0,                /* grib_pack procedures string    */
    &unpack_string,              /* grib_unpack procedures string  */
    0,          /* grib_pack array procedures string    */
    0,        /* grib_unpack array procedures string  */
    0,                 /* grib_pack procedures bytes     */
    &unpack_bytes,               /* grib_unpack procedures bytes   */
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


grib_accessor_class* grib_accessor_class_bits = &_grib_accessor_class_bits;


static void init_class(grib_accessor_class* c)
{
	c->dump	=	(*(c->super))->dump;
	c->next_offset	=	(*(c->super))->next_offset;
	c->string_length	=	(*(c->super))->string_length;
	c->value_count	=	(*(c->super))->value_count;
	c->byte_offset	=	(*(c->super))->byte_offset;
	c->sub_section	=	(*(c->super))->sub_section;
	c->pack_missing	=	(*(c->super))->pack_missing;
	c->is_missing	=	(*(c->super))->is_missing;
	c->pack_string	=	(*(c->super))->pack_string;
	c->pack_string_array	=	(*(c->super))->pack_string_array;
	c->unpack_string_array	=	(*(c->super))->unpack_string_array;
	c->pack_bytes	=	(*(c->super))->pack_bytes;
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

#ifdef ECCODES_ON_WINDOWS
#define round(a) ( (a) >=0 ? ((a)+0.5) : ((a)-0.5) )
#endif

static void init(grib_accessor* a,const long l, grib_arguments* c)
{
    grib_accessor_bits* self = (grib_accessor_bits*)a;
    grib_expression* e=NULL;
    int n = 0;

    self->argument = grib_arguments_get_name(grib_handle_of_accessor(a),c,n++);
    self->start    = grib_arguments_get_long(grib_handle_of_accessor(a),c,n++);
    self->len    = grib_arguments_get_long(grib_handle_of_accessor(a),c,n++);
    e=grib_arguments_get_expression(grib_handle_of_accessor(a),c,n++);
    if (e) {
        grib_expression_evaluate_double(grib_handle_of_accessor(a),e,&(self->referenceValue));
        self->referenceValuePresent=1;
    } else {
        self->referenceValuePresent=0;
    }
    self->scale = 1;
    if (self->referenceValuePresent) {
        self->scale=grib_arguments_get_double(grib_handle_of_accessor(a),c,n++);
    }

    Assert(self->len <= sizeof(long)*8);

    a->length=0;
}

static int unpack_long(grib_accessor* a, long* val, size_t *len)
{
    grib_accessor_bits* self = (grib_accessor_bits*)a;
    grib_accessor* x=NULL;
    unsigned char* p=NULL;
    grib_handle* h=grib_handle_of_accessor(a);
    long start,length;
    int ret=0;

    if(*len < 1) return GRIB_WRONG_ARRAY_SIZE;

    start=self->start;
    length=self->len;

    x=grib_find_accessor(grib_handle_of_accessor(a),self->argument);
    if (!x) return GRIB_NOT_FOUND;

    p  = h->buffer->data + grib_byte_offset(x);
    *val=grib_decode_unsigned_long(p,&start,length);

    *len=1;

    return ret;
}

static int unpack_double(grib_accessor* a, double* val, size_t *len)
{
    grib_accessor_bits* self = (grib_accessor_bits*)a;
    grib_accessor* x=NULL;
    unsigned char* p=NULL;
    grib_handle* h=grib_handle_of_accessor(a);
    long start,length;
    int ret=0;

    if(*len < 1) return GRIB_WRONG_ARRAY_SIZE;

    start=self->start;
    length=self->len;

    x=grib_find_accessor(grib_handle_of_accessor(a),self->argument);
    if (!x) return GRIB_NOT_FOUND;

    p  = h->buffer->data + grib_byte_offset(x);
    *val=grib_decode_unsigned_long(p,&start,length);

    *val=((long)*val+self->referenceValue)/self->scale;

    *len=1;

    return ret;
}

static int pack_double(grib_accessor* a, const double* val, size_t *len)
{
    grib_accessor_bits* self = (grib_accessor_bits*)a;
    grib_accessor* x=NULL;
    grib_handle* h=grib_handle_of_accessor(a);
    unsigned char* p=NULL;
    long start,length,lval;

    if(*len != 1) return GRIB_WRONG_ARRAY_SIZE;

    start  = self->start;
    length = self->len;

    x=grib_find_accessor(grib_handle_of_accessor(a),self->argument);
    if (!x) return GRIB_NOT_FOUND;

    p=h->buffer->data + grib_byte_offset(x);

    lval= round(*val * self->scale) - self->referenceValue;
    return grib_encode_unsigned_longb(p,lval,&start,length);
}

static int pack_long(grib_accessor* a, const long* val, size_t *len)
{
    grib_accessor_bits* self = (grib_accessor_bits*)a;
    grib_accessor* x=NULL;
    grib_handle* h=grib_handle_of_accessor(a);
    unsigned char* p=NULL;
    long start,length, maxval;

    if(*len != 1) return GRIB_WRONG_ARRAY_SIZE;

    if (get_native_type(a) == GRIB_TYPE_DOUBLE) {
        /* ECC-402 */
        const double dVal = (double)(*val);
        return pack_double(a, &dVal, len);
    }

    start  = self->start;
    length = self->len;

    x=grib_find_accessor(grib_handle_of_accessor(a),self->argument);
    if (!x) return GRIB_NOT_FOUND;

    /* Check the input value */
    if (*val < 0) {
        grib_context_log(h->context, GRIB_LOG_ERROR, "key=%s: value cannot be negative",a->name);
        return GRIB_ENCODING_ERROR;
    }
    maxval = (1 << length) - 1;
    if (*val > maxval){
        grib_context_log(h->context, GRIB_LOG_ERROR,
                "key=%s: Trying to encode value of %ld but the maximum allowable value is %ld (number of bits=%ld)",
                a->name, *val, maxval, length);
        return GRIB_ENCODING_ERROR;
    }

    p=h->buffer->data + grib_byte_offset(x);
    return grib_encode_unsigned_longb(p,*val,&start,length);
}

static int get_native_type(grib_accessor* a)
{
    int type=GRIB_TYPE_BYTES;
    grib_accessor_bits* self = (grib_accessor_bits*)a;

    if (a->flags & GRIB_ACCESSOR_FLAG_STRING_TYPE)
        type=GRIB_TYPE_STRING;

    if (a->flags & GRIB_ACCESSOR_FLAG_LONG_TYPE)
        type=GRIB_TYPE_LONG;

    if (self->referenceValuePresent)
        type=GRIB_TYPE_DOUBLE;

    return type;
}

static int unpack_string(grib_accessor*a , char*  v, size_t *len)
{
    int ret=0;
    double dval=0;
    long lval=0;
    size_t llen=1;
    grib_accessor_class* super = NULL;

    switch (get_native_type(a)) {
    case GRIB_TYPE_LONG:
        ret=unpack_long(a,&lval,&llen);
        sprintf(v,"%ld",lval);
        *len=strlen(v);
        break;

    case GRIB_TYPE_DOUBLE:
        ret=unpack_double(a,&dval,&llen);
        sprintf(v,"%g",dval);
        *len=strlen(v);
        break;

    default:
        super = *(a->cclass->super);
        ret = super->unpack_string(a,v,len);
    }
    return ret;
}

static long byte_count(grib_accessor* a)
{
    grib_context_log(a->context,GRIB_LOG_DEBUG,"byte_count of %s = %ld",a->name,a->length);
    return a->length;
}

static int unpack_bytes (grib_accessor* a,unsigned char* buffer, size_t *len)
{
    if (*len < a->length) {
        *len = a->length;
        return GRIB_ARRAY_TOO_SMALL;
    }
    *len = a->length;

    memcpy(buffer, grib_handle_of_accessor(a)->buffer->data + a->offset, *len);

    return GRIB_SUCCESS;
}
