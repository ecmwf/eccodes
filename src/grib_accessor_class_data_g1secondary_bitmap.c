/*
 * Copyright 2005-2018 ECMWF.
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
   SUPER      = grib_accessor_class_data_secondary_bitmap
   IMPLEMENTS = init
   IMPLEMENTS = pack_double
   IMPLEMENTS = value_count
   MEMBERS=const char*  number_of_ones
   END_CLASS_DEF
 */

/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "accessor.class" and rerun ./make_class.pl

*/

static int pack_double(grib_accessor*, const double* val,size_t *len);
static int value_count(grib_accessor*,long*);
static void init(grib_accessor*,const long, grib_arguments* );
static void init_class(grib_accessor_class*);

typedef struct grib_accessor_data_g1secondary_bitmap {
    grib_accessor          att;
/* Members defined in gen */
/* Members defined in data_secondary_bitmap */
	const char*  primary_bitmap;
	const char*  secondary_bitmap;
	const char*  missing_value;
	const char*  expand_by;
/* Members defined in data_g1secondary_bitmap */
	const char*  number_of_ones;
} grib_accessor_data_g1secondary_bitmap;

extern grib_accessor_class* grib_accessor_class_data_secondary_bitmap;

static grib_accessor_class _grib_accessor_class_data_g1secondary_bitmap = {
    &grib_accessor_class_data_secondary_bitmap,                      /* super                     */
    "data_g1secondary_bitmap",                      /* name                      */
    sizeof(grib_accessor_data_g1secondary_bitmap),  /* size                      */
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
    0,              /* grib_unpack procedures double  */
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


grib_accessor_class* grib_accessor_class_data_g1secondary_bitmap = &_grib_accessor_class_data_g1secondary_bitmap;


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
	c->unpack_double	=	(*(c->super))->unpack_double;
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
    grib_accessor_data_g1secondary_bitmap *self =(grib_accessor_data_g1secondary_bitmap*)a;
    self->number_of_ones   = grib_arguments_get_name(grib_handle_of_accessor(a),args,4);
}

static int value_count(grib_accessor* a,long* count)
{
    grib_accessor_data_g1secondary_bitmap *self =(grib_accessor_data_g1secondary_bitmap*)a;
    size_t len = 0;
    int err = 0;
    long expand_by;
    *count = 0;

    err = grib_get_long_internal(grib_handle_of_accessor(a),self->expand_by,&expand_by);
    if (err) return err;

    err = grib_get_size(grib_handle_of_accessor(a),self->primary_bitmap,&len);
    if (err) return err;

    *count=expand_by*len;
    return err;
}

static int pack_double(grib_accessor* a, const double* val, size_t *len)
{
    grib_accessor_data_g1secondary_bitmap* self =  (grib_accessor_data_g1secondary_bitmap*)a;

    int err = 0;

    long primary_len = 0;
    long secondary_len = 0;
    double* primary_bitmap = NULL;
    double* secondary_bitmap = NULL;
    long i = 0;
    long j = 0;
    long on = 0;
    long k;
    long m;
    double missing_value = 0;
    double present_value = 0;
    long expand_by =0;

    if (*len ==0) return GRIB_NO_VALUES;

    if((err = grib_get_long(grib_handle_of_accessor(a),self->expand_by,&expand_by)) != GRIB_SUCCESS)
        return err;

    if((err = grib_get_double_internal(grib_handle_of_accessor(a),self->missing_value,&missing_value)) != GRIB_SUCCESS)
        return err;

    Assert(expand_by);

    if(*len % expand_by)
    {
        /*TODO: issue warning */
        return GRIB_ENCODING_ERROR;
    }

    primary_len = *len / expand_by;
    primary_bitmap= (double*)grib_context_malloc_clear(a->context,primary_len*sizeof(double));
    if(!primary_bitmap) return GRIB_OUT_OF_MEMORY;

    secondary_len = *len ;
    secondary_bitmap= (double*)grib_context_malloc_clear(a->context,secondary_len*sizeof(double));
    if(!secondary_bitmap) {
        grib_context_free(a->context,primary_bitmap);
        return GRIB_OUT_OF_MEMORY;
    }

    if(missing_value == 0)
        present_value = 1;
    else
        present_value = 0;

    k = 0;
    m = 0;
    for(i=0; i<*len ; i += expand_by)
    {
        int cnt = 0;
        for(j = 0; j < expand_by; j++)
            if(val[i+j] == missing_value)
                cnt++;

        if(cnt == expand_by) /* all expand_by values are missing */
            primary_bitmap[k++] = missing_value;
        else {
            primary_bitmap[k++] = present_value;
            for(j = 0; j < expand_by; j++)
                secondary_bitmap[m++] = val[i+j];
            on++;
        }
    }

    *len = k;

    /*printf("QQQQQQQ %ld %ld second=%ld\n",primary_len,on,m);*/
    Assert(k == primary_len);

    err = grib_set_double_array_internal(grib_handle_of_accessor(a),self->primary_bitmap,primary_bitmap,k);
    if(err == GRIB_SUCCESS)
        err = grib_set_double_array_internal(grib_handle_of_accessor(a),self->secondary_bitmap,secondary_bitmap,m);

    grib_context_free(a->context,primary_bitmap);
    grib_context_free(a->context,secondary_bitmap);

    if(err == GRIB_SUCCESS)
        err = grib_set_long_internal(grib_handle_of_accessor(a),self->number_of_ones,on);

    return err;
}
