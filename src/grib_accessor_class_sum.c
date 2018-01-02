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
   SUPER      = grib_accessor_class_double
   IMPLEMENTS = unpack_long
   IMPLEMENTS = unpack_double
   IMPLEMENTS = value_count
   IMPLEMENTS = init
   MEMBERS=const char*    values
   END_CLASS_DEF

 */

/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "accessor.class" and rerun ./make_class.pl

*/

static int unpack_double(grib_accessor*, double* val,size_t *len);
static int unpack_long(grib_accessor*, long* val,size_t *len);
static int value_count(grib_accessor*,long*);
static void init(grib_accessor*,const long, grib_arguments* );
static void init_class(grib_accessor_class*);

typedef struct grib_accessor_sum {
    grib_accessor          att;
/* Members defined in gen */
/* Members defined in double */
/* Members defined in sum */
	const char*    values;
} grib_accessor_sum;

extern grib_accessor_class* grib_accessor_class_double;

static grib_accessor_class _grib_accessor_class_sum = {
    &grib_accessor_class_double,                      /* super                     */
    "sum",                      /* name                      */
    sizeof(grib_accessor_sum),  /* size                      */
    0,                           /* inited */
    &init_class,                 /* init_class */
    &init,                       /* init                      */
    0,                  /* post_init                      */
    0,                    /* free mem                       */
    0,                       /* describes himself         */
    0,                /* get length of section     */
    0,              /* get length of string      */
    &value_count,                /* get number of values      */
    0,                 /* get number of bytes      */
    0,                /* get offset to bytes           */
    0,            /* get native type               */
    0,                /* get sub_section                */
    0,               /* grib_pack procedures long      */
    0,                 /* grib_pack procedures long      */
    0,                  /* grib_pack procedures long      */
    &unpack_long,                /* grib_unpack procedures long    */
    0,                /* grib_pack procedures double    */
    &unpack_double,              /* grib_unpack procedures double  */
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


grib_accessor_class* grib_accessor_class_sum = &_grib_accessor_class_sum;


static void init_class(grib_accessor_class* c)
{
	c->dump	=	(*(c->super))->dump;
	c->next_offset	=	(*(c->super))->next_offset;
	c->string_length	=	(*(c->super))->string_length;
	c->byte_count	=	(*(c->super))->byte_count;
	c->byte_offset	=	(*(c->super))->byte_offset;
	c->get_native_type	=	(*(c->super))->get_native_type;
	c->sub_section	=	(*(c->super))->sub_section;
	c->pack_missing	=	(*(c->super))->pack_missing;
	c->is_missing	=	(*(c->super))->is_missing;
	c->pack_long	=	(*(c->super))->pack_long;
	c->pack_double	=	(*(c->super))->pack_double;
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
    grib_accessor_sum* self = (grib_accessor_sum*)a;
    int n = 0;
    self->values = grib_arguments_get_name(grib_handle_of_accessor(a),c,n++);
    a->length=0;
    a->flags |= GRIB_ACCESSOR_FLAG_READ_ONLY;
}

static int unpack_long   (grib_accessor* a, long* val, size_t *len)
{
    grib_accessor_sum* self = (grib_accessor_sum*)a;
    int ret = 0;
    size_t size=0;
    long* values=0;
    long i;
    long count=0;

    ret=value_count(a,&count);
    if (ret) return ret;
    size=count;

    if (size==0) {
        *val=0;
        return ret;
    }
    values=(long*)grib_context_malloc_clear(a->context,sizeof(long)*size);
    if (!values) return GRIB_OUT_OF_MEMORY;

    grib_get_long_array(grib_handle_of_accessor(a),self->values,values,&size);

    *val=0;
    for (i=0;i<size;i++)
        *val+=values[i];

    grib_context_free(a->context,values);

    return ret;
}

static int unpack_double   (grib_accessor* a, double* val, size_t *len)
{
    grib_accessor_sum* self = (grib_accessor_sum*)a;
    int ret = 0;
    size_t size=0;
    double* values=0;
    long i;
    long count=0;

    ret=value_count(a,&count);
    if (ret) return ret;
    size=count;

    if (size==0) {
        *val=0;
        return ret;
    }
    values=(double*)grib_context_malloc_clear(a->context,sizeof(double)*size);
    if (!values) return GRIB_OUT_OF_MEMORY;

    grib_get_double_array(grib_handle_of_accessor(a),self->values,values,&size);

    *val=0;
    for (i=0;i<size;i++)
        *val+=values[i];

    grib_context_free(a->context,values);

    return ret;
}

static int value_count(grib_accessor* a,long* count)
{
    grib_accessor_sum* self = (grib_accessor_sum*)a;
    size_t n=0;
    int ret=0;

    ret = grib_get_size(grib_handle_of_accessor(a), self->values,&n);
    *count=n;

    if (ret)
        grib_context_log(a->context,GRIB_LOG_ERROR,
                "%s is unable to get size of %s",a->name,self->values);

    return ret;
}
