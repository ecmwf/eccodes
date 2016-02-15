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
   SUPER      = grib_accessor_class_ascii
   IMPLEMENTS = pack_string;unpack_string
   IMPLEMENTS = init; string_length
   MEMBERS=  const char* paramId
   MEMBERS=  const char* table
   MEMBERS=  const char* param
   END_CLASS_DEF

 */

/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "accessor.class" and rerun ./make_class.pl

*/

static int pack_string(grib_accessor*, const char*, size_t *len);
static int unpack_string (grib_accessor*, char*, size_t *len);
static size_t string_length(grib_accessor*);
static void init(grib_accessor*,const long, grib_arguments* );
static void init_class(grib_accessor_class*);

typedef struct grib_accessor_mars_param {
    grib_accessor          att;
/* Members defined in gen */
/* Members defined in ascii */
/* Members defined in mars_param */
	const char* paramId;
	const char* table;
	const char* param;
} grib_accessor_mars_param;

extern grib_accessor_class* grib_accessor_class_ascii;

static grib_accessor_class _grib_accessor_class_mars_param = {
    &grib_accessor_class_ascii,                      /* super                     */
    "mars_param",                      /* name                      */
    sizeof(grib_accessor_mars_param),  /* size                      */
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
    0,            /* get native type               */
    0,                /* get sub_section                */
    0,               /* grib_pack procedures long      */
    0,                 /* grib_pack procedures long      */
    0,                  /* grib_pack procedures long      */
    0,                /* grib_unpack procedures long    */
    0,                /* grib_pack procedures double    */
    0,              /* grib_unpack procedures double  */
    &pack_string,                /* grib_pack procedures string    */
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


grib_accessor_class* grib_accessor_class_mars_param = &_grib_accessor_class_mars_param;


static void init_class(grib_accessor_class* c)
{
	c->dump	=	(*(c->super))->dump;
	c->next_offset	=	(*(c->super))->next_offset;
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
	c->unpack_double	=	(*(c->super))->unpack_double;
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
	int n=0;
	grib_accessor_mars_param* self = (grib_accessor_mars_param*)a;
	self->paramId= grib_arguments_get_name(grib_handle_of_accessor(a),c,n++);
	self->table= grib_arguments_get_name(grib_handle_of_accessor(a),c,n++);
	self->param= grib_arguments_get_name(grib_handle_of_accessor(a),c,n++);
}

static int pack_string(grib_accessor* a, const char* val, size_t *len){
#if 1
	return GRIB_NOT_IMPLEMENTED;
#else
	grib_accessor_mars_param* self = (grib_accessor_mars_param*)a; 
	long paramId=0;
	long param=0;
	long table=0;
	char* p=(char*)val;
	char* q=NULL;

	param=strtol(val, &p,10);
	if ( *p!=0 ) table=strtol(++p, &q,10);
	else table=128;

	paramId=table*1000+param;

	return grib_set_long_internal(grib_handle_of_accessor(a),self->paramId,paramId);
#endif		
}


static int    unpack_string(grib_accessor* a, char* val, size_t *len)
{   
	grib_accessor_mars_param* self = (grib_accessor_mars_param*)a; 
	long param=0;
	long table=0;
	int ret=0;

#if 1
	if(self->table!=NULL && (ret = grib_get_long_internal(grib_handle_of_accessor(a),self->table,&table)) != GRIB_SUCCESS) return ret;
	if(self->param!=NULL && (ret = grib_get_long_internal(grib_handle_of_accessor(a),self->param,&param)) != GRIB_SUCCESS) return ret;
#else
	{
	long paramId=0;
	grib_get_long(grib_handle_of_accessor(a),self->paramId,&paramId);

	if (paramId==0 || (paramId < 4000 && paramId > 1000 )) {
		if(self->table!=NULL && (ret = grib_get_long_internal(grib_handle_of_accessor(a),self->table,&table))
				  != GRIB_SUCCESS) return ret;
		if(self->param!=NULL && (ret = grib_get_long_internal(grib_handle_of_accessor(a),self->param,&param))
				 != GRIB_SUCCESS) return ret;
	} else if (paramId<1000) {
		table=128;
		param=paramId;
	} else {
    	table=paramId/1000;
		param=paramId-table*1000;
	}
	}
#endif

	/*if (table==200) table=128;*/
	sprintf(val,"%ld.%ld",param,table);
	
	return GRIB_SUCCESS;
}

static size_t string_length(grib_accessor* a)
{
    return 7;
}

