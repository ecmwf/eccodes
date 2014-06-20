/**
* Copyright 2005-2014 ECMWF
*
* Licensed under the GNU Lesser General Public License which
* incorporates the terms and conditions of version 3 of the GNU
* General Public License.
* See LICENSE and gpl-3.0.txt for details.
*/

/*****************************************
 *  Enrico Fucile
 ****************************************/

#include "grib_api_internal.h"
#include <ctype.h>

/*
   This is used by make_class.pl

   START_CLASS_DEF
   CLASS      = accessor
   SUPER      = grib_accessor_class_gen
   IMPLEMENTS = init;dump
   IMPLEMENTS = unpack_string;unpack_string_array;unpack_long; unpack_double
   IMPLEMENTS = value_count; destroy; get_native_type;
   MEMBERS    = int type
   MEMBERS    = size_t numberOfValues
   MEMBERS    = double* dvalues
   MEMBERS    = long* lvalues
   MEMBERS    = grib_sarray* svalues
   MEMBERS    = int isConstant

   END_CLASS_DEF

 */

/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "accessor.class" and rerun ./make_class.pl

*/

static int  get_native_type(grib_accessor*);
static int unpack_double(grib_accessor*, double* val,size_t *len);
static int unpack_long(grib_accessor*, long* val,size_t *len);
static int unpack_string (grib_accessor*, char*, size_t *len);
static int unpack_string_array (grib_accessor*, char**, size_t *len);
static int value_count(grib_accessor*,long*);
static void destroy(grib_context*,grib_accessor*);
static void dump(grib_accessor*, grib_dumper*);
static void init(grib_accessor*,const long, grib_arguments* );
static void init_class(grib_accessor_class*);

typedef struct grib_accessor_bufr_data_element {
    grib_accessor          att;
/* Members defined in gen */
/* Members defined in bufr_data_element */
	int type;
	size_t numberOfValues;
	double* dvalues;
	long* lvalues;
	grib_sarray* svalues;
	int isConstant;
} grib_accessor_bufr_data_element;

extern grib_accessor_class* grib_accessor_class_gen;

static grib_accessor_class _grib_accessor_class_bufr_data_element = {
    &grib_accessor_class_gen,                      /* super                     */
    "bufr_data_element",                      /* name                      */
    sizeof(grib_accessor_bufr_data_element),  /* size                      */
    0,                           /* inited */
    &init_class,                 /* init_class */
    &init,                       /* init                      */
    0,                  /* post_init                      */
    &destroy,                    /* free mem                       */
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
    &unpack_long,                /* grib_unpack procedures long    */
    0,                /* grib_pack procedures double    */
    &unpack_double,              /* grib_unpack procedures double  */
    0,                /* grib_pack procedures string    */
    &unpack_string,              /* grib_unpack procedures string  */
    0,          /* grib_pack array procedures string    */
    &unpack_string_array,        /* grib_unpack array procedures string  */
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
    0,             		/* clear          */
};


grib_accessor_class* grib_accessor_class_bufr_data_element = &_grib_accessor_class_bufr_data_element;


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
	c->pack_double	=	(*(c->super))->pack_double;
	c->pack_string	=	(*(c->super))->pack_string;
	c->pack_string_array	=	(*(c->super))->pack_string_array;
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
}

/* END_CLASS_IMP */


static void init(grib_accessor* a, const long len, grib_arguments* params) {

  a->length = 0;
  /* a->flags |= GRIB_ACCESSOR_FLAG_READ_ONLY; */
}

static void dump(grib_accessor* a, grib_dumper* dumper)
{
  int type=get_native_type(a);

  switch (type) {
    case GRIB_TYPE_LONG:
      grib_dump_long(dumper,a,NULL);
      break;
    case GRIB_TYPE_DOUBLE:
      grib_dump_values(dumper,a);
      break;
    case GRIB_TYPE_STRING:
      grib_dump_string_array(dumper,a,NULL);
      break;
  }
}

static int unpack_string_array (grib_accessor* a, char** val, size_t *len)
{
  grib_accessor_bufr_data_element* self = (grib_accessor_bufr_data_element*)a;
  long i=0;
  long size=0;
  int err=0;

  err=value_count(a,&size);
  if (err) return err;

  if (*len < size) return GRIB_ARRAY_TOO_SMALL;

  for (i=0;i<size;i++) val[i]=self->svalues->v[i];
  *len=size;

  return GRIB_SUCCESS;
}

static int unpack_string (grib_accessor* a, char* val, size_t *len)
{
  grib_accessor_bufr_data_element* self = (grib_accessor_bufr_data_element*)a;
  char buffer[100]={0,};

  if (self->type==GRIB_TYPE_STRING) {
    if (*len < strlen(self->svalues->v[0]) )
    	return GRIB_ARRAY_TOO_SMALL;
    sprintf(val,"%s",self->svalues->v[0]);
  } else {
    sprintf(buffer,"%g",self->dvalues[0]);
    if (*len < strlen(buffer) )
    	return GRIB_ARRAY_TOO_SMALL;
    sprintf(val,"%s",buffer);
  }

  return GRIB_SUCCESS;
}

static int unpack_long (grib_accessor* a, long* val, size_t *len)
{
  grib_accessor_bufr_data_element* self = (grib_accessor_bufr_data_element*)a;
  size_t i=0,j=0;
  long size=0;
  int err=0;
  long subsetNumber;
  size_t start,end;
  double missingValue=GRIB_MISSING_DOUBLE;
  int navigate_subgroups;

  if (self->type == GRIB_TYPE_STRING) return GRIB_WRONG_TYPE;

  err=value_count(a,&size);
  if (err) return err;

  if (*len < size) return GRIB_ARRAY_TOO_SMALL;

  navigate_subgroups=a->parent->h->navigate_subgroups;
  a->parent->h->navigate_subgroups=0;
  grib_get_double(a->parent->h,"missingValue",&missingValue);

  if (self->isConstant) {
    subsetNumber=0;
    size=1;
  } else {
    grib_get_long(a->parent->h,"subsetNumber",&subsetNumber);
  }

  if (subsetNumber) {
    start=subsetNumber-1;
    end=subsetNumber-1;
  } else {
    start=0;
    end=size-1;
  }

  j=0;
  for (i=start;i<=end;i++) {
  	if (self->dvalues[i]==GRIB_MISSING_DOUBLE)
      val[j]=missingValue;
    else
      val[j]=self->dvalues[i];

    j++;
  }
  *len=end-start+1;
  a->parent->h->navigate_subgroups=navigate_subgroups;

  return GRIB_SUCCESS;
}

static int unpack_double (grib_accessor* a, double* val, size_t *len)
{
  grib_accessor_bufr_data_element* self = (grib_accessor_bufr_data_element*)a;
  int err=0;
  size_t i=0,j=0;
  long size=0;
  size_t start,end;
  double missingValue=GRIB_MISSING_DOUBLE;
  long subsetNumber=0;
  int navigate_subgroups;

  if (self->type == GRIB_TYPE_STRING) return GRIB_WRONG_TYPE;

  err=value_count(a,&size);
  if (err) return err;

  if (*len < size) return GRIB_ARRAY_TOO_SMALL;
  navigate_subgroups=a->parent->h->navigate_subgroups;
  a->parent->h->navigate_subgroups=0;
  grib_get_double(a->parent->h,"missingValue",&missingValue);

  if (self->isConstant) {
    subsetNumber=0;
    size=1;
  } else {
    grib_get_long(a->parent->h,"subsetNumber",&subsetNumber);
  }

  if (subsetNumber) {
    start=subsetNumber-1;
    end=subsetNumber-1;
  } else {
    start=0;
    end=size-1;
  }

  j=0;
  for (i=start;i<=end;i++) {
    if (self->dvalues[i]==GRIB_MISSING_DOUBLE)
      val[j]=missingValue;
    else
      val[j]=self->dvalues[i];

    j++;
  }
  *len=end-start+1;
  a->parent->h->navigate_subgroups=navigate_subgroups;

  return GRIB_SUCCESS;
}

static int value_count(grib_accessor* a,long* count)
{
  grib_accessor_bufr_data_element* self = (grib_accessor_bufr_data_element*)a;
  if (self->isConstant) *count=1;
  else *count=self->numberOfValues;

  return 0;
}

static void destroy(grib_context* context,grib_accessor* a)
{
  grib_accessor_bufr_data_element* self = (grib_accessor_bufr_data_element*)a;
  if (self->dvalues) grib_context_free(a->parent->h->context,self->dvalues);
  if (self->svalues) grib_sarray_delete(a->parent->h->context,self->svalues);
}

static int  get_native_type(grib_accessor* a){
  grib_accessor_bufr_data_element* self = (grib_accessor_bufr_data_element*)a;
  return self->type;
}


