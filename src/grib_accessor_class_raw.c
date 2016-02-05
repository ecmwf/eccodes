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
   IMPLEMENTS = get_native_type;init;update_size; pack_bytes
   IMPLEMENTS = compare;unpack_bytes;byte_count;value_count
   MEMBERS    = const char* totalLength
   MEMBERS    = const char* sectionLength
   MEMBERS    = long relativeOffset
   END_CLASS_DEF

 */

/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "accessor.class" and rerun ./make_class.pl

*/

static int  get_native_type(grib_accessor*);
static int pack_bytes(grib_accessor*,const unsigned char*, size_t *len);
static int unpack_bytes (grib_accessor*,unsigned char*, size_t *len);
static long byte_count(grib_accessor*);
static int value_count(grib_accessor*,long*);
static void init(grib_accessor*,const long, grib_arguments* );
static void init_class(grib_accessor_class*);
static void update_size(grib_accessor*,size_t);
static int compare(grib_accessor*, grib_accessor*);

typedef struct grib_accessor_raw {
    grib_accessor          att;
/* Members defined in gen */
/* Members defined in raw */
	const char* totalLength;
	const char* sectionLength;
	long relativeOffset;
} grib_accessor_raw;

extern grib_accessor_class* grib_accessor_class_gen;

static grib_accessor_class _grib_accessor_class_raw = {
    &grib_accessor_class_gen,                      /* super                     */
    "raw",                      /* name                      */
    sizeof(grib_accessor_raw),  /* size                      */
    0,                           /* inited */
    &init_class,                 /* init_class */
    &init,                       /* init                      */
    0,                  /* post_init                      */
    0,                    /* free mem                       */
    0,                       /* describes himself         */
    0,                /* get length of section     */
    0,              /* get length of string      */
    &value_count,                /* get number of values      */
    &byte_count,                 /* get number of bytes      */
    0,                /* get offset to bytes           */
    &get_native_type,            /* get native type               */
    0,                /* get sub_section                */
    0,               /* grib_pack procedures long      */
    0,                 /* grib_pack procedures long      */
    0,                  /* grib_pack procedures long      */
    0,                /* grib_unpack procedures long    */
    0,                /* grib_pack procedures double    */
    0,              /* grib_unpack procedures double  */
    0,                /* grib_pack procedures string    */
    0,              /* grib_unpack procedures string  */
    0,          /* grib_pack array procedures string    */
    0,        /* grib_unpack array procedures string  */
    &pack_bytes,                 /* grib_pack procedures bytes     */
    &unpack_bytes,               /* grib_unpack procedures bytes   */
    0,            /* pack_expression */
    0,              /* notify_change   */
    &update_size,                /* update_size   */
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


grib_accessor_class* grib_accessor_class_raw = &_grib_accessor_class_raw;


static void init_class(grib_accessor_class* c)
{
	c->dump	=	(*(c->super))->dump;
	c->next_offset	=	(*(c->super))->next_offset;
	c->string_length	=	(*(c->super))->string_length;
	c->byte_offset	=	(*(c->super))->byte_offset;
	c->sub_section	=	(*(c->super))->sub_section;
	c->pack_missing	=	(*(c->super))->pack_missing;
	c->is_missing	=	(*(c->super))->is_missing;
	c->pack_long	=	(*(c->super))->pack_long;
	c->unpack_long	=	(*(c->super))->unpack_long;
	c->pack_double	=	(*(c->super))->pack_double;
	c->unpack_double	=	(*(c->super))->unpack_double;
	c->pack_string	=	(*(c->super))->pack_string;
	c->unpack_string	=	(*(c->super))->unpack_string;
	c->pack_string_array	=	(*(c->super))->pack_string_array;
	c->unpack_string_array	=	(*(c->super))->unpack_string_array;
	c->pack_expression	=	(*(c->super))->pack_expression;
	c->notify_change	=	(*(c->super))->notify_change;
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

static void init(grib_accessor* a, const long len , grib_arguments* arg )
{
    int n=0;
    int err=0;
    long sectionLength;
    grib_accessor_raw *self =(grib_accessor_raw*)a;
    grib_expression* e;

    a->length=0;
    self->totalLength = grib_arguments_get_name(grib_handle_of_accessor(a),arg,n++);
    self->sectionLength = grib_arguments_get_name(grib_handle_of_accessor(a),arg,n++);

    e=grib_arguments_get_expression(grib_handle_of_accessor(a), arg,n++);
    grib_expression_evaluate_long(grib_handle_of_accessor(a),e,&(self->relativeOffset));
    if (err) grib_context_log(grib_handle_of_accessor(a)->context,GRIB_LOG_FATAL,"unable to evaluate relativeOffset");

    grib_get_long(grib_handle_of_accessor(a),self->sectionLength,&sectionLength);

    a->length=sectionLength-self->relativeOffset;
    if (a->length<0) a->length=0;

    /* Assert(a->length>=0); */
}

static int  get_native_type(grib_accessor* a){
    return GRIB_TYPE_BYTES;
}


static int compare(grib_accessor* a, grib_accessor* b) {
    int retval=GRIB_SUCCESS;

    size_t alen = (size_t)grib_byte_count(a);
    size_t blen = (size_t)grib_byte_count(b);

    if (alen != blen) return GRIB_COUNT_MISMATCH;

    return retval;
}

static long byte_count(grib_accessor* a){
    return a->length;
}

static int value_count(grib_accessor* a,long* len){
    *len=a->length;
    return 0;
}

static int unpack_bytes (grib_accessor* a,unsigned char* buffer, size_t *len) {
    if (*len < a->length) {
        *len = a->length;
        return GRIB_ARRAY_TOO_SMALL;
    }
    *len = a->length;

    memcpy(buffer, grib_handle_of_accessor(a)->buffer->data + a->offset, *len);

    return GRIB_SUCCESS;
}

static void update_size(grib_accessor* a,size_t s)
{
    grib_context_log(a->context,GRIB_LOG_DEBUG,"updating size of %s old %ld new %ld",a->name,a->length,s);
    a->length = s;
    Assert(a->length>=0);
}

void accessor_raw_set_length(grib_accessor* a,size_t len) {
    a->length=len;
}

long accessor_raw_get_offset(grib_accessor* a) {
    return a->offset;
}

static int pack_bytes(grib_accessor* a, const unsigned char* val, size_t *len)
{
    size_t length = *len;
    long totalLength;
    long sectionLength;
    grib_handle* h=grib_handle_of_accessor(a);
    grib_accessor_raw *self =(grib_accessor_raw*)a;
    long dlen=length-a->length;

    grib_get_long(h,self->totalLength,&totalLength);
    totalLength+=dlen;
    grib_get_long(h,self->sectionLength,&sectionLength);
    sectionLength+=dlen;

    grib_buffer_replace(a, val, length,1,1);

    grib_set_long(h,self->totalLength,totalLength);
    grib_set_long(h,self->sectionLength,sectionLength);
    a->length = length;

    return GRIB_SUCCESS;
}
