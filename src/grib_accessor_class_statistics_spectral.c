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
   SUPER      = grib_accessor_class_abstract_vector
   IMPLEMENTS = unpack_double; destroy
   IMPLEMENTS = value_count;compare
   IMPLEMENTS = init
   MEMBERS = const char* values
   MEMBERS = const char* J
   MEMBERS = const char* K
   MEMBERS = const char* M
   MEMBERS = const char* JS
   END_CLASS_DEF

 */

/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "accessor.class" and rerun ./make_class.pl

*/

static int unpack_double(grib_accessor*, double* val,size_t *len);
static int value_count(grib_accessor*,long*);
static void destroy(grib_context*,grib_accessor*);
static void init(grib_accessor*,const long, grib_arguments* );
static void init_class(grib_accessor_class*);
static int compare(grib_accessor*, grib_accessor*);

typedef struct grib_accessor_statistics_spectral {
    grib_accessor          att;
/* Members defined in gen */
/* Members defined in double */
/* Members defined in abstract_vector */
	double* v;
	int number_of_elements;
/* Members defined in statistics_spectral */
	const char* values;
	const char* J;
	const char* K;
	const char* M;
	const char* JS;
} grib_accessor_statistics_spectral;

extern grib_accessor_class* grib_accessor_class_abstract_vector;

static grib_accessor_class _grib_accessor_class_statistics_spectral = {
    &grib_accessor_class_abstract_vector,                      /* super                     */
    "statistics_spectral",                      /* name                      */
    sizeof(grib_accessor_statistics_spectral),  /* size                      */
    0,                           /* inited */
    &init_class,                 /* init_class */
    &init,                       /* init                      */
    0,                  /* post_init                      */
    &destroy,                    /* free mem                       */
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
    0,                /* grib_unpack procedures long    */
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
    &compare,                    /* compare vs. another accessor   */
    0,     /* unpack only ith value          */
    0,     /* unpack a subarray         */
    0,              		/* clear          */
    0,               		/* clone accessor          */
};


grib_accessor_class* grib_accessor_class_statistics_spectral = &_grib_accessor_class_statistics_spectral;


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
	c->unpack_long	=	(*(c->super))->unpack_long;
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
	c->unpack_double_element	=	(*(c->super))->unpack_double_element;
	c->unpack_double_subarray	=	(*(c->super))->unpack_double_subarray;
	c->clear	=	(*(c->super))->clear;
	c->make_clone	=	(*(c->super))->make_clone;
}

/* END_CLASS_IMP */

static void init(grib_accessor* a,const long l, grib_arguments* c)
{
    grib_accessor_statistics_spectral* self = (grib_accessor_statistics_spectral*)a;
    int n = 0;

    self->values = grib_arguments_get_name(grib_handle_of_accessor(a),c,n++);
    self->J = grib_arguments_get_name(grib_handle_of_accessor(a),c,n++);
    self->K = grib_arguments_get_name(grib_handle_of_accessor(a),c,n++);
    self->M = grib_arguments_get_name(grib_handle_of_accessor(a),c,n++);
    self->JS = grib_arguments_get_name(grib_handle_of_accessor(a),c,n++);

    a->flags  |= GRIB_ACCESSOR_FLAG_READ_ONLY;
    a->flags |= GRIB_ACCESSOR_FLAG_FUNCTION;

    self->number_of_elements=4;
    self->v=(double*)grib_context_malloc(a->context,
            sizeof(double)*self->number_of_elements);

    a->length=0;
    a->dirty=1;
}

static int unpack_double   (grib_accessor* a, double* val, size_t *len)
{
    grib_accessor_statistics_spectral* self = (grib_accessor_statistics_spectral*)a;
    int ret = 0,i=0;
    double* values;
    size_t size=0;
    long J,K,M,N;
    double avg,enorm,sd;
    grib_context* c=a->context;
    grib_handle* h=grib_handle_of_accessor(a);

    if (!a->dirty) return GRIB_SUCCESS;

    if ( (ret=grib_get_size(h,self->values,&size)) != GRIB_SUCCESS) return ret;

    if((ret=grib_get_long(grib_handle_of_accessor(a),self->J,&J))
            != GRIB_SUCCESS) return ret;

    if((ret=grib_get_long(grib_handle_of_accessor(a),self->K,&K))
            != GRIB_SUCCESS) return ret;

    if((ret=grib_get_long(grib_handle_of_accessor(a),self->M,&M))
            != GRIB_SUCCESS) return ret;

    if (J != M || M != K) return GRIB_NOT_IMPLEMENTED;

    N=(M+1)*(M+2)/2;

    if (2*N != size) {
        grib_context_log(a->context,GRIB_LOG_ERROR,
                "wrong number of components for spherical harmonics %ld != %ld",2*N,size);
        return GRIB_WRONG_ARRAY_SIZE;
    }

    values=(double*)grib_context_malloc(c,size*sizeof(double));
    if (!values) return GRIB_OUT_OF_MEMORY;

    if((ret = grib_get_double_array_internal(h,self->values,values,&size))
            != GRIB_SUCCESS) {
        grib_context_free(c,values);
        return ret;
    }

    avg=values[0];
    enorm=0;
    sd=0;

    for (i=2; i < 2*J ; i+=2)
        sd += values[i]*values[i];

    for (i= 2*J; i < size; i+=2)
        sd += 2*values[i]*values[i] + 2*values[i+1]*values[i+1];

    enorm=sd+avg*avg;

    sd=sqrt(sd);
    enorm=sqrt(enorm);

    a->dirty=0;

    grib_context_free(c,values);

    self->v[0]=avg;
    self->v[1]=enorm;
    self->v[2]=sd;
    self->v[3]= sd == 0 ? 1 : 0;

    for (i=0;i<self->number_of_elements;i++)
        val[i]=self->v[i];

    return ret;
}

static int value_count(grib_accessor* a,long* count)
{
    grib_accessor_statistics_spectral* self = (grib_accessor_statistics_spectral*)a;
    *count= self->number_of_elements;
    return 0;
}

static void destroy(grib_context* c,grib_accessor* a)
{
    grib_accessor_statistics_spectral* self = (grib_accessor_statistics_spectral*)a;
    grib_context_free(c,self->v);
}

static int compare(grib_accessor* a, grib_accessor* b)
{
    int retval=GRIB_SUCCESS;
    double *aval=0;
    double *bval=0;

    size_t alen = 0;
    size_t blen = 0;
    int err=0;
    long count=0;

    err=grib_value_count(a,&count);
    if (err) return err;
    alen=count;

    err=grib_value_count(b,&count);
    if (err) return err;
    blen=count;

    if (alen != blen) return GRIB_COUNT_MISMATCH;

    aval=(double*)grib_context_malloc(a->context,alen*sizeof(double));
    bval=(double*)grib_context_malloc(b->context,blen*sizeof(double));

    b->dirty=1;
    a->dirty=1;

    grib_unpack_double(a,aval,&alen);
    grib_unpack_double(b,bval,&blen);

    retval = GRIB_SUCCESS;
    while (alen != 0) {
        if (*bval != *aval) retval = GRIB_DOUBLE_VALUE_MISMATCH;
        alen--;
    }

    grib_context_free(a->context,aval);
    grib_context_free(b->context,bval);

    return retval;
}
