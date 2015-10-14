/*
 * Copyright 2005-2015 ECMWF.
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
   IMPLEMENTS = init
   IMPLEMENTS = dump;destroy
   IMPLEMENTS = next_offset
   IMPLEMENTS = byte_count; value_count
   IMPLEMENTS = byte_offset; unpack_double
   IMPLEMENTS = get_native_type
   IMPLEMENTS = update_size
   IMPLEMENTS = compare
   IMPLEMENTS = pack_long; unpack_double; pack_double
   MEMBERS    = const char* offsetSection4Name
   MEMBERS    = const char* offsetBeforeDataName
   MEMBERS    = const char* offsetEndSection4Name
   MEMBERS    = const char* section4LengthName
   MEMBERS    = const char* numberOfSubsetsName
   MEMBERS    = const char* subsetNumberName
   MEMBERS    = const char* expandedDescriptorsName
   MEMBERS    = const char* flagsName
   MEMBERS    = const char* unitsName
   MEMBERS    = const char* elementsDescriptorsIndexName
   MEMBERS    = const char* compressedDataName
   MEMBERS    = bufr_descriptors_array* expanded
   MEMBERS    = grib_accessor* expandedAccessor
   MEMBERS    = int* canBeMissing
   MEMBERS    = long subsetNumber
   MEMBERS    = long numberOfSubsets
   MEMBERS    = long compressedData
   MEMBERS    = grib_vdarray* numericValues
   MEMBERS    = grib_vsarray* stringValues
   MEMBERS    = grib_viarray* elementsDescriptorsIndex
   MEMBERS    = int do_decode
   MEMBERS    = int bitmapStartElementsDescriptorsIndex
   MEMBERS    = int bitmapCurrentElementsDescriptorsIndex
   MEMBERS    = int bitmapSize
   MEMBERS    = int bitmapStart
   MEMBERS    = int bitmapCurrent
   MEMBERS    = grib_accessors_list* dataAccessors
   MEMBERS    = int unpackMode
   MEMBERS    = int bitsToEndData
   MEMBERS    = grib_section* dataKeys;

   END_CLASS_DEF

 */

/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "accessor.class" and rerun ./make_class.pl

*/

static int  get_native_type(grib_accessor*);
static int pack_double(grib_accessor*, const double* val,size_t *len);
static int pack_long(grib_accessor*, const long* val,size_t *len);
static int unpack_double(grib_accessor*, double* val,size_t *len);
static long byte_count(grib_accessor*);
static long byte_offset(grib_accessor*);
static long next_offset(grib_accessor*);
static int value_count(grib_accessor*,long*);
static void destroy(grib_context*,grib_accessor*);
static void dump(grib_accessor*, grib_dumper*);
static void init(grib_accessor*,const long, grib_arguments* );
static void init_class(grib_accessor_class*);
static void update_size(grib_accessor*,size_t);
static int compare(grib_accessor*, grib_accessor*);

typedef struct grib_accessor_bufr_data_array {
    grib_accessor          att;
/* Members defined in gen */
/* Members defined in bufr_data_array */
	const char* offsetSection4Name;
	const char* offsetBeforeDataName;
	const char* offsetEndSection4Name;
	const char* section4LengthName;
	const char* numberOfSubsetsName;
	const char* subsetNumberName;
	const char* expandedDescriptorsName;
	const char* flagsName;
	const char* unitsName;
	const char* elementsDescriptorsIndexName;
	const char* compressedDataName;
	bufr_descriptors_array* expanded;
	grib_accessor* expandedAccessor;
	int* canBeMissing;
	long subsetNumber;
	long numberOfSubsets;
	long compressedData;
	grib_vdarray* numericValues;
	grib_vsarray* stringValues;
	grib_viarray* elementsDescriptorsIndex;
	int do_decode;
	int bitmapStartElementsDescriptorsIndex;
	int bitmapCurrentElementsDescriptorsIndex;
	int bitmapSize;
	int bitmapStart;
	int bitmapCurrent;
	grib_accessors_list* dataAccessors;
	int unpackMode;
	int bitsToEndData;
	grib_section* dataKeys;
} grib_accessor_bufr_data_array;

extern grib_accessor_class* grib_accessor_class_gen;

static grib_accessor_class _grib_accessor_class_bufr_data_array = {
    &grib_accessor_class_gen,                      /* super                     */
    "bufr_data_array",                      /* name                      */
    sizeof(grib_accessor_bufr_data_array),  /* size                      */
    0,                           /* inited */
    &init_class,                 /* init_class */
    &init,                       /* init                      */
    0,                  /* post_init                      */
    &destroy,                    /* free mem                       */
    &dump,                       /* describes himself         */
    &next_offset,                /* get length of section     */
    0,              /* get length of string      */
    &value_count,                /* get number of values      */
    &byte_count,                 /* get number of bytes      */
    &byte_offset,                /* get offset to bytes           */
    &get_native_type,            /* get native type               */
    0,                /* get sub_section                */
    0,               /* grib_pack procedures long      */
    0,                 /* grib_pack procedures long      */
    &pack_long,                  /* grib_pack procedures long      */
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
    &update_size,                /* update_size   */
    0,            /* preferred_size   */
    0,                    /* resize   */
    0,      /* nearest_smaller_value */
    0,                       /* next accessor    */
    &compare,                    /* compare vs. another accessor   */
    0,     /* unpack only ith value          */
    0,     /* unpack a subarray         */
    0,              		/* clear          */
};


grib_accessor_class* grib_accessor_class_bufr_data_array = &_grib_accessor_class_bufr_data_array;


static void init_class(grib_accessor_class* c)
{
	c->string_length	=	(*(c->super))->string_length;
	c->sub_section	=	(*(c->super))->sub_section;
	c->pack_missing	=	(*(c->super))->pack_missing;
	c->is_missing	=	(*(c->super))->is_missing;
	c->unpack_long	=	(*(c->super))->unpack_long;
	c->pack_string	=	(*(c->super))->pack_string;
	c->unpack_string	=	(*(c->super))->unpack_string;
	c->pack_string_array	=	(*(c->super))->pack_string_array;
	c->unpack_string_array	=	(*(c->super))->unpack_string_array;
	c->pack_bytes	=	(*(c->super))->pack_bytes;
	c->unpack_bytes	=	(*(c->super))->unpack_bytes;
	c->pack_expression	=	(*(c->super))->pack_expression;
	c->notify_change	=	(*(c->super))->notify_change;
	c->preferred_size	=	(*(c->super))->preferred_size;
	c->resize	=	(*(c->super))->resize;
	c->nearest_smaller_value	=	(*(c->super))->nearest_smaller_value;
	c->next	=	(*(c->super))->next;
	c->unpack_double_element	=	(*(c->super))->unpack_double_element;
	c->unpack_double_subarray	=	(*(c->super))->unpack_double_subarray;
	c->clear	=	(*(c->super))->clear;
}

/* END_CLASS_IMP */

#define MAX_NESTED_REPLICATIONS 8

#define PROCESS_DECODE     0
#define PROCESS_NEW_DATA   1
#define PROCESS_ENCODE     2

static int process_elements(grib_accessor* a,int flag);

typedef int (*codec_element_proc) (grib_context* c,grib_accessor_bufr_data_array* self,int subsetIndex, grib_buffer* b,unsigned char* data,long *pos,int i,long elementIndex,grib_darray* dval,grib_sarray* sval); 
typedef int (*codec_replication_proc) (grib_context* c,grib_accessor_bufr_data_array* self,int subsetIndex,grib_buffer* buff,unsigned char* data,long *pos,int i,long elementIndex,grib_darray* dval,long* numberOfRepetitions);

static int can_be_missing(int descriptor)
{
    int ret=1;
    if (descriptor==31031) ret=0;
    return ret;
}

static void restart_bitmap(grib_accessor_bufr_data_array *self)
{
    self->bitmapCurrent=-1;
    self->bitmapCurrentElementsDescriptorsIndex=self->bitmapStartElementsDescriptorsIndex-1;
}

static void cancel_bitmap(grib_accessor_bufr_data_array *self) { self->bitmapCurrent=-1;self->bitmapStart=-1; }

static int is_bitmap_start_defined(grib_accessor_bufr_data_array *self) { return self->bitmapStart==-1 ? 0 : 1; }

static long init_length(grib_accessor* a)
{
    grib_accessor_bufr_data_array *self =(grib_accessor_bufr_data_array*)a;
    long section4Length=0;

    grib_handle* h=a->parent->h;

    grib_get_long(h,self->section4LengthName,&section4Length);

    return section4Length-4;
}

static void init(grib_accessor* a,const long v, grib_arguments* params)
{
    grib_accessor_bufr_data_array *self =(grib_accessor_bufr_data_array*)a;
    int n = 0;
    const char* dataKeysName=NULL;
    grib_accessor* dataKeysAcc=NULL;

    self->offsetSection4Name = grib_arguments_get_name(a->parent->h,params,n++);
    self->offsetBeforeDataName = grib_arguments_get_name(a->parent->h,params,n++);
    self->offsetEndSection4Name = grib_arguments_get_name(a->parent->h,params,n++);
    self->section4LengthName = grib_arguments_get_name(a->parent->h,params,n++);
    self->numberOfSubsetsName = grib_arguments_get_name(a->parent->h,params,n++);
    self->subsetNumberName    = grib_arguments_get_name(a->parent->h,params,n++);
    self->expandedDescriptorsName = grib_arguments_get_name(a->parent->h,params,n++);
    self->flagsName = grib_arguments_get_name(a->parent->h,params,n++);
    self->elementsDescriptorsIndexName = grib_arguments_get_name(a->parent->h,params,n++);
    self->compressedDataName = grib_arguments_get_name(a->parent->h,params,n++);
    dataKeysName = grib_arguments_get_name(a->parent->h,params,n++);

    dataKeysAcc=grib_find_accessor(a->parent->h,dataKeysName);
    self->dataKeys=dataKeysAcc->parent;
    self->do_decode=1;
    self->elementsDescriptorsIndex=0;
    self->numericValues=0;
    self->stringValues=0;
    cancel_bitmap(self);
    self->expanded=0;
    self->expandedAccessor=0;

    a->length = init_length(a);
    self->bitsToEndData=a->length*8;
    self->unpackMode=CODES_BUFR_UNPACK_STRUCTURE;

    /* Assert(a->length>=0); */
}

static void clean_string(char* s,int len)
{
    int i=len-1;
    while (i) {
        if (!isalnum(s[i])) s[i]=0;
        else break;
        i--;
    }
}

static int check_end_data(grib_context* c,grib_accessor_bufr_data_array* self,int size)
{
    grib_context_log(c, GRIB_LOG_DEBUG,"BUFR data decoding: \tbitsToEndData=%ld elementSize=%ld", self->bitsToEndData,size);
    self->bitsToEndData-=size;
    if (self->bitsToEndData<0)
        return GRIB_DECODING_ERROR;
    return 0;
}

static void self_clear(grib_context* c,grib_accessor_bufr_data_array* self)
{
    grib_context_free(c,self->canBeMissing);
    grib_vdarray_delete_content(c,self->numericValues);
    grib_vdarray_delete(c,self->numericValues);
    if(self->stringValues) {
        /* grib_vsarray_delete_content(c,self->stringValues); */
        grib_vsarray_delete(c,self->stringValues);
    }
    grib_viarray_delete_content(c,self->elementsDescriptorsIndex);
    grib_viarray_delete(c,self->elementsDescriptorsIndex);
}

static int  get_native_type(grib_accessor* a)
{
    return GRIB_TYPE_DOUBLE;
}

static long byte_count(grib_accessor* a)
{
    return a->length;
}

static long byte_offset(grib_accessor* a)
{
    return a->offset;
}

static long next_offset(grib_accessor* a)
{
    return a->offset+a->length;
}

static void update_size(grib_accessor* a,size_t s)
{
    grib_context_log(a->parent->h->context,GRIB_LOG_DEBUG,"updating size of %s old %ld new %ld",a->name,a->length,s);
    a->length = s;
    Assert(a->length>=0);
}

static int compare(grib_accessor* a, grib_accessor* b)
{
    return GRIB_NOT_IMPLEMENTED;
}

static int pack_long(grib_accessor* a, const long* val, size_t *len)
{
    grib_accessor_bufr_data_array *self =(grib_accessor_bufr_data_array*)a;
    self->do_decode=1;

    return GRIB_NOT_IMPLEMENTED;
}

static int pack_double(grib_accessor* a, const double* val, size_t *len)
{
    grib_accessor_bufr_data_array *self =(grib_accessor_bufr_data_array*)a;
    self->do_decode=1;
    return process_elements(a,PROCESS_ENCODE);
}

grib_vsarray* accessor_bufr_data_array_get_stringValues(grib_accessor* a)
{
  grib_accessor_bufr_data_array *self =(grib_accessor_bufr_data_array*)a;
  process_elements(a,PROCESS_DECODE);

  return self->stringValues;
}

grib_accessors_list* accessor_bufr_data_array_get_dataAccessors(grib_accessor* a)
{
    grib_accessor_bufr_data_array *self =(grib_accessor_bufr_data_array*)a;
    return self->dataAccessors;
}

void accessor_bufr_data_array_set_unpackMode(grib_accessor* a,int unpackMode)
{
    grib_accessor_bufr_data_array *self =(grib_accessor_bufr_data_array*)a;
    self->unpackMode=unpackMode;
}

static int get_descriptors(grib_accessor* a)
{
    grib_accessor_bufr_data_array *self =(grib_accessor_bufr_data_array*)a;
    int ret=0,i,numberOfDescriptors;
    grib_handle* h=a->parent->h;
    grib_context* c=a->parent->h->context;

    if (!self->expandedAccessor)
        self->expandedAccessor=grib_find_accessor(a->parent->h,self->expandedDescriptorsName);
    self->expanded=grib_accessor_class_expanded_descriptors_get_expanded(self->expandedAccessor,&ret);

    numberOfDescriptors=grib_bufr_descriptors_array_used_size(self->expanded);
    self->canBeMissing=(int*)grib_context_malloc_clear(c,numberOfDescriptors*sizeof(int));
    for (i=0;i<numberOfDescriptors;i++)
        self->canBeMissing[i]=can_be_missing(self->expanded->v[i]->code);

    ret=grib_get_long(h,self->numberOfSubsetsName,&(self->numberOfSubsets));
    ret=grib_get_long(h,self->compressedDataName,&(self->compressedData));
    ret=grib_get_long(h,self->subsetNumberName,&(self->subsetNumber));

    return ret;
}

static void decode_string_array(grib_context* c,unsigned char* data,long* pos, int i,
        grib_accessor_bufr_data_array* self,int *err)
{
    char* sval=0;
    int j,modifiedWidth,modifiedReference,width;
    double modifiedFactor;
    grib_sarray* sa=grib_sarray_new(c,self->numberOfSubsets,10);

    modifiedWidth= self->expanded->v[i]->width;
    modifiedReference= self->expanded->v[i]->reference;
    modifiedFactor= self->expanded->v[i]->factor;

    sval=(char*)grib_context_malloc_clear(c,modifiedWidth/8+1);
    *err=check_end_data(c,self,modifiedWidth);
    if (*err) return ;
    grib_decode_string(data,pos,modifiedWidth/8,sval);
    /* clean_string(sval,modifiedWidth/8); */
    *err=check_end_data(c,self,6);
    if (*err) return ;
    width=grib_decode_unsigned_long(data,pos,6);
    if (width) {
        grib_context_free(c,sval);
        *err=check_end_data(c,self,width*8*self->numberOfSubsets);
        if (*err) return ;
        for (j=0;j<self->numberOfSubsets;j++) {
            sval=(char*)grib_context_malloc_clear(c,width+1);
            grib_decode_string(data,pos,width,sval);
            /* clean_string(sval,width); */
            grib_sarray_push(c,sa,sval);
        }
    } else {
        /* for (j=0;j<self->numberOfSubsets;j++) { */
            grib_sarray_push(c,sa,sval);
        /* } */
    }
    grib_vsarray_push(c,self->stringValues,sa);
}

static grib_darray* decode_double_array(grib_context* c,unsigned char* data,long* pos,int i,
        grib_accessor_bufr_data_array* self,int *err)
{
    grib_darray* ret=NULL;
    int j;
    unsigned long lval;
    int localReference,localWidth,modifiedWidth,modifiedReference;
    double modifiedFactor,dval;

    *err=0;

    modifiedReference= self->expanded->v[i]->reference;
    modifiedFactor= self->expanded->v[i]->factor;
    modifiedWidth= self->expanded->v[i]->width;

    *err=check_end_data(c,self,modifiedWidth+6);
    if (*err) return NULL;
    lval=grib_decode_unsigned_long(data,pos,modifiedWidth);
    localReference=(long)lval+modifiedReference;
    localWidth=grib_decode_unsigned_long(data,pos,6);
    grib_context_log(c, GRIB_LOG_DEBUG,"BUFR data decoding: \tlocalWidth=%ld",localWidth);
    ret=grib_darray_new(c,100,100);
    if (localWidth) {
        *err=check_end_data(c,self,localWidth*self->numberOfSubsets);
        if (*err) return NULL;
        for (j=0;j<self->numberOfSubsets;j++) {
            lval=grib_decode_unsigned_long(data,pos,localWidth);
            if (grib_is_all_bits_one(lval,localWidth) && self->canBeMissing[i]) {
                dval=GRIB_MISSING_DOUBLE;
            } else {
                dval=((long)lval+localReference)*modifiedFactor;
            }
            grib_darray_push(c,ret,dval);
        }
    } else {
        if (grib_is_all_bits_one(lval,modifiedWidth) && self->canBeMissing[i]) {
            dval=GRIB_MISSING_DOUBLE;
        } else {
            dval=localReference*modifiedFactor;
        }
        grib_darray_push(c,ret,dval);
    }

    return ret;
}

static int encode_string_array(grib_context* c,grib_buffer* buff,long* pos, int i,
        grib_accessor_bufr_data_array* self,grib_sarray* stringValues)
{
    char* sval=0;
    int err=0,end,start;
    int j,modifiedWidth,modifiedReference,width;
    double modifiedFactor;

    start=0;
    end=grib_sarray_used_size(stringValues);

    modifiedWidth= self->expanded->v[i]->width;
    modifiedReference= self->expanded->v[i]->reference;
    modifiedFactor= self->expanded->v[i]->factor;

    grib_buffer_set_ulength_bits(c,buff,buff->ulength_bits+modifiedWidth);
    grib_encode_string(buff->data,pos,modifiedWidth/8,stringValues->v[0]);
    width= end > 1 ? modifiedWidth : 0;

    grib_buffer_set_ulength_bits(c,buff,buff->ulength_bits+6);
    grib_encode_unsigned_longb(buff->data,width/8,pos,6);
    if (width) {
        grib_buffer_set_ulength_bits(c,buff,buff->ulength_bits+width*end);
        for (j=start;j<end;j++) {
            grib_encode_string(buff->data,pos,width/8,stringValues->v[j]);
        }
    }
    return err;
}

static int encode_double_array(grib_context* c,grib_buffer* buff,long* pos,int i,
        grib_accessor_bufr_data_array* self,grib_darray* dvalues)
{
    int err=0;
    int j;
    unsigned long lval;
    long localReference=0,localWidth=0,modifiedWidth,modifiedReference;
    long reference,allone;
    double localRange,modifiedFactor,inverseFactor;
    size_t nvals,ii;
    double min,max,maxAllowed,minAllowed;
    double* v=NULL;
    int thereIsAMissing=0;

    modifiedReference= self->expanded->v[i]->reference;
    modifiedFactor= self->expanded->v[i]->factor;
    inverseFactor= grib_power(self->expanded->v[i]->scale,10);
    modifiedWidth= self->expanded->v[i]->width;

    maxAllowed=(grib_power(modifiedWidth,2)+modifiedReference)*modifiedFactor;
    minAllowed=modifiedReference*modifiedFactor;

    nvals=grib_darray_used_size(dvalues);
    v=dvalues->v;

    if (nvals==1) {
      localWidth=0;
      grib_buffer_set_ulength_bits(c,buff,buff->ulength_bits+modifiedWidth);
      if (*v == GRIB_MISSING_DOUBLE) {
        grib_set_bits_on(buff->data,pos,modifiedWidth);
      } else {
        lval=round(*v * inverseFactor)-modifiedReference;
        grib_encode_unsigned_longb(buff->data,lval,pos,modifiedWidth);
      }
      grib_buffer_set_ulength_bits(c,buff,buff->ulength_bits+6);
      grib_encode_unsigned_longb(buff->data,localWidth,pos,6);
      return err;
    }

    ii=0;
    while (*v==GRIB_MISSING_DOUBLE && ii<nvals) {
      thereIsAMissing=1;
      v++;
      ii++;
    }
    min=*v;
    max=*v;
    while (ii<nvals) {
      if (*v<min && *v!=GRIB_MISSING_DOUBLE) min=*v;
      if (*v>max && *v!=GRIB_MISSING_DOUBLE) max=*v;
      if (*v == GRIB_MISSING_DOUBLE) thereIsAMissing=1;
      ii++;
      v++;
    }
    if (max>maxAllowed && max!=GRIB_MISSING_DOUBLE) return GRIB_OUT_OF_RANGE;
    if (min<minAllowed && min!=GRIB_MISSING_DOUBLE) return GRIB_OUT_OF_RANGE;

    reference=round(min*inverseFactor);
    localReference=reference-modifiedReference;
    if (max!=min) {
      localRange = (max-min)*inverseFactor+1;
      localWidth=ceil(log(localRange)/log(2.0));
      lval=round(max*inverseFactor)-reference;
      allone=grib_power(localWidth,2)-1;
      while (allone <= lval) {
        localWidth++;
        allone=grib_power(localWidth,2)-1;
      }
      if (localWidth == 1 ) localWidth++;
    } else {
      if (thereIsAMissing==1) localWidth=1;
      else localWidth=0;
    }

    grib_buffer_set_ulength_bits(c,buff,buff->ulength_bits+modifiedWidth);
    if (localWidth) {
      grib_encode_unsigned_longb(buff->data,localReference,pos,modifiedWidth);
    } else {
      if (min==GRIB_MISSING_DOUBLE) {
        grib_set_bits_on(buff->data,pos,modifiedWidth);
      } else {
        lval=localReference-modifiedReference;
        grib_encode_unsigned_longb(buff->data,lval,pos,modifiedWidth);
      }
    }
    grib_buffer_set_ulength_bits(c,buff,buff->ulength_bits+6);
    grib_encode_unsigned_longb(buff->data,localWidth,pos,6);

    if (localWidth) {
      grib_buffer_set_ulength_bits(c,buff,buff->ulength_bits+self->numberOfSubsets*localWidth);
      for (j=0;j<self->numberOfSubsets;j++) {
        if (dvalues->v[j]==GRIB_MISSING_DOUBLE) {
          grib_set_bits_on(buff->data,pos,localWidth);
        } else {
          lval=round(dvalues->v[j]*inverseFactor)-reference;
          grib_encode_unsigned_longb(buff->data,lval,pos,localWidth);
        }
      }
    }

    return err;
}

static int encode_double_value(grib_context* c,grib_buffer* buff,long* pos,int i,
        grib_accessor_bufr_data_array* self,double value)
{
    unsigned long lval;
    double maxAllowed,minAllowed;
    int err=0;
    int modifiedWidth,modifiedReference;
    double modifiedFactor;
    double dval=0;

    modifiedReference= self->expanded->v[i]->reference;
    modifiedFactor= self->expanded->v[i]->factor;
    modifiedWidth= self->expanded->v[i]->width;

    grib_buffer_set_ulength_bits(c,buff,buff->ulength_bits+modifiedWidth);
    if (value==GRIB_MISSING_DOUBLE) {
        grib_set_bits_on(buff->data,pos,modifiedWidth);
    } else {
        maxAllowed=(grib_power(modifiedWidth,2)+modifiedReference)*modifiedFactor;
        minAllowed=modifiedReference*modifiedFactor;
        if (value>maxAllowed || value<minAllowed) return GRIB_OUT_OF_RANGE;
        lval=round(value/modifiedFactor)-modifiedReference;
        grib_encode_unsigned_longb(buff->data,lval,pos,modifiedWidth);
    }

    return err;
}

static int encode_string_value(grib_context* c,grib_buffer* buff,long* pos, int i,
        grib_accessor_bufr_data_array* self,char* sval)
{
    int err=0;
    int len;

    len= self->expanded->v[i]->width / 8;
    grib_buffer_set_ulength_bits(c,buff,buff->ulength_bits+self->expanded->v[i]->width);

    grib_encode_string(buff->data,pos,len,sval);

    return err;
}

static char* decode_string_value(grib_context* c,unsigned char* data,long* pos, int i,
        grib_accessor_bufr_data_array* self,int *err)
{
    char* sval=0;
    int len;

    *err=0;

    len= self->expanded->v[i]->width / 8;

    *err=check_end_data(c,self,self->expanded->v[i]->width);
    if (*err) return NULL;
    sval=(char*)grib_context_malloc_clear(c,len+1);
    grib_decode_string(data,pos,len,sval);

    /* clean_string(sval,len); */

    return sval;
}

static double decode_double_value(grib_context* c,unsigned char* data,long* pos,int i,
        grib_accessor_bufr_data_array* self,int* err)
{
    unsigned long lval;
    int modifiedWidth,modifiedReference;
    double modifiedFactor;
    double dval=0;

    *err=0;

    modifiedReference= self->expanded->v[i]->reference;
    modifiedFactor= self->expanded->v[i]->factor;
    modifiedWidth= self->expanded->v[i]->width;

    *err=check_end_data(c,self,modifiedWidth);
    if (*err) return dval;
    lval=grib_decode_unsigned_long(data,pos,modifiedWidth);
    if (grib_is_all_bits_one(lval,modifiedWidth) && self->canBeMissing[i]) {
        dval=GRIB_MISSING_DOUBLE;
    } else {
        dval=((long)lval+modifiedReference)*modifiedFactor;
    }
    return dval;
}

static int decode_element(grib_context* c,grib_accessor_bufr_data_array* self,int subsetIndex,
        grib_buffer* b,unsigned char* data,long *pos,int i,long elementIndex,grib_darray* dval,grib_sarray* sval)
{
    grib_accessor* a=(grib_accessor*)self;
    grib_darray* dar=0;
    grib_sarray* sar=0;
    int index=0,ii,stringValuesLen;
    char* csval=0;
    double cdval=0,x;
    int err=0;

    grib_context_log(c, GRIB_LOG_DEBUG,"BUFR data decoding: \tcode=%6.6ld width=%ld pos=%ld -> %ld",
            self->expanded->v[i]->code,self->expanded->v[i]->width,(long)*pos,(long)(*pos-a->offset*8));
    if (self->expanded->v[i]->type==BUFR_DESCRIPTOR_TYPE_STRING) {
        /* string */
        if (self->compressedData) {
            decode_string_array(c,data,pos,i,self,&err);
            index=grib_vsarray_used_size(self->stringValues);
            dar=grib_darray_new(c,self->numberOfSubsets,10);
            index=self->numberOfSubsets*(index-1);
            for (ii=1;ii<=self->numberOfSubsets;ii++) {
                x=(index+ii)*1000+self->expanded->v[i]->width/8;
                grib_darray_push(c,dar,x);
            }
            grib_vdarray_push(c,self->numericValues,dar);
        } else {
            csval=decode_string_value(c,data,pos,i,self,&err);
            grib_context_log(c, GRIB_LOG_DEBUG,"BUFR data decoding: \t %s = %s",
                    self->expanded->v[i]->shortName,csval);
            sar=grib_sarray_push(c,sar,csval);
            grib_vsarray_push(c,self->stringValues,sar);
            stringValuesLen=grib_vsarray_used_size(self->stringValues);
            index=0;
            for (ii=0;ii<stringValuesLen;ii++) {
              index+=grib_sarray_used_size(self->stringValues->v[ii]);
            }
            cdval=index*1000+self->expanded->v[i]->width / 8;
            grib_darray_push(c,dval,cdval);
        }
    } else {
        /* numeric or codetable or flagtable */
        if (self->compressedData) {
            dar=decode_double_array(c,data,pos,i,self,&err);
            grib_vdarray_push(c,self->numericValues,dar);
        } else {
            cdval=decode_double_value(c,data,pos,i,self,&err);
            grib_context_log(c, GRIB_LOG_DEBUG,"BUFR data decoding: \t %s = %g",
                    self->expanded->v[i]->shortName,cdval);
            grib_darray_push(c,dval,cdval);
        }
    }
    return err;
}

static int decode_replication(grib_context* c,grib_accessor_bufr_data_array* self,int subsetIndex,
            grib_buffer* buff,unsigned char* data,long *pos,int i,long elementIndex,grib_darray* dval,long* numberOfRepetitions)
{
    int err=0;
    int localReference,width;
    bufr_descriptor** descriptors=0;
    descriptors=self->expanded->v;
    if (self->compressedData) {
        grib_context_log(c, GRIB_LOG_DEBUG,"BUFR data decoding: \tdelayed replication localReference width=%ld", descriptors[i]->width);
        err=check_end_data(c,self,descriptors[i]->width+6);
        if (err) return err;
        localReference=grib_decode_unsigned_long(data,pos,descriptors[i]->width)+descriptors[i]->reference;
        grib_context_log(c, GRIB_LOG_DEBUG,"BUFR data decoding: \tdelayed replication localWidth width=6");
        width=grib_decode_unsigned_long(data,pos,6);
        if (width) {
            /* delayed replication number is not constant. NOT IMPLEMENTED */
            Assert(0);
        } else {
            *numberOfRepetitions=localReference*descriptors[i]->factor;
            grib_context_log(c, GRIB_LOG_DEBUG,"BUFR data decoding: \tdelayed replication value=%ld",*numberOfRepetitions);
        }
    } else {
        err=check_end_data(c,self,descriptors[i]->width);
        if (err) return err;
        *numberOfRepetitions=grib_decode_unsigned_long(data,pos,descriptors[i]->width)+
                descriptors[i]->reference*descriptors[i]->factor;
        grib_context_log(c, GRIB_LOG_DEBUG,"BUFR data decoding: \tdelayed replication value=%ld",*numberOfRepetitions);
    }
    if (self->compressedData) {
        dval=grib_darray_new(c,1,100);
        grib_darray_push(c,dval,(double)(*numberOfRepetitions));
        grib_vdarray_push(c,self->numericValues,dval);
    } else {
        grib_darray_push(c,dval,(double)(*numberOfRepetitions));
    }
    return err;
}

static int encode_new_element(grib_context* c,grib_accessor_bufr_data_array* self,int subsetIndex,
        grib_buffer* buff,unsigned char* data,long *pos,int i,long elementIndex,grib_darray* dval,grib_sarray* sval)
{
    int index=0,ii;
    char* csval=0;
    unsigned char missingChar=0xFF;
    double cdval=GRIB_MISSING_DOUBLE;
    int err=0;
    size_t slen;

    grib_context_log(c, GRIB_LOG_DEBUG,"BUFR data encoding: \tcode=%6.6ld width=%ld pos=%ld ulength=%ld ulength_bits=%ld",
            self->expanded->v[i]->code,self->expanded->v[i]->width,(long)*pos,buff->ulength,buff->ulength_bits);
    if (self->expanded->v[i]->type==BUFR_DESCRIPTOR_TYPE_STRING) {
        /* string */
        slen=self->expanded->v[i]->width/8;
        csval=grib_context_malloc_clear(c,slen);
        for (ii=0;ii<slen;ii++) csval[ii]=missingChar;
        grib_context_log(c, GRIB_LOG_DEBUG,"BUFR data encoding: \t %s = %s",
                self->expanded->v[i]->shortName,csval);
        if (self->compressedData) {
            grib_sarray* stringValues=grib_sarray_new(c,1,1);
            grib_sarray_push(c,stringValues,csval);
            err=encode_string_array(c,buff,pos,i,self,stringValues);
            grib_sarray_delete_content(c,stringValues);
            grib_sarray_delete(c,stringValues);
        } else {
            err=encode_string_value(c,buff,pos,i,self,csval);
        }
    } else {
        /* numeric or codetable or flagtable */
        grib_context_log(c, GRIB_LOG_DEBUG,"BUFR data encoding: \t %s = %g",
                self->expanded->v[i]->shortName,cdval);
        if (self->expanded->v[i]->code==31031) cdval=0;
        if (self->compressedData) {
            grib_darray* doubleValues=grib_darray_new(c,1,1);
            grib_darray_push(c,doubleValues,cdval);
            err=encode_double_array(c,buff,pos,i,self,doubleValues);
            grib_darray_delete(c,doubleValues);
        } else {
            err=encode_double_value(c,buff,pos,i,self,cdval);
        }
    }
    return err;
}

static int encode_new_replication(grib_context* c,grib_accessor_bufr_data_array* self,int subsetIndex,
            grib_buffer* buff,unsigned char* data,long *pos,int i,long elementIndex,grib_darray* dval,long* numberOfRepetitions)
{
    int err=0;
    /*new data -> only 1 repetition*/
    unsigned long repetitions=1;
    bufr_descriptor** descriptors=self->expanded->v;

    *numberOfRepetitions=1;

    grib_context_log(c, GRIB_LOG_DEBUG,"BUFR data encoding replication: \twidth=%ld pos=%ld ulength=%ld ulength_bits=%ld",
            (long)descriptors[i]->width,(long)*pos,(long)buff->ulength,(long)buff->ulength_bits);
    grib_buffer_set_ulength_bits(c,buff,buff->ulength_bits+descriptors[i]->width);
    grib_encode_unsigned_longb(data,repetitions,pos,descriptors[i]->width);

    if (self->compressedData) {
        grib_buffer_set_ulength_bits(c,buff,buff->ulength_bits+6);
        grib_encode_unsigned_longb(buff->data,0,pos,6);
    };

    return err;
}

static int encode_element(grib_context* c,grib_accessor_bufr_data_array* self,int subsetIndex,
        grib_buffer* buff,unsigned char* data,long *pos,int i,long elementIndex,grib_darray* dval,grib_sarray* sval)
{
    int index=0,ii,idx,j;
    char* csval=0;
    int err=0;
    size_t slen;

    grib_context_log(c, GRIB_LOG_DEBUG,"BUFR data encoding: \tcode=%6.6ld width=%ld pos=%ld ulength=%ld ulength_bits=%ld",
            self->expanded->v[i]->code,self->expanded->v[i]->width,(long)*pos,buff->ulength,buff->ulength_bits);
    if (self->expanded->v[i]->type==BUFR_DESCRIPTOR_TYPE_STRING) {
        /* string */
        /* grib_context_log(c, GRIB_LOG_DEBUG,"BUFR data encoding: \t %s = %s",
                 self->expanded->v[i]->shortName,csval); */
        if (self->compressedData) {
            idx=((int)self->numericValues->v[elementIndex]->v[0]/1000-1)/self->numberOfSubsets;
            err=encode_string_array(c,buff,pos,i,self,self->stringValues->v[idx]);
        } else {
            idx=((int)self->numericValues->v[0]->v[elementIndex]/1000-1)/self->numberOfSubsets;
            err=encode_string_value(c,buff,pos,i,self,self->stringValues->v[idx]->v[self->subsetNumber]);
        }
    } else {
        /* numeric or codetable or flagtable */
        if (self->compressedData) {
            err=encode_double_array(c,buff,pos,i,self,self->numericValues->v[elementIndex]);
            if (err) {
              grib_context_log(c,GRIB_LOG_ERROR,"encoding %s ( code=%6.6ld width=%ld scale=%g reference=%d )",
                self->expanded->v[i]->shortName, self->expanded->v[i]->code, self->expanded->v[i]->width,
                self->expanded->v[i]->scale, self->expanded->v[i]->reference);
              for (j=0;j<grib_darray_used_size(self->numericValues->v[elementIndex]);j++)
                grib_context_log(c,GRIB_LOG_ERROR,"%g ",self->numericValues->v[elementIndex]->v[i]);
            }
        } else {
            err=encode_double_value(c,buff,pos,i,self,self->numericValues->v[subsetIndex]->v[elementIndex]);
            if (err) {
              grib_context_log(c,GRIB_LOG_ERROR,"encoding %s=%g",self->expanded->v[i]->name,self->numericValues->v[subsetIndex]->v[elementIndex]);
            }
        }
    }
    return err;
}

static int encode_replication(grib_context* c,grib_accessor_bufr_data_array* self,int subsetIndex,
grib_buffer* buff,unsigned char* data,long *pos,int i,long elementIndex,grib_darray* dval,long* numberOfRepetitions)
{
    bufr_descriptor** descriptors=0;

    if (self->compressedData)
      Assert(grib_darray_used_size(self->numericValues->v[elementIndex])==1);

    return encode_element(c,self,subsetIndex,buff,data,pos,i,elementIndex,dval,0);

}

static int build_bitmap(grib_accessor_bufr_data_array *self,unsigned char* data,long* pos,grib_iarray* elementsDescriptorsIndex,int iBitmapOperator)
{
    int bitmapSize=0,iDelayedReplication=0;
    int i,localReference,width,bitmapEndElementsDescriptorsIndex;
    long ppos,n;
    grib_accessor* a=(grib_accessor*)self;
    grib_context* c=a->parent->h->context;
    bufr_descriptor** descriptors=self->expanded->v;
    long* edi=elementsDescriptorsIndex->v;
    int iel=grib_iarray_used_size(elementsDescriptorsIndex)-1;
    int err=0;

    switch (descriptors[iBitmapOperator]->code) {
    case 236000:
        cancel_bitmap(self);
        while (descriptors[edi[iel]]->code>=100000) iel--;
        bitmapEndElementsDescriptorsIndex=iel;
        i=iBitmapOperator+1;
        if (descriptors[i]->code==101000)  {
            iDelayedReplication=iBitmapOperator+2;
            Assert( descriptors[iDelayedReplication]->code==31001 ||
                    descriptors[iDelayedReplication]->code==31002 );
            i=iDelayedReplication;
            if (self->compressedData) {
                ppos=*pos;
                if (err) return err;
                localReference=grib_decode_unsigned_long(data,pos,descriptors[i]->width)+descriptors[i]->reference;
                width=grib_decode_unsigned_long(data,pos,6);
                *pos=ppos;
                if (width) {
                    /* delayed replication number is not constant. NOT IMPLEMENTED */
                    Assert(0);
                } else {
                    bitmapSize=localReference*descriptors[i]->factor;
                }
            } else {
                ppos=*pos;
                if (err) return err;
                bitmapSize=grib_decode_unsigned_long(data,pos,descriptors[i]->width)+
                        descriptors[i]->reference*descriptors[i]->factor;
                *pos=ppos;
            }
        } else if (descriptors[i]->code==31031){
            bitmapSize=0;
            while (descriptors[i]->code==31031) {bitmapSize++;i++;}
        }
        iel=bitmapEndElementsDescriptorsIndex;
        n=bitmapSize-1;
        while ( n>0 && iel>=0 ) {
            if (descriptors[edi[iel]]->code<100000) n--;
            iel--;
        }
        self->bitmapStartElementsDescriptorsIndex=iel;
        restart_bitmap(self);
        break;
    default :
        grib_context_log(c,GRIB_LOG_ERROR,"unsupported operator %d\n",
                descriptors[iBitmapOperator]->code);
        return GRIB_INTERNAL_ERROR;
    }
    return GRIB_SUCCESS;
}

static int get_next_bitmap_descriptor_index(grib_accessor_bufr_data_array *self,grib_iarray* elementsDescriptorsIndex,grib_darray* numericValues)
{
    int i;
    bufr_descriptor** descriptors=self->expanded->v;
    self->bitmapCurrent++;
    self->bitmapCurrentElementsDescriptorsIndex++;
    i=self->bitmapCurrent+self->bitmapStart;

    if (self->compressedData) {
        while (self->numericValues->v[i]->v[0]==1) {
            self->bitmapCurrent++;
            self->bitmapCurrentElementsDescriptorsIndex++;
            while (descriptors[elementsDescriptorsIndex->v[self->bitmapCurrentElementsDescriptorsIndex]]->code
                    >100000)
                self->bitmapCurrentElementsDescriptorsIndex++;
            i++;
        }
    } else {
        while (numericValues->v[i]==1) {
            self->bitmapCurrent++;
            self->bitmapCurrentElementsDescriptorsIndex++;
            while (descriptors[elementsDescriptorsIndex->v[self->bitmapCurrentElementsDescriptorsIndex]]->code
                    >100000)
                self->bitmapCurrentElementsDescriptorsIndex++;
            i++;
        }
    }
    while (descriptors[elementsDescriptorsIndex->v[self->bitmapCurrentElementsDescriptorsIndex]]->code
            >100000)
        self->bitmapCurrentElementsDescriptorsIndex++;
    return elementsDescriptorsIndex->v[self->bitmapCurrentElementsDescriptorsIndex];
}

static void push_zero_element(grib_accessor_bufr_data_array* self,grib_darray* dval)
{
    grib_darray* d=0;
    grib_accessor* a=(grib_accessor*)self;
    grib_context* c=a->parent->h->context;
    if (self->compressedData) {
        d=grib_darray_new(c,1,100);
        grib_darray_push(c,d,0);
        grib_vdarray_push(c,self->numericValues,d);
    } else {
        grib_darray_push(c,dval,0);
    }
}

static grib_accessor* create_attribute(char* name,grib_section* section,int type,char* sval,double dval,long lval,unsigned long flags)
{
    grib_accessor* a=NULL;
    grib_action creator = {0, };
    size_t len;
    creator.op         = "variable";
    creator.name_space = "";
    creator.flags     = GRIB_ACCESSOR_FLAG_READ_ONLY | flags ;
    creator.set        = 0;

    creator.name=name;
    a=grib_accessor_factory(section, &creator, 0, NULL);
    accessor_variable_set_type(a,type);
    len=1;
    switch (type) {
    case GRIB_TYPE_LONG:
        grib_pack_long(a,&lval,&len);
        break;
    case GRIB_TYPE_DOUBLE:
        grib_pack_double(a,&dval,&len);
        break;
    case GRIB_TYPE_STRING:
        len=strlen(sval);
        grib_pack_string(a,sval,&len);
        break;
    }

    return a;
}

static void set_creator_name(grib_action* creator,int code)
{
    switch (code) {
    case 222000:
        creator->name="qualityInformationFollows";
        break;
    case 223000:
        creator->name="substitutedValuesOperator";
        break;
    case 223255:
        creator->name="substitutedValue";
        break;
    case 224000:
        creator->name="firstOrderStatiticalValuesFollow";
        break;
    case 224255:
        creator->name="firstOrderStatisticalValue";
        break;
    case 225000:
        creator->name="differenceStatisticalValuesFollow";
        break;
    case 225255:
        creator->name="differenceStatisticalValue";
        break;
    case 232000:
        creator->name="replacedRetainedValuesFollow";
        break;
    case 232255:
        creator->name="replacedRetainedValue";
        break;
    case 235000:
        creator->name="cancelBackwardDataReference";
        break;
    case 236000:
        creator->name="defineDataPresentBitmap";
        break;
    case 237000:
        creator->name="useDefinedDataPresentBitmap";
        break;
    case 237255:
        creator->name="cancelUseDefinedDataPresentBitmap";
        break;
    case 241000:
        creator->name="defineEvent";
        break;
    case 241255:
        creator->name="cancelDefineEvent";
        break;
    case 242000:
        creator->name="defineConditioningEvent";
        break;
    case 242255:
        creator->name="canceDefineConditioningEvent";
        break;
    case 243000:
        creator->name="categoricalForecastValuesFollow";
        break;
    case 243255:
        creator->name="cancelCategoricalForecastValuesFollow";
        break;
    case 999999:
        creator->name="associatedField";
        break;
    default :
        creator->name="operator";
        break;
    }
}

static grib_accessor* create_accessor_from_descriptor(grib_accessor* a,grib_accessor* attribute,grib_section* section,long ide,long subset,int dump,int count)
{
    grib_accessor_bufr_data_array *self =(grib_accessor_bufr_data_array*)a;
    char code[10]={0,};
    int idx=0;
    int i;
    unsigned long flags=0;
    grib_accessor* operatorAccessor=NULL;
    grib_action operatorCreator = {0, };
    grib_accessor* elementAccessor=NULL;
    grib_action creator = {0, };
    creator.op         = "bufr_data_element";
    creator.name_space = "";
    creator.set        = 0;

    operatorCreator.op         = "variable";
    operatorCreator.name_space = "";
    operatorCreator.flags     = GRIB_ACCESSOR_FLAG_READ_ONLY;
    operatorCreator.set        = 0;
    operatorCreator.name="operator";

    if (dump) {
        creator.flags    = GRIB_ACCESSOR_FLAG_DUMP;
        operatorCreator.flags  |= GRIB_ACCESSOR_FLAG_DUMP;
    }

    idx = self->compressedData ? self->elementsDescriptorsIndex->v[0]->v[ide] :
            self->elementsDescriptorsIndex->v[subset]->v[ide] ;

    switch (self->expanded->v[idx]->F) {
    case 0:
    case 1:
        creator.name=grib_context_strdup(a->parent->h->context,self->expanded->v[idx]->shortName);
        elementAccessor = grib_accessor_factory(section, &creator, 0, NULL);
        if (self->canBeMissing[idx]) elementAccessor->flags |= GRIB_ACCESSOR_FLAG_CAN_BE_MISSING;
        accessor_bufr_data_element_set_index(elementAccessor,ide);
        accessor_bufr_data_element_set_descriptors(elementAccessor,self->expanded);
        accessor_bufr_data_element_set_elementsDescriptorsIndex(elementAccessor,self->elementsDescriptorsIndex);
        accessor_bufr_data_element_set_numericValues(elementAccessor,self->numericValues);
        accessor_bufr_data_element_set_stringValues(elementAccessor,self->stringValues);
        accessor_bufr_data_element_set_compressedData(elementAccessor,self->compressedData);
        accessor_bufr_data_element_set_type(elementAccessor,self->expanded->v[idx]->type);
        accessor_bufr_data_element_set_numberOfSubsets(elementAccessor,self->numberOfSubsets);
        accessor_bufr_data_element_set_subsetNumber(elementAccessor,subset);

        self->expanded->v[idx]->a=elementAccessor;

        if (attribute) {
          attribute->parent=elementAccessor->parent;
          for (i=0;i<MAX_ACCESSOR_ATTRIBUTES;i++) {
            if (attribute->attributes[i]) attribute->attributes[i]->parent=elementAccessor->parent;
          }
          grib_accessor_add_attribute(elementAccessor,attribute);
        }

        attribute=create_attribute("index",section,GRIB_TYPE_LONG,0,0,count,flags);
        grib_accessor_add_attribute(elementAccessor,attribute);

        sprintf(code,"%06ld",self->expanded->v[idx]->code);
        attribute=create_attribute("code",section,GRIB_TYPE_STRING,code,0,0,flags);
        grib_accessor_add_attribute(elementAccessor,attribute);

        attribute=create_attribute("units",section,GRIB_TYPE_STRING,self->expanded->v[idx]->units,0,0,GRIB_ACCESSOR_FLAG_DUMP);
        grib_accessor_add_attribute(elementAccessor,attribute);

        attribute=create_attribute("scale",section,GRIB_TYPE_LONG,0,0,self->expanded->v[idx]->scale,flags);
        grib_accessor_add_attribute(elementAccessor,attribute);

        attribute=create_attribute("reference",section,GRIB_TYPE_DOUBLE,0,self->expanded->v[idx]->reference,0,flags);
        grib_accessor_add_attribute(elementAccessor,attribute);

        attribute=create_attribute("width",section,GRIB_TYPE_LONG,0,0,self->expanded->v[idx]->width,flags);
        grib_accessor_add_attribute(elementAccessor,attribute);
        break;
    case 2:
        set_creator_name(&creator,self->expanded->v[idx]->code);
        if (self->expanded->v[idx]->isMarker) {
            elementAccessor = grib_accessor_factory(section, &creator, 0, NULL);
            if (self->canBeMissing[idx]) elementAccessor->flags |= GRIB_ACCESSOR_FLAG_CAN_BE_MISSING;
            accessor_bufr_data_element_set_index(elementAccessor,ide);
            accessor_bufr_data_element_set_descriptors(elementAccessor,self->expanded);
            accessor_bufr_data_element_set_elementsDescriptorsIndex(elementAccessor,self->elementsDescriptorsIndex);
            accessor_bufr_data_element_set_numericValues(elementAccessor,self->numericValues);
            accessor_bufr_data_element_set_stringValues(elementAccessor,self->stringValues);
            accessor_bufr_data_element_set_compressedData(elementAccessor,self->compressedData);
            accessor_bufr_data_element_set_type(elementAccessor,self->expanded->v[idx]->type);
            accessor_bufr_data_element_set_numberOfSubsets(elementAccessor,self->numberOfSubsets);
            accessor_bufr_data_element_set_subsetNumber(elementAccessor,subset);

            attribute=create_attribute("index",section,GRIB_TYPE_LONG,0,0,count,flags);
            grib_accessor_add_attribute(elementAccessor,attribute);

        } else {
            elementAccessor = grib_accessor_factory(section, &operatorCreator, 0, NULL);
            accessor_variable_set_type(elementAccessor,GRIB_TYPE_LONG);

            attribute=create_attribute("index",section,GRIB_TYPE_LONG,0,0,count,flags);
            grib_accessor_add_attribute(elementAccessor,attribute);

            sprintf(code,"%06ld",self->expanded->v[idx]->code);
            attribute=create_attribute("code",section,GRIB_TYPE_STRING,code,0,0,flags);
            grib_accessor_add_attribute(elementAccessor,attribute);
        }
        self->expanded->v[idx]->a=elementAccessor;
        break;
    case 9:
        set_creator_name(&creator,self->expanded->v[idx]->code);
        elementAccessor = grib_accessor_factory(section, &creator, 0, NULL);
        accessor_bufr_data_element_set_index(elementAccessor,ide);
        accessor_bufr_data_element_set_descriptors(elementAccessor,self->expanded);
        accessor_bufr_data_element_set_elementsDescriptorsIndex(elementAccessor,self->elementsDescriptorsIndex);
        accessor_bufr_data_element_set_numericValues(elementAccessor,self->numericValues);
        accessor_bufr_data_element_set_stringValues(elementAccessor,self->stringValues);
        accessor_bufr_data_element_set_compressedData(elementAccessor,self->compressedData);
        accessor_bufr_data_element_set_type(elementAccessor,self->expanded->v[idx]->type);
        accessor_bufr_data_element_set_numberOfSubsets(elementAccessor,self->numberOfSubsets);
        accessor_bufr_data_element_set_subsetNumber(elementAccessor,subset);

        attribute=create_attribute("index",section,GRIB_TYPE_LONG,0,0,count,flags);
        grib_accessor_add_attribute(elementAccessor,attribute);

        sprintf(code,"%06ld",self->expanded->v[idx]->code);
        attribute=create_attribute("code",section,GRIB_TYPE_STRING,code,0,0,flags);
        grib_accessor_add_attribute(elementAccessor,attribute);

        attribute=create_attribute("units",section,GRIB_TYPE_STRING,self->expanded->v[idx]->units,0,0,GRIB_ACCESSOR_FLAG_DUMP);
        grib_accessor_add_attribute(elementAccessor,attribute);

        attribute=create_attribute("scale",section,GRIB_TYPE_LONG,0,0,self->expanded->v[idx]->scale,flags);
        grib_accessor_add_attribute(elementAccessor,attribute);

        attribute=create_attribute("reference",section,GRIB_TYPE_DOUBLE,0,self->expanded->v[idx]->reference,0,flags);
        grib_accessor_add_attribute(elementAccessor,attribute);

        attribute=create_attribute("width",section,GRIB_TYPE_LONG,0,0,self->expanded->v[idx]->width,flags);
        grib_accessor_add_attribute(elementAccessor,attribute);
        break;
    }

    return elementAccessor;
}

#define IS_QUALIFIER(a) (a==8 || a==1 || a==2 || a==4 || a==5 || a==6 || a==7)
#define NUMBER_OF_QUALIFIERS_PER_CATEGORY 256
#define NUMBER_OF_QUALIFIERS_CATEGORIES 7
#define MAX_NUMBER_OF_BITMAPS 5

static int number_of_qualifiers=NUMBER_OF_QUALIFIERS_PER_CATEGORY*NUMBER_OF_QUALIFIERS_CATEGORIES;

static GRIB_INLINE int significanceQualifierIndex(int X,int Y)
{
    int a[]={-1,0,1,-1,2,3,4,5,6};
    int ret=Y+a[X]*NUMBER_OF_QUALIFIERS_PER_CATEGORY;
    Assert(ret>0);
    return ret;
}

static GRIB_INLINE void reset_deeper_qualifiers(grib_accessor* significanceQualifierGroup[],int* significanceQualifierDepth, int depth)
{
    int i;
    for (i=0;i<number_of_qualifiers;i++) {
        if (significanceQualifierDepth[i]>depth) {
            significanceQualifierGroup[i]=0;
        }
    }
}

typedef struct bitmap_s {
    grib_accessors_list* cursor;
    grib_accessors_list* referredElement;
} bitmap_s;

static grib_accessor* get_element_from_bitmap(grib_accessor* a,bitmap_s* bitmap)
{
    int ret;
    long bitmapVal=1;
    size_t len;
    grib_accessor_bufr_data_array *self =(grib_accessor_bufr_data_array*)a;

    while (bitmapVal) {
        len=1;
        if (bitmap->cursor && bitmap->cursor->accessor) {
            ret=grib_unpack_long(bitmap->cursor->accessor,&bitmapVal,&len);
        } else {
            return NULL;
        }
        if (ret!=0) return NULL;
        bitmap->cursor=bitmap->cursor->next;
        if(bitmap->referredElement) bitmap->referredElement=bitmap->referredElement->next;
    }

    return bitmap->referredElement ? bitmap->referredElement->prev->accessor : NULL ;
}

static GRIB_INLINE void reset_qualifiers(grib_accessor* significanceQualifierGroup[])
{
    int i;
    for (i=0;i<number_of_qualifiers;i++)
        significanceQualifierGroup[i]=0;
}

static int create_keys(grib_accessor* a)
{
    grib_accessor_bufr_data_array *self =(grib_accessor_bufr_data_array*)a;
    int err=0;
    grib_accessor* elementAccessor=0;
    grib_accessor* associatedFieldAccessor=0;
    long iss,end,elementsInSubset,ide;
    grib_section* section=NULL;
    grib_section* rootSection=NULL;
    bufr_descriptor* descriptor;
    grib_section* sectionUp=0;
    grib_section* groupSection=0;
    long groupNumber=0;
    long indexOfGroupNumber=0;
    int depth;
    int idx;
    grib_context* c=a->parent->h->context;
    int qualityPresent=0;
    bitmap_s bitmap={0,};
    int extraElement=0;

    grib_accessor* gaGroup=0;
    grib_action creatorGroup = {0, };
    grib_accessor* significanceQualifierGroup[NUMBER_OF_QUALIFIERS_PER_CATEGORY*NUMBER_OF_QUALIFIERS_CATEGORIES]={0,};
    int significanceQualifierDepth[NUMBER_OF_QUALIFIERS_PER_CATEGORY*NUMBER_OF_QUALIFIERS_CATEGORIES]={0,};

    grib_accessor* bitmapGroup[MAX_NUMBER_OF_BITMAPS]={0,};
    int bitmapDepth[MAX_NUMBER_OF_BITMAPS]={0,};
    int bitmapSize[MAX_NUMBER_OF_BITMAPS]={0,};
    grib_accessors_list* bitmapStart[MAX_NUMBER_OF_BITMAPS]={0,};
    int bitmapIndex=-1;
    int incrementBitmapIndex=1;
    grib_accessor* elementFromBitmap=NULL;
    int reuseBitmap=0;
    int i,dump=1,count=0;
    int forceGroupClosure=0;

    creatorGroup.op         = "bufr_group";
    creatorGroup.name="groupNumber";
    creatorGroup.name_space = "";
    creatorGroup.flags     = GRIB_ACCESSOR_FLAG_DUMP;
    creatorGroup.set        = 0;


    if (self->dataAccessors) grib_accessors_list_delete(c,self->dataAccessors);
    self->dataAccessors=grib_accessors_list_create(c);

    end= self->compressedData ? 1 : self->numberOfSubsets;
    groupNumber=1;

    gaGroup = grib_accessor_factory(self->dataKeys, &creatorGroup, 0, NULL);
    gaGroup->bufr_group_number=groupNumber;
    gaGroup->sub_section=grib_section_create(a->parent->h,gaGroup);
    section=gaGroup->sub_section;
    rootSection=section;
    sectionUp=self->dataKeys;
    accessor_constant_set_type(gaGroup,GRIB_TYPE_LONG);
    accessor_constant_set_dval(gaGroup,groupNumber);
    self->dataKeys->block->first=0;
    self->dataKeys->block->last=0;
    grib_push_accessor(gaGroup,self->dataKeys->block);

    indexOfGroupNumber=0;
    depth=0;
    extraElement=0;

    for (iss=0;iss<end;iss++) {
      qualityPresent=0;
      forceGroupClosure=0;
      elementsInSubset= self->compressedData ? grib_iarray_used_size(self->elementsDescriptorsIndex->v[0]) :
        grib_iarray_used_size(self->elementsDescriptorsIndex->v[iss]);
      associatedFieldAccessor=NULL;
      for (ide=0;ide<elementsInSubset;ide++) {
        idx = self->compressedData ? self->elementsDescriptorsIndex->v[0]->v[ide] :
          self->elementsDescriptorsIndex->v[iss]->v[ide] ;

        descriptor=self->expanded->v[idx];
        elementFromBitmap=NULL;
        if (descriptor->F==0 && IS_QUALIFIER(descriptor->X)
            && self->unpackMode==CODES_BUFR_UNPACK_STRUCTURE) {
          int sidx=significanceQualifierIndex(descriptor->X,descriptor->Y);
          groupNumber++;

          if (significanceQualifierGroup[sidx]) {
            groupSection=significanceQualifierGroup[sidx]->parent;
            depth=significanceQualifierDepth[sidx];
            reset_deeper_qualifiers(significanceQualifierGroup,significanceQualifierDepth,depth);
          } else {
            /* if (forceGroupClosure) { */
            /* groupSection=sectionUp; */
            /* forceGroupClosure=0; */
            /* depth=0; */
            /* } else { */
            groupSection=section;
            depth++;
            /* } */
          }

          gaGroup = grib_accessor_factory(groupSection, &creatorGroup, 0, NULL);
          gaGroup->sub_section=grib_section_create(a->parent->h,gaGroup);
          gaGroup->bufr_group_number=groupNumber;
          accessor_constant_set_type(gaGroup,GRIB_TYPE_LONG);
          accessor_constant_set_dval(gaGroup,groupNumber);
          grib_push_accessor(gaGroup,groupSection->block);

          section=gaGroup->sub_section;
          sectionUp=gaGroup->parent;

          significanceQualifierGroup[sidx]=gaGroup;
          significanceQualifierDepth[sidx]=depth;
          incrementBitmapIndex=1;
          dump=1;
        } else if (descriptor->code == 31031 && incrementBitmapIndex!=0) {
          /* bitmap */
          bitmapIndex++;
          groupNumber++;
          incrementBitmapIndex=0;
          bitmapStart[bitmapIndex]=grib_accessors_list_last(self->dataAccessors);
          bitmapSize[bitmapIndex]=1;
          if (self->expanded->v[idx-1]->code ==31002 || self->expanded->v[idx-1]->code==31001)
            extraElement=1;
          if (bitmapGroup[bitmapIndex]) {
            groupSection=bitmapGroup[bitmapIndex]->parent;
            depth=bitmapDepth[bitmapIndex];
            reset_deeper_qualifiers(significanceQualifierGroup,significanceQualifierDepth,depth);
            reset_deeper_qualifiers(bitmapGroup,bitmapDepth,depth);
          } else {
            groupSection=section;
            depth++;
          }
          gaGroup = grib_accessor_factory(groupSection, &creatorGroup, 0, NULL);
          gaGroup->sub_section=grib_section_create(a->parent->h,gaGroup);
          gaGroup->bufr_group_number=groupNumber;
          accessor_constant_set_type(gaGroup,GRIB_TYPE_LONG);
          accessor_constant_set_dval(gaGroup,groupNumber);
          grib_push_accessor(gaGroup,groupSection->block);

          section=gaGroup->sub_section;
          sectionUp=gaGroup->parent;
          bitmapGroup[bitmapIndex]=gaGroup;
          bitmapDepth[bitmapIndex]=depth;
          dump=1;
        } else if (descriptor->code == 31031) {
          dump=1;
          bitmapSize[bitmapIndex]++;
          bitmap.cursor=0;
        } else if (descriptor->code == 222000 || descriptor->code == 224000 ) {
          bitmap.referredElement=NULL;
          qualityPresent=1;
          incrementBitmapIndex=1;
          dump=1;
        } else if (descriptor->code == 236000 ) {
          bitmap.referredElement=NULL;
          bitmap.cursor=0;
          reuseBitmap=1;
          extraElement=1;
          dump=1;
        } else if (descriptor->code == 236000 || descriptor->code == 237000 ) {
          bitmap.referredElement=NULL;
          bitmap.cursor=0;
          reuseBitmap=1;
          dump=1;
        } else if (descriptor->code == 237255 ) {
          reuseBitmap=0;
          incrementBitmapIndex=1;
          bitmap.cursor=0;
          dump=1;
        } else if ( ( descriptor->X==33 || descriptor->isMarker )  && qualityPresent) {
          if (!bitmap.referredElement) {
            bitmap.cursor=bitmapStart[bitmapIndex]->next;
            bitmap.referredElement=bitmapStart[bitmapIndex];
            for (i=0;i<bitmapSize[bitmapIndex]+extraElement && bitmap.referredElement!=NULL ;i++) {
              bitmap.referredElement=bitmap.referredElement->prev;
            }
          }
          elementFromBitmap=get_element_from_bitmap(a,&bitmap);
          dump=1;
          /* } else if ( descriptor->Y==1 && IS_QUALIFIER(self->expanded->v[idx-1]->X)==0) { */
          /* forceGroupClosure=1; */
          /* reset_qualifiers(significanceQualifierGroup); */
      }

      count++;
      elementAccessor=create_accessor_from_descriptor(a,associatedFieldAccessor,section,ide,iss,dump,count);
      associatedFieldAccessor=NULL;
      if (elementFromBitmap && self->unpackMode==CODES_BUFR_UNPACK_STRUCTURE) {
        grib_accessor_add_attribute(elementFromBitmap,elementAccessor);
      } else if (elementAccessor) {

        if (descriptor->code == 999999) {
          associatedFieldAccessor=elementAccessor;
        } else {
          grib_push_accessor(elementAccessor,section->block);
          grib_accessors_list_push(self->dataAccessors,elementAccessor);
        }
      }
      }
    }

    return err;
}

static int process_elements(grib_accessor* a,int flag)
{
    int err=0;
    int associatedFieldWidth=0,localDescriptorWidth=0;
    long  inr,innr,ir;
    long n[MAX_NESTED_REPLICATIONS]={0,};
    long nn[MAX_NESTED_REPLICATIONS]={0,};
    long numberOfElementsToRepeat[MAX_NESTED_REPLICATIONS]={0,};
    long numberOfRepetitions[MAX_NESTED_REPLICATIONS]={0,};
    long startRepetition[MAX_NESTED_REPLICATIONS]={0,};
    long numberOfNestedRepetitions=0;
    unsigned char* data=0;
    int i;
    grib_iarray* elementsDescriptorsIndex=0;
    long localReference=0,width=0;
    long pos=0;
    int iss,end,elementIndex,index;
    long numberOfDescriptors;
    long totalSize;
    bufr_descriptor** descriptors=0;
    long icount;
    int decoding=0,do_clean=1;
    grib_buffer* buffer=NULL;
    codec_element_proc codec_element;
    codec_replication_proc codec_replication;

    grib_darray* dval = NULL;
    grib_sarray* sval = NULL;
    grib_accessor_bufr_data_array *self =(grib_accessor_bufr_data_array*)a;

    grib_handle* h=a->parent->h;
    grib_context* c=h->context;

    totalSize=self->bitsToEndData;

    switch (flag) {
      case PROCESS_DECODE:
        if (!self->do_decode) return 0;
        self->do_decode=0;
        buffer=h->buffer;
        decoding=1;
        do_clean=1;
        pos=a->offset*8;
        codec_element=&decode_element;
        codec_replication=&decode_replication;
        break;
      case PROCESS_NEW_DATA:
        buffer=grib_create_growable_buffer(c);
        decoding=0;
        do_clean=1;
        self->do_decode=1;
        pos=0;
        codec_element=&encode_new_element;
        codec_replication=&encode_new_replication;
        break;
      case PROCESS_ENCODE:
        buffer=grib_create_growable_buffer(c);
        decoding=0;
        do_clean=0;
        self->do_decode=0;
        pos=0;
        codec_element=&encode_element;
        codec_replication=&encode_replication;
        break;
      default :
        return GRIB_NOT_IMPLEMENTED;
    }
    data=(unsigned char*)buffer->data;

    err=get_descriptors(a);
    if (err) return err;

    descriptors=self->expanded->v;

    if (do_clean==1 && self->numericValues) {
      grib_vdarray_delete_content(c,self->numericValues);
      grib_vdarray_delete(c,self->numericValues);
      grib_vsarray_delete_content(c,self->stringValues);
      grib_vsarray_delete(c,self->stringValues);
    }

    if (flag!=PROCESS_ENCODE) {
      self->numericValues=grib_vdarray_new(c,100,100);
      self->stringValues=grib_vsarray_new(c,10,10);

      if (self->elementsDescriptorsIndex) grib_viarray_delete(c,self->elementsDescriptorsIndex);
      self->elementsDescriptorsIndex=grib_viarray_new(c,100,100);
    }

    end= self->compressedData ? 1 : self->numberOfSubsets;

    numberOfDescriptors=grib_bufr_descriptors_array_used_size(self->expanded);

    for (iss=0;iss<end;iss++) {
      icount=1;
      grib_context_log(c, GRIB_LOG_DEBUG,"BUFR data processing: subsetNumber=%ld", iss+1);

      if (flag!=PROCESS_ENCODE) {
        elementsDescriptorsIndex=grib_iarray_new(c,100,100);
        if (!self->compressedData) {
          dval=grib_darray_new(c,100,100);
          /* sval=grib_sarray_new(c,10,10); */
        }
      } else {
        elementsDescriptorsIndex=self->elementsDescriptorsIndex->v[iss];
      }
      elementIndex=0;

      for (i=0;i<numberOfDescriptors;i++) {
        grib_context_log(c, GRIB_LOG_DEBUG,"BUFR data processing: elementNumber=%ld code=%6.6ld", icount++,descriptors[i]->code);
        switch(descriptors[i]->F) {
          case 0:
            /* Table B element */
            if (flag!=PROCESS_ENCODE) grib_iarray_push(elementsDescriptorsIndex,i);
            if (descriptors[i]->code==31031 && !is_bitmap_start_defined(self))
              self->bitmapStart=grib_iarray_used_size(elementsDescriptorsIndex)-1;
            err=codec_element(c,self,iss,buffer,data,&pos,i,elementIndex,dval,sval);
            if (err) return err;
            elementIndex++;
            break;
          case 1:
            /* Delayed replication */
            inr=numberOfNestedRepetitions;
            numberOfNestedRepetitions++;
            Assert(numberOfNestedRepetitions<=MAX_NESTED_REPLICATIONS);
            numberOfElementsToRepeat[inr]=descriptors[i]->X;
            n[inr]=numberOfElementsToRepeat[inr];
            i++;

            err=codec_replication(c,self,iss,buffer,data,&pos,i,elementIndex,dval,&(numberOfRepetitions[inr]));
            if (err) return err;

            startRepetition[inr]=i;
            nn[inr]=numberOfRepetitions[inr];
            if (flag!=PROCESS_ENCODE) grib_iarray_push(elementsDescriptorsIndex,i);
            elementIndex++;
            if (numberOfRepetitions[inr]==0) {
              i+=numberOfElementsToRepeat[inr];
              if (inr>0) n[inr-1]-=numberOfElementsToRepeat[inr]+2;
              numberOfNestedRepetitions--;
            }
            continue;
          case 2:
            /* Operator */
            associatedFieldWidth=0;
            localDescriptorWidth=0;
            switch(descriptors[i]->X) {
              case 5:
                descriptors[i]->width=descriptors[i]->Y*8;
                descriptors[i]->type=BUFR_DESCRIPTOR_TYPE_STRING; 
                err=codec_element(c,self,iss,buffer,data,&pos,i,elementIndex,dval,sval);
                if (err) return err;
                if (flag!=PROCESS_ENCODE) grib_iarray_push(elementsDescriptorsIndex,i);
                elementIndex++;
                break;
              case 22:
              case 26:
              case 27:
              case 29:
              case 30:
              case 31:
              case 33:
              case 34:
              case 38:
              case 39:
              case 40:
              case 41:
              case 42:
                if (flag!=PROCESS_ENCODE) grib_iarray_push(elementsDescriptorsIndex,i);
                if (decoding) push_zero_element(self,dval);
                elementIndex++;
                break;
              case 23:
                /* substituted values marker operator */
              case 24:
                /*first-order statistical values marker operator*/
              case 32:
                /*replaced/retained values marker operator*/
                if (descriptors[i]->Y==255) {
                  index=get_next_bitmap_descriptor_index(self,elementsDescriptorsIndex,dval);
                  err=codec_element(c,self,iss,buffer,data,&pos,index,elementIndex,dval,sval);
                  if (err) return err;
                  /* self->expanded->v[index] */
                  if (flag!=PROCESS_ENCODE) grib_iarray_push(elementsDescriptorsIndex,i);
                  elementIndex++;
                } else {
                  if (flag!=PROCESS_ENCODE) grib_iarray_push(elementsDescriptorsIndex,i);
                  if (decoding) push_zero_element(self,dval);
					        elementIndex++;
                }
                break;
              case 25:
                /*difference statistical values marker operator*/
                break;
              case 35:
                /* cancel bitmap */
                if (flag!=PROCESS_ENCODE) {
                  grib_iarray_push(elementsDescriptorsIndex,i);
                  if (decoding) push_zero_element(self,dval);
                  if (descriptors[i]->Y==0) cancel_bitmap(self);
                }
				        elementIndex++;
                break;
              case 36:
                /* bitmap */
                if (flag!=PROCESS_ENCODE) {
                  grib_iarray_push(elementsDescriptorsIndex,i);
                  if (decoding) push_zero_element(self,dval);
                  build_bitmap(self,data,&pos,elementsDescriptorsIndex,i);
                }
				        elementIndex++;
                break;
              case 37:
                /* reuse defined bitmap */
                if (flag!=PROCESS_ENCODE) {
                  grib_iarray_push(elementsDescriptorsIndex,i);
                  if (decoding) push_zero_element(self,dval);
                  if (descriptors[i]->Y==0) restart_bitmap(self);
                  /* cancel reuse */
                  else cancel_bitmap(self);
                }
				        elementIndex++;
                break;
              default :
                grib_context_log(c,GRIB_LOG_ERROR,"unsupported operator %d\n",descriptors[i]->X);
                return GRIB_INTERNAL_ERROR;
            }
            break;
          case 9:
            /* associated field */
            if (descriptors[i]->X==99 && descriptors[i]->Y==999) {
              err=codec_element(c,self,iss,buffer,data,&pos,i,elementIndex,dval,sval);
              if (err) return err;
              if (flag!=PROCESS_ENCODE) grib_iarray_push(elementsDescriptorsIndex,i);
              elementIndex++;
            } else {
              return GRIB_INTERNAL_ERROR;
            }
            break;
          default:
            err=GRIB_INTERNAL_ERROR;
            return err;
        }

        /* delayed repetition check */
        innr=numberOfNestedRepetitions-1;
        for (ir=innr;ir>=0;ir--) {
          if (nn[ir])  {
            if (n[ir]>1) {
              n[ir]--;
              break;
            } else {
              n[ir]=numberOfElementsToRepeat[ir];
              nn[ir]--;
              if (nn[ir]) {
                i=startRepetition[ir];
                break;
              } else {
                if (ir>0)  {
                  n[ir-1]-=numberOfElementsToRepeat[ir]+2;
                }
                i=startRepetition[ir]+numberOfElementsToRepeat[ir];
                numberOfNestedRepetitions--;
              }
            }
          } else {
            if (ir==0) {
              i=startRepetition[ir]+numberOfElementsToRepeat[ir]+1;
              numberOfNestedRepetitions=0;
            } else {
              numberOfNestedRepetitions--;
            }
          }
        }

      }
        if (flag!=PROCESS_ENCODE) grib_viarray_push(c,self->elementsDescriptorsIndex,elementsDescriptorsIndex);
        if (decoding && !self->compressedData) {
            grib_vdarray_push(c,self->numericValues,dval);
        }
    }

    if(decoding) {
        err=create_keys(a);
        self->bitsToEndData=totalSize;
    } else {
        long totalLength;
        long section4Length;

        grib_get_long(a->parent->h,"totalLength",&totalLength);
        totalLength+=buffer->ulength-a->length;
        grib_get_long(a->parent->h,"section4Length",&section4Length);
        section4Length+=buffer->ulength-a->length;

        grib_buffer_replace(a,buffer->data,buffer->ulength,1,1);
        grib_buffer_delete(c,buffer);

        grib_set_long(a->parent->h,"totalLength",totalLength);
        grib_set_long(a->parent->h,"section4Length",section4Length);
    }

    return err;
}

static void dump(grib_accessor* a, grib_dumper* dumper)
{
    /* grib_accessor_bufr_data_array *self =(grib_accessor_bufr_data_array*)a; */
    /* int err=process_elements(a,PROCESS_DECODE); */

    /* grib_dump_section(dumper,a,self->dataKeys->block); */

    return;
}

static int value_count(grib_accessor* a,long* count)
{
    int err=0,l;
    long i,subsetNumber=0;
    grib_accessor_bufr_data_array *self =(grib_accessor_bufr_data_array*)a;
    grib_context* c=a->parent->h->context;

    err=process_elements(a,PROCESS_DECODE);
    if (err) return err;

    err=grib_get_long(a->parent->h,self->subsetNumberName,&subsetNumber);
    if (err) return err;
    if (subsetNumber>self->numberOfSubsets) {
        err=GRIB_INVALID_KEY_VALUE;
        grib_context_log(c,GRIB_LOG_ERROR,"%s=%ld is too big, %s=%ld",self->subsetNumberName,self->numberOfSubsetsName);
        return err;
    }

    if (self->compressedData) {
        l=grib_vdarray_used_size(self->numericValues);

        *count=l;
        if (subsetNumber<=0) {
            *count *= self->numberOfSubsets;
        }
    } else {
        if (subsetNumber>0) {
            *count=grib_iarray_used_size(self->elementsDescriptorsIndex->v[subsetNumber-1]);
        } else {
            *count=0;
            for (i=0;i<self->numberOfSubsets;i++)
                *count+=grib_iarray_used_size(self->elementsDescriptorsIndex->v[i]);
        }
    }

    return err;
}

static int unpack_double(grib_accessor* a, double* val, size_t *len)
{
    int err=0,i,k,ii;
    long n=0;
    int proc_flag=PROCESS_DECODE;
    size_t l=0,elementsInSubset;
    long numberOfSubsets=0;
    grib_accessor_bufr_data_array *self =(grib_accessor_bufr_data_array*)a;
    grib_context* c=a->parent->h->context;

    if (self->unpackMode == CODES_BUFR_NEW_DATA) proc_flag=PROCESS_NEW_DATA;

    err=process_elements(a,proc_flag);
    if (err) return err;
    if (!val) return err;

    l=grib_vdarray_used_size(self->numericValues);
    err=grib_get_long(a->parent->h,self->subsetNumberName,&n);
    if (err) return err;
    err=grib_get_long(a->parent->h,self->numberOfSubsetsName,&numberOfSubsets);
    if (err) return err;
    if (n>numberOfSubsets) {
        err=GRIB_INVALID_KEY_VALUE;
        grib_context_log(c,GRIB_LOG_ERROR,"%s=%ld is too big, %s=%ld",self->subsetNumberName,self->numberOfSubsetsName);
        return err;
    }

    if (self->compressedData) {
        if (n>0) {
            for (i=0;i<l;i++) {
                val[i]=self->numericValues->v[i]->n > 1 ? self->numericValues->v[i]->v[n-1] : self->numericValues->v[i]->v[0];
            }
        } else {
            ii=0;
            for (k=0;k<numberOfSubsets;k++) {
                for (i=0;i<l;i++) {
                    val[ii++]=self->numericValues->v[i]->n > 1 ? self->numericValues->v[i]->v[k] : self->numericValues->v[i]->v[0];
                }
            }
        }
    } else {
        if (n>0) {
            elementsInSubset=grib_iarray_used_size(self->elementsDescriptorsIndex->v[n]);
            for (i=0;i<elementsInSubset;i++) val[i]=self->numericValues->v[n-1]->v[i];
        } else {
            ii=0;
            for (k=0;k<numberOfSubsets;k++) {
                elementsInSubset=grib_iarray_used_size(self->elementsDescriptorsIndex->v[k]);
                for (i=0;i<elementsInSubset;i++) {
                    val[ii++]=self->numericValues->v[k]->v[i];
                }
            }
        }
    }

    return GRIB_SUCCESS;
}

static void destroy(grib_context* c,grib_accessor* a)
{
    grib_accessor_bufr_data_array *self =(grib_accessor_bufr_data_array*)a;
    self_clear(c,self);
    if (self->dataAccessors) grib_accessors_list_delete(c,self->dataAccessors);
}
