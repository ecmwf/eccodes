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
   IMPLEMENTS = init;unpack_string; get_native_type;string_length
   MEMBERS = const char* N
   MEMBERS = const char* Ni
   MEMBERS = const char* isOctahedral
   END_CLASS_DEF

 */

/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "accessor.class" and rerun ./make_class.pl

*/

static int  get_native_type(grib_accessor*);
static int unpack_string (grib_accessor*, char*, size_t *len);
static size_t string_length(grib_accessor*);
static void init(grib_accessor*,const long, grib_arguments* );
static void init_class(grib_accessor_class*);

typedef struct grib_accessor_gaussian_grid_name {
    grib_accessor          att;
/* Members defined in gen */
/* Members defined in gaussian_grid_name */
	const char* N;
	const char* Ni;
	const char* isOctahedral;
} grib_accessor_gaussian_grid_name;

extern grib_accessor_class* grib_accessor_class_gen;

static grib_accessor_class _grib_accessor_class_gaussian_grid_name = {
    &grib_accessor_class_gen,                      /* super                     */
    "gaussian_grid_name",                      /* name                      */
    sizeof(grib_accessor_gaussian_grid_name),  /* size                      */
    0,                           /* inited */
    &init_class,                 /* init_class */
    &init,                       /* init                      */
    0,                  /* post_init                      */
    0,                    /* free mem                       */
    0,                       /* describes himself         */
    0,                /* get length of section     */
    &string_length,              /* get length of string      */
    0,                /* get number of values      */
    0,                 /* get number of bytes      */
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
    0,                    /* compare vs. another accessor   */
    0,     /* unpack only ith value          */
    0,     /* unpack a subarray         */
    0,              		/* clear          */
    0,               		/* clone accessor          */
};


grib_accessor_class* grib_accessor_class_gaussian_grid_name = &_grib_accessor_class_gaussian_grid_name;


static void init_class(grib_accessor_class* c)
{
	c->dump	=	(*(c->super))->dump;
	c->next_offset	=	(*(c->super))->next_offset;
	c->value_count	=	(*(c->super))->value_count;
	c->byte_count	=	(*(c->super))->byte_count;
	c->byte_offset	=	(*(c->super))->byte_offset;
	c->sub_section	=	(*(c->super))->sub_section;
	c->pack_missing	=	(*(c->super))->pack_missing;
	c->is_missing	=	(*(c->super))->is_missing;
	c->pack_long	=	(*(c->super))->pack_long;
	c->unpack_long	=	(*(c->super))->unpack_long;
	c->pack_double	=	(*(c->super))->pack_double;
	c->unpack_double	=	(*(c->super))->unpack_double;
	c->pack_string	=	(*(c->super))->pack_string;
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

static void init(grib_accessor* a, const long len , grib_arguments* arg )
{
    grib_accessor_gaussian_grid_name* self = (grib_accessor_gaussian_grid_name*)a;
    int n=0;

    self->N = grib_arguments_get_name(a->parent->h,arg,n++);
    self->Ni = grib_arguments_get_name(a->parent->h,arg,n++);
    self->isOctahedral = grib_arguments_get_name(a->parent->h,arg,n++);
    a->length = 0;
    a->flags |= GRIB_ACCESSOR_FLAG_READ_ONLY;
    a->flags |= GRIB_ACCESSOR_FLAG_EDITION_SPECIFIC;
}

static int get_native_type(grib_accessor* a)
{
    return GRIB_TYPE_STRING;
}

#define MAX_GRIDNAME_LEN 16

static int unpack_string(grib_accessor*a , char* v, size_t *len)
{
    grib_accessor_gaussian_grid_name* self = (grib_accessor_gaussian_grid_name*)a;

    long N=0, Ni=0, isOctahedral=0;
    char tmp[MAX_GRIDNAME_LEN] = {0,};
    size_t length = sizeof(tmp);
    int ret = GRIB_SUCCESS;

    if ((ret = grib_get_long_internal(a->parent->h,self->N,&N)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_long_internal(a->parent->h,self->Ni,&Ni))!= GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_long_internal(a->parent->h,self->isOctahedral,&isOctahedral)) != GRIB_SUCCESS)
        return ret;

    if (Ni == GRIB_MISSING_LONG ) {
        /* reduced gaussian grid */
        if (isOctahedral == 1) {
            sprintf(tmp,"O%ld", N);
        } else {
            sprintf(tmp,"N%ld", N);
        }
    } else {
        /* regular gaussian grid */
        sprintf(tmp,"F%ld", N);
    }
    length = strlen(tmp)+1;

    if(*len < length) {
        *len = length;
        return GRIB_BUFFER_TOO_SMALL;
    }

    strcpy(v, tmp);
    *len = length;
    return GRIB_SUCCESS;
}

static size_t string_length(grib_accessor* a)
{
    return MAX_GRIDNAME_LEN;
}
