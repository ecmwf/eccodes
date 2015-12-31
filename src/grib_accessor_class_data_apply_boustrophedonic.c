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
   IMPLEMENTS = init
   IMPLEMENTS = unpack_double;unpack_double_element
   IMPLEMENTS = pack_double
   IMPLEMENTS = value_count
   IMPLEMENTS = dump;get_native_type
   MEMBERS=const char*  values
   MEMBERS=const char*  numberOfRows 
   MEMBERS=const char*  numberOfColumns
   MEMBERS=const char*  numberOfPoints
   MEMBERS=const char*  pl
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
static int unpack_double_element(grib_accessor*,size_t i, double* val);

typedef struct grib_accessor_data_apply_boustrophedonic {
    grib_accessor          att;
/* Members defined in gen */
/* Members defined in data_apply_boustrophedonic */
	const char*  values;
	const char*  numberOfRows;
	const char*  numberOfColumns;
	const char*  numberOfPoints;
	const char*  pl;
} grib_accessor_data_apply_boustrophedonic;

extern grib_accessor_class* grib_accessor_class_gen;

static grib_accessor_class _grib_accessor_class_data_apply_boustrophedonic = {
    &grib_accessor_class_gen,                      /* super                     */
    "data_apply_boustrophedonic",                      /* name                      */
    sizeof(grib_accessor_data_apply_boustrophedonic),  /* size                      */
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
    &unpack_double_element,     /* unpack only ith value          */
    0,     /* unpack a subarray         */
    0,              		/* clear          */
    0,               		/* clone accessor          */
};


grib_accessor_class* grib_accessor_class_data_apply_boustrophedonic = &_grib_accessor_class_data_apply_boustrophedonic;


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
	c->unpack_double_subarray	=	(*(c->super))->unpack_double_subarray;
	c->clear	=	(*(c->super))->clear;
	c->make_clone	=	(*(c->super))->make_clone;
}

/* END_CLASS_IMP */

static void init(grib_accessor* a,const long v, grib_arguments* args)
{
    int n=0;
    grib_accessor_data_apply_boustrophedonic *self =(grib_accessor_data_apply_boustrophedonic*)a;

    self->values  = grib_arguments_get_name(grib_handle_of_accessor(a),args,n++);
    self->numberOfRows = grib_arguments_get_name(grib_handle_of_accessor(a),args,n++);
    self->numberOfColumns = grib_arguments_get_name(grib_handle_of_accessor(a),args,n++);
    self->numberOfPoints = grib_arguments_get_name(grib_handle_of_accessor(a),args,n++);
    self->pl        = grib_arguments_get_name(grib_handle_of_accessor(a),args,n++);

    a->length = 0;
}
static void dump(grib_accessor* a, grib_dumper* dumper)
{
    grib_dump_values(dumper,a);
}

static int value_count(grib_accessor* a, long* numberOfPoints)
{
    grib_accessor_data_apply_boustrophedonic *self =(grib_accessor_data_apply_boustrophedonic*)a;
    int ret=0;

    *numberOfPoints=0;
    ret=grib_get_long_internal(grib_handle_of_accessor(a),self->numberOfPoints,numberOfPoints);

    return ret;
}

static int unpack_double(grib_accessor* a, double* val, size_t *len)
{
    grib_accessor_data_apply_boustrophedonic* self =  (grib_accessor_data_apply_boustrophedonic*)a;
    size_t plSize=0;
    long *pl=0;
    double *values=0;
    double *pvalues=0;
    double *pval=0;
    size_t valuesSize=0;
    long i,j;
    int ret;
    long numberOfPoints,numberOfRows,numberOfColumns;

    ret=grib_get_long_internal(grib_handle_of_accessor(a),self->numberOfPoints,&numberOfPoints);
    if (ret) return ret;

    if(*len < numberOfPoints) {
        *len = numberOfPoints;
        return GRIB_ARRAY_TOO_SMALL;
    }

    ret=grib_get_size(grib_handle_of_accessor(a),self->values,&valuesSize);
    if (ret) return ret;

    /* constant field */
    if (valuesSize==0) return 0;

    if (valuesSize!=numberOfPoints) {
        grib_context_log(a->context,GRIB_LOG_ERROR,"boustrophedonic ordering error: ( %s=%ld ) != (sizeOf(%s)=%ld)",
                self->numberOfPoints,numberOfPoints,self->values,(long)valuesSize);
        return GRIB_DECODING_ERROR;
    }

    values=(double*)grib_context_malloc_clear(a->context,sizeof(double)*numberOfPoints);
    ret=grib_get_double_array_internal(grib_handle_of_accessor(a),self->values,values,&valuesSize);
    if (ret) return ret;

    pvalues=values;
    pval=val;

    ret=grib_get_long_internal(grib_handle_of_accessor(a),self->numberOfRows,&numberOfRows);
    if (ret) return ret;

    ret=grib_get_long_internal(grib_handle_of_accessor(a),self->numberOfColumns,&numberOfColumns);
    if (ret) return ret;

    if (grib_get_size(grib_handle_of_accessor(a),self->pl,&plSize) == GRIB_SUCCESS) {
        Assert(plSize==numberOfRows);
        pl=(long*)grib_context_malloc_clear(a->context,sizeof(long)*plSize);
        ret=grib_get_long_array_internal(grib_handle_of_accessor(a),self->pl,pl,&plSize);
        if (ret) return ret;

        for (j=0;j<numberOfRows;j++) {
            if (j%2) {
                pval+=pl[j];
                for (i=0;i<pl[j];i++) *(pval--)=*(pvalues++);
                pval+=pl[j];
            } else {
                for (i=0;i<pl[j];i++) *(pval++)=*(pvalues++);
            }
        }

        grib_context_free(a->context,pl);

    } else {

        for (j=0;j<numberOfRows;j++) {
            if (j%2) {
                pval+=numberOfColumns-1;
                for (i=0;i<numberOfColumns;i++) *(pval--)=*(pvalues++);
                pval+=numberOfColumns+1;
            } else {
                for (i=0;i<numberOfColumns;i++) *(pval++)=*(pvalues++);
            }
        }

    }

    grib_context_free(a->context,values);

    return GRIB_SUCCESS;
}

static int unpack_double_element(grib_accessor* a, size_t idx,double* val)
{
    size_t size;
    double* values;
    int err = 0;
    
    /* GRIB-564: The index idx relates to codedValues NOT values! */

    err=grib_get_size(a->parent->h,"codedValues",&size);
    if (err) return err;
    if (idx > size) return GRIB_INVALID_NEAREST;

    values=(double*)grib_context_malloc_clear(a->parent->h->context,size*sizeof(double));
    err=grib_get_double_array(a->parent->h,"codedValues",values,&size);
    if (err) return err;
    *val=values[idx];
    grib_context_free(a->parent->h->context,values);
    return err;
}

static int pack_double(grib_accessor* a, const double* val, size_t *len)
{
    grib_accessor_data_apply_boustrophedonic* self =  (grib_accessor_data_apply_boustrophedonic*)a;
    size_t plSize=0;
    long *pl=0;
    double *values=0;
    double *pvalues=0;
    double *pval=0;
    size_t valuesSize=0;
    long i,j;
    int ret;
    long numberOfPoints,numberOfRows,numberOfColumns;

    ret=grib_get_long_internal(grib_handle_of_accessor(a),self->numberOfPoints,&numberOfPoints);
    if (ret) return ret;

    if(*len < numberOfPoints) {
        *len = numberOfPoints;
        return GRIB_ARRAY_TOO_SMALL;
    }

    valuesSize=numberOfPoints;

    values=(double*)grib_context_malloc_clear(a->context,sizeof(double)*numberOfPoints);

    pvalues=values;
    pval=(double*)val;

    ret=grib_get_long_internal(grib_handle_of_accessor(a),self->numberOfRows,&numberOfRows);
    if (ret) return ret;

    ret=grib_get_long_internal(grib_handle_of_accessor(a),self->numberOfColumns,&numberOfColumns);
    if (ret) return ret;

    if (grib_get_size(grib_handle_of_accessor(a),self->pl,&plSize) == GRIB_SUCCESS) {
        Assert(plSize==numberOfRows);
        pl=(long*)grib_context_malloc_clear(a->context,sizeof(long)*plSize);
        ret=grib_get_long_array_internal(grib_handle_of_accessor(a),self->pl,pl,&plSize);
        if (ret) return ret;

        for (j=0;j<numberOfRows;j++) {
            if (j%2) {
                pvalues+=pl[j];
                for (i=0;i<pl[j] ;i++) { *(--pvalues)=*(pval++); }
                pvalues+=pl[j];
            } else {
                for (i=0;i<pl[j];i++) *(pvalues++)=*(pval++);
            }
        }

        grib_context_free(a->context,pl);

    } else {

        for (j=0;j<numberOfRows;j++) {
            if (j%2) {
                pvalues+=numberOfColumns;
                for (i=0;i<numberOfColumns;i++) *(--pvalues)=*(pval++);
                pvalues+=numberOfColumns;
            } else {
                for (i=0;i<numberOfColumns;i++) *(pvalues++)=*(pval++);
            }
        }

    }
    ret=grib_set_double_array_internal(grib_handle_of_accessor(a),self->values,values,valuesSize);
    if (ret) return ret;

    grib_context_free(a->context,values);

    return ret;
}

static int get_native_type(grib_accessor* a)
{
    return GRIB_TYPE_DOUBLE;
}
