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
   SUPER      = grib_accessor_class_long
   IMPLEMENTS = unpack_long
   IMPLEMENTS = init
   MEMBERS=const char* J
   MEMBERS=const char* K
   MEMBERS=const char* M
   MEMBERS=const char* T
   END_CLASS_DEF

 */

/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "accessor.class" and rerun ./make_class.pl

*/

static int unpack_long(grib_accessor*, long* val,size_t *len);
static void init(grib_accessor*,const long, grib_arguments* );
static void init_class(grib_accessor_class*);

typedef struct grib_accessor_spectral_truncation {
    grib_accessor          att;
/* Members defined in gen */
/* Members defined in long */
/* Members defined in spectral_truncation */
	const char* J;
	const char* K;
	const char* M;
	const char* T;
} grib_accessor_spectral_truncation;

extern grib_accessor_class* grib_accessor_class_long;

static grib_accessor_class _grib_accessor_class_spectral_truncation = {
    &grib_accessor_class_long,                      /* super                     */
    "spectral_truncation",                      /* name                      */
    sizeof(grib_accessor_spectral_truncation),  /* size                      */
    0,                           /* inited */
    &init_class,                 /* init_class */
    &init,                       /* init                      */
    0,                  /* post_init                      */
    0,                    /* free mem                       */
    0,                       /* describes himself         */
    0,                /* get length of section     */
    0,              /* get length of string      */
    0,                /* get number of values      */
    0,                 /* get number of bytes      */
    0,                /* get offset to bytes           */
    0,            /* get native type               */
    0,                /* get sub_section                */
    0,               /* grib_pack procedures long      */
    0,                 /* grib_pack procedures long      */
    0,                  /* grib_pack procedures long      */
    &unpack_long,                /* grib_unpack procedures long    */
    0,                /* grib_pack procedures double    */
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


grib_accessor_class* grib_accessor_class_spectral_truncation = &_grib_accessor_class_spectral_truncation;


static void init_class(grib_accessor_class* c)
{
	c->dump	=	(*(c->super))->dump;
	c->next_offset	=	(*(c->super))->next_offset;
	c->string_length	=	(*(c->super))->string_length;
	c->value_count	=	(*(c->super))->value_count;
	c->byte_count	=	(*(c->super))->byte_count;
	c->byte_offset	=	(*(c->super))->byte_offset;
	c->get_native_type	=	(*(c->super))->get_native_type;
	c->sub_section	=	(*(c->super))->sub_section;
	c->pack_missing	=	(*(c->super))->pack_missing;
	c->is_missing	=	(*(c->super))->is_missing;
	c->pack_long	=	(*(c->super))->pack_long;
	c->pack_double	=	(*(c->super))->pack_double;
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

static void init(grib_accessor* a,const long l, grib_arguments* c)
{
  grib_accessor_spectral_truncation* self = (grib_accessor_spectral_truncation*)a;
  int n = 0;

  self->J = grib_arguments_get_name(grib_handle_of_accessor(a),c,n++);
  self->K    = grib_arguments_get_name(grib_handle_of_accessor(a),c,n++);
  self->M = grib_arguments_get_name(grib_handle_of_accessor(a),c,n++);
  self->T    = grib_arguments_get_name(grib_handle_of_accessor(a),c,n++);

  a->flags |= GRIB_ACCESSOR_FLAG_READ_ONLY;
}

static int    unpack_long   (grib_accessor* a, long* val, size_t *len)
{
  grib_accessor_spectral_truncation* self = (grib_accessor_spectral_truncation*)a;
  int ret = 0;

  long J,K,M,T,Tc;

  if(*len < 1) return GRIB_ARRAY_TOO_SMALL;

  if((ret = grib_get_long_internal(grib_handle_of_accessor(a), self->J,&J))
          != GRIB_SUCCESS)
    return ret;

  if((ret = grib_get_long_internal(grib_handle_of_accessor(a), self->K,&K))
          != GRIB_SUCCESS)
    return ret;

  if((ret = grib_get_long_internal(grib_handle_of_accessor(a), self->M,&M))
          != GRIB_SUCCESS)
    return ret;

  Tc=-1;
  if (J==K && K==M ) {
     /* Triangular truncation */
     Tc=(M+1)*(M+2);
  }
  if (K==J+M ) {
    /* Rhomboidal truncation */
    Tc=2*J*M;
  }
  if (J==K && K > M ) {
    /* Trapezoidal truncation */
    Tc=M*(2*J-M);
  }

  *val=Tc;

  if((ret = grib_get_long_internal(grib_handle_of_accessor(a), self->T,&T))
          != GRIB_SUCCESS) {

    if (Tc == -1)
      grib_context_log(a->context, GRIB_LOG_ERROR,
        "%s. Spectral Truncation Type Unknown: %s=%d %s=%d %s=%d \n",
        a->name, self->J,J, self->K,K, self->M,M);

    Tc=0;
    grib_set_long(grib_handle_of_accessor(a), self->T,Tc);

  } else {

    if (Tc != -1 && Tc != T ) grib_set_long(grib_handle_of_accessor(a), self->T,Tc);

  }

  if (ret == GRIB_SUCCESS) *len = 1;

  return ret;
}

