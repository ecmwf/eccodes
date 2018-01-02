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
#include <math.h>
/*
   This is used by make_class.pl

   START_CLASS_DEF
   CLASS      = accessor
   SUPER      = grib_accessor_class_data_simple_packing
   IMPLEMENTS = unpack_double
   IMPLEMENTS = value_count
   IMPLEMENTS = init
   MEMBERS= const char*  GRIBEX_sh_bug_present
   MEMBERS= const char*  ieee_floats
   MEMBERS= const char*  laplacianOperatorIsSet
   MEMBERS= const char*  laplacianOperator
   MEMBERS= const char*  sub_j
   MEMBERS= const char*  sub_k
   MEMBERS= const char*  sub_m
   MEMBERS= const char*  pen_j
   MEMBERS= const char*  pen_k
   MEMBERS= const char*  pen_m
   END_CLASS_DEF

 */


/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "accessor.class" and rerun ./make_class.pl

*/

static int unpack_double(grib_accessor*, double* val,size_t *len);
static int value_count(grib_accessor*,long*);
static void init(grib_accessor*,const long, grib_arguments* );
static void init_class(grib_accessor_class*);

typedef struct grib_accessor_data_sh_unpacked {
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
/* Members defined in data_sh_unpacked */
	const char*  GRIBEX_sh_bug_present;
	const char*  ieee_floats;
	const char*  laplacianOperatorIsSet;
	const char*  laplacianOperator;
	const char*  sub_j;
	const char*  sub_k;
	const char*  sub_m;
	const char*  pen_j;
	const char*  pen_k;
	const char*  pen_m;
} grib_accessor_data_sh_unpacked;

extern grib_accessor_class* grib_accessor_class_data_simple_packing;

static grib_accessor_class _grib_accessor_class_data_sh_unpacked = {
    &grib_accessor_class_data_simple_packing,                      /* super                     */
    "data_sh_unpacked",                      /* name                      */
    sizeof(grib_accessor_data_sh_unpacked),  /* size                      */
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


grib_accessor_class* grib_accessor_class_data_sh_unpacked = &_grib_accessor_class_data_sh_unpacked;


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

typedef unsigned long (*encode_float_proc)(double);
typedef double        (*decode_float_proc)(unsigned long);

static void init(grib_accessor* a,const long v, grib_arguments* args)
{
  grib_accessor_data_sh_unpacked *self =(grib_accessor_data_sh_unpacked*)a;

  self->GRIBEX_sh_bug_present     = grib_arguments_get_name(grib_handle_of_accessor(a),args,self->carg++);
  self->ieee_floats               = grib_arguments_get_name(grib_handle_of_accessor(a),args,self->carg++);
  self->laplacianOperatorIsSet    = grib_arguments_get_name(grib_handle_of_accessor(a),args,self->carg++);
  self->laplacianOperator         = grib_arguments_get_name(grib_handle_of_accessor(a),args,self->carg++);
  self->sub_j                     = grib_arguments_get_name(grib_handle_of_accessor(a),args,self->carg++);
  self->sub_k                     = grib_arguments_get_name(grib_handle_of_accessor(a),args,self->carg++);
  self->sub_m                     = grib_arguments_get_name(grib_handle_of_accessor(a),args,self->carg++);
  self->pen_j                     = grib_arguments_get_name(grib_handle_of_accessor(a),args,self->carg++);
  self->pen_k                     = grib_arguments_get_name(grib_handle_of_accessor(a),args,self->carg++);
  self->pen_m                     = grib_arguments_get_name(grib_handle_of_accessor(a),args,self->carg++);

  a->flags |= GRIB_ACCESSOR_FLAG_DATA;
  a->length=0;
}


static int value_count(grib_accessor* a,long* count)
{
  grib_accessor_data_sh_unpacked *self =(grib_accessor_data_sh_unpacked*)a;
  int ret = 0;

  long   sub_j= 0;
  long   sub_k= 0;
  long   sub_m= 0;

  if((ret = grib_get_long_internal(grib_handle_of_accessor(a),self->sub_j,&sub_j)) != GRIB_SUCCESS)                         
  	return ret;
  if((ret = grib_get_long_internal(grib_handle_of_accessor(a),self->sub_k,&sub_k)) != GRIB_SUCCESS)                         
  	return ret;
  if((ret = grib_get_long_internal(grib_handle_of_accessor(a),self->sub_m,&sub_m)) != GRIB_SUCCESS)                         
  	return ret;

  if (sub_j != sub_k || sub_j!=sub_m ) {
    grib_context_log(a->context,GRIB_LOG_ERROR,"sub_j=%ld, sub_k=%ld, sub_m=%ld\n",sub_j,sub_k,sub_m);
  	Assert ((sub_j ==  sub_k) && (sub_j == sub_m));
  }
  *count=(sub_j+1)*(sub_j+2);
  return ret;
}


static int  unpack_double(grib_accessor* a, double* val, size_t *len)
{
  grib_accessor_data_sh_unpacked* self =  (grib_accessor_data_sh_unpacked*)a;

  size_t i = 0;
  int ret = GRIB_SUCCESS;
  long   hcount = 0;
  long   lcount = 0;
  long   hpos = 0;
  long   lup = 0;
  long   mmax = 0;
  long   n_vals = 0;
  double *scals  = NULL;
  /* double *pscals=NULL; */
  double dummy=0;

  double s = 0;
  double d = 0;
  double laplacianOperator = 0;
  unsigned char* buf = NULL;
  unsigned char* hres = NULL;
  unsigned char* lres = NULL;
  unsigned long packed_offset;
  long   lpos = 0;

  long   maxv = 0;
  long   GRIBEX_sh_bug_present =0;
  long ieee_floats  = 0;

  long   offsetdata           = 0;
  long   bits_per_value          = 0;
  double reference_value      = 0;
  long   binary_scale_factor         = 0;
  long   decimal_scale_factor = 0;


  long   sub_j= 0;
  long   sub_k= 0;
  long   sub_m= 0;
  long   pen_j= 0;
  long   pen_k= 0;
  long   pen_m= 0;

  double operat= 0;
  int bytes;
  int err=0;

  decode_float_proc decode_float = NULL;

  n_vals = 0;
  err=grib_value_count(a,&n_vals);
  if (err) return err;

  if(*len < n_vals){
    *len = n_vals;
    return GRIB_ARRAY_TOO_SMALL;
  }

  if((ret = grib_get_long_internal(grib_handle_of_accessor(a),self->offsetdata,&offsetdata))
      != GRIB_SUCCESS)   return ret;

  if((ret = grib_get_long_internal(grib_handle_of_accessor(a),self->GRIBEX_sh_bug_present,&GRIBEX_sh_bug_present))
      != GRIB_SUCCESS)
    return ret;

  if((ret = grib_get_long_internal(grib_handle_of_accessor(a),self->ieee_floats,&ieee_floats)) != GRIB_SUCCESS)
    return ret;

  if((ret = grib_get_long_internal(grib_handle_of_accessor(a),self->sub_j,&sub_j)) != GRIB_SUCCESS)
    return ret;
  if((ret = grib_get_long_internal(grib_handle_of_accessor(a),self->sub_k,&sub_k)) != GRIB_SUCCESS)
    return ret;
  if((ret = grib_get_long_internal(grib_handle_of_accessor(a),self->sub_m,&sub_m)) != GRIB_SUCCESS)
    return ret;
  if((ret = grib_get_long_internal(grib_handle_of_accessor(a),self->pen_j,&pen_j)) != GRIB_SUCCESS)
    return ret;
  if((ret = grib_get_long_internal(grib_handle_of_accessor(a),self->pen_k,&pen_k)) != GRIB_SUCCESS)
    return ret;
  if((ret = grib_get_long_internal(grib_handle_of_accessor(a),self->pen_m,&pen_m)) != GRIB_SUCCESS)
    return ret;

  self->dirty=0;

  switch (ieee_floats) {
    case 0:
      decode_float=grib_long_to_ibm;
      bytes=4;
      break;
    case 1:
      decode_float=grib_long_to_ieee;
      bytes=4;
      break;
    case 2:
      decode_float=grib_long_to_ieee64;
      bytes=8;
      break;
    default:
      return GRIB_NOT_IMPLEMENTED;
  }
  
  Assert (sub_j == sub_k);
  Assert (sub_j == sub_m);
  Assert (pen_j == pen_k);
  Assert (pen_j == pen_m);

  buf = (unsigned char*)grib_handle_of_accessor(a)->buffer->data;

  maxv = pen_j+1;

  buf  += offsetdata;
  hres = buf;
  lres = buf;

  packed_offset = offsetdata +  bytes*(sub_k+1)*(sub_k+2);

  lpos = 8*(packed_offset-offsetdata);

  s = grib_power(binary_scale_factor,2);
  d = grib_power(-decimal_scale_factor,10) ;

  scals   = (double*)grib_context_malloc(a->context,maxv*sizeof(double));
  Assert(scals);
  if((ret = grib_get_double_internal(grib_handle_of_accessor(a),self->laplacianOperator,&laplacianOperator))
      != GRIB_SUCCESS)
    return ret;

  scals[0] = 0;
  for(i=1;i<maxv;i++){
    operat = pow(i*(i+1),laplacianOperator);
    if(operat !=  0)
      scals[i] = (1.0/operat);
    else{
      scals[i] = 0;
    }
  }


  i=0;

  while(maxv>0)
  {
    lup=mmax;
    if(sub_k>=0)
    {
      for(hcount=0;hcount<sub_k+1;hcount++)
      {
        val[i++] =  decode_float(grib_decode_unsigned_long(hres,&hpos,8*bytes));
        val[i++] =  decode_float(grib_decode_unsigned_long(hres,&hpos,8*bytes));

        if (GRIBEX_sh_bug_present && hcount==sub_k){
          /*  bug in ecmwf data, last row (K+1)is scaled but should not */
          val[i-2] *= scals[lup];
          val[i-1] *= scals[lup];
        }
        lup++;
      }
      sub_k--;
    }

    /* pscals=scals+lup; */
    for(lcount=hcount; lcount < maxv ; lcount++)
    {
      dummy =  d * (double) ((grib_decode_unsigned_long(lres, &lpos,
                   bits_per_value)*s)+reference_value);
      dummy =  d * (double) ((grib_decode_unsigned_long(lres, &lpos,
                   bits_per_value)*s)+reference_value);
      lup++;
    }

    maxv--;
    hcount=0;
    mmax++;
  }

  Assert(*len >= i);
  *len = n_vals;


  (void)dummy; /* suppress gcc warning */
  grib_context_free(a->context,scals);

  return ret;

}

