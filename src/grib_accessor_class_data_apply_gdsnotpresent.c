/*
 * Copyright 2005-2018 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */


/****************************************
 *  Enrico Fucile
 ***************************************/

#include "grib_api_internal.h"

/*
   This is used by make_class.pl

   START_CLASS_DEF
   CLASS      = accessor
   SUPER      = grib_accessor_class_gen
   IMPLEMENTS = init
   IMPLEMENTS = unpack_double
   IMPLEMENTS = pack_double
   IMPLEMENTS = value_count
   IMPLEMENTS = dump;get_native_type
   MEMBERS=const char*  coded_values
   MEMBERS=const char*  number_of_values
   MEMBERS=const char*  number_of_points
   MEMBERS=const char*  latitude_of_first_point
   MEMBERS=const char*  ni
   MEMBERS=const char*  missing_value
   MEMBERS=const char*  bitmap_present
   MEMBERS=const char*  bitmap
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
static int unpack_double(grib_accessor*, double* val,size_t *len);
static int value_count(grib_accessor*,long*);
static void dump(grib_accessor*, grib_dumper*);
static void init(grib_accessor*,const long, grib_arguments* );
static void init_class(grib_accessor_class*);

typedef struct grib_accessor_data_apply_gdsnotpresent {
    grib_accessor          att;
/* Members defined in gen */
/* Members defined in data_apply_gdsnotpresent */
	const char*  coded_values;
	const char*  number_of_values;
	const char*  number_of_points;
	const char*  latitude_of_first_point;
	const char*  ni;
	const char*  missing_value;
	const char*  bitmap_present;
	const char*  bitmap;
} grib_accessor_data_apply_gdsnotpresent;

extern grib_accessor_class* grib_accessor_class_gen;

static grib_accessor_class _grib_accessor_class_data_apply_gdsnotpresent = {
    &grib_accessor_class_gen,                      /* super                     */
    "data_apply_gdsnotpresent",                      /* name                      */
    sizeof(grib_accessor_data_apply_gdsnotpresent),  /* size                      */
    0,                           /* inited */
    &init_class,                 /* init_class */
    &init,                       /* init                      */
    0,                  /* post_init                      */
    0,                    /* free mem                       */
    &dump,                       /* describes himself         */
    0,                /* get length of section     */
    0,              /* get length of string      */
    &value_count,                /* get number of values      */
    0,                 /* get number of bytes      */
    0,                /* get offset to bytes           */
    &get_native_type,            /* get native type               */
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


grib_accessor_class* grib_accessor_class_data_apply_gdsnotpresent = &_grib_accessor_class_data_apply_gdsnotpresent;


static void init_class(grib_accessor_class* c)
{
	c->next_offset	=	(*(c->super))->next_offset;
	c->string_length	=	(*(c->super))->string_length;
	c->byte_count	=	(*(c->super))->byte_count;
	c->byte_offset	=	(*(c->super))->byte_offset;
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
    int n=0;
    grib_accessor_data_apply_gdsnotpresent *self =(grib_accessor_data_apply_gdsnotpresent*)a;

    self->coded_values  = grib_arguments_get_name(grib_handle_of_accessor(a),args,n++);
    self->number_of_values        = grib_arguments_get_name(grib_handle_of_accessor(a),args,n++);
    self->number_of_points = grib_arguments_get_name(grib_handle_of_accessor(a),args,n++);
    self->latitude_of_first_point = grib_arguments_get_name(grib_handle_of_accessor(a),args,n++);
    self->ni = grib_arguments_get_name(grib_handle_of_accessor(a),args,n++);
    self->missing_value = grib_arguments_get_name(grib_handle_of_accessor(a),args,n++);
    self->bitmap_present = grib_arguments_get_name(grib_handle_of_accessor(a),args,n++);
    self->bitmap = grib_arguments_get_name(grib_handle_of_accessor(a),args,n++);

    a->length = 0;
}

static void dump(grib_accessor* a, grib_dumper* dumper)
{
    grib_dump_values(dumper,a);
}

static int value_count(grib_accessor* a,long* number_of_points)
{
    int ret=0;

    grib_accessor_data_apply_gdsnotpresent *self =(grib_accessor_data_apply_gdsnotpresent*)a;

    *number_of_points=0;
    if((ret = grib_get_long(grib_handle_of_accessor(a),self->number_of_points,number_of_points))
            !=  GRIB_SUCCESS) {
        grib_context_log(a->context, GRIB_LOG_ERROR,
                "grib_accessor_data_apply_gdsnotpresent: value_count: unable to get number of points");
    }

    return ret;
}

static int unpack_double(grib_accessor* a, double* val, size_t *len)
{
    grib_accessor_data_apply_gdsnotpresent* self =  (grib_accessor_data_apply_gdsnotpresent*)a;

    long number_of_points=0,number_of_values=0,ni=0;
    long latitude_of_first_point=0;
    size_t i = 0;
    size_t  n_vals = 0;
    long nn=0;
    int err=0;
    size_t size=0;
    long missing_value;

    double* coded_vals = NULL;

    err=grib_value_count(a,&nn);
    n_vals=nn;
    if (err) return err;

    if((err = grib_get_long(grib_handle_of_accessor(a),self->number_of_points,&number_of_points))
            !=  GRIB_SUCCESS) return err;

    if((err = grib_get_long(grib_handle_of_accessor(a),self->number_of_values,&number_of_values))
            !=  GRIB_SUCCESS) return err;

    if((err = grib_get_long(grib_handle_of_accessor(a),self->latitude_of_first_point,&latitude_of_first_point))
            !=  GRIB_SUCCESS) return err;

    if((err = grib_get_long(grib_handle_of_accessor(a),self->missing_value,&missing_value))
            !=  GRIB_SUCCESS) return err;

    if((err = grib_get_long(grib_handle_of_accessor(a),self->ni,&ni))
            !=  GRIB_SUCCESS) return err;

    if(*len < number_of_points)
    {
        *len = n_vals;
        return GRIB_ARRAY_TOO_SMALL;
    }

    if(number_of_values > 0){
        coded_vals = (double*)grib_context_malloc(a->context,number_of_values*sizeof(double));

        if(coded_vals == NULL)
            return GRIB_OUT_OF_MEMORY;
    }

    size=number_of_values;
    if((err=grib_get_double_array_internal(grib_handle_of_accessor(a),self->coded_values,coded_vals,&size))
            != GRIB_SUCCESS)  {
        grib_context_free(a->context,coded_vals);
        return err;
    }
    if (number_of_values!=size) {
        grib_context_log(a->context, GRIB_LOG_ERROR,
                "grib_accessor_data_apply_gdsnotpresent : wrong numberOfValues %ld != %ld",
                number_of_values,size);
    }

    grib_context_log(a->context, GRIB_LOG_DEBUG,
            "grib_accessor_data_apply_gdsnotpresent : unpack_double : creating %s, %d values",
            a->name, number_of_points);

    if (latitude_of_first_point == 0) {
        for (i=0;i < number_of_values;i++) val[i]=coded_vals[i];
        for (i=number_of_values;i<number_of_points;i++)
            val[i]=coded_vals[number_of_values-1];
    } else {
        for(i=0;i<ni-1;i++) val[i]=coded_vals[0];
        for(i=ni-1;i<number_of_points;i++) val[i]=coded_vals[i-ni+1];
    }

    *len =  number_of_points;

    grib_context_free(a->context,coded_vals);
    return err;
}

static int pack_double(grib_accessor* a, const double* val, size_t *len)
{
    int ret=GRIB_SUCCESS;
    long bitmap_present=0;

    grib_accessor_data_apply_gdsnotpresent* self =  (grib_accessor_data_apply_gdsnotpresent*)a;

    if (*len ==0) return GRIB_NO_VALUES;

    ret=grib_set_long(grib_handle_of_accessor(a),self->bitmap_present,bitmap_present);
    if(ret) {
        grib_context_log(a->context, GRIB_LOG_ERROR,
                "Accessor %s cannont pack value for %s error %d \n", a->name, self->bitmap_present, ret);
        return ret;
    }

#if 0
    if(!grib_find_accessor(grib_handle_of_accessor(a),self->bitmap)){
        grib_context_log(a->context, GRIB_LOG_ERROR,
                "Accessor %s cannont access bitmap \n", a->name, self->bitmap_present, ret);
        return ret;
    }
#endif

    ret = grib_set_double_array_internal(grib_handle_of_accessor(a),self->coded_values,val,*len);
    if(ret) {
        grib_context_log(a->context, GRIB_LOG_ERROR,
                "Accessor %s cannont pack value for %s error %d \n", a->name, self->coded_values, ret);
        return ret;
    }

    return ret;
}

static int get_native_type(grib_accessor* a)
{
    /*  grib_accessor_data_apply_gdsnotpresent* self =  (grib_accessor_data_apply_gdsnotpresent*)a;
    return grib_accessor_get_native_type(grib_find_accessor(grib_handle_of_accessor(a),self->coded_values));*/

    return GRIB_TYPE_DOUBLE;
}
