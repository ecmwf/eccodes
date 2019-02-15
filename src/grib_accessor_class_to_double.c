/*
 * Copyright 2005-2018 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */
/***********************************************************
 *  Enrico Fucile
 ***********************************************************/

#include "grib_api_internal.h"
/*
   This is used by make_class.pl

   START_CLASS_DEF
   CLASS      = accessor
   SUPER      = grib_accessor_class_gen
   IMPLEMENTS = unpack_string;pack_string
   IMPLEMENTS = unpack_long;pack_long
   IMPLEMENTS = unpack_double;pack_double
   IMPLEMENTS = init;dump;string_length
   IMPLEMENTS = post_init;value_count
   IMPLEMENTS = next_offset
   IMPLEMENTS = get_native_type
   IMPLEMENTS = compare
   MEMBERS = const char* key
   MEMBERS = long start
   MEMBERS = size_t length
   MEMBERS = long scale
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
static int pack_string(grib_accessor*, const char*, size_t *len);
static int unpack_double(grib_accessor*, double* val,size_t *len);
static int unpack_long(grib_accessor*, long* val,size_t *len);
static int unpack_string (grib_accessor*, char*, size_t *len);
static size_t string_length(grib_accessor*);
static long next_offset(grib_accessor*);
static int value_count(grib_accessor*,long*);
static void dump(grib_accessor*, grib_dumper*);
static void init(grib_accessor*,const long, grib_arguments* );
static void post_init(grib_accessor*);
static void init_class(grib_accessor_class*);
static int compare(grib_accessor*, grib_accessor*);

typedef struct grib_accessor_to_double {
    grib_accessor          att;
/* Members defined in gen */
/* Members defined in to_double */
	const char* key;
	long start;
	size_t length;
	long scale;
} grib_accessor_to_double;

extern grib_accessor_class* grib_accessor_class_gen;

static grib_accessor_class _grib_accessor_class_to_double = {
    &grib_accessor_class_gen,                      /* super                     */
    "to_double",                      /* name                      */
    sizeof(grib_accessor_to_double),  /* size                      */
    0,                           /* inited */
    &init_class,                 /* init_class */
    &init,                       /* init                      */
    &post_init,                  /* post_init                      */
    0,                    /* free mem                       */
    &dump,                       /* describes himself         */
    &next_offset,                /* get length of section     */
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
    &pack_double,                /* grib_pack procedures double    */
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


grib_accessor_class* grib_accessor_class_to_double = &_grib_accessor_class_to_double;


static void init_class(grib_accessor_class* c)
{
	c->byte_count	=	(*(c->super))->byte_count;
	c->byte_offset	=	(*(c->super))->byte_offset;
	c->sub_section	=	(*(c->super))->sub_section;
	c->pack_missing	=	(*(c->super))->pack_missing;
	c->is_missing	=	(*(c->super))->is_missing;
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

static void init(grib_accessor* a, const long len , grib_arguments* arg )
{
    grib_accessor_to_double* self=(grib_accessor_to_double*)a;

    self->key=grib_arguments_get_name(grib_handle_of_accessor(a),arg,0);
    self->start=grib_arguments_get_long(grib_handle_of_accessor(a),arg,1);
    self->length=grib_arguments_get_long(grib_handle_of_accessor(a),arg,2);
    self->scale=grib_arguments_get_long(grib_handle_of_accessor(a),arg,3);
    if (!self->scale) self->scale=1;

    a->flags |= GRIB_ACCESSOR_FLAG_READ_ONLY;
    a->length=0;
}

static void post_init(grib_accessor* a )
{
    return;
}

static int value_count(grib_accessor* a,long* count)
{
    grib_accessor_to_double* self=(grib_accessor_to_double*)a;
    size_t size=0;

    int err=grib_get_size(grib_handle_of_accessor(a), self->key,&size);
    *count=size;

    return err;
}

static size_t string_length(grib_accessor* a)
{
    grib_accessor_to_double* self=(grib_accessor_to_double*)a;
    size_t size=0;

    if (self->length) return self->length;

    _grib_get_string_length(a,&size);
    return size;
}

static void dump(grib_accessor* a, grib_dumper* dumper)
{
    grib_dump_string(dumper,a,NULL);
}

static int  get_native_type(grib_accessor* a)
{
    return GRIB_TYPE_LONG;
}

static int unpack_string(grib_accessor* a, char* val, size_t *len)
{
    int err=0;
    grib_accessor_to_double* self=(grib_accessor_to_double*)a;
    char buff[512]={0,};
    size_t length;
    size_t size=512;

    length=string_length(a);

    if( len[0] < length+1 )
    {
        grib_context_log(a->context, GRIB_LOG_ERROR, "unpack_string: Wrong size (%d) for %s it contains %d values ", len[0], a->name , a->length+1 );
        len[0] = 0;
        return GRIB_ARRAY_TOO_SMALL;
    }

    err=grib_get_string(grib_handle_of_accessor(a),self->key,buff,&size);
    if (err) return err;
    if (length > size) {
        err=GRIB_STRING_TOO_SMALL;
        length=size;
    }

    memcpy(val,buff+self->start,length);

    val[length] = 0;
    len[0] = length;
    return GRIB_SUCCESS;
}

static int pack_string(grib_accessor* a, const char* val, size_t *len)
{
    return GRIB_NOT_IMPLEMENTED;
}

static int pack_long(grib_accessor* a, const long*  v, size_t *len)
{
    grib_context_log(a->context,GRIB_LOG_ERROR, " Should not pack %s as long", a->name);
    return GRIB_NOT_IMPLEMENTED;
}

static int pack_double(grib_accessor* a, const double*v, size_t *len)
{
    grib_context_log(a->context,GRIB_LOG_ERROR, " Should not pack %s  as double", a->name);
    return GRIB_NOT_IMPLEMENTED;
}

static int  unpack_long   (grib_accessor* a, long*  v, size_t *len)
{
    grib_accessor_to_double* self=(grib_accessor_to_double*)a;
    char val[1024]={0,};
    size_t l = sizeof(val);
    char  *last = NULL;
    int err=grib_unpack_string (a , val, &l);

    if (err) return err;

    *v = strtol(val,&last,10);
    if (*last) {err=GRIB_WRONG_CONVERSION;}
    *v/=self->scale;

    return err;
}

static int  unpack_double   (grib_accessor* a, double*  v, size_t *len)
{
    grib_accessor_to_double* self=(grib_accessor_to_double*)a;
    char val[1024]={0,};
    size_t l = sizeof(val);
    char  *last = NULL;
    int err=grib_unpack_string (a , val, &l);

    if (err) return err;

    *v = strtod(val,&last);
    if (*last) {err=GRIB_WRONG_CONVERSION;}
    *v/=self->scale;

    return err;
}

static long next_offset(grib_accessor* a)
{
    return a->offset+a->length;
}

static int compare(grib_accessor* a,grib_accessor* b)
{
    int retval=0;
    char *aval=0;
    char *bval=0;
    int err=0;

    size_t alen = 0;
    size_t blen = 0;
    long count=0;

    err=grib_value_count(a,&count);
    if (err) return err;
    alen=count;

    err=grib_value_count(b,&count);
    if (err) return err;
    blen=count;

    if (alen != blen) return GRIB_COUNT_MISMATCH;

    aval=(char*)grib_context_malloc(a->context,alen*sizeof(char));
    bval=(char*)grib_context_malloc(b->context,blen*sizeof(char));

    grib_unpack_string(a,aval,&alen);
    grib_unpack_string(b,bval,&blen);

    retval = GRIB_SUCCESS;
    if (strcmp(aval,bval)) retval = GRIB_STRING_VALUE_MISMATCH;

    grib_context_free(a->context,aval);
    grib_context_free(b->context,bval);

    return retval;
}
