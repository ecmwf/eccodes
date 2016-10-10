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
   SUPER      = grib_accessor_class_data_simple_packing
   IMPLEMENTS = init
   IMPLEMENTS = pack_double
   IMPLEMENTS = unpack_double
   IMPLEMENTS = unpack_double_element
   IMPLEMENTS = value_count
   IMPLEMENTS = destroy
   MEMBERS=const char* half_byte
   MEMBERS=const char* packingType
   MEMBERS=const char* ieee_packing
   MEMBERS=const char* precision
   MEMBERS=const char* widthOfFirstOrderValues
   MEMBERS=const char* firstOrderValues
   MEMBERS=const char* N1
   MEMBERS=const char* N2
   MEMBERS=const char* numberOfGroups
   MEMBERS=const char* codedNumberOfGroups
   MEMBERS=const char* numberOfSecondOrderPackedValues
   MEMBERS=const char* extraValues
   MEMBERS=const char* groupWidths
   MEMBERS=const char* widthOfWidths
   MEMBERS=const char* groupLengths
   MEMBERS=const char* widthOfLengths
   MEMBERS=const char* NL
   MEMBERS=const char* SPD
   MEMBERS=const char* widthOfSPD
   MEMBERS=const char* orderOfSPD
   MEMBERS=const char* numberOfPoints
   MEMBERS=const char* dataFlag
   MEMBERS=double* values
   MEMBERS=size_t size


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
static void destroy(grib_context*,grib_accessor*);
static void init(grib_accessor*,const long, grib_arguments* );
static void init_class(grib_accessor_class*);
static int unpack_double_element(grib_accessor*,size_t i, double* val);

typedef struct grib_accessor_data_g1second_order_general_extended_packing {
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
/* Members defined in data_g1second_order_general_extended_packing */
	const char* half_byte;
	const char* packingType;
	const char* ieee_packing;
	const char* precision;
	const char* widthOfFirstOrderValues;
	const char* firstOrderValues;
	const char* N1;
	const char* N2;
	const char* numberOfGroups;
	const char* codedNumberOfGroups;
	const char* numberOfSecondOrderPackedValues;
	const char* extraValues;
	const char* groupWidths;
	const char* widthOfWidths;
	const char* groupLengths;
	const char* widthOfLengths;
	const char* NL;
	const char* SPD;
	const char* widthOfSPD;
	const char* orderOfSPD;
	const char* numberOfPoints;
	const char* dataFlag;
	double* values;
	size_t size;
} grib_accessor_data_g1second_order_general_extended_packing;

extern grib_accessor_class* grib_accessor_class_data_simple_packing;

static grib_accessor_class _grib_accessor_class_data_g1second_order_general_extended_packing = {
    &grib_accessor_class_data_simple_packing,                      /* super                     */
    "data_g1second_order_general_extended_packing",                      /* name                      */
    sizeof(grib_accessor_data_g1second_order_general_extended_packing),  /* size                      */
    0,                           /* inited */
    &init_class,                 /* init_class */
    &init,                       /* init                      */
    0,                  /* post_init                      */
    &destroy,                    /* free mem                       */
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
    &unpack_double_element,     /* unpack only ith value          */
    0,     /* unpack a subarray         */
    0,              		/* clear          */
    0,               		/* clone accessor          */
};


grib_accessor_class* grib_accessor_class_data_g1second_order_general_extended_packing = &_grib_accessor_class_data_g1second_order_general_extended_packing;


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
	c->unpack_double_subarray	=	(*(c->super))->unpack_double_subarray;
	c->clear	=	(*(c->super))->clear;
	c->make_clone	=	(*(c->super))->make_clone;
}

/* END_CLASS_IMP */

#define MAX_NUMBER_OF_GROUPS 65534
#define EFDEBUG 0

static unsigned long nbits[32]={
        0x1, 0x2, 0x4, 0x8, 0x10, 0x20,
        0x40, 0x80, 0x100, 0x200, 0x400, 0x800,
        0x1000, 0x2000, 0x4000, 0x8000, 0x10000, 0x20000,
        0x40000, 0x80000, 0x100000, 0x200000, 0x400000, 0x800000,
        0x1000000, 0x2000000, 0x4000000, 0x8000000, 0x10000000, 0x20000000,
        0x40000000, 0x80000000
};

static long number_of_bits(grib_handle*h, unsigned long x)
{
    unsigned long *n=nbits;
    const int count = sizeof(nbits)/sizeof(nbits[0]);
    long i=0;
    while (x>=*n) {
        n++;
        i++;
        if (i>=count) {
            /*grib_dump_content(h, stdout,"debug", ~0, NULL);*/
            grib_context_log(h->context, GRIB_LOG_FATAL,
                    "grib_accessor_class_data_g1second_order_general_extended_packing: Number out of range: %ld", x);
        }
    }
    return i;
}

static void init(grib_accessor* a,const long v, grib_arguments* args)
{
    grib_accessor_data_g1second_order_general_extended_packing *self =(grib_accessor_data_g1second_order_general_extended_packing*)a;
    grib_handle* handle = grib_handle_of_accessor(a);

    self->half_byte    = grib_arguments_get_name(handle,args,self->carg++);
    self->packingType    = grib_arguments_get_name(handle,args,self->carg++);
    self->ieee_packing    = grib_arguments_get_name(handle,args,self->carg++);
    self->precision    = grib_arguments_get_name(handle,args,self->carg++);
    self->widthOfFirstOrderValues    = grib_arguments_get_name(handle,args,self->carg++);
    self->firstOrderValues    = grib_arguments_get_name(handle,args,self->carg++);
    self->N1 = grib_arguments_get_name(handle,args,self->carg++);
    self->N2 = grib_arguments_get_name(handle,args,self->carg++);
    self->numberOfGroups = grib_arguments_get_name(handle,args,self->carg++);
    self->codedNumberOfGroups = grib_arguments_get_name(handle,args,self->carg++);
    self->numberOfSecondOrderPackedValues = grib_arguments_get_name(handle,args,self->carg++);
    self->extraValues = grib_arguments_get_name(handle,args,self->carg++);
    self->groupWidths = grib_arguments_get_name(handle,args,self->carg++);
    self->widthOfWidths = grib_arguments_get_name(handle,args,self->carg++);
    self->groupLengths = grib_arguments_get_name(handle,args,self->carg++);
    self->widthOfLengths = grib_arguments_get_name(handle,args,self->carg++);
    self->NL = grib_arguments_get_name(handle,args,self->carg++);
    self->SPD = grib_arguments_get_name(handle,args,self->carg++);
    self->widthOfSPD = grib_arguments_get_name(handle,args,self->carg++);
    self->orderOfSPD = grib_arguments_get_name(handle,args,self->carg++);
    self->numberOfPoints = grib_arguments_get_name(handle,args,self->carg++);
    self->dataFlag = grib_arguments_get_name(handle,args,self->carg++);
    self->edition=1;
    self->dirty=1;
    self->values=NULL;
    self->size=0;
    a->flags |= GRIB_ACCESSOR_FLAG_DATA;
}

static int value_count(grib_accessor* a,long* count)
{
    grib_accessor_data_g1second_order_general_extended_packing* self =  (grib_accessor_data_g1second_order_general_extended_packing*)a;
    long numberOfCodedValues=0;
    long numberOfGroups=0;
    size_t ngroups;
    long *groupLengths;
    long orderOfSPD=0;
    long i;
    int err=0;

    *count=0;

    err=grib_get_long(grib_handle_of_accessor(a),self->numberOfGroups,&numberOfGroups);
    if (err) return err;
    if (numberOfGroups==0) return 0;

    groupLengths=(long*)grib_context_malloc_clear(a->context,sizeof(long)*numberOfGroups);
    ngroups=numberOfGroups;
    err=grib_get_long_array(grib_handle_of_accessor(a),self->groupLengths,groupLengths,&ngroups);
    if (err) return err;

    for (i=0;i<numberOfGroups;i++) numberOfCodedValues+=groupLengths[i];

    grib_context_free(a->context,groupLengths);

    err=grib_get_long(grib_handle_of_accessor(a),self->orderOfSPD,&orderOfSPD);

    *count=numberOfCodedValues+orderOfSPD;

    return err;
}

static int unpack_double_element(grib_accessor* a, size_t idx, double* val)
{
    size_t size;
    double* values;
    int err=0;

    /* GRIB-564: The index idx relates to codedValues NOT values! */

    err=grib_get_size(grib_handle_of_accessor(a),"codedValues",&size);
    if (err) return err;
    if (idx >= size) return GRIB_INVALID_NEAREST;

    values=(double*)grib_context_malloc_clear(a->context,size*sizeof(double));
    err=grib_get_double_array(grib_handle_of_accessor(a),"codedValues",values,&size);
    if (err) return err;
    *val=values[idx];
    grib_context_free(a->context,values);
    return err;
}

static int unpack_double(grib_accessor* a, double* values, size_t *len)
{
    grib_accessor_data_g1second_order_general_extended_packing* self =  (grib_accessor_data_g1second_order_general_extended_packing*)a;
    int ret=0;
    long numberOfGroups,numberOfSecondOrderPackedValues;
    long* firstOrderValues=0;
    long* X=0;
    long pos=0;
    grib_handle* handle = grib_handle_of_accessor(a);
    unsigned char* buf = (unsigned char*)handle->buffer->data;
    long i,n;
    double reference_value;
    long binary_scale_factor;
    long decimal_scale_factor;
    double s,d;
    long j,count=0;
    long *groupWidths=NULL,*groupLengths=NULL;
    long orderOfSPD=0;
    long *SPD=0;
    long numberOfValues=0;
    long bias=0;
    long y=0,z=0,w=0;
    size_t k,ngroups;

    if (!self->dirty) {
        if (*len<self->size) {
            return GRIB_ARRAY_TOO_SMALL;
        }
        for (k=0;k<self->size;k++)
            values[k]=self->values[k];

        *len=self->size;
        return GRIB_SUCCESS;
    }

    self->dirty=0;

    buf += grib_byte_offset(a);
    ret=value_count(a,&numberOfValues);
    if (ret) return ret;

    if((ret=grib_get_long_internal(handle,self->numberOfGroups,&numberOfGroups)) != GRIB_SUCCESS)
        return ret;

    if((ret=grib_get_long_internal(handle,self->binary_scale_factor,&binary_scale_factor)) != GRIB_SUCCESS)
        return ret;

    ngroups=numberOfGroups;
    groupWidths=(long*)grib_context_malloc_clear(a->context,sizeof(long)*numberOfGroups);
    ret=grib_get_long_array(handle,self->groupWidths,groupWidths,&ngroups);
    if(ret != GRIB_SUCCESS) return ret;

    groupLengths=(long*)grib_context_malloc_clear(a->context,sizeof(long)*numberOfGroups);
    ret=grib_get_long_array(handle,self->groupLengths,groupLengths,&ngroups);
    if(ret != GRIB_SUCCESS) return ret;

    firstOrderValues=(long*)grib_context_malloc_clear(a->context,sizeof(long)*numberOfGroups);
    ret=grib_get_long_array(handle,self->firstOrderValues,firstOrderValues,&ngroups);
    if(ret != GRIB_SUCCESS) return ret;

    if((ret=grib_get_long_internal(handle,self->decimal_scale_factor,&decimal_scale_factor)) != GRIB_SUCCESS)
        return ret;

    if((ret=grib_get_double_internal(handle,self->reference_value,&reference_value)) != GRIB_SUCCESS)
        return ret;

    if((ret=grib_get_long_internal(handle,self->numberOfSecondOrderPackedValues,
            &numberOfSecondOrderPackedValues)) != GRIB_SUCCESS)
        return ret;

    if((ret=grib_get_long_internal(handle,self->orderOfSPD,&orderOfSPD)) != GRIB_SUCCESS)
        return ret;

    if (orderOfSPD) {
        size_t nSPD=orderOfSPD+1;
        SPD=(long*)grib_context_malloc_clear(a->context,sizeof(long)*nSPD);
        ret=grib_get_long_array(handle,self->SPD,SPD,&nSPD);
        bias=SPD[orderOfSPD];
        if(ret != GRIB_SUCCESS) return ret;
    }

    X=(long*)grib_context_malloc_clear(a->context,sizeof(long)*numberOfValues);

    n=orderOfSPD;
    for (i=0;i<numberOfGroups;i++) {
        if (groupWidths[i]>0) {
            grib_decode_long_array(buf, &pos, groupWidths[i], groupLengths[i],
                    &X[n]);
            for (j=0;j<groupLengths[i];j++) {
                X[n]+=firstOrderValues[i];
                count++;
                n++;
            }
#if 0
            for (j=0;j<groupLengths[i];j++) {
                X[n]=grib_decode_unsigned_long(buf,&pos,groupWidths[i]);
#if EFDEBUG
                printf("DXXXXX %ld %ld %ld %ld\n",n,X[n],groupWidths[i],groupLengths[i]);
#endif
                X[n]+=firstOrderValues[i];
                count++;
                n++;
            }
#endif
        } else {
            for (j=0;j<groupLengths[i];j++) {
                X[n]=firstOrderValues[i];
                n++;
            }
        }
    }

    for (i=0;i<orderOfSPD;i++) {
        X[i]=SPD[i];
    }

    switch (orderOfSPD) {
    case 1:
        y=X[0];
        for (i=1;i<numberOfValues;i++) {
            y+=X[i]+bias;
            X[i]=y;
        }

        break;
    case 2:
        y=X[1]-X[0];
        z=X[1];
        for (i=2;i<numberOfValues;i++) {
            y+=X[i]+bias;
            z+=y;
            X[i]=z;
        }

        break;
    case 3:
        y=X[2]-X[1];
        z=y-(X[1]-X[0]);
        w=X[2];
        for (i=3;i<numberOfValues;i++) {
            z+=X[i]+bias;
            y+=z;
            w+=y;
            X[i]=w;
        }
        break;
    }

    if (self->values) {
        if (numberOfValues!=self->size) {
            grib_context_free(a->context,self->values);
            self->values=(double*)grib_context_malloc_clear(a->context,sizeof(double)*numberOfValues);
        }
    } else {
        self->values=(double*)grib_context_malloc_clear(a->context,sizeof(double)*numberOfValues);
    }

    s = grib_power(binary_scale_factor,2);
    d = grib_power(-decimal_scale_factor,10) ;
    for (i=0; i<numberOfValues; i++) {
        values[i] = (double) (((X[i]*s)+reference_value)*d);
        self->values[i]=values[i];
    }

    *len=numberOfValues;
    self->size=numberOfValues;

    grib_context_free(a->context,X);
    grib_context_free(a->context,groupWidths);
    grib_context_free(a->context,groupLengths);
    grib_context_free(a->context,firstOrderValues);
    if (orderOfSPD)
        grib_context_free(a->context,SPD);

    return ret;
}

static void grib_split_long_groups(grib_handle* hand, grib_context* c,long* numberOfGroups,long* lengthOfSecondOrderValues,
        long* groupLengths,long* widthOfLengths,
        long* groupWidths, long widthOfWidths,
        long* firstOrderValues,long widthOfFirstOrderValues) {

    long i,j;
    long newWidth,delta;
    long *widthsOfLengths;
    long *localWidthsOfLengths;
    long *localLengths;
    long *localWidths;
    long *localFirstOrderValues;
    int maxNumberOfGroups=*numberOfGroups*2;


    /* the widthOfLengths is the same for all the groupLengths and therefore if
        few big groups are present all the groups have to be coded with a large number
        of bits (big widthOfLengths) even if the majority of them is small.
        Here we try to reduce the size of the message splitting the big groups.
     */

    widthsOfLengths=(long*)grib_context_malloc_clear(c,sizeof(long)*maxNumberOfGroups);
    j=0;
    /* compute the widthOfLengths and the number of big groups */
    for (i=0;i<*numberOfGroups;i++) {
        widthsOfLengths[i]=number_of_bits(hand, groupLengths[i]);
        if (*widthOfLengths==widthsOfLengths[i]) {
            j++;
        }
    }

    /* variation of the size of message due to decrease of groupLengths
       of 1*/
    newWidth=*widthOfLengths-1;
    delta=j*(widthOfWidths+widthOfFirstOrderValues+newWidth)-*numberOfGroups;

    if (delta>=0) {
        grib_context_free(c,widthsOfLengths);
        return;
    }

    localWidthsOfLengths=(long*)grib_context_malloc_clear(c,sizeof(long)*maxNumberOfGroups);
    localLengths=(long*)grib_context_malloc_clear(c,sizeof(long)*maxNumberOfGroups);
    localWidths=(long*)grib_context_malloc_clear(c,sizeof(long)*maxNumberOfGroups);
    localFirstOrderValues=(long*)grib_context_malloc_clear(c,sizeof(long)*maxNumberOfGroups);

    while (newWidth>0) {
        /* it is worth to split big groups */
        j=0;
        for (i=0;i<*numberOfGroups;i++) {
            if (newWidth<widthsOfLengths[i]) {
                localLengths[j]=groupLengths[i]/2;
                localWidthsOfLengths[j]=number_of_bits(hand, localLengths[j]);
                localWidths[j]=groupWidths[i];
                localFirstOrderValues[j]=firstOrderValues[i];
                j++;
                localLengths[j]=groupLengths[i]-localLengths[j-1];
                localWidthsOfLengths[j]=number_of_bits(hand, localLengths[j]);
                localWidths[j]=groupWidths[i];
                localFirstOrderValues[j]=firstOrderValues[i];
                if (localWidthsOfLengths[j]>newWidth) {
                    localLengths[j]--;
                    localWidthsOfLengths[j]--;
                    j++;
                    localLengths[j]=1;
                    localWidthsOfLengths[j]=1;
                    localWidths[j]=groupWidths[i];
                    localFirstOrderValues[j]=firstOrderValues[i];
                }
                j++;
            } else {
                localLengths[j]=groupLengths[i];
                localWidthsOfLengths[j]=widthsOfLengths[i];
                localWidths[j]=groupWidths[i];
                localFirstOrderValues[j]=firstOrderValues[i];
                j++;
            }
        }

        if (j>maxNumberOfGroups) break;

        *numberOfGroups=j;
        *widthOfLengths=newWidth;
        j=0;
        *lengthOfSecondOrderValues=0;
        for (i=0;i<*numberOfGroups;i++) {
            groupLengths[i]=localLengths[i];
            widthsOfLengths[i]=localWidthsOfLengths[i];
            groupWidths[i]=localWidths[i];
            firstOrderValues[i]=localFirstOrderValues[i];
            *lengthOfSecondOrderValues+=groupLengths[i]*groupWidths[i];
            if (*widthOfLengths==widthsOfLengths[i]) j++;
        }

        newWidth--;
        delta=j*(widthOfWidths+widthOfFirstOrderValues+newWidth)-*numberOfGroups;
        if (delta>=0) break;
    }

    grib_context_free(c,widthsOfLengths);
    grib_context_free(c,localWidthsOfLengths);
    grib_context_free(c,localLengths);
    grib_context_free(c,localWidths);
    grib_context_free(c,localFirstOrderValues);
}

static int pack_double(grib_accessor* a, const double* val, size_t *len)
{
    grib_accessor_data_g1second_order_general_extended_packing* self =  (grib_accessor_data_g1second_order_general_extended_packing*)a;
    int ret=0;
    int grib2=0;
    long bits_per_value,orderOfSPD,binary_scale_factor;
    long numberOfValues;
    double max,min;
    double decimal,divisor;
    double reference_value;
    size_t size,sizebits;
    long half_byte;
    long* X;
    long* Xp;
    long i;
    long incrementGroupLengthA,groupWidthA,prevGroupLength,offsetD,remainingValuesB,groupLengthB;
    long maxB,minB,maxAB,minAB;
    long offsetBeforeData,offsetSection4;
    unsigned char*  buffer = NULL;
    long maxWidth,maxLength,widthOfWidths,NL,widthOfLengths,N1,N2,extraValues,codedNumberOfGroups,numberOfSecondOrderPackedValues;
    long pos;

    long numberOfGroups;
    long groupLengthC,groupLengthA,remainingValues,count;
    long maxA=0,minA=0;
    long maxC,minC,offsetC;
    long maxAC,minAC;
    long range,bias=0,maxSPD;
    long firstOrderValuesMax,offset,groupLength,j,groupWidth,firstOrderValue,lengthOfSecondOrderValues;
    long *groupLengths,*groupWidths,*firstOrderValues;
    /* long groupLengths[MAX_NUMBER_OF_GROUPS],groupWidths[MAX_NUMBER_OF_GROUPS],firstOrderValues[MAX_NUMBER_OF_GROUPS]; */

    /* TODO put these parameters in def file */
    long startGroupLength=15;
    long incrementGroupLength=3;
    long minGroupLength=3;
    long widthOfSPD=0,widthOfBias=0;
    long *offsets;
    long widthOfFirstOrderValues;
    int computeGroupA=1;
    long dataHeadersLength,widthsLength,lengthsLength,firstOrderValuesLength;
    long decimal_scale_factor;
    grib_handle* handle = grib_handle_of_accessor(a);

    self->dirty=1;

    numberOfValues=*len;

    max = val[0];
    min = max;
    for(i=1;i< numberOfValues;i++) {
        if (val[i] > max ) max = val[i];
        if (val[i] < min ) min = val[i];
    }

    /* For constant fields set decimal scale factor to 0 (See GRIB-165) */
    if (min==max) {
        grib_set_long_internal(handle,self->decimal_scale_factor, 0);
    }

    if((ret = grib_get_long_internal(handle,self->decimal_scale_factor, &decimal_scale_factor))
            != GRIB_SUCCESS)
        return ret;
    decimal = grib_power(decimal_scale_factor,10);

    max*=decimal;
    min*=decimal;

    if (grib_get_nearest_smaller_value(handle,self->reference_value,min,&reference_value)
            !=GRIB_SUCCESS) {
        grib_context_log(a->context,GRIB_LOG_ERROR,
                "unable to find nearest_smaller_value of %g for %s",min,self->reference_value);
        return GRIB_INTERNAL_ERROR;
    }
    if((ret = grib_set_double_internal(handle,self->reference_value, reference_value)) !=
            GRIB_SUCCESS)
        return ret;

    if((ret=grib_get_long_internal(handle,self->bits_per_value,&bits_per_value)) != GRIB_SUCCESS)
        return ret;

    if((ret=grib_get_long_internal(handle,self->offsetdata,&offsetBeforeData)) != GRIB_SUCCESS)
        return ret;

    if((ret=grib_get_long_internal(handle,self->offsetsection,&offsetSection4)) != GRIB_SUCCESS)
        return ret;

    if((ret=grib_get_long_internal(handle,self->orderOfSPD,&orderOfSPD)) != GRIB_SUCCESS)
        return ret;

    binary_scale_factor = grib_get_binary_scale_fact(max,reference_value,bits_per_value,&ret);
    if (ret != GRIB_SUCCESS) return ret;

    if((ret = grib_set_long_internal(handle,self->binary_scale_factor, binary_scale_factor)) !=
            GRIB_SUCCESS)
        return ret;

    divisor = grib_power(-binary_scale_factor,2);
    X=(long*)grib_context_malloc_clear(a->context,sizeof(long)*numberOfValues);
    for(i=0;i< numberOfValues;i++){
        X[i] = (((val[i]*decimal)-reference_value)*divisor)+0.5;
    }

    groupLengths=(long*)grib_context_malloc_clear(a->context,sizeof(long)*numberOfValues);
    groupWidths=(long*)grib_context_malloc_clear(a->context,sizeof(long)*numberOfValues);
    firstOrderValues=(long*)grib_context_malloc_clear(a->context,sizeof(long)*numberOfValues);

    /* spatial differencing */
    switch (orderOfSPD) {
    case 1:
        for (i=numberOfValues-1;i>0;i--) {
            X[i]-=X[i-1];
        }
        break;
    case 2:
        for (i=numberOfValues-1;i>1;i--) {
            X[i]-=2*X[i-1]-X[i-2];
        }
        break;
    case 3:
        for (i=numberOfValues-1;i>2;i--) {
            X[i]-=3*(X[i-1]-X[i-2])+X[i-3];
        }
        break;
    }
    if (orderOfSPD) {
        Assert(orderOfSPD >=0 && orderOfSPD < numberOfValues);
        bias=X[orderOfSPD];
        for (i=orderOfSPD+1;i<numberOfValues;i++) {
            if ( bias > X[i] ) bias=X[i];
        }
        for (i=orderOfSPD;i<numberOfValues;i++) {
            X[i]-=bias;
        }
        maxSPD=X[0];
        for (i=1;i<orderOfSPD;i++) {
            if ( maxSPD < X[i] ) maxSPD=X[i];
        }
        /* widthOfSPD=(long)ceil(log((double)(maxSPD+1))/log(2.0)); */
        widthOfSPD=number_of_bits(handle, maxSPD);
        widthOfBias=number_of_bits(handle, labs(bias))+1;

        if ( widthOfSPD < widthOfBias  ) widthOfSPD=widthOfBias;

    }
    /* end of spatial differencing */

    count=orderOfSPD;
    remainingValues=numberOfValues-count;
    numberOfGroups=0;
    incrementGroupLengthA=startGroupLength;

    computeGroupA=1;
    while (remainingValues) {
        /* group A created with length=incrementGroupLengthA (if enough values remain)
           incrementGroupLengthA=startGroupLength always except when coming from an A+C or A+B ok branch
         */
        groupLengthA= incrementGroupLengthA < remainingValues ? incrementGroupLengthA : remainingValues ;
        if (computeGroupA) {
            maxA=X[count];
            minA=X[count];
            for (i=1;i<groupLengthA;i++) {
                DebugAssertAccess(X, count+i, numberOfValues);
                if (maxA<X[count+i]) maxA=X[count+i];
                if (minA>X[count+i]) minA=X[count+i];
            }
        }
        groupWidthA=number_of_bits(handle, maxA-minA);
        range=(long)grib_power(groupWidthA,2)-1;

        offsetC=count+groupLengthA;
        if (offsetC==numberOfValues) {
            /* no more values close group A and end loop */
            groupLengths[numberOfGroups]=groupLengthA;
            groupWidths[numberOfGroups]=groupWidthA;
            /* firstOrderValues[numberOfGroups]=minA; */
            /* to optimise the width of first order variable */
            firstOrderValues[numberOfGroups] = maxA-range > 0 ? maxA-range : 0;
            numberOfGroups++;
            break;
        }

        /* group C created with length=incrementGroupLength (fixed)
           or remaining values if close to end
         */
        groupLengthC=incrementGroupLength;
        if ( groupLengthC + offsetC > numberOfValues - startGroupLength/2) {
            groupLengthC=numberOfValues-offsetC;
        }
        maxC=X[offsetC];
        minC=X[offsetC];
        for (i=1;i<groupLengthC;i++) {
            DebugAssertAccess(X, offsetC+i, numberOfValues);
            if (maxC<X[offsetC+i]) maxC=X[offsetC+i];
            if (minC>X[offsetC+i]) minC=X[offsetC+i];
        }

        maxAC= maxA > maxC ? maxA : maxC;
        minAC= minA < minC ? minA : minC;

        /* check if A+C can be represented with the same width as A*/
        if (maxAC-minAC > range) {
            /* A could not be expanded adding C. Check if A could be expanded taking
               some elements from preceding group. The condition is always that width of
               A doesn't increase.
             */
            if (numberOfGroups>0 && groupWidths[numberOfGroups-1] > groupWidthA ) {
                prevGroupLength=groupLengths[numberOfGroups-1]-incrementGroupLength;
                offsetC=count-incrementGroupLength;
                /* preceding group length cannot be less than a minimum value */
                while (prevGroupLength >= minGroupLength) {
                    maxAC=maxA;
                    minAC=minA;
                    for (i=0;i<incrementGroupLength;i++) {
                        if (maxAC<X[offsetC+i]) maxAC=X[offsetC+i];
                        if (minAC>X[offsetC+i]) minAC=X[offsetC+i];
                    }

                    /* no more elements can be transfered, exit loop*/
                    if (maxAC-minAC > range) break;

                    maxA=maxAC;
                    minA=minAC;
                    groupLengths[numberOfGroups-1]-=incrementGroupLength;
                    groupLengthA+=incrementGroupLength;
                    count-=incrementGroupLength;
                    remainingValues+=incrementGroupLength;

                    offsetC-=incrementGroupLength;
                    prevGroupLength-=incrementGroupLength;
                }
            }
            /* close group A*/
            groupLengths[numberOfGroups]=groupLengthA;
            groupWidths[numberOfGroups]=groupWidthA;
            /* firstOrderValues[numberOfGroups]=minA; */
            /* to optimise the width of first order variable */
            firstOrderValues[numberOfGroups] = maxA-range > 0 ? maxA-range : 0;
            count+=groupLengthA;
            remainingValues-=groupLengthA;
            numberOfGroups++;
            /* incrementGroupLengthA is reset to the fixed startGroupLength as it
               could have been changed after the A+C or A+B ok condition.
             */
            incrementGroupLengthA=startGroupLength;
            computeGroupA=1;
#if 0
            if (numberOfGroups==MAX_NUMBER_OF_GROUPS) {
                groupLengthA= remainingValues ;
                maxA=X[count];
                minA=X[count];
                for (i=1;i<groupLengthA;i++) {
                    if (maxA<X[count+i]) maxA=X[count+i];
                    if (minA>X[count+i]) minA=X[count+i];
                }
                groupWidthA=number_of_bits(maxA-minA);
                range=(long)grib_power(groupWidthA,2)-1;
                groupLengths[numberOfGroups]=groupLengthA;
                groupWidths[numberOfGroups]=groupWidthA;
                firstOrderValues[numberOfGroups] = maxA-range > 0 ? maxA-range : 0;
                break;
            }
#endif
            continue;
        }

        /* A+C could be coded with the same width as A*/
        offsetD=offsetC+groupLengthC;
        if (offsetD==numberOfValues) {
            groupLengths[numberOfGroups]=groupLengthA+groupLengthC;
            groupWidths[numberOfGroups]=groupWidthA;

            /* range of AC is the same as A*/
            /* firstOrderValues[numberOfGroups]=minAC; */
            /* to optimise the width of first order variable */
            firstOrderValues[numberOfGroups] = maxAC-range > 0 ? maxAC-range : 0;
            numberOfGroups++;
            break;
        }

        /* group B is created with length startGroupLength, starting at the
           same offset as C.
         */
        remainingValuesB=numberOfValues-offsetC;
        groupLengthB= startGroupLength < remainingValuesB ? startGroupLength : remainingValuesB ;
        maxB=maxC;
        minB=minC;
        for (i=groupLengthC;i<groupLengthB;i++) {
            if (maxB<X[offsetC+i]) maxB=X[offsetC+i];
            if (minB>X[offsetC+i]) minB=X[offsetC+i];
        }

        /* check if group B can be coded with a smaller width than A */
        if (maxB-minB <= range/2 && range>0 ) {

            /* TODO Add code to try if A can be expanded taking some elements
               from the left (preceding) group.
                A possible variation is to do this left check (and the previous one)
                in the final loop when checking that the width of each group.
             */

            /* close group A and continue loop*/
            groupLengths[numberOfGroups]=groupLengthA;
            groupWidths[numberOfGroups]=groupWidthA;
            /* firstOrderValues[numberOfGroups]=minA; */
            /* to optimise the width of first order variable */
            firstOrderValues[numberOfGroups] = maxA-range > 0 ? maxA-range : 0;
            count+=groupLengthA;
            remainingValues-=groupLengthA;
            numberOfGroups++;
#if 0
            if (numberOfGroups==MAX_NUMBER_OF_GROUPS) {
                groupLengthA= remainingValues ;
                maxA=X[count];
                minA=X[count];
                for (i=1;i<groupLengthA;i++) {
                    if (maxA<X[count+i]) maxA=X[count+i];
                    if (minA>X[count+i]) minA=X[count+i];
                }
                groupWidthA=number_of_bits(maxA-minA);
                range=(long)grib_power(groupWidthA,2)-1;
                groupLengths[numberOfGroups]=groupLengthA;
                groupWidths[numberOfGroups]=groupWidthA;
                firstOrderValues[numberOfGroups] = maxA-range > 0 ? maxA-range : 0;
                break;
            }
#endif
            incrementGroupLengthA=startGroupLength;
            computeGroupA=1;
            continue;
        }

        /* check if A+B can be coded with same with as A */
        maxAB= maxA > maxB ? maxA : maxB;
        minAB= minA < minB ? minA : minB;
        if (maxAB-minAB <= range) {
            /* A+B can be merged. The increment used at the beginning of the loop to
               build group C is increased to the size of group B
             */
            incrementGroupLengthA+=groupLengthB;
            maxA=maxAB;
            minA=minAB;
            computeGroupA=0;
            continue;
        }

        /* A+B cannot be merged, A+C can be merged*/
        incrementGroupLengthA+=groupLengthC;
        computeGroupA=1;

    } /* end of the while*/

    /* computing bitsPerValue as the number of bits needed to represent
       the firstOrderValues.
     */
    max=firstOrderValues[0];
    min=firstOrderValues[0];
    for (i=1;i<numberOfGroups;i++) {
        if (max<firstOrderValues[i]) max=firstOrderValues[i];
        if (min>firstOrderValues[i]) min=firstOrderValues[i];
    }
    widthOfFirstOrderValues=number_of_bits(handle, max-min);
    firstOrderValuesMax=(long)grib_power(widthOfFirstOrderValues,2)-1;

    if (numberOfGroups>2) {
        /* loop through all the groups except the last in reverse order to
           check if each group width is still appropriate for the group.
           Focus on groups which have been shrank as left groups of an A group taking
           some of their elements.
         */
        offsets=(long*)grib_context_malloc_clear(a->context,sizeof(long)*numberOfGroups);
        offsets[0]=orderOfSPD;
        for (i=1;i<numberOfGroups;i++) offsets[i]=offsets[i-1]+groupLengths[i-1];
        for (i=numberOfGroups-2;i>=0;i--) {
            offset=offsets[i];
            groupLength=groupLengths[i];

            if (groupLength >= startGroupLength) continue;

            max=X[offset];
            min=X[offset];
            for (j=1;j<groupLength;j++) {
                if (max<X[offset+j]) max=X[offset+j];
                if (min>X[offset+j]) min=X[offset+j];
            }
            groupWidth=number_of_bits(handle, max-min);
            range=(long)grib_power(groupWidth,2)-1;

            /* width of first order values has to be unchanged.*/
            for (j=groupWidth;j<groupWidths[i];j++) {
                firstOrderValue= max>range ? max-range : 0;
                if (firstOrderValue <= firstOrderValuesMax ) {
                    groupWidths[i]=j;
                    firstOrderValues[i]=firstOrderValue;
                    break;
                }
            }

            offsetC=offset;
            /*  group width of the current group (i) can have been reduced
                and it is worth to try to expand the group to get some elements
                from the left group if it has bigger width.
             */
            if (i>0 && (groupWidths[i-1] > groupWidths[i]) ) {
                prevGroupLength=groupLengths[i-1]-incrementGroupLength;
                offsetC-=incrementGroupLength;
                while (prevGroupLength >= minGroupLength) {
                    for (j=0;j<incrementGroupLength;j++) {
                        if (max<X[offsetC+j]) max=X[offsetC+j];
                        if (min>X[offsetC+j]) min=X[offsetC+j];
                    }

                    /* width of first order values has to be unchanged*/
                    firstOrderValue=max>range ? max-range : 0;
                    if (max-min > range || firstOrderValue > firstOrderValuesMax ) break;

                    groupLengths[i-1]-=incrementGroupLength;
                    groupLengths[i]+=incrementGroupLength;
                    firstOrderValues[i]=firstOrderValue;

                    offsetC-=incrementGroupLength;
                    prevGroupLength-=incrementGroupLength;
                }
            }

        }
        grib_context_free(a->context,offsets);
    }

    maxWidth=groupWidths[0];
    maxLength=groupLengths[0];
    for (i=1;i<numberOfGroups;i++) {
        if (maxWidth<groupWidths[i]) maxWidth=groupWidths[i];
        if (maxLength<groupLengths[i]) maxLength=groupLengths[i];
    }

    if (maxWidth < 0 || maxLength < 0) {
        grib_context_log(a->parent->h->context, GRIB_LOG_ERROR, "Cannot compute parameters for second order packing.");
        return GRIB_ENCODING_ERROR;
    }
    widthOfWidths=number_of_bits(handle, maxWidth);
    widthOfLengths=number_of_bits(handle, maxLength);

    lengthOfSecondOrderValues=0;
    for ( i=0; i<numberOfGroups;i++) {
        lengthOfSecondOrderValues+=groupLengths[i]*groupWidths[i];
    }

    if (!a->context->no_big_group_split) {
        grib_split_long_groups(handle, a->context,&numberOfGroups,&lengthOfSecondOrderValues,
                groupLengths,&widthOfLengths,groupWidths,widthOfWidths,
                firstOrderValues,widthOfFirstOrderValues);
    }

    Xp=X+orderOfSPD;
    for ( i=0; i<numberOfGroups;i++) {
        for (j=0; j<groupLengths[i]; j++) {
            *(Xp++)-=firstOrderValues[i];
        }
    }

    /* start writing to message */

    /* writing SPD */
    if (orderOfSPD) {
        if((ret = grib_set_long_internal(handle,self->widthOfSPD, widthOfSPD))
                != GRIB_SUCCESS)
            return ret;
    }

    /* end writing SPD */
    if((ret = grib_set_long_internal(handle,self->widthOfFirstOrderValues, widthOfFirstOrderValues))
            != GRIB_SUCCESS)
        return ret;

    dataHeadersLength=25;
    if (orderOfSPD) dataHeadersLength+=1+((orderOfSPD+1)*widthOfSPD+7)/8;
    widthsLength=(widthOfWidths*numberOfGroups+7)/8;
    lengthsLength=(widthOfLengths*numberOfGroups+7)/8;
    firstOrderValuesLength=(widthOfFirstOrderValues*numberOfGroups+7)/8;

    NL=widthsLength+dataHeadersLength+1;
    N1=NL+lengthsLength;
    N2=N1+firstOrderValuesLength;

    NL= NL > 65535 ? 65535 : NL;
    N2= N2 > 65535 ? 65535 : N2;
    N1= N1 > 65535 ? 65535 : N1;

    grib_set_long(handle,self->NL, NL);
    grib_set_long(handle,self->N1, N1);
    grib_set_long(handle,self->N2, N2);

    if (numberOfGroups > 65535 ) {
        extraValues=numberOfGroups/65536;
        codedNumberOfGroups=numberOfGroups%65536;
    } else {
        extraValues=0;
        codedNumberOfGroups=numberOfGroups;
    }

    /* if no extraValues key present it is a GRIB2*/
    grib2=0;
    if((ret = grib_set_long(handle,self->extraValues, extraValues)) != GRIB_SUCCESS) {
        codedNumberOfGroups=numberOfGroups;
        grib2=1;
    }

    if((ret = grib_set_long_internal(handle,self->codedNumberOfGroups, codedNumberOfGroups)) != GRIB_SUCCESS)
        return ret;

    numberOfSecondOrderPackedValues=numberOfValues-orderOfSPD;
    if (!grib2 && numberOfSecondOrderPackedValues > 65535 )
        numberOfSecondOrderPackedValues= 65535;

    if((ret = grib_set_long_internal(handle,self->numberOfSecondOrderPackedValues, numberOfSecondOrderPackedValues))
            != GRIB_SUCCESS)
        return ret;

    if (grib2) {
        if((ret = grib_set_long_internal(handle,self->bits_per_value, bits_per_value)) != GRIB_SUCCESS)
            return ret;
    } else {
        if((ret = grib_set_long_internal(handle,self->bits_per_value, 0)) != GRIB_SUCCESS)
            return ret;
    }

    if((ret = grib_set_long_internal(handle,self->widthOfWidths, widthOfWidths)) != GRIB_SUCCESS)
        return ret;

    if((ret = grib_set_long_internal(handle,self->widthOfLengths, widthOfLengths)) != GRIB_SUCCESS)
        return ret;

    lengthOfSecondOrderValues=0;
    for ( i=0; i<numberOfGroups;i++) {
        lengthOfSecondOrderValues+=groupLengths[i]*groupWidths[i];
    }

    size=(lengthOfSecondOrderValues+7)/8;
    sizebits=lengthOfSecondOrderValues;

    /* padding section 4 to an even number of octets*/
    size = (size+offsetBeforeData-offsetSection4) % 2 ? size+1 : size;
    half_byte=8*size-sizebits;
    if((ret = grib_set_long_internal(handle,self->half_byte, half_byte)) != GRIB_SUCCESS)
        return ret;

    buffer=(unsigned char*)grib_context_malloc_clear(a->context,size);

    pos=0;
    if (orderOfSPD) {
        long SPD[4]={0,};
        size_t nSPD=orderOfSPD+1;
        Assert(orderOfSPD<=3);
        for (i=0;i<orderOfSPD;i++) SPD[i]=X[i];
        SPD[orderOfSPD]=bias;
        ret=grib_set_long_array_internal(handle,self->SPD,SPD,nSPD);
        if(ret) return ret;
    }

    ret=grib_set_long_array_internal(handle,self->groupWidths,groupWidths,(size_t)numberOfGroups);
    if(ret) return ret;

    ret=grib_set_long_array_internal(handle,self->groupLengths,groupLengths,(size_t)numberOfGroups);
    if(ret) return ret;

    ret=grib_set_long_array_internal(handle,self->firstOrderValues,firstOrderValues,(size_t)numberOfGroups);
    if(ret) return ret;

    Xp=X+orderOfSPD;
    pos=0;
    count=0;
    for (i=0;i<numberOfGroups;i++) {
        if (groupWidths[i]>0) {
            for (j=0;j<groupLengths[i];j++) {
#if EFDEBUG
                printf("CXXXXX %ld %ld %ld %ld\n",count,*Xp,groupWidths[i],groupLengths[i]);
                count++;
#endif
                grib_encode_unsigned_longb(buffer,*(Xp++),&pos,groupWidths[i]);
            }
        } else  {
            Xp+=groupLengths[i];
#if EFDEBUG
            count+=groupLengths[i];
#endif
        }
    }

    /* ECC-259: Set correct number of values */
    ret=grib_set_long_internal(a->parent->h,self->number_of_values, *len);
    if(ret) return ret;

    grib_buffer_replace(a, buffer, size,1,1);

    grib_context_free(a->context,buffer);
    grib_context_free(a->context,X);
    grib_context_free(a->context,groupLengths);
    grib_context_free(a->context,groupWidths);
    grib_context_free(a->context,firstOrderValues);

    return ret;
}

static void destroy(grib_context* context,grib_accessor* a)
{
    grib_accessor_data_g1second_order_general_extended_packing *self =(grib_accessor_data_g1second_order_general_extended_packing*)a;
    if (self->values != NULL) {
        grib_context_free(context, self->values);
        self->values=NULL;
    }
}
