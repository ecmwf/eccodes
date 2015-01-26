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
   MEMBERS    = const char* stringValuesName
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
	const char* stringValuesName;
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

static int can_be_missing(int descriptor) {
  int ret=1;
  if (descriptor==31031) ret=0;
  return ret;
}

static void restart_bitmap(grib_accessor_bufr_data_array *self) { 
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

  a->sub_section           = grib_section_create(a->parent->h,a);

  self->offsetSection4Name = grib_arguments_get_name(a->parent->h,params,n++);
  self->offsetBeforeDataName = grib_arguments_get_name(a->parent->h,params,n++);
  self->offsetEndSection4Name = grib_arguments_get_name(a->parent->h,params,n++);
  self->section4LengthName = grib_arguments_get_name(a->parent->h,params,n++);
  self->numberOfSubsetsName = grib_arguments_get_name(a->parent->h,params,n++);
  self->subsetNumberName    = grib_arguments_get_name(a->parent->h,params,n++);
  self->expandedDescriptorsName = grib_arguments_get_name(a->parent->h,params,n++);
  self->flagsName = grib_arguments_get_name(a->parent->h,params,n++);
  self->stringValuesName = grib_arguments_get_name(a->parent->h,params,n++);
  self->elementsDescriptorsIndexName = grib_arguments_get_name(a->parent->h,params,n++);
  self->compressedDataName = grib_arguments_get_name(a->parent->h,params,n++);
  self->do_decode=1;
  self->elementsDescriptorsIndex=0;
  self->numericValues=0;
  self->stringValues=0;
  cancel_bitmap(self);
  self->expanded=0;
  self->expandedAccessor=0;
  a->parent->h->unpacked=0;

  a->length = init_length(a);

  /* Assert(a->length>=0); */
}


static void self_clear(grib_context* c,grib_accessor_bufr_data_array* self) {
  grib_context_free(c,self->canBeMissing);
	grib_vdarray_delete(c,self->numericValues);
	grib_vsarray_delete(c,self->stringValues);
	grib_viarray_delete(c,self->elementsDescriptorsIndex);
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

static grib_sarray* decode_string_array(grib_context* c,unsigned char* data,long* pos, int i,
              grib_accessor_bufr_data_array* self) {
  grib_sarray* ret=NULL;
  char* sval=0;
  int j,modifiedWidth,modifiedReference,width;
  double modifiedFactor;
  modifiedWidth= self->expanded->v[i]->width;
  modifiedReference= self->expanded->v[i]->reference;
  modifiedFactor= self->expanded->v[i]->factor;

  ret=grib_sarray_new(c,10,10);
  sval=(char*)grib_context_malloc_clear(c,modifiedWidth/8+1);
  grib_decode_string(data,pos,modifiedWidth/8,sval);
  width=grib_decode_unsigned_long(data,pos,6);
  ret=grib_sarray_new(c,10,10);
  if (width) {
    grib_context_free(c,sval);
    for (j=0;j<self->numberOfSubsets;j++) {
      sval=(char*)grib_context_malloc_clear(c,width+1);
      grib_decode_string(data,pos,width,sval);
      grib_sarray_push(c,ret,sval);
    }
  } else {
    grib_sarray_push(c,ret,sval);
  }

  return ret;
}

static grib_darray* decode_double_array(grib_context* c,unsigned char* data,long* pos,int i,
              grib_accessor_bufr_data_array* self) {
  grib_darray* ret=NULL;
  int j;
  unsigned long lval;
  int localReference,localWidth,modifiedWidth,modifiedReference;
  double modifiedFactor,dval;

  modifiedReference= self->expanded->v[i]->reference;
  modifiedFactor= self->expanded->v[i]->factor;
  modifiedWidth= self->expanded->v[i]->width;

  lval=grib_decode_unsigned_long(data,pos,modifiedWidth);
  localReference=(long)lval+modifiedReference;
  localWidth=grib_decode_unsigned_long(data,pos,6);
  ret=grib_darray_new(c,100,100);
  if (localWidth) {
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

static char* decode_string_value(grib_context* c,unsigned char* data,long* pos, int i,
              grib_accessor_bufr_data_array* self) {
  char* sval=0;
  int modifiedWidth,modifiedReference;
  double modifiedFactor;
  modifiedWidth= self->expanded->v[i]->width;
  modifiedReference= self->expanded->v[i]->reference;
  modifiedFactor= self->expanded->v[i]->factor;

  sval=(char*)grib_context_malloc_clear(c,modifiedWidth/8+1);
  grib_decode_string(data,pos,modifiedWidth/8,sval);

  return sval;
}

static double decode_double_value(grib_context* c,unsigned char* data,long* pos,int i,
              grib_accessor_bufr_data_array* self) {
  unsigned long lval;
  int modifiedWidth,modifiedReference;
  double modifiedFactor,dval;

  modifiedReference= self->expanded->v[i]->reference;
  modifiedFactor= self->expanded->v[i]->factor;
  modifiedWidth= self->expanded->v[i]->width;

  lval=grib_decode_unsigned_long(data,pos,modifiedWidth);
  if (grib_is_all_bits_one(lval,modifiedWidth) && self->canBeMissing[i]) {
    dval=GRIB_MISSING_DOUBLE;
  } else {
    dval=((long)lval+modifiedReference)*modifiedFactor;
  }
  return dval;
}

static void decode_element(grib_context* c,grib_accessor_bufr_data_array* self,int subsetIndex,
            unsigned char* data,long *pos,int i,grib_darray* dval,grib_sarray* sval) {
  grib_darray* dar=0;
  grib_sarray* sar=0;
  int index=0,ii,sar_size;
  char* csval=0;
  double cdval=0,x;
  if (self->expanded->v[i]->type==BUFR_DESCRIPTOR_TYPE_STRING) {
    /* string */
    if (self->compressedData) {
      sar=decode_string_array(c,data,pos,i,self);
      grib_vsarray_push(c,self->stringValues,sar);
      index=grib_vsarray_used_size(self->stringValues);
      dar=grib_darray_new(c,self->numberOfSubsets,10);
      sar_size=grib_sarray_used_size(sar);
      index=self->numberOfSubsets*(index-1);
      for (ii=1;ii<=self->numberOfSubsets;ii++) {
        x=(index+ii)*1000+self->expanded->v[i]->width/8;
        grib_darray_push(c,dar,x);
      }
      grib_vdarray_push(c,self->numericValues,dar);
    } else {
      csval=decode_string_value(c,data,pos,i,self);
      grib_sarray_push(c,sval,csval);
      index=grib_sarray_used_size(sval)*(subsetIndex+1);
      cdval=index*1000+strlen(csval);
      grib_darray_push(c,dval,cdval);
    }
  } else {
    /* numeric or codetable or flagtable */
    if (self->compressedData) {
      dar=decode_double_array(c,data,pos,i,self);
      grib_vdarray_push(c,self->numericValues,dar);
    } else {
      cdval=decode_double_value(c,data,pos,i,self);
      grib_darray_push(c,dval,cdval);
    }
  }
}

static int build_bitmap(grib_accessor_bufr_data_array *self,unsigned char* data,long* pos,grib_iarray* elementsDescriptorsIndex,int iBitmapOperator) {
  int bitmapSize=0,iDelayedReplication=0;
  int i,localReference,width,bitmapEndElementsDescriptorsIndex;
  long ppos,n;
  grib_accessor* a=(grib_accessor*)self;
  grib_context* c=a->parent->h->context;
  bufr_descriptor** descriptors=self->expanded->v;
  long* edi=elementsDescriptorsIndex->v;
  int iel=grib_iarray_used_size(elementsDescriptorsIndex)-1;

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

static int get_next_bitmap_descriptor_index(grib_accessor_bufr_data_array *self,grib_iarray* elementsDescriptorsIndex,grib_darray* numericValues) {
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

static void push_zero_element(grib_accessor_bufr_data_array* self,grib_darray* dval) {
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

static grib_accessor* create_accessor_from_descriptor(grib_accessor* a,grib_section* section,long ide,long subset) {
  grib_accessor_bufr_data_array *self =(grib_accessor_bufr_data_array*)a;
  int idx=0;
  grib_accessor* elementAccessor=NULL;
  grib_action creator = {0, };
  creator.op         = "bufr_data_element";
  creator.name_space = "";
  creator.flags     = GRIB_ACCESSOR_FLAG_DUMP;
  creator.set        = 0;

  idx = self->compressedData ? self->elementsDescriptorsIndex->v[0]->v[ide] :
	self->elementsDescriptorsIndex->v[subset]->v[ide] ;

  creator.name=self->expanded->v[idx]->shortName;
  if (creator.name) {
    elementAccessor = grib_accessor_factory(section, &creator, 0, NULL);
    if (self->canBeMissing[idx]) elementAccessor->flags |= GRIB_ACCESSOR_FLAG_CAN_BE_MISSING;
    accessor_bufr_data_element_set_index(elementAccessor,ide);
    accessor_bufr_data_element_set_descriptors(elementAccessor,self->expanded);
    accessor_bufr_data_element_set_numericValues(elementAccessor,self->numericValues);
    accessor_bufr_data_element_set_stringValues(elementAccessor,self->stringValues);
    accessor_bufr_data_element_set_compressedData(elementAccessor,self->compressedData);
    accessor_bufr_data_element_set_type(elementAccessor,self->expanded->v[idx]->type);
    accessor_bufr_data_element_set_numberOfSubsets(elementAccessor,self->numberOfSubsets);
    accessor_bufr_data_element_set_subsetNumber(elementAccessor,subset);
  }

  return elementAccessor;
}

#define IS_QUALIFIER(a) (a==8 || a==1 || a==2)
#define NUMBER_OF_QUALIFIERS_PER_CATEGORY 256
#define NUMBER_OF_QUALIFIERS_CATEGORIES 3

static int number_of_qualifiers=NUMBER_OF_QUALIFIERS_PER_CATEGORY*NUMBER_OF_QUALIFIERS_CATEGORIES;

static GRIB_INLINE int significanceQualifierIndex(int X,int Y) {
  int a[]={-1,0,1,-1,-1,-1,-1,-1,2};
  return Y+a[X]*NUMBER_OF_QUALIFIERS_PER_CATEGORY;
}


static GRIB_INLINE void reset_deeper_qualifiers(grib_accessor* significanceQualifierGroup[],int* significanceQualifierDepth, int depth) {
  int i;
  for (i=0;i<number_of_qualifiers;i++) {
    if (significanceQualifierDepth[i]>depth) {
      significanceQualifierGroup[i]=0;
    }
  }
}

static int create_keys(grib_accessor* a) {
  grib_accessor_bufr_data_array *self =(grib_accessor_bufr_data_array*)a;
  int err=0;
  grib_accessor* elementAccessor=0;
  long iss,end,elementsInSubset,ide;
  grib_section* section=NULL;
  bufr_descriptor* descriptor;
  grib_section* sectionUp=0;
  grib_section* groupSection=0;
  long groupNumber=0;
  int forceGroupClosure=0,forceOneLevelClosure=0;
  long indexOfGroupNumber=0;
  int depth;
  int idx;
  grib_context* c=a->parent->h->context;

  grib_accessor* gaGroup=0;
  grib_action creatorGroup = {0, };
  grib_iarray* groupNumberIndex=0;
  grib_accessor* significanceQualifierGroup[NUMBER_OF_QUALIFIERS_PER_CATEGORY*NUMBER_OF_QUALIFIERS_CATEGORIES]={0,};
  int significanceQualifierDepth[NUMBER_OF_QUALIFIERS_PER_CATEGORY*NUMBER_OF_QUALIFIERS_CATEGORIES]={0,};

  creatorGroup.op         = "bufr_group";
  creatorGroup.name="groupNumber";
  creatorGroup.name_space = "";
  creatorGroup.flags     = GRIB_ACCESSOR_FLAG_DUMP;
  creatorGroup.set        = 0;

  groupNumberIndex=grib_iarray_new(c,100,100);
  

  end= self->compressedData ? 1 : self->numberOfSubsets;
  groupNumber=1;

  gaGroup = grib_accessor_factory(a->sub_section, &creatorGroup, 0, NULL);
  gaGroup->bufr_group_number=groupNumber;
  gaGroup->sub_section=grib_section_create(a->parent->h,gaGroup);
  section=gaGroup->sub_section;
  sectionUp=a->sub_section;
  accessor_constant_set_type(gaGroup,GRIB_TYPE_LONG);
  accessor_constant_set_dval(gaGroup,groupNumber);
  grib_push_accessor(gaGroup,a->sub_section->block);

  forceGroupClosure=0;
  indexOfGroupNumber=0;
  depth=0;

  for (iss=0;iss<end;iss++) {
    elementsInSubset= self->compressedData ? grib_iarray_used_size(self->elementsDescriptorsIndex->v[0]) :
                                             grib_iarray_used_size(self->elementsDescriptorsIndex->v[iss]);
    for (ide=0;ide<elementsInSubset;ide++) {
      idx = self->compressedData ? self->elementsDescriptorsIndex->v[0]->v[ide] :
                            self->elementsDescriptorsIndex->v[iss]->v[ide] ;

      descriptor=self->expanded->v[idx];
      if (descriptor->F==0 && IS_QUALIFIER(descriptor->X)) {
        int sidx=significanceQualifierIndex(descriptor->X,descriptor->Y);
        groupNumber++;

        if (significanceQualifierGroup[sidx]) {
          groupSection=significanceQualifierGroup[sidx]->parent;
          depth=significanceQualifierDepth[sidx];
          reset_deeper_qualifiers(significanceQualifierGroup,significanceQualifierDepth,depth);
          forceGroupClosure=0;
        } else {
          if (forceGroupClosure) {
            groupSection=sectionUp;
            forceGroupClosure=0;
            forceOneLevelClosure=1;
            depth=0;
          } else {
            groupSection=section;
            depth++;
          }
        }

        gaGroup = grib_accessor_factory(groupSection, &creatorGroup, 0, NULL);
        a->parent->h->groups[groupNumber]=gaGroup;
        gaGroup->sub_section=grib_section_create(a->parent->h,gaGroup);
        gaGroup->bufr_group_number=groupNumber;
        accessor_constant_set_type(gaGroup,GRIB_TYPE_LONG);
        accessor_constant_set_dval(gaGroup,groupNumber);
        grib_push_accessor(gaGroup,groupSection->block);

        section=gaGroup->sub_section;
        sectionUp=gaGroup->parent;

        significanceQualifierGroup[sidx]=gaGroup;
        significanceQualifierDepth[sidx]=depth;
      }

      elementAccessor=create_accessor_from_descriptor(a,section,ide,iss);
      if (elementAccessor) grib_push_accessor(elementAccessor,section->block);
    }
  }

  return err;
}

#define MAX_NESTED_REPLICATIONS 8

static int decode_elements(grib_accessor* a) {
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
  bufr_descriptor** descriptors=0;

  grib_darray* dval = NULL;
  grib_sarray* sval = NULL;
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

  descriptors=self->expanded->v;

  if (self->numericValues) {
    grib_vdarray_delete_content(c,self->numericValues);
    grib_vdarray_delete(c,self->numericValues);
    grib_vsarray_delete_content(c,self->stringValues);
    grib_vsarray_delete(c,self->stringValues);
  }
  self->numericValues=grib_vdarray_new(c,100,100);
  self->stringValues=grib_vsarray_new(c,10,10);

  if (self->elementsDescriptorsIndex) grib_viarray_delete(c,self->elementsDescriptorsIndex);
  self->elementsDescriptorsIndex=grib_viarray_new(c,100,100);

  end= self->compressedData ? 1 : self->numberOfSubsets;

  numberOfDescriptors=grib_bufr_descriptors_array_used_size(self->expanded);

  for (iss=0;iss<end;iss++) {
    elementsDescriptorsIndex=grib_iarray_new(c,100,100);
    if (!self->compressedData) {
      dval=grib_darray_new(c,100,100);
      sval=grib_sarray_new(c,10,10);
    }
    for (i=0;i<numberOfDescriptors;i++) {
      elementIndex=grib_iarray_used_size(elementsDescriptorsIndex);
      switch(descriptors[i]->F) {
        case 0:
          /* Table B element */
          grib_iarray_push(elementsDescriptorsIndex,i);
          if (descriptors[i]->code==31031 && !is_bitmap_start_defined(self))
            self->bitmapStart=grib_iarray_used_size(elementsDescriptorsIndex)-1;
          decode_element(c,self,iss,data,&pos,i,dval,sval);
          break;
        case 1:
          /* Delayed replication */
          inr=numberOfNestedRepetitions;
          numberOfNestedRepetitions++;
          Assert(numberOfNestedRepetitions<=MAX_NESTED_REPLICATIONS);
          numberOfElementsToRepeat[inr]=descriptors[i]->X;
          n[inr]=numberOfElementsToRepeat[inr];
          i++;
          if (self->compressedData) {
            localReference=grib_decode_unsigned_long(data,&pos,descriptors[i]->width)+descriptors[i]->reference;
            width=grib_decode_unsigned_long(data,&pos,6);
            if (width) {
              /* delayed replication number is not constant. NOT IMPLEMENTED */
              Assert(0);
            } else {
              numberOfRepetitions[inr]=localReference*descriptors[i]->factor;
              startRepetition[inr]=i;
            }
          } else {
            numberOfRepetitions[inr]=grib_decode_unsigned_long(data,&pos,descriptors[i]->width)+
                                      descriptors[i]->reference*descriptors[i]->factor;
            startRepetition[inr]=i;
          }
          nn[inr]=numberOfRepetitions[inr];
          grib_iarray_push(elementsDescriptorsIndex,i);
          if (self->compressedData) {
            dval=grib_darray_new(c,1,100);
            grib_darray_push(c,dval,(double)numberOfRepetitions[inr]);
            grib_vdarray_push(c,self->numericValues,dval);
          } else {
            grib_darray_push(c,dval,(double)numberOfRepetitions[inr]);
          }
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
              grib_iarray_push(elementsDescriptorsIndex,i);
              push_zero_element(self,dval);
              break;
            case 23:
              /* substituted values marker operator */
            case 24:
              /*first-order statistical values marker operator*/
            case 32:
              /*replaced/retained values marker operator*/
              if (descriptors[i]->Y==255) {
                index=get_next_bitmap_descriptor_index(self,elementsDescriptorsIndex,dval);
                decode_element(c,self,iss,data,&pos,index,dval,sval);
                grib_iarray_push(elementsDescriptorsIndex,index);
              } else {
                grib_iarray_push(elementsDescriptorsIndex,i);
                push_zero_element(self,dval);
              }
              break;
            case 25:
              /*difference statistical values marker operator*/
              break;
            case 35:
              /* cancel bitmap */
              grib_iarray_push(elementsDescriptorsIndex,i);
              push_zero_element(self,dval);
              if (descriptors[i]->Y==0) cancel_bitmap(self);
              break;
            case 36:
              /* bitmap */
              grib_iarray_push(elementsDescriptorsIndex,i);
              push_zero_element(self,dval);
              build_bitmap(self,data,&pos,elementsDescriptorsIndex,i);
              break;
            case 37:
              /* reuse defined bitmap */
              grib_iarray_push(elementsDescriptorsIndex,i);
              push_zero_element(self,dval);
              if (descriptors[i]->Y==0) restart_bitmap(self);
              /* cancel reuse */
              else cancel_bitmap(self);
              break;
            default :
              grib_context_log(c,GRIB_LOG_ERROR,"unsupported operator %d\n",descriptors[i]->X);
              return GRIB_INTERNAL_ERROR;
          }
          break;
        case 9:
          /* associated field */
          if (descriptors[i]->X==99 && descriptors[i]->Y==999) {
            grib_iarray_push(elementsDescriptorsIndex,i);
            decode_element(c,self,iss,data,&pos,i,dval,sval);
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
    grib_viarray_push(c,self->elementsDescriptorsIndex,elementsDescriptorsIndex);
    if (!self->compressedData) {
      grib_vdarray_push(c,self->numericValues,dval);
      grib_vsarray_push(c,self->stringValues,sval);
    }
  }

  err=create_keys(a);

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
  long i,subsetNumber=0;
	grib_accessor_bufr_data_array *self =(grib_accessor_bufr_data_array*)a;
  grib_context* c=a->parent->h->context;

  err=decode_elements(a);
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

static int unpack_double(grib_accessor* a, double* val, size_t *len) {
    int err=0,i,k,ii;
    long n=0;
    size_t l=0,elementsInSubset;
    long numberOfSubsets=0;
	  grib_accessor_bufr_data_array *self =(grib_accessor_bufr_data_array*)a;
    grib_context* c=a->parent->h->context;

    err=decode_elements(a);
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

static void destroy(grib_context* c,grib_accessor* a) {
  grib_accessor_bufr_data_array *self =(grib_accessor_bufr_data_array*)a;
  self_clear(c,self);
  grib_section_delete(c,a->sub_section);
}

