/*
 * Copyright 2005-2018 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

/***********************************
 *  Enrico Fucile
 **********************************/

#include "grib_api_internal.h"
#define DIRECT 0
#define INVERSE 1


/*
   This is used by make_class.pl

   START_CLASS_DEF
   CLASS      = accessor
   SUPER      = grib_accessor_class_data_g2simple_packing
   IMPLEMENTS = init
   IMPLEMENTS = pack_double
   IMPLEMENTS = unpack_double
   IMPLEMENTS = value_count
   MEMBERS=const char*  pre_processing
   MEMBERS=const char*  pre_processing_parameter
   END_CLASS_DEF

 */

static int pre_processing_func(double* values,long length, long pre_processing,
                               double *pre_processing_parameter,int mode);

/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "accessor.class" and rerun ./make_class.pl

*/

static int pack_double(grib_accessor*, const double* val,size_t *len);
static int unpack_double(grib_accessor*, double* val,size_t *len);
static int value_count(grib_accessor*,long*);
static void init(grib_accessor*,const long, grib_arguments* );
static void init_class(grib_accessor_class*);

typedef struct grib_accessor_data_g2simple_packing_with_preprocessing {
    grib_accessor          att;
/* Members defined in gen */
/* Members defined in values */
	int  carg;
	const char* seclen;
	const char* offsetdata;
	const char* offsetsection;
	int dirty;
/* Members defined in data_simple_packing */
	int edition;
	const char*  units_factor;
	const char*  units_bias;
	const char*  changing_precision;
	const char*  number_of_values;
	const char*  bits_per_value;
	const char*  reference_value;
	const char*  binary_scale_factor;
	const char*  decimal_scale_factor;
	const char*  optimize_scaling_factor;
/* Members defined in data_g2simple_packing */
/* Members defined in data_g2simple_packing_with_preprocessing */
	const char*  pre_processing;
	const char*  pre_processing_parameter;
} grib_accessor_data_g2simple_packing_with_preprocessing;

extern grib_accessor_class* grib_accessor_class_data_g2simple_packing;

static grib_accessor_class _grib_accessor_class_data_g2simple_packing_with_preprocessing = {
    &grib_accessor_class_data_g2simple_packing,                      /* super                     */
    "data_g2simple_packing_with_preprocessing",                      /* name                      */
    sizeof(grib_accessor_data_g2simple_packing_with_preprocessing),  /* size                      */
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


grib_accessor_class* grib_accessor_class_data_g2simple_packing_with_preprocessing = &_grib_accessor_class_data_g2simple_packing_with_preprocessing;


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

static void init(grib_accessor* a,const long v, grib_arguments* args)
{
    grib_accessor_data_g2simple_packing_with_preprocessing *self =(grib_accessor_data_g2simple_packing_with_preprocessing*)a;
    self->pre_processing = grib_arguments_get_name(grib_handle_of_accessor(a),args,self->carg++);
    self->pre_processing_parameter = grib_arguments_get_name(grib_handle_of_accessor(a),args,self->carg++);
    a->flags |= GRIB_ACCESSOR_FLAG_DATA;
}

static int value_count(grib_accessor* a,long* n_vals)
{
    grib_accessor_data_g2simple_packing_with_preprocessing *self =(grib_accessor_data_g2simple_packing_with_preprocessing*)a;
    *n_vals= 0;

    return grib_get_long_internal(grib_handle_of_accessor(a),self->number_of_values,n_vals);
}

static int unpack_double(grib_accessor* a, double* val, size_t *len)
{
    grib_accessor_data_g2simple_packing_with_preprocessing* self =  (grib_accessor_data_g2simple_packing_with_preprocessing*)a;
    grib_accessor_class* super = *(a->cclass->super);

    size_t n_vals = 0;
    long nn=0;
    int err = 0;

    long    pre_processing;
    double    pre_processing_parameter;

    err=grib_value_count(a,&nn);
    n_vals=nn;
    if (err) return err;

    if(n_vals==0){
        *len = 0;
        return GRIB_SUCCESS;
    }

    self->dirty=0;

    if((err = grib_get_long_internal(grib_handle_of_accessor(a),self->pre_processing, &pre_processing)) != GRIB_SUCCESS){
        grib_context_log(a->context, GRIB_LOG_ERROR, "Accessor %s cannont gather value for %s error %d \n", a->name, self->pre_processing, err);
        return err;
    }

    if((err = grib_get_double_internal(grib_handle_of_accessor(a),self->pre_processing_parameter, &pre_processing_parameter)) != GRIB_SUCCESS){
        grib_context_log(a->context, GRIB_LOG_ERROR, "Accessor %s cannont gather value for %s error %d \n", a->name, self->pre_processing_parameter, err);
        return err;
    }

    err =   super->unpack_double(a,val,&n_vals);
    if (err!=GRIB_SUCCESS) return err;

    err=pre_processing_func(val,n_vals,pre_processing,&pre_processing_parameter,INVERSE);
    if (err != GRIB_SUCCESS) return err;

    *len = (long) n_vals;

    return err;
}

static int pack_double(grib_accessor* a, const double* val, size_t *len)
{
    grib_accessor_data_g2simple_packing_with_preprocessing* self =  (grib_accessor_data_g2simple_packing_with_preprocessing*)a;
    grib_accessor_class* super = *(a->cclass->super);

    size_t n_vals = *len;
    int err = 0;

    long pre_processing=0;
    double pre_processing_parameter=0;

    self->dirty=1;

    if((err = grib_get_long_internal(grib_handle_of_accessor(a),self->pre_processing, &pre_processing)) != GRIB_SUCCESS)
        return err;

    err=pre_processing_func((double*)val,n_vals,pre_processing,&pre_processing_parameter,DIRECT);
    if (err != GRIB_SUCCESS) return err;

    err =   super->pack_double(a,val,len);
    if (err!=GRIB_SUCCESS) return err;

    if((err = grib_set_double_internal(grib_handle_of_accessor(a),self->pre_processing_parameter, pre_processing_parameter)) != GRIB_SUCCESS)
        return err;

    if((err = grib_set_long_internal(grib_handle_of_accessor(a),self->number_of_values, n_vals)) != GRIB_SUCCESS)
        return err;

    return GRIB_SUCCESS;
}

static int pre_processing_func(double* values,long length, long pre_processing,
        double *pre_processing_parameter,int mode) {
    int i;
    int ret=0;
    double min=values[0];
    double next_min=values[0];

    switch (pre_processing) {
    /* NONE */
    case 0:
        break;
    /* LOGARITHM */
    case 1:
        if (mode == DIRECT) {
            for (i=0;i<length;i++) {
                if (values[i] < min)
                    min=values[i];
                if (values[i] > next_min)
                    next_min=values[i];
            }
            for (i=0;i<length;i++) {
                if (values[i] > min && values[i] < next_min )
                    next_min=values[i];
            }
            if ( min > 0 ) {
                *pre_processing_parameter=0;
                for (i=0;i<length;i++)
                    values[i]=log(values[i]);
            } else {
                *pre_processing_parameter=next_min-2*min;
                if (next_min == min) return ret;
                for (i=0;i<length;i++)
                    values[i]=log(values[i]+*pre_processing_parameter);
            }
        } else {
            if ( *pre_processing_parameter == 0 ) {
                for (i=0;i<length;i++)
                    values[i]=exp(values[i]);
            } else {
                for (i=0;i<length;i++)
                    values[i]=exp(values[i])-*pre_processing_parameter;
            }
        }
        break;
    default:
        ret=GRIB_NOT_IMPLEMENTED;
        break;
    }

    return ret;
}
