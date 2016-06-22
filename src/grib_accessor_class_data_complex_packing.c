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
#include <math.h>
/*
   This is used by make_class.pl

   START_CLASS_DEF
   CLASS      = accessor
   SUPER      = grib_accessor_class_data_simple_packing
   IMPLEMENTS = unpack_double
   IMPLEMENTS = pack_double
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

static int pack_double(grib_accessor*, const double* val,size_t *len);
static int unpack_double(grib_accessor*, double* val,size_t *len);
static int value_count(grib_accessor*,long*);
static void init(grib_accessor*,const long, grib_arguments* );
static void init_class(grib_accessor_class*);

typedef struct grib_accessor_data_complex_packing {
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
/* Members defined in data_complex_packing */
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
} grib_accessor_data_complex_packing;

extern grib_accessor_class* grib_accessor_class_data_simple_packing;

static grib_accessor_class _grib_accessor_class_data_complex_packing = {
    &grib_accessor_class_data_simple_packing,                      /* super                     */
    "data_complex_packing",                      /* name                      */
    sizeof(grib_accessor_data_complex_packing),  /* size                      */
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


grib_accessor_class* grib_accessor_class_data_complex_packing = &_grib_accessor_class_data_complex_packing;


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
    grib_accessor_data_complex_packing *self =(grib_accessor_data_complex_packing*)a;
    grib_handle* gh = grib_handle_of_accessor(a);

    self->GRIBEX_sh_bug_present     = grib_arguments_get_name(gh,args,self->carg++);
    self->ieee_floats               = grib_arguments_get_name(gh,args,self->carg++);
    self->laplacianOperatorIsSet    = grib_arguments_get_name(gh,args,self->carg++);
    self->laplacianOperator         = grib_arguments_get_name(gh,args,self->carg++);
    self->sub_j                     = grib_arguments_get_name(gh,args,self->carg++);
    self->sub_k                     = grib_arguments_get_name(gh,args,self->carg++);
    self->sub_m                     = grib_arguments_get_name(gh,args,self->carg++);
    self->pen_j                     = grib_arguments_get_name(gh,args,self->carg++);
    self->pen_k                     = grib_arguments_get_name(gh,args,self->carg++);
    self->pen_m                     = grib_arguments_get_name(gh,args,self->carg++);

    a->flags |= GRIB_ACCESSOR_FLAG_DATA;
}

static int value_count(grib_accessor* a,long* count)
{
    grib_accessor_data_complex_packing *self =(grib_accessor_data_complex_packing*)a;
    int ret = 0;
    grib_handle* gh = grib_handle_of_accessor(a);

    long   pen_j= 0;
    long   pen_k= 0;
    long   pen_m= 0;

    *count=0;

    if(a->length == 0) return 0;

    if((ret = grib_get_long_internal(gh,self->pen_j,&pen_j)) != GRIB_SUCCESS)
        return ret;
    if((ret = grib_get_long_internal(gh,self->pen_k,&pen_k)) != GRIB_SUCCESS)
        return ret;
    if((ret = grib_get_long_internal(gh,self->pen_m,&pen_m)) != GRIB_SUCCESS)
        return ret;

    if (pen_j != pen_k || pen_j!=pen_m ) {
        grib_context_log(a->context,GRIB_LOG_ERROR,"pen_j=%ld, pen_k=%ld, pen_m=%ld\n",pen_j,pen_k,pen_m);
        Assert ((pen_j ==  pen_k) && (pen_j == pen_m));
    }
    *count=(pen_j+1)*(pen_j+2);

    return ret;
}


static int  unpack_double(grib_accessor* a, double* val, size_t *len)
{
    grib_accessor_data_complex_packing* self =  (grib_accessor_data_complex_packing*)a;
    grib_handle* gh = grib_handle_of_accessor(a);

    size_t i = 0;
    int ret = GRIB_SUCCESS;
    long   hcount = 0;
    long   lcount = 0;
    long   hpos = 0;
    long   lup = 0;
    long   mmax = 0;
    long   n_vals = 0;
    double *scals  = NULL;
    double *pscals=NULL,*pval=NULL;

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

    err=grib_value_count(a,&n_vals);
    if (err) return err;

    if(*len < n_vals){
        *len = n_vals;
        return GRIB_ARRAY_TOO_SMALL;
    }

    if((ret = grib_get_long_internal(gh,self->offsetdata,&offsetdata))
            != GRIB_SUCCESS)   return ret;
    if((ret = grib_get_long_internal(gh,self->bits_per_value,&bits_per_value))
            != GRIB_SUCCESS)   return ret;
    if((ret = grib_get_double_internal(gh,self->reference_value,&reference_value))
            != GRIB_SUCCESS)   return ret;
    if((ret = grib_get_long_internal(gh,self->binary_scale_factor,&binary_scale_factor))
            != GRIB_SUCCESS)           return ret;

    if((ret = grib_get_long_internal(gh,self->decimal_scale_factor,&decimal_scale_factor))
            != GRIB_SUCCESS)
        return ret;

    if((ret = grib_get_long_internal(gh,self->GRIBEX_sh_bug_present,&GRIBEX_sh_bug_present))
            != GRIB_SUCCESS)
        return ret;

    if((ret = grib_get_long_internal(gh,self->ieee_floats,&ieee_floats)) != GRIB_SUCCESS)
        return ret;

    if((ret = grib_get_double_internal(gh,self->laplacianOperator,&laplacianOperator))
            != GRIB_SUCCESS)
        return ret;
    if((ret = grib_get_long_internal(gh,self->sub_j,&sub_j)) != GRIB_SUCCESS)
        return ret;
    if((ret = grib_get_long_internal(gh,self->sub_k,&sub_k)) != GRIB_SUCCESS)
        return ret;
    if((ret = grib_get_long_internal(gh,self->sub_m,&sub_m)) != GRIB_SUCCESS)
        return ret;
    if((ret = grib_get_long_internal(gh,self->pen_j,&pen_j)) != GRIB_SUCCESS)
        return ret;
    if((ret = grib_get_long_internal(gh,self->pen_k,&pen_k)) != GRIB_SUCCESS)
        return ret;
    if((ret = grib_get_long_internal(gh,self->pen_m,&pen_m)) != GRIB_SUCCESS)
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

    buf = (unsigned char*)gh->buffer->data;

    maxv = pen_j+1;

    buf  += grib_byte_offset(a);
    hres = buf;
    lres = buf;

    if (pen_j == sub_j) {
        n_vals = (pen_j+1)*(pen_j+2);
        d = grib_power(-decimal_scale_factor,10) ;
        grib_ieee_decode_array(a->context,buf,n_vals,bytes,val);
        if (d) {
            for (i=0;i<n_vals;i++) val[i]*=d;
        }
        return 0;
    }

    packed_offset = grib_byte_offset(a) +  4*(sub_k+1)*(sub_k+2);

    lpos = 8*(packed_offset-offsetdata);

    s = grib_power(binary_scale_factor,2);
    d = grib_power(-decimal_scale_factor,10) ;

    scals   = (double*)grib_context_malloc(a->context,maxv*sizeof(double));
    Assert(scals);

    scals[0] = 0;
    for(i=1;i<maxv;i++){
        operat = pow(i*(i+1),laplacianOperator);
        if(operat !=  0)
            scals[i] = (1.0/operat);
        else{
            grib_context_log(a->context,GRIB_LOG_WARNING,
                    "COMPLEX_PACKING : problem with operator div by zero at index %d of %d \n",
                    i , maxv);
            scals[i] = 0;
        }
    }

    /*
  printf("UNPACKING LAPLACE=%.20f\n",laplacianOperator);

  printf("packed offset=%ld\n",packed_offset);
  for(i=0;i<maxv;i++)
    printf("scals[%d]=%g\n",i,scals[i]);*/

    i=0;

    while(maxv>0)
    {
        lup=mmax;
        if(sub_k>=0)
        {
            for(hcount=0;hcount<sub_k+1;hcount++)
            {
                val[i++] =  decode_float(grib_decode_unsigned_long(hres,&hpos,32))*d;
                val[i++] =  decode_float(grib_decode_unsigned_long(hres,&hpos,32))*d;

                if (GRIBEX_sh_bug_present && hcount==sub_k){
                    /*  bug in ecmwf data, last row (K+1)is scaled but should not */
                    val[i-2] *= scals[lup];
                    val[i-1] *= scals[lup];
                }
                lup++;
            }
            sub_k--;
        }

        pscals=scals+lup;
        pval=val+i;
#if FAST_BIG_ENDIAN
        grib_decode_double_array_complex(lres,
                &lpos,bits_per_value,
                reference_value,s,pscals,(maxv-hcount)*2,pval);
        i+=(maxv-hcount)*2;
#else
        (void)pscals; /* suppress gcc warning */
        (void)pval;   /* suppress gcc warning */
        for(lcount=hcount; lcount < maxv ; lcount++)
        {
            val[i++] =  (double) ((grib_decode_unsigned_long(lres, &lpos,
                    bits_per_value)*s)+reference_value)*scals[lup];
            val[i++] =  (double) ((grib_decode_unsigned_long(lres, &lpos,
                    bits_per_value)*s)+reference_value)*scals[lup];
            lup++;
        }
#endif

        maxv--;
        hcount=0;
        mmax++;
    }

    Assert(*len >= i);
    *len = i;

    if(d != 1) {
        for(i=0;i<*len;i++)
            val[i++] *= d;
    }

    grib_context_free(a->context,scals);

    return ret;
}


#define MAXVAL(a,b) a>b?a:b

static double calculate_pfactor(grib_context *ctx,const double* spectralField, long fieldTruncation, long subsetTruncation)
{
    /*long n_vals = ((fieldTruncation+1)*(fieldTruncation+2));*/
    long loop, index, m, n = 0;
    double pFactor, zeps = 1.0e-15;
    long ismin = (subsetTruncation+1), ismax = (fieldTruncation+1);
    double* weights, range, * norms;
    double weightedSumOverX = 0.0, weightedSumOverY = 0.0, sumOfWeights = 0.0, x, y;
    double numerator = 0.0, denominator = 0.0, slope;

    /* Catch corner case. See GRIB-172 */
    if (ismax-ismin <= 1) {
        return 1; /* any value will do! we cannot do linear fit on a single point! */
    }

    /*
     * Setup the weights
     */
    range = (double) (ismax - ismin +1);
    weights = (double*) grib_context_malloc(ctx,(ismax+1)*sizeof(double));
    for( loop = ismin; loop <= ismax; loop++ )
        weights[loop] = range / (double) (loop-ismin+1);

    /*
     * Compute norms
     * Handle values 2 at a time (real and imaginary parts).
     */
    norms = (double*) grib_context_malloc(ctx,(ismax+1)*sizeof(double));
    for( loop = 0; loop < ismax+1; loop++ ) norms[loop] = 0.0;

    /*
     * Form norms for the rows which contain part of the unscaled subset.
     */

    index = -2;
    for( m = 0; m < subsetTruncation; m++ ) {
        for( n = m; n <= fieldTruncation; n++ ) {
            index += 2;
            if( n >= subsetTruncation ) {
                norms[n] = MAXVAL(norms[n],fabs(spectralField[index]));
                norms[n] = MAXVAL(norms[n],fabs(spectralField[index+1]));
            }
        }
    }

    /*
     * Form norms for the rows which do not contain part of the unscaled subset.
     */

    for( m = subsetTruncation; m <= fieldTruncation; m++ ) {
        for( n = m; n <= fieldTruncation; n++ ) {
            index += 2;
            norms[n] = MAXVAL(norms[n],fabs(spectralField[index]));
            norms[n] = MAXVAL(norms[n],fabs(spectralField[index+1]));
        }
    }

    /*
     * Ensure the norms have a value which is not too small in case of
     * problems with math functions (e.g. LOG).
     */
    for( loop = ismin; loop <= ismax; loop++ ) {
        norms[loop] = MAXVAL(norms[loop],zeps);
        if( norms[loop] == zeps ) weights[loop] = 100.0 * zeps;
    }

    /*
     * Do linear fit to find the slope
     */
    for( loop = ismin; loop <= ismax; loop++ ) {
        x = log( (double) (loop*(loop+1)) );
        Assert( norms[loop] > 0 );
        y = log( norms[loop] );
        weightedSumOverX = weightedSumOverX + x * weights[loop];
        weightedSumOverY = weightedSumOverY + y * weights[loop];
        sumOfWeights = sumOfWeights + weights[loop];
    }
    weightedSumOverX = weightedSumOverX / sumOfWeights;
    weightedSumOverY = weightedSumOverY / sumOfWeights;

    /*
     * Perform a least square fit for the equation
     */

    for( loop = ismin; loop <= ismax; loop++ ) {
        x = log( (double)(loop*(loop+1)) );
        y = log( norms[loop] );
        numerator =
                numerator + weights[loop] * (y-weightedSumOverY) * (x-weightedSumOverX);
        denominator =
                denominator + weights[loop] * ((x-weightedSumOverX) * (x-weightedSumOverX));
    }
    slope = numerator / denominator;

    grib_context_free(ctx,weights);
    grib_context_free(ctx,norms);

    pFactor = -slope;
    if( pFactor < -9999.9 ) pFactor = -9999.9;
    if( pFactor > 9999.9 )  pFactor = 9999.9;
    return pFactor;
}


static int pack_double(grib_accessor* a, const double* val, size_t *len) {

    grib_accessor_data_complex_packing* self =  (grib_accessor_data_complex_packing*)a;
    grib_handle* gh = grib_handle_of_accessor(a);

    size_t i = 0;
    int ret = GRIB_SUCCESS;
    long   hcount = 0;
    long   lcount = 0;
    long   hpos = 0;
    long   lup = 0;
    long   mmax = 0;
    long   n_vals = 0;
    double *scals  = NULL;

    double s = 0;
    double d = 0;

    unsigned char* buf    = NULL;

    size_t         buflen = 0;
    size_t         hsize = 0;
    size_t         lsize = 0;

    unsigned char* hres = NULL;
    unsigned char* lres = NULL;

    long   lpos = 0;
    long   maxv = 0;

    long   offsetdata           = 0;
    long   bits_per_value          = 0;
    double reference_value      = 0;
    long   binary_scale_factor         = 0;
    long   decimal_scale_factor = 0;
    long   laplacianOperatorIsSet = 0;

    double laplacianOperator = 0;
    long   sub_j= 0;
    long   sub_k= 0;
    long   sub_m= 0;
    long   pen_j= 0;
    long   pen_k= 0;
    long   pen_m= 0;
    long   GRIBEX_sh_bug_present =0;
    long   ieee_floats =0;
    double min = 0;
    double max = 0;
    double current_val = 0;
    short mixmax_unset = 0;
    int bytes;

    encode_float_proc encode_float = NULL;

    if (*len ==0) return GRIB_NO_VALUES;

    if((ret = grib_get_long_internal(gh,self->offsetdata,&offsetdata)) != GRIB_SUCCESS)
        return ret;
    if((ret = grib_get_long_internal(gh,self->bits_per_value,&bits_per_value)) != GRIB_SUCCESS)
        return ret;

    if((ret = grib_get_long_internal(gh,self->decimal_scale_factor,&decimal_scale_factor)) != GRIB_SUCCESS)
        return ret;

    if((ret = grib_get_long_internal(gh,self->GRIBEX_sh_bug_present,&GRIBEX_sh_bug_present)) != GRIB_SUCCESS)
        return ret;

    if((ret = grib_get_long_internal(gh,self->ieee_floats,&ieee_floats)) != GRIB_SUCCESS)
        return ret;

    if((ret = grib_get_long_internal(gh,self->laplacianOperatorIsSet,&laplacianOperatorIsSet)) != GRIB_SUCCESS)
        return ret;
    if((ret = grib_get_double_internal(gh,self->laplacianOperator,&laplacianOperator)) != GRIB_SUCCESS)
        return ret;

    if((ret = grib_get_long_internal(gh,self->sub_j,&sub_j)) != GRIB_SUCCESS)
        return ret;
    if((ret = grib_get_long_internal(gh,self->sub_k,&sub_k)) != GRIB_SUCCESS)
        return ret;
    if((ret = grib_get_long_internal(gh,self->sub_m,&sub_m)) != GRIB_SUCCESS)
        return ret;
    if((ret = grib_get_long_internal(gh,self->pen_j,&pen_j)) != GRIB_SUCCESS)
        return ret;
    if((ret = grib_get_long_internal(gh,self->pen_k,&pen_k)) != GRIB_SUCCESS)
        return ret;
    if((ret = grib_get_long_internal(gh,self->pen_m,&pen_m)) != GRIB_SUCCESS)
        return ret;

    self->dirty=1;


    switch (ieee_floats) {
    case 0:
        encode_float =grib_ibm_to_long;
        bytes=4;
        break;
    case 1:
        encode_float =grib_ieee_to_long;
        bytes=4;
        break;
    case 2:
        encode_float =grib_ieee64_to_long;
        bytes=8;
        break;
    default:
        return GRIB_NOT_IMPLEMENTED;
    }

    Assert (sub_j == sub_k); Assert( sub_j == sub_m);
    Assert (pen_j == pen_k); Assert( pen_j == pen_m);

    n_vals = (pen_j+1)*(pen_j+2);
    d = grib_power(decimal_scale_factor,10) ;

    if(*len != n_vals){
        grib_context_log(a->context,GRIB_LOG_ERROR,"COMPLEX_PACKING : wrong number of values, expected %d - got %d",n_vals,*len);
        return GRIB_INTERNAL_ERROR;
    }

    if (pen_j == sub_j) {
        double* values;
        if (d) {
            values=(double*)grib_context_malloc_clear(a->context,sizeof(double)*n_vals);
            for (i=0;i<n_vals;i++) values[i]=val[i]*d;
        } else {
            values=(double*)val;
        }
        buflen=n_vals*bytes;
        buf = (unsigned char*)grib_context_malloc_clear(a->context,buflen);
        grib_ieee_encode_array(a->context,values,n_vals,bytes,buf);
        if (d) grib_context_free(a->context,values);
        grib_buffer_replace(a, buf, buflen,1,1);
        grib_context_free(a->context,buf);
        return 0;
    }

    if(!laplacianOperatorIsSet) {
        laplacianOperator = calculate_pfactor(a->context,val,pen_j,sub_j);
        if((ret = grib_set_double_internal(gh,self->laplacianOperator,laplacianOperator))
                != GRIB_SUCCESS) return ret;
        grib_get_double_internal(gh,self->laplacianOperator,&laplacianOperator);
    }

    /*
   printf("PACKING LAPLACE set=%ld value=%.20f\n",laplacianOperatorIsSet,laplacianOperator);
     */

    hsize = 4*(sub_k+1)*(sub_k+2);
    lsize = ((n_vals - ((sub_k+1)*(sub_k+2)))*bits_per_value)/8;

    buflen = hsize+lsize;

    buf  = (unsigned char*)grib_context_malloc(a->context,buflen);
    hres = buf;
    lres = buf+hsize;

    maxv = pen_j+1;

    lpos = 0;
    hpos = 0;

    scals   = (double*) grib_context_malloc(a->context,maxv*sizeof(double));
    Assert(scals);

    scals[0] =0;
    for(i=1;i<maxv;i++)
        scals[i] = ((double)pow(i*(i+1),laplacianOperator));


    i=0;

    mmax = 0;
    maxv = pen_j+1;
    i=0;
    lcount=0;
    hcount=0;
    sub_k = sub_j;

    while(maxv>0)
    {
        lup=mmax;

        if(sub_k>=0)
        {
            i   += 2*(sub_k+1);
            lup +=    sub_k+1 ;
            hcount += sub_k+1 ;
            sub_k--;
        }

        for(lcount=hcount; lcount < maxv ; lcount++)
        {
            current_val = ((val[i++]*d) * scals[lup]);
            if(mixmax_unset == 0){
                max = current_val;
                min = current_val;
                mixmax_unset = 1;
            }

            if(current_val > max) max = current_val;
            if(current_val < min) min = current_val;

            current_val = ((val[i++]*d) * scals[lup]);
            if(current_val > max) max = current_val;
            if(current_val < min) min = current_val;

            lup++;
        }
        maxv--;
        hcount=0;
        mmax++;
    }

    if (grib_get_nearest_smaller_value(gh,self->reference_value,min,&reference_value)
            !=GRIB_SUCCESS) {
        grib_context_log(a->context,GRIB_LOG_ERROR,
                "unable to find nearest_smaller_value of %g for %s",min,self->reference_value);
        return GRIB_INTERNAL_ERROR;
    }
    binary_scale_factor = grib_get_binary_scale_fact(max,reference_value,bits_per_value,&ret);

    if (ret==GRIB_UNDERFLOW) {
        d=0;
        binary_scale_factor = 0;
        reference_value=0;
    } else {
        if (ret!=GRIB_SUCCESS) {
            grib_context_log(a->context,GRIB_LOG_ERROR,"COMPLEX_PACKING : Cannot compute binary_scale_factor");
            return ret;
        }
    }
    s = grib_power(-binary_scale_factor,2);

    /* printf("D : %.30f\n",d); */

    i=0;

    mmax = 0;
    maxv = pen_j+1;
    i=0;
    lcount=0;
    hcount=0;
    sub_k = sub_j;

    while(maxv>0)
    {
        lup=mmax;

        if(sub_k>=0)
        {
            for(hcount=0;hcount<sub_k+1;hcount++)
            {
                if ( GRIBEX_sh_bug_present && hcount==sub_k ) {
                    /* _test(val[i]*d*scals[lup],1); */
                    grib_encode_unsigned_long(hres, encode_float((val[i++]*d)*scals[lup]) , &hpos, 32);
                    /* _test(val[i]*d*scals[lup],1); */
                    grib_encode_unsigned_long(hres, encode_float((val[i++]*d)*scals[lup]) , &hpos, 32);
                }else{

                    /* _test(val[i]*d,0); */

                    grib_encode_unsigned_long(hres, encode_float(val[i++]*d) , &hpos, 32);
                    /* _test(val[i]*d,0); */
                    grib_encode_unsigned_long(hres, encode_float(val[i++]*d) , &hpos, 32);
                }
                lup++;
            }
            sub_k--;
        }

#if FAST_BIG_ENDIAN
        grib_encode_double_array_complex((maxv-hcount)*2,&(val[i]),bits_per_value,reference_value,&(scals[lup]),d,s,lres,&lpos);
        i+=(maxv-hcount)*2;
#else
        if (bits_per_value % 8) {
            for(lcount=hcount; lcount < maxv ; lcount++)
            {
                current_val = (((((val[i++]*d) * scals[lup])-reference_value)*s)+0.5);
                if(current_val < 0)
                    grib_context_log(a->context,GRIB_LOG_ERROR,
                            "COMPLEX_PACKING : negative coput before packing (%g)", current_val);
                grib_encode_unsigned_longb(lres, current_val, &lpos, bits_per_value);

                current_val = (((((val[i++]*d) * scals[lup])-reference_value)*s)+0.5);
                if(current_val < 0)
                    grib_context_log(a->context,GRIB_LOG_ERROR,
                            "COMPLEX_PACKING : negative coput before packing (%g)", current_val);
                grib_encode_unsigned_longb(lres, current_val, &lpos, bits_per_value);
                lup++;
            }
        } else {
            for(lcount=hcount; lcount < maxv ; lcount++)
            {
                current_val = (((((val[i++]*d) * scals[lup])-reference_value)*s)+0.5);
                if(current_val < 0)
                    grib_context_log(a->context,GRIB_LOG_ERROR,
                            "COMPLEX_PACKING : negative coput before packing (%g)", current_val);
                grib_encode_unsigned_long(lres, current_val, &lpos, bits_per_value);

                current_val = (((((val[i++]*d) * scals[lup])-reference_value)*s)+0.5);
                if(current_val < 0)
                    grib_context_log(a->context,GRIB_LOG_ERROR,
                            "COMPLEX_PACKING : negative coput before packing (%g)", current_val);
                grib_encode_unsigned_long(lres, current_val, &lpos, bits_per_value);
                lup++;
            }
        }
#endif

        maxv--;
        hcount=0;
        mmax++;
    }

    if(((hpos/8) != hsize) &&((lpos/8) != lsize))
    {
        grib_context_log(a->context,GRIB_LOG_ERROR,
                "COMPLEX_PACKING : Mismatch in packing between high resolution and low resolution part");
        grib_context_free(a->context,buf);
        grib_context_free(a->context,scals);
        return GRIB_INTERNAL_ERROR;
    }

    buflen = ((hpos + lpos)/8);

    if((ret = grib_set_double_internal(gh,self->reference_value, reference_value)) != GRIB_SUCCESS)
        return ret;
    {
        /* Make sure we can decode it again */
        double ref = 1e-100;
        grib_get_double_internal(gh,self->reference_value,&ref);
        Assert(ref == reference_value);
    }

    if((ret = grib_set_long_internal(gh,self->binary_scale_factor, binary_scale_factor)) != GRIB_SUCCESS)
        return ret;

    grib_buffer_replace(a, buf, buflen,1,1);
    grib_context_free(a->context,buf);
    grib_context_free(a->context,scals);

    return ret;

}
