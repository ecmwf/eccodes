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
   SUPER      = grib_accessor_class_gen
   IMPLEMENTS = unpack_long; pack_long
   IMPLEMENTS = init; get_native_type
   MEMBERS=const char*    paramId
   MEMBERS=const char*    type
   END_CLASS_DEF

 */

/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "accessor.class" and rerun ./make_class.pl

*/

static int  get_native_type(grib_accessor*);
static int pack_long(grib_accessor*, const long* val,size_t *len);
static int unpack_long(grib_accessor*, long* val,size_t *len);
static void init(grib_accessor*,const long, grib_arguments* );
static void init_class(grib_accessor_class*);

typedef struct grib_accessor_ifs_param {
    grib_accessor          att;
/* Members defined in gen */
/* Members defined in ifs_param */
	const char*    paramId;
	const char*    type;
} grib_accessor_ifs_param;

extern grib_accessor_class* grib_accessor_class_gen;

static grib_accessor_class _grib_accessor_class_ifs_param = {
    &grib_accessor_class_gen,                      /* super                     */
    "ifs_param",                      /* name                      */
    sizeof(grib_accessor_ifs_param),  /* size                      */
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
    &get_native_type,            /* get native type               */
    0,                /* get sub_section                */
    0,               /* grib_pack procedures long      */
    0,                 /* grib_pack procedures long      */
    &pack_long,                  /* grib_pack procedures long      */
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


grib_accessor_class* grib_accessor_class_ifs_param = &_grib_accessor_class_ifs_param;


static void init_class(grib_accessor_class* c)
{
	c->dump	=	(*(c->super))->dump;
	c->next_offset	=	(*(c->super))->next_offset;
	c->string_length	=	(*(c->super))->string_length;
	c->value_count	=	(*(c->super))->value_count;
	c->byte_count	=	(*(c->super))->byte_count;
	c->byte_offset	=	(*(c->super))->byte_offset;
	c->sub_section	=	(*(c->super))->sub_section;
	c->pack_missing	=	(*(c->super))->pack_missing;
	c->is_missing	=	(*(c->super))->is_missing;
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
  grib_accessor_ifs_param* self = (grib_accessor_ifs_param*)a;
  int n = 0;

  self->paramId = grib_arguments_get_name(grib_handle_of_accessor(a),c,n++);
  self->type = grib_arguments_get_name(grib_handle_of_accessor(a),c,n++);

}

static int    unpack_long   (grib_accessor* a, long* val, size_t *len)
{
  grib_accessor_ifs_param* self = (grib_accessor_ifs_param*)a;
  int ret = 0;
  long paramId = 0;

  if((ret = grib_get_long_internal(grib_handle_of_accessor(a), self->paramId,&paramId)) != GRIB_SUCCESS)
    return ret;

  if (paramId > 129000 && paramId < 129999 ) 
  	*val = paramId - 129000;
  else if (paramId > 200000 && paramId < 200999 ) 
  	*val = paramId - 200000;
  else if (paramId > 211000 && paramId < 211999 )
	  *val = paramId - 1000;
  else
	 *val = paramId;

  return ret;
}


static int pack_long(grib_accessor* a, const long* val, size_t *len)
{
  grib_accessor_ifs_param* self = (grib_accessor_ifs_param*)a;
  long type = 0;
  long table=128;
  long paramId = *val;
  long param;

  grib_get_long(grib_handle_of_accessor(a), self->type,&type);

  if (type==33 || type==35) {
	if (paramId>1000) {
		table=paramId/1000;
		param=paramId-table*1000;
		paramId=param;
	}
	switch (table) {
		case 210 :
			paramId+=211000;
			break;
	  	case 128:
			paramId+=200000;
			break;
	  	default:
			break;
  	}
  }
  if (type == 50 || type == 52 ) {
	if (paramId>1000) {
		table=paramId/1000;
		param=paramId-table*1000;
		paramId=param;
	}
	switch (table) {
		case 128:
			paramId+=129000;
		    break;
	  	default:
			break;
	}
  }

  return grib_set_long_internal(grib_handle_of_accessor(a),self->paramId,paramId);
}

static int  get_native_type(grib_accessor* a){
  return GRIB_TYPE_LONG;
}

