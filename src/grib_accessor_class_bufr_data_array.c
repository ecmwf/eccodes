/*
 * Copyright 2005-2014 ECMWF.
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
   MEMBERS= const char* offsetSection4Name
   MEMBERS= const char* offsetBeforeDataName
   MEMBERS= const char* offsetEndSection4Name
   MEMBERS= const char* section4LengthName
   MEMBERS= const char* numberOfDataSubsetsName
   MEMBERS= const char* subsetNumberName
   MEMBERS= const char* expandedDescriptorsName
   MEMBERS= const char* typeName
   MEMBERS= const char* referenceName
   MEMBERS= const char* scaleName
   MEMBERS= const char* widthName
   MEMBERS= const char* stringValuesName
   MEMBERS= const char* elementsFXYName
   MEMBERS= const char* compressedDataName
   MEMBERS= long* expandedDescriptors
   MEMBERS= char** type
   MEMBERS= long* reference
   MEMBERS= double* factor
   MEMBERS= long* width
   MEMBERS= long subsetNumber
   MEMBERS= long numberOfDataSubsets
   MEMBERS= long compressedData
   MEMBERS= size_t numberOfDescriptors
   MEMBERS= grib_vdarray* numericValues
   MEMBERS= grib_vsarray* stringValues
   MEMBERS= grib_viarray* elementsFXY
   MEMBERS= int do_decode

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
	const char* numberOfDataSubsetsName;
	const char* subsetNumberName;
	const char* expandedDescriptorsName;
	const char* typeName;
	const char* referenceName;
	const char* scaleName;
	const char* widthName;
	const char* stringValuesName;
	const char* elementsFXYName;
	const char* compressedDataName;
	long* expandedDescriptors;
	char** type;
	long* reference;
	double* factor;
	long* width;
	long subsetNumber;
	long numberOfDataSubsets;
	long compressedData;
	size_t numberOfDescriptors;
	grib_vdarray* numericValues;
	grib_vsarray* stringValues;
	grib_viarray* elementsFXY;
	int do_decode;
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
    0,             		/* clear          */
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

  a->sub_section           = grib_section_create(a->parent->h,a);

  self->offsetSection4Name = grib_arguments_get_name(a->parent->h,params,n++);
  self->offsetBeforeDataName = grib_arguments_get_name(a->parent->h,params,n++);
  self->offsetEndSection4Name = grib_arguments_get_name(a->parent->h,params,n++);
  self->section4LengthName = grib_arguments_get_name(a->parent->h,params,n++);
  self->numberOfDataSubsetsName = grib_arguments_get_name(a->parent->h,params,n++);
  self->subsetNumberName    = grib_arguments_get_name(a->parent->h,params,n++);
  self->expandedDescriptorsName = grib_arguments_get_name(a->parent->h,params,n++);
  self->typeName = grib_arguments_get_name(a->parent->h,params,n++);
  self->referenceName = grib_arguments_get_name(a->parent->h,params,n++);
  self->scaleName = grib_arguments_get_name(a->parent->h,params,n++);
  self->widthName = grib_arguments_get_name(a->parent->h,params,n++);
  self->stringValuesName = grib_arguments_get_name(a->parent->h,params,n++);
  self->elementsFXYName = grib_arguments_get_name(a->parent->h,params,n++);
  self->compressedDataName = grib_arguments_get_name(a->parent->h,params,n++);
  self->do_decode=1;
  self->elementsFXY=0;
  self->numericValues=0;
  self->stringValues=0;
  a->parent->h->unpacked=0;

  a->length = init_length(a);

  /* Assert(a->length>=0); */
}

static void self_clear(grib_context* c,grib_accessor_bufr_data_array* self) {
  int i=0;
  for (i=0;i<self->numberOfDescriptors;i++) {
    grib_context_free(c,self->type[i]);
  }
  grib_context_free(c,self->expandedDescriptors);
  grib_context_free(c,self->type);
  grib_context_free(c,self->reference);
  grib_context_free(c,self->width);
  grib_context_free(c,self->factor);
}
static int  get_native_type(grib_accessor* a){
  return GRIB_TYPE_DOUBLE;
}

static long byte_count(grib_accessor* a){
  return a->length;

}

static long byte_offset(grib_accessor* a){
  return a->offset;
}

static long next_offset(grib_accessor* a){
  return a->offset+a->length;
}

static void update_size(grib_accessor* a,size_t s)
{
    grib_context_log(a->parent->h->context,GRIB_LOG_DEBUG,"updating size of %s old %ld new %ld",a->name,a->length,s);
  a->length = s;
  Assert(a->length>=0);
}

static int compare(grib_accessor* a, grib_accessor* b) {
  return GRIB_NOT_IMPLEMENTED;
}

static int pack_long(grib_accessor* a, const long* val, size_t *len)
{
  grib_accessor_bufr_data_array *self =(grib_accessor_bufr_data_array*)a;
  self->do_decode=1;
  a->parent->h->unpacked=0;

  return GRIB_NOT_IMPLEMENTED;
}

static int pack_double(grib_accessor* a, const double* val, size_t *len)
{
  grib_accessor_bufr_data_array *self =(grib_accessor_bufr_data_array*)a;
  self->do_decode=1;
  a->parent->h->unpacked=0;
  return GRIB_NOT_IMPLEMENTED;
}

static int get_descriptors(grib_accessor* a) {
    int err=0;
    size_t size=0;
    long* scale=0;
    double* factor=0;
    int i=0;
    grib_accessor_bufr_data_array *self =(grib_accessor_bufr_data_array*)a;
    grib_accessor* expandedDescriptors=0;
    grib_handle* h=a->parent->h;
    grib_context* c=a->parent->h->context;

    expandedDescriptors=grib_find_accessor(a->parent->h,self->expandedDescriptorsName);
    if (!expandedDescriptors) {
          grib_context_log(a->parent->h->context,GRIB_LOG_ERROR,
              "unable to find accessor %s",self->expandedDescriptorsName);
          return GRIB_NOT_FOUND;
    }

    if (self->numberOfDescriptors) self_clear(c,self);
    err=_grib_get_size(a->parent->h,expandedDescriptors,&(self->numberOfDescriptors));
    if (err) return err;

    self->expandedDescriptors=grib_context_malloc_clear(a->parent->h->context,sizeof(long)*self->numberOfDescriptors);
    if (!self->expandedDescriptors) {
      grib_context_log(a->parent->h->context,GRIB_LOG_FATAL,
          "unable to allocate %ld bytes",(long)(self->numberOfDescriptors));
      return GRIB_OUT_OF_MEMORY;
    }

    size=self->numberOfDescriptors;
    if( (err = grib_unpack_long(expandedDescriptors,self->expandedDescriptors,&(self->numberOfDescriptors)))
                != GRIB_SUCCESS)
      return err;

    size=self->numberOfDescriptors;
    self->type=(char**)grib_context_malloc_clear(c,size*sizeof(char*));
    err=grib_get_string_array(h,self->typeName,self->type,&size);

    size=self->numberOfDescriptors;
    self->reference=grib_context_malloc_clear(c,size*sizeof(long));
    err=grib_get_long_array(h,self->referenceName,self->reference,&size);

    size=self->numberOfDescriptors;
    scale=grib_context_malloc_clear(c,size*sizeof(long));
    factor=grib_context_malloc_clear(c,size*sizeof(double));
    err=grib_get_long_array(h,self->scaleName,scale,&size);
    for (i=0;i<size;i++) {
      if (scale[i]==GRIB_MISSING_LONG && self->expandedDescriptors[i]<100000) {
        grib_context_log(c,GRIB_LOG_FATAL,"Missing table entry for descriptor %ld",self->expandedDescriptors[i]);
        return GRIB_MISSING_BUFR_ENTRY;
      }
      if (scale[i]!=GRIB_MISSING_LONG) factor[i]=grib_power(-scale[i],10);
    }
    self->factor=factor;
    grib_context_free(c,scale);

    size=self->numberOfDescriptors;
    self->width=grib_context_malloc_clear(c,size*sizeof(long));
    err=grib_get_long_array(h,self->widthName,self->width,&size);

    err=grib_get_long(h,self->numberOfDataSubsetsName,&(self->numberOfDataSubsets));
    err=grib_get_long(h,self->compressedDataName,&(self->compressedData));
    err=grib_get_long(h,self->subsetNumberName,&(self->subsetNumber));

    return err;
}

static grib_sarray* decode_string_array(grib_context* c,unsigned char* data,long* pos, int i,
              grib_accessor_bufr_data_array* self,
              int extraWidth,int extraScale,double referenceFactor,int associatedFieldWidth,int localDescriptorWidth) {
  grib_sarray* ret=NULL;
  char* sval=0;
  int j,modifiedWidth,modifiedReference,width;
  double modifiedFactor;
  modifiedWidth= extraWidth ? self->width[i]+extraWidth : self->width[i];
  modifiedReference= referenceFactor ? self->reference[i]*referenceFactor : self->reference[i];
  modifiedFactor= extraScale ? self->factor[i]*grib_power(-extraScale,10) : self->factor[i];

  ret=grib_sarray_new(c,10,10);
  sval=grib_context_malloc_clear(c,modifiedWidth/8+1);
  grib_decode_string(data,pos,modifiedWidth/8,sval);
  width=grib_decode_unsigned_long(data,pos,6);
  ret=grib_sarray_new(c,10,10);
  if (width) {
    grib_context_free(c,sval);
    for (j=1;j<self->numberOfDataSubsets;j++) {
      sval=grib_context_malloc_clear(c,width/8+1);
      grib_decode_string(data,pos,width/8,sval);
      grib_sarray_push(c,ret,sval);
    }
  } else {
    grib_sarray_push(c,ret,sval);
  }

  return ret;
}

static grib_darray* decode_double_array(grib_context* c,unsigned char* data,long* pos,int i,
              grib_accessor_bufr_data_array* self,int canBeMissing,
              int extraWidth,int extraScale,double referenceFactor,int associatedFieldWidth,int localDescriptorWidth) {
  grib_darray* ret=NULL;
  int j;
  long lval;
  long localReference;
  int width,modifiedWidth,modifiedReference;
  double modifiedFactor,dval;
  modifiedReference= self->reference[i]*referenceFactor;
  modifiedFactor= extraScale ? self->factor[i]*grib_power(-extraScale,10) : self->factor[i];

  if (localDescriptorWidth) {
    modifiedWidth=localDescriptorWidth;
  } else {
    modifiedWidth= self->width[i]+extraWidth;
  }

  lval=grib_decode_unsigned_long(data,pos,modifiedWidth);
  localReference=lval+modifiedReference;
  width=grib_decode_unsigned_long(data,pos,6);
  ret=grib_darray_new(c,100,100);
  if (width) {
    for (j=0;j<self->numberOfDataSubsets;j++) {
      lval=grib_decode_unsigned_long(data,pos,width);
      if (grib_is_all_bits_one(lval,width) && canBeMissing) {
        dval=GRIB_MISSING_DOUBLE;
      } else {
        dval=(lval+localReference)*modifiedFactor;
      }
      grib_darray_push(c,ret,dval);
    }
  } else {
    if (grib_is_all_bits_one(lval,modifiedWidth) && canBeMissing) {
      dval=GRIB_MISSING_DOUBLE;
    } else {
      dval=localReference*modifiedFactor;
    }
    grib_darray_push(c,ret,dval);
  }
  /*
  switch (width) {
    case 0:
      if (!grib_is_all_bits_one(lval,modifiedWidth) || self->expandedDescriptors[i]==31031) {
        dval=localReference*modifiedFactor;
      } else dval=GRIB_MISSING_DOUBLE;
      grib_darray_push(c,ret,dval);
      break;
    case 1:
      for (j=0;j<self->numberOfDataSubsets;j++) {
        lval=grib_decode_unsigned_long(data,pos,width);
        if (self->expandedDescriptors[i]!=31031 && lval==1) {
          dval=GRIB_MISSING_DOUBLE;
        } else {
          dval=localReference*modifiedFactor;
        }
        grib_darray_push(c,ret,dval);
      }
      break;
    default :
      for (j=0;j<self->numberOfDataSubsets;j++) {
        lval=grib_decode_unsigned_long(data,pos,width);
        if (!grib_is_all_bits_one(lval,modifiedWidth)) {
          dval=(lval+localReference)*modifiedFactor;
        } else {
          dval=GRIB_MISSING_DOUBLE;
        }
        grib_darray_push(c,ret,dval);
      }
   }
   */

  return ret;
}

static int can_be_missing(int F,int X,int Y) {
  int ret=1;
  if (F==0 && X==31 && Y==31) ret=0;
  return ret;
}

static int decode_elements(grib_accessor* a) {
  int err=0;
  int *F=0,*X=0,*Y=0;
  int extraWidth,extraScale=0,associatedFieldWidth=0,localDescriptorWidth=0;
  double referenceFactor=1;
  int nn;
  int numberOfElementsToRepeat=0,numberOfRepetitions=0;
  int startRepetition=0;
  unsigned char* data=0;
  int i,canBeMissing=1;
  grib_sarray* sval=0;
  grib_darray* dval;
  grib_iarray* elementsFXY=0;
  long localReference=0,width,pos=0;
  int index,sval_size,ii;

  grib_vdarray* dvalues = NULL;
  grib_vsarray* svalues = NULL;
  grib_accessor_bufr_data_array *self =(grib_accessor_bufr_data_array*)a;

  grib_handle* h=a->parent->h;
  grib_context* c=h->context;

  if (!self->do_decode) return 0;
  self->do_decode=0;
  a->parent->h->unpacked=1;

  data = (unsigned char*)h->buffer->data;
  pos=a->offset*8;

  err=get_descriptors(a);
  if (err) return err;

  F=grib_context_malloc_clear(c,sizeof(int)*self->numberOfDescriptors);
  X=grib_context_malloc_clear(c,sizeof(int)*self->numberOfDescriptors);
  Y=grib_context_malloc_clear(c,sizeof(int)*self->numberOfDescriptors);
  for (i=0;i<self->numberOfDescriptors;i++) {
    F[i]=self->expandedDescriptors[i]/100000;
    X[i]=(self->expandedDescriptors[i]-F[i]*100000)/1000;
    Y[i]=(self->expandedDescriptors[i]-F[i]*100000)%1000;
  }

  dvalues=grib_vdarray_new(c,100,100);
  svalues=grib_vsarray_new(c,10,10);
  extraWidth=0;
  extraScale=0;
  referenceFactor=1;

  if (self->elementsFXY) grib_viarray_delete(c,self->elementsFXY);
  self->elementsFXY=grib_viarray_new(c,100,100);
  elementsFXY=grib_iarray_new(c,100,100);
  for (i=0;i<self->numberOfDescriptors;i++) {
    canBeMissing=can_be_missing(F[i],X[i],Y[i]);
    switch(F[i]) {
      case 0:
        /* Table B element */
        grib_iarray_push(elementsFXY,self->expandedDescriptors[i]);
        if (*(self->type[i])=='s') {
          /* string */
          sval=decode_string_array(c,data,&pos,i,self,extraWidth,extraScale,referenceFactor,associatedFieldWidth,localDescriptorWidth);
          grib_vsarray_push(c,svalues,sval);
          index=grib_vsarray_used_size(svalues)-1;
          sval_size=grib_sarray_used_size(sval);
          dval=grib_darray_new(c,sval_size,10);
          for (ii=0;ii<sval_size;ii++) {
            double x=index*1000+strlen(sval->v[ii]);
            grib_darray_push(c,dval,x);
          }
          grib_vdarray_push(c,dvalues,dval);

        } else {
          /* numeric */
          dval=decode_double_array(c,data,&pos,i,self,canBeMissing,extraWidth,extraScale,referenceFactor,associatedFieldWidth,localDescriptorWidth);
          grib_vdarray_push(c,dvalues,dval);
        }
        break;
      case 1:
        /* Delayed replication */
        numberOfElementsToRepeat=X[i];
        nn=numberOfElementsToRepeat;
        i++;
        localReference=grib_decode_unsigned_long(data,&pos,self->width[i])+self->reference[i];
        width=grib_decode_unsigned_long(data,&pos,6);
        if (width) {
          /* delayed replication number is not constant. NOT IMPLEMENTED */
          Assert(0);
        } else {
          numberOfRepetitions=localReference*self->factor[i];
          startRepetition=i;
        }
        continue;
      case 2:
        /* Operator */
        associatedFieldWidth=0;
        localDescriptorWidth=0;
        switch(X[i]) {
          case 1:
            extraWidth = Y[i] ? Y[i]-128 : 0;
            break;
          case 2:
            extraScale = Y[i] ? Y[i]-128 : 0;
            break;
          /* case 4: */
            /* associated field*/
            /* associatedFieldWidth=Y[i]; */
            /* break; */
          case 6:
            /*signify data width*/
            localDescriptorWidth=Y[i];
            break;
          case 7:
            if (Y) {
              extraScale = Y[i];
              referenceFactor=grib_power(Y[i],10);
              extraWidth=((10*Y[i])+2)/3;
            } else {
              extraWidth=0;
              extraScale=0;
              referenceFactor=1;
            }
            break;
          case 22:
          case 23:
          case 24:
          case 25:
          case 26:
          case 27:
          case 29:
          case 30:
          case 31:
          case 32:
          case 33:
          case 34:
          case 35:
          case 36:
          case 37:
          case 38:
          case 39:
          case 40:
          case 41:
          case 42:
            grib_iarray_push(elementsFXY,self->expandedDescriptors[i]);
            dval=grib_darray_new(c,1,100);
            grib_darray_push(c,dval,0);
            grib_vdarray_push(c,dvalues,dval);
            break;
          default :
            grib_context_log(c,GRIB_LOG_ERROR,"unsupported operator %d\n",X[i]);
            return GRIB_INTERNAL_ERROR;
        }
        break;
      default:
        err=GRIB_INTERNAL_ERROR;
        return err;
    }

    /* delayed repetition check */
    if (numberOfRepetitions)  {
      if (nn>1) {
        nn--;
      } else {
        nn=numberOfElementsToRepeat;
        numberOfRepetitions--;
        if (numberOfRepetitions) i=startRepetition;
      }
    }

  }
  grib_viarray_push(c,self->elementsFXY,elementsFXY);

  if (self->numericValues) {
    grib_vdarray_delete_content(c,dvalues);
    grib_vdarray_delete(c,dvalues);
    grib_vsarray_delete_content(c,svalues);
    grib_vsarray_delete(c,svalues);
  }
  self->numericValues=dvalues;
  self->stringValues=svalues;
  return err;
}

static void dump(grib_accessor* a, grib_dumper* dumper)
{
  decode_elements(a);

  grib_dump_section(dumper,a,a->sub_section->block);

  return;
}

static int value_count(grib_accessor* a,long* count)
{
	int err=0,l;
  long numberOfSubsets=0,subsetNumber=0;
	grib_accessor_bufr_data_array *self =(grib_accessor_bufr_data_array*)a;

  err=decode_elements(a);
  if (err) return err;

  err=grib_get_long(a->parent->h,self->subsetNumberName,&subsetNumber);
  if (err) return err;

  l=grib_vdarray_used_size(self->numericValues);

  *count=l;
  if (subsetNumber<=0) {
    err=grib_get_long(a->parent->h,self->numberOfDataSubsetsName,&numberOfSubsets);
    if (err) return err;
    *count *= numberOfSubsets;
  }

	return err;
}

static int unpack_double(grib_accessor* a, double* val, size_t *len) {
    int err=0,i,k,ii;
    long n=0;
    size_t l=0;
    long numberOfSubsets=0;
	  grib_accessor_bufr_data_array *self =(grib_accessor_bufr_data_array*)a;

    err=decode_elements(a);
    if (err) return err;
    if (!val) return err;

    l=grib_vdarray_used_size(self->numericValues);
    err=grib_get_long(a->parent->h,self->subsetNumberName,&n);
    if (err) return err;

    if (n>0) {
      for (i=0;i<l;i++) {
        val[i]=self->numericValues->v[i]->n > 1 ? self->numericValues->v[i]->v[n-1] : self->numericValues->v[i]->v[0];
      }
    } else {
      err=grib_get_long(a->parent->h,self->numberOfDataSubsetsName,&numberOfSubsets);
      if (err) return err;

      ii=0;
      for (k=0;k<numberOfSubsets;k++) {
        for (i=0;i<l;i++) {
          val[ii++]=self->numericValues->v[i]->n > 1 ? self->numericValues->v[i]->v[k] : self->numericValues->v[i]->v[0];
        }
      }
    }

    return GRIB_SUCCESS;
}

static void destroy(grib_context* c,grib_accessor* a) {
  grib_accessor_bufr_data_array *self =(grib_accessor_bufr_data_array*)a;
  self_clear(c,self);
  grib_section_delete(c,a->sub_section);
}

