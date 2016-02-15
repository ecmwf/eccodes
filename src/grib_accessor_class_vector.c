/*
 * Copyright 2005-2016 ECMWF.
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
   IMPLEMENTS = init
   IMPLEMENTS = unpack_double
   MEMBERS = const char* vector
   MEMBERS = int index
   END_CLASS_DEF

 */

/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "accessor.class" and rerun ./make_class.pl

*/

static int unpack_double(grib_accessor*, double* val,size_t *len);
static void init(grib_accessor*,const long, grib_arguments* );
static void init_class(grib_accessor_class*);

typedef struct grib_accessor_vector {
    grib_accessor          att;
/* Members defined in gen */
/* Members defined in double */
/* Members defined in abstract_vector */
	double* v;
	int number_of_elements;
/* Members defined in vector */
	const char* vector;
	int index;
} grib_accessor_vector;

extern grib_accessor_class* grib_accessor_class_abstract_vector;

static grib_accessor_class _grib_accessor_class_vector = {
    &grib_accessor_class_abstract_vector,                      /* super                     */
    "vector",                      /* name                      */
    sizeof(grib_accessor_vector),  /* size                      */
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
    0,                    /* compare vs. another accessor   */
    0,     /* unpack only ith value          */
    0,     /* unpack a subarray         */
    0,              		/* clear          */
    0,               		/* clone accessor          */
};


grib_accessor_class* grib_accessor_class_vector = &_grib_accessor_class_vector;


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
	c->compare	=	(*(c->super))->compare;
	c->unpack_double_element	=	(*(c->super))->unpack_double_element;
	c->unpack_double_subarray	=	(*(c->super))->unpack_double_subarray;
	c->clear	=	(*(c->super))->clear;
	c->make_clone	=	(*(c->super))->make_clone;
}

/* END_CLASS_IMP */

typedef struct grib_accessor_abstract_vector {
    grib_accessor          att;
/* Members defined in gen */
/* Members defined in double */
/* Members defined in abstract_vector */
  double* v;
  int number_of_elements;
} grib_accessor_abstract_vector;

static void init(grib_accessor* a,const long l, grib_arguments* c)
{
  grib_accessor_vector* self = (grib_accessor_vector*)a;
  int n = 0;

  self->vector = grib_arguments_get_name(grib_handle_of_accessor(a),c,n++);
  self->index = grib_arguments_get_long(grib_handle_of_accessor(a),c,n++);
  a->flags  |= GRIB_ACCESSOR_FLAG_READ_ONLY;
  a->flags |= GRIB_ACCESSOR_FLAG_FUNCTION;
  a->length=0;
}

static int    unpack_double   (grib_accessor* a, double* val, size_t *len)
{
  size_t size=0;
  double* stat;
  grib_accessor_vector* self = (grib_accessor_vector*)a;
  grib_accessor* va=(grib_accessor*)grib_find_accessor(grib_handle_of_accessor(a),self->vector);
  grib_accessor_abstract_vector* v = (grib_accessor_abstract_vector*)va;

  Assert(self->index>=0);

  if (self->index>=v->number_of_elements) {
    grib_context_log(a->context,GRIB_LOG_FATAL,"index=%d number_of_elements=%d for %s",self->index,v->number_of_elements,a->name);
    Assert(self->index < v->number_of_elements);
  }

  if (va->dirty) {
    grib_get_size(grib_handle_of_accessor(a),self->vector,&size);
    stat=(double*)grib_context_malloc_clear(a->context,sizeof(double)*size);
    grib_unpack_double(va,stat,&size);
    grib_context_free(a->context,stat);
  }

  
  *val = v->v[self->index];
  


  return GRIB_SUCCESS;
}




