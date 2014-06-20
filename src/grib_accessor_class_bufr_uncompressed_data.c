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
   IMPLEMENTS = dump
   IMPLEMENTS = next_offset
   IMPLEMENTS = byte_count; value_count
   IMPLEMENTS = byte_offset
   IMPLEMENTS = get_native_type
   IMPLEMENTS = update_size
   IMPLEMENTS = compare; next
   IMPLEMENTS = pack_long; unpack_double; pack_double
   MEMBERS= const char* offsetSection4Name
   MEMBERS= const char* offsetBeforeDataName
   MEMBERS= const char* offsetEndSection4Name
   MEMBERS= const char* section4LengthName
   MEMBERS= const char* numberOfDataSubsetsName
   MEMBERS= const char* subsetNumberName
   MEMBERS= const char* expandedDescriptorsName
   MEMBERS= const char* abbreviationName
   MEMBERS= const char* typeName
   MEMBERS= const char* nameName
   MEMBERS= const char* unitName
   MEMBERS= const char* referenceName
   MEMBERS= const char* scaleName
   MEMBERS= const char* widthName
   MEMBERS= const char* codeFlags
   MEMBERS= const char* bitmapNumberName
   MEMBERS= const char* associatedBitmapNumberName
   MEMBERS= const char* associatedBitmapIndexName
   MEMBERS= long* expandedDescriptors
   MEMBERS= char** abbreviation
   MEMBERS= char** type
   MEMBERS= char** names
   MEMBERS= char** units
   MEMBERS= long* reference
   MEMBERS= double* factor
   MEMBERS= long* width
   MEMBERS= long* bitmapNumber
   MEMBERS= long* associatedBitmapNumber
   MEMBERS= long* associatedBitmapIndex
   MEMBERS= long numberOfElements
   MEMBERS= long numberOfDataSubsets
   MEMBERS= size_t numberOfDescriptors
   MEMBERS= grib_darray* dvalues
   MEMBERS= grib_sarray* svalues
   MEMBERS= int do_compute
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
static void dump(grib_accessor*, grib_dumper*);
static void init(grib_accessor*,const long, grib_arguments* );
static void init_class(grib_accessor_class*);
static void update_size(grib_accessor*,size_t);
static grib_accessor* next(grib_accessor*, int);
static int compare(grib_accessor*, grib_accessor*);

typedef struct grib_accessor_bufr_uncompressed_data {
    grib_accessor          att;
/* Members defined in gen */
/* Members defined in bufr_uncompressed_data */
	const char* offsetSection4Name;
	const char* offsetBeforeDataName;
	const char* offsetEndSection4Name;
	const char* section4LengthName;
	const char* numberOfDataSubsetsName;
	const char* subsetNumberName;
	const char* expandedDescriptorsName;
	const char* abbreviationName;
	const char* typeName;
	const char* nameName;
	const char* unitName;
	const char* referenceName;
	const char* scaleName;
	const char* widthName;
	const char* codeFlags;
	const char* bitmapNumberName;
	const char* associatedBitmapNumberName;
	const char* associatedBitmapIndexName;
	long* expandedDescriptors;
	char** abbreviation;
	char** type;
	char** names;
	char** units;
	long* reference;
	double* factor;
	long* width;
	long* bitmapNumber;
	long* associatedBitmapNumber;
	long* associatedBitmapIndex;
	long numberOfElements;
	long numberOfDataSubsets;
	size_t numberOfDescriptors;
	grib_darray* dvalues;
	grib_sarray* svalues;
	int do_compute;
} grib_accessor_bufr_uncompressed_data;

extern grib_accessor_class* grib_accessor_class_gen;

static grib_accessor_class _grib_accessor_class_bufr_uncompressed_data = {
    &grib_accessor_class_gen,                      /* super                     */
    "bufr_uncompressed_data",                      /* name                      */
    sizeof(grib_accessor_bufr_uncompressed_data),  /* size                      */
    0,                           /* inited */
    &init_class,                 /* init_class */
    &init,                       /* init                      */
    0,                  /* post_init                      */
    0,                    /* free mem                       */
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
    &next,                       /* next accessor    */
    &compare,                    /* compare vs. another accessor   */
    0,     /* unpack only ith value          */
    0,     /* unpack a subarray         */
    0,             		/* clear          */
};


grib_accessor_class* grib_accessor_class_bufr_uncompressed_data = &_grib_accessor_class_bufr_uncompressed_data;


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
	c->unpack_double_element	=	(*(c->super))->unpack_double_element;
	c->unpack_double_subarray	=	(*(c->super))->unpack_double_subarray;
	c->clear	=	(*(c->super))->clear;
}

/* END_CLASS_IMP */

typedef struct grib_accessor_constant {
    grib_accessor          att;
/* Members defined in gen */
/* Members defined in variable */
  double dval;
  char*  cval;
  int    type;
/* Members defined in constant */
} grib_accessor_constant;

typedef struct grib_accessor_bufr_element {
    grib_accessor          att;
/* Members defined in gen */
/* Members defined in bufr_element */
        grib_accessor* data_accessor;
        int index;
        int ielement;
        int type;
} grib_accessor_bufr_element;

static long init_length(grib_accessor* a)
{
  grib_accessor_bufr_uncompressed_data *self =(grib_accessor_bufr_uncompressed_data*)a;
  long section4Length=0;

  grib_handle* h=a->parent->h;

  grib_get_long(h,self->section4LengthName,&section4Length);

  return section4Length-4;
}

static void init(grib_accessor* a,const long v, grib_arguments* params)
{
  grib_accessor_bufr_uncompressed_data *self =(grib_accessor_bufr_uncompressed_data*)a;
  int n = 0;

  a->sub_section           = grib_section_create(a->parent->h,a);

  self->offsetSection4Name = grib_arguments_get_name(a->parent->h,params,n++);
  self->offsetBeforeDataName = grib_arguments_get_name(a->parent->h,params,n++);
  self->offsetEndSection4Name = grib_arguments_get_name(a->parent->h,params,n++);
  self->section4LengthName = grib_arguments_get_name(a->parent->h,params,n++);
  self->numberOfDataSubsetsName = grib_arguments_get_name(a->parent->h,params,n++);
  self->subsetNumberName    = grib_arguments_get_name(a->parent->h,params,n++);
  self->expandedDescriptorsName = grib_arguments_get_name(a->parent->h,params,n++);
  self->abbreviationName = grib_arguments_get_name(a->parent->h,params,n++);
  self->typeName = grib_arguments_get_name(a->parent->h,params,n++);
  self->nameName = grib_arguments_get_name(a->parent->h,params,n++);
  self->unitName = grib_arguments_get_name(a->parent->h,params,n++);
  self->referenceName = grib_arguments_get_name(a->parent->h,params,n++);
  self->scaleName = grib_arguments_get_name(a->parent->h,params,n++);
  self->widthName = grib_arguments_get_name(a->parent->h,params,n++);
  self->codeFlags = grib_arguments_get_name(a->parent->h,params,n++);
  self->bitmapNumberName = grib_arguments_get_name(a->parent->h,params,n++);
  self->associatedBitmapNumberName = grib_arguments_get_name(a->parent->h,params,n++);
  self->associatedBitmapIndexName = grib_arguments_get_name(a->parent->h,params,n++);
  self->expandedDescriptors=0;
  self->numberOfDescriptors=0;
  self->numberOfElements=0;
  self->numberOfDataSubsets=0;
  self->expandedDescriptors=0;
  self->abbreviation=0;
  self->type=0;
  self->names=0;
  self->units=0;
  self->reference=0;
  self->factor=0;
  self->width=0;
  self->dvalues=0;
  self->svalues=0;
  self->do_compute=1;

  a->length = init_length(a);

  /* Assert(a->length>=0); */
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
  grib_accessor_bufr_uncompressed_data *self =(grib_accessor_bufr_uncompressed_data*)a;
  self->do_compute=1;

  return GRIB_NOT_IMPLEMENTED;
}

static int pack_double(grib_accessor* a, const double* val, size_t *len)
{
  grib_accessor_bufr_uncompressed_data *self =(grib_accessor_bufr_uncompressed_data*)a;
  self->do_compute=1;
  if (self->dvalues) grib_context_free(a->parent->h->context,self->dvalues);
  if (self->svalues) grib_context_free(a->parent->h->context,self->svalues);

  return GRIB_NOT_IMPLEMENTED;
}


static int get_descriptors(grib_accessor* a) {
    int err=0;
    size_t size=0;
    long* scale=0;
    double* factor=0;
    int i=0;
    grib_accessor_bufr_uncompressed_data *self =(grib_accessor_bufr_uncompressed_data*)a;
    grib_accessor* expandedDescriptors=0;
    grib_handle* h=a->parent->h;
    grib_context* c=a->parent->h->context;

    expandedDescriptors=grib_find_accessor(a->parent->h,self->expandedDescriptorsName);
    if (!expandedDescriptors) {
          grib_context_log(a->parent->h->context,GRIB_LOG_ERROR,
              "unable to find accessor %s",self->expandedDescriptorsName);
          return GRIB_NOT_FOUND;
    }

    err=_grib_get_size(a->parent->h,expandedDescriptors,&(self->numberOfDescriptors));
    if (err) return err;

    self->expandedDescriptors=grib_context_malloc_clear(a->parent->h->context,sizeof(long)*self->numberOfDescriptors);
    if (!self->expandedDescriptors) {
      grib_context_log(a->parent->h->context,GRIB_LOG_FATAL,
          "unable to allocate %ld bytes",(long)(self->numberOfDescriptors));
      return GRIB_OUT_OF_MEMORY;
    }

    if( (err = grib_unpack_long(expandedDescriptors,self->expandedDescriptors,&(self->numberOfDescriptors)))
    		!= GRIB_SUCCESS)
      return err;

    size=self->numberOfDescriptors;
    self->abbreviation=(char**)grib_context_malloc_clear(c,size*sizeof(char*));
    err=grib_get_string_array(h,self->abbreviationName,self->abbreviation,&size);

    self->type=(char**)grib_context_malloc_clear(c,size*sizeof(char*));
    err=grib_get_string_array(h,self->typeName,self->type,&size);

    self->names=(char**)grib_context_malloc_clear(c,size*sizeof(char*));
    err=grib_get_string_array(h,self->nameName,self->names,&size);

    self->units=(char**)grib_context_malloc_clear(c,size*sizeof(char*));
    err=grib_get_string_array(h,self->unitName,self->units,&size);

    self->reference=grib_context_malloc_clear(c,size*sizeof(long));
    err=grib_get_long_array(h,self->referenceName,self->reference,&size);

    scale=grib_context_malloc_clear(c,size*sizeof(long));
    factor=grib_context_malloc_clear(c,size*sizeof(double));
    err=grib_get_long_array(h,self->scaleName,scale,&size);
    for (i=0;i<size;i++) factor[i]=grib_power(-scale[i],10);
    self->factor=factor;
    grib_context_free(c,scale);

    self->width=grib_context_malloc_clear(c,size*sizeof(long));
    err=grib_get_long_array(h,self->widthName,self->width,&size);

    self->bitmapNumber=grib_context_malloc_clear(c,size*sizeof(long));
    err=grib_get_long_array(h,self->bitmapNumberName,self->bitmapNumber,&size);

    self->associatedBitmapNumber=grib_context_malloc_clear(c,size*sizeof(long));
    err=grib_get_long_array(h,self->associatedBitmapNumberName,self->associatedBitmapNumber,&size);

    self->associatedBitmapIndex=grib_context_malloc_clear(c,size*sizeof(long));
    err=grib_get_long_array(h,self->associatedBitmapIndexName,self->associatedBitmapIndex,&size);

    return err;
}

static void push_units_accessor(grib_accessor* a,long groupNumber,long i) {
  char buf[1024]={0,};
  grib_accessor_bufr_uncompressed_data *self =(grib_accessor_bufr_uncompressed_data*)a;
  grib_accessor* gaUnits=0;
  grib_action creatorUnits = {0, };
  grib_context* c=a->parent->h->context;

  if (self->units[i][0]=='C' && ( !strcmp(self->units[i],"CCITTIA5") ||
                                  !strncmp(self->units[i],"COMMON",6)) )
                        return;

  creatorUnits.op         = "variable";
  creatorUnits.name_space = "";
  creatorUnits.flags     = GRIB_ACCESSOR_FLAG_DUMP;
  creatorUnits.set        = 0;

  sprintf(buf,"%sUnits",self->abbreviation[i]);
  creatorUnits.name=grib_context_strdup(c,buf);
  gaUnits = grib_accessor_factory(a->sub_section, &creatorUnits, 0, NULL);
  gaUnits->bufr_group_number=groupNumber;
  ((grib_accessor_constant*)gaUnits)->type=GRIB_TYPE_STRING;

  if (self->units[i][0]=='C' && !strcmp(self->units[i],"CODETABLE") ) {
    sprintf(buf,"codetable %ld",self->expandedDescriptors[i]);
    ((grib_accessor_constant*)gaUnits)->cval=grib_context_strdup(c,buf);
  } else {
    ((grib_accessor_constant*)gaUnits)->cval=grib_context_strdup(c,self->units[i]);
  }

  grib_push_accessor(gaUnits,a->sub_section->block);
}

static int decode_elements(grib_accessor* a) {
    int err=0;
    grib_accessor_bufr_uncompressed_data *self =(grib_accessor_bufr_uncompressed_data*)a;
    unsigned char* data =NULL;
    long i=0,k=0,jj=0;
    char buf[1024]={0,};
    long pos=0;
    long n=0;
    long* width;
    long* reference;
    long groupNumber;
    int notSignificanceElement;
    char* sval;
    double* factor;
    double val=0;
    int numberOfElementsToRepeat=0,numberOfRepetitions=0;
    int startRepetition;
    int *F,*X,*Y;
    int bitmapIndex[10]={0,};
    long lval=0;
    grib_iarray* index=0;
    grib_handle* h=a->parent->h;
    grib_context* c=h->context;

    grib_accessor* gaGroup=0;
    grib_action creatorGroup = {0, };

    grib_accessor* ga=0;
    grib_action creator = {0, };
    creator.op         = "bufr_element";
    creator.name_space = "";
    creator.flags     = GRIB_ACCESSOR_FLAG_DUMP;
    creator.set        = 0;

    creatorGroup.op         = "variable";
    creatorGroup.name_space = "";
    creatorGroup.flags     = GRIB_ACCESSOR_FLAG_DUMP;
    creatorGroup.set        = 0;

    data = (unsigned char*)h->buffer->data;
    pos=a->offset*8;

    err=get_descriptors(a);
    if (err) return err;

    width=self->width;
    factor=self->factor;
    reference=self->reference;

    grib_get_long(h,self->numberOfDataSubsetsName,&(self->numberOfDataSubsets));

    /* TODO: implement multiple subsets*/
    Assert(self->numberOfDataSubsets==1);

    F=grib_context_malloc_clear(c,sizeof(int)*self->numberOfDescriptors);
    X=grib_context_malloc_clear(c,sizeof(int)*self->numberOfDescriptors);
    Y=grib_context_malloc_clear(c,sizeof(int)*self->numberOfDescriptors);
    for (i=0;i<self->numberOfDescriptors;i++) {
        F[i]=self->expandedDescriptors[i]/100000;
        X[i]=(self->expandedDescriptors[i]-F[i]*100000)/1000;
        Y[i]=(self->expandedDescriptors[i]-F[i]*100000)%1000;
    }

    if (!self->dvalues)
        self->dvalues=grib_darray_new(c,self->numberOfDescriptors,100);
    if (!self->svalues)
	    self->svalues=grib_sarray_new(c,self->numberOfDescriptors,100);

    index=grib_iarray_new(c,self->numberOfDescriptors,100);

    self->dvalues->n=0;
    self->svalues->n=0;
    self->numberOfElements=0;
    numberOfElementsToRepeat=0;
    numberOfRepetitions=0;
    k=0;
    n=0;
    groupNumber=1;

    sprintf(buf,"groupNumber");
    notSignificanceElement=0;
    creatorGroup.name=grib_context_strdup(c,buf);
    gaGroup = grib_accessor_factory(a->sub_section, &creatorGroup, 0, NULL);
    gaGroup->bufr_group_number=groupNumber;
    ((grib_accessor_constant*)gaGroup)->type=GRIB_TYPE_LONG;
    ((grib_accessor_constant*)gaGroup)->dval=groupNumber;
    grib_push_accessor(gaGroup,a->sub_section->block);

    for (i=0;i<self->numberOfDescriptors;i++) {
        if (F[i]==0 && X[i]==8 && notSignificanceElement) {
          groupNumber++;
          sprintf(buf,"groupNumber");
          creatorGroup.name=grib_context_strdup(c,buf);
          gaGroup = grib_accessor_factory(a->sub_section, &creatorGroup, 0, NULL);
          gaGroup->bufr_group_number=groupNumber;
          ((grib_accessor_constant*)gaGroup)->type=GRIB_TYPE_LONG;
          ((grib_accessor_constant*)gaGroup)->dval=groupNumber;
          grib_push_accessor(gaGroup,a->sub_section->block);
          notSignificanceElement=0;
        } else {
          notSignificanceElement=1;
        }

        if (F[i]==1) {
            /* delayed replication */
            numberOfElementsToRepeat=X[i];
            n=numberOfElementsToRepeat;
            i++;
            lval=grib_decode_unsigned_long(data,&pos,width[i]);
            numberOfRepetitions=(reference[i]+lval)*factor[i];
            startRepetition=i;
            continue;
        }
        if (self->bitmapNumber[i]) {
            /* bitmap extraction */
            lval=grib_decode_unsigned_long(data,&pos,1);
            if(!bitmapIndex[self->bitmapNumber[i]])
                bitmapIndex[self->bitmapNumber[i]]=self->dvalues->n;
            grib_iarray_push(index,i);
            grib_darray_push(c,self->dvalues,(double)lval) ;
            k=0;
            jj=0;
        } else if (self->associatedBitmapNumber[i]) {
            /* bitmap application */
            char name[1024]={0,};
            if (!k) k=bitmapIndex[self->associatedBitmapNumber[i]];
            else k++;

            sprintf(name,"%sOf-%s",self->abbreviation[i],self->abbreviation[index->v[jj]]);
            jj++;
            creator.name = grib_context_strdup(c,name);
            ga = grib_accessor_factory(a->sub_section, &creator, 0, NULL);
            ga->bufr_group_number=groupNumber;
            ((grib_accessor_bufr_element*)ga)->data_accessor=a;
            ((grib_accessor_bufr_element*)ga)->index=self->dvalues->n;
            ((grib_accessor_bufr_element*)ga)->ielement=i;
            ((grib_accessor_bufr_element*)ga)->type=GRIB_TYPE_DOUBLE;
            grib_iarray_push(index,i);
            if (self->dvalues->v[k]==0) {
                lval=grib_decode_unsigned_long(data,&pos,width[i]);
                val=(reference[i]+lval)*factor[i];
                grib_darray_push(c,self->dvalues,val);
                grib_push_accessor(ga,a->sub_section->block);
            } else {
                val=GRIB_MISSING_DOUBLE;
                i--;
                grib_darray_push(c,self->dvalues,val);
                grib_push_accessor(ga,a->sub_section->block);
                continue;
            }
            /* printf("++++++> %d %d %s=%g\n",i,jj,self->abbreviation[index->v[jj]],val); */
        } else {
            /* element decoding and accessor creation */
            creator.name = self->abbreviation[i];
            ga = grib_accessor_factory(a->sub_section, &creator, 0, NULL);
            ga->bufr_group_number=groupNumber;
            ((grib_accessor_bufr_element*)ga)->data_accessor=a;

            push_units_accessor(a,groupNumber,i);

            if ( *(self->type[i])=='s') {
                /* string element */
                size_t widthInBytes=width[i]/8;
                sval=grib_context_malloc_clear(c,widthInBytes+1);
                sval=grib_decode_string(data,&pos,widthInBytes,sval);
                ((grib_accessor_bufr_element*)ga)->index=self->svalues->n;
                ((grib_accessor_bufr_element*)ga)->ielement=i;
                ((grib_accessor_bufr_element*)ga)->type=GRIB_TYPE_STRING;
                grib_sarray_push(c,self->svalues,sval);
            } else {
                /* number element */
                lval=grib_decode_unsigned_long(data,&pos,width[i]);
                if (!grib_is_all_bits_one(lval,width[i])) {
                /* if ( 1 ) { */
                    val=(reference[i]+lval)*factor[i];
                } else {
                    val=GRIB_MISSING_DOUBLE;
                }
                ((grib_accessor_bufr_element*)ga)->index=self->dvalues->n;
                ((grib_accessor_bufr_element*)ga)->ielement=i;
                grib_iarray_push(index,i);
                grib_darray_push(c,self->dvalues,val);
                ((grib_accessor_bufr_element*)ga)->type=GRIB_TYPE_DOUBLE;
                if ( *(self->type[i])=='l')
                    ((grib_accessor_bufr_element*)ga)->type=GRIB_TYPE_LONG;
            }
            grib_push_accessor(ga,a->sub_section->block);

            k=0;
            jj=0;
        }

        /* delayed repetition check */
        if (numberOfRepetitions)  {
            if (n>1) {
                n--;
            } else {
                n=numberOfElementsToRepeat;
                numberOfRepetitions--;
                if (numberOfRepetitions) i=startRepetition;
            }
        }
    }

    /* for (i=0;i<self->dvalues->n;i++) printf("%ld %g\n",i,self->dvalues->v[i]); */

    grib_context_free(c,F);
    grib_context_free(c,X);
    grib_context_free(c,Y);
    grib_iarray_delete(index);
    return err;
}

static void dump(grib_accessor* a, grib_dumper* dumper)
{
  grib_accessor_bufr_uncompressed_data *self =(grib_accessor_bufr_uncompressed_data*)a;
  if (self->do_compute) {
     decode_elements(a);
     self->do_compute=0;
     a->parent->h->unpacked=1;
  }
  grib_dump_section(dumper,a,a->sub_section->block);

  return;
}

static int unpack_double(grib_accessor* a, double* val, size_t *len) {
    grib_accessor_bufr_uncompressed_data *self =(grib_accessor_bufr_uncompressed_data*)a;
    int err=0;

    if (self->do_compute) {
       err=decode_elements(a);
       self->do_compute=0;
       a->parent->h->unpacked=1;
    }

    if (!val) return GRIB_SUCCESS;
    else return GRIB_NOT_IMPLEMENTED;

#if 0
    if (*len < n)
        return GRIB_ARRAY_TOO_SMALL;


    if (self->values) {
        grib_get_long(h,self->subsetNumberName,&subsetNumber);
        if (subsetNumber>0) {
            /* return only this subset */
            for (i=0;i<self->numberOfElements;i++)
                val[i]=self->values[numberOfDataSubsets*i+subsetNumber-1];
            return 0;
        }
    }

    for (i=0;i<n;i++) val[i]=self->dvalues->v[i];

    return err;
#endif
}

static int value_count(grib_accessor* a,long* count)
{
  int err=0;
	grib_accessor_bufr_uncompressed_data *self =(grib_accessor_bufr_uncompressed_data*)a;
  grib_handle* h=a->parent->h;

	err=get_descriptors(a);
  if (err) return err;

	err=grib_get_long(h,self->numberOfDataSubsetsName,&(self->numberOfDataSubsets));
  if (err) return err;

  /* TODO: implement multiple subsets*/
  Assert(self->numberOfDataSubsets==1);

	*count=self->numberOfElements;
  return err;
}

static grib_accessor* next(grib_accessor* a,int explore) {
  grib_accessor* next=NULL;
  if (explore) {
    next=a->sub_section->block->first;
    if (!next) next=a->next;
  } else {
      next=a->next;
  }
  if (!next) {
    if (a->parent->owner)
      next=a->parent->owner->cclass->next(a->parent->owner,0);
  }
  return next;
}

