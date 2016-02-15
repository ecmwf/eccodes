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
   MEMBERS= const char* associatedInfoNumberName
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
   MEMBERS= long* associatedInfoNumber
   MEMBERS= long numberOfElements
   MEMBERS= long numberOfDataSubsets
   MEMBERS= size_t numberOfDescriptors
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
static void destroy(grib_context*,grib_accessor*);
static void dump(grib_accessor*, grib_dumper*);
static void init(grib_accessor*,const long, grib_arguments* );
static void init_class(grib_accessor_class*);
static void update_size(grib_accessor*,size_t);
static int compare(grib_accessor*, grib_accessor*);

typedef struct grib_accessor_bufr_data {
    grib_accessor          att;
/* Members defined in gen */
/* Members defined in bufr_data */
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
	const char* associatedInfoNumberName;
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
	long* associatedInfoNumber;
	long numberOfElements;
	long numberOfDataSubsets;
	size_t numberOfDescriptors;
	int do_compute;
} grib_accessor_bufr_data;

extern grib_accessor_class* grib_accessor_class_gen;

static grib_accessor_class _grib_accessor_class_bufr_data = {
    &grib_accessor_class_gen,                      /* super                     */
    "bufr_data",                      /* name                      */
    sizeof(grib_accessor_bufr_data),  /* size                      */
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
    0,               		/* clone accessor          */
};


grib_accessor_class* grib_accessor_class_bufr_data = &_grib_accessor_class_bufr_data;


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
	c->make_clone	=	(*(c->super))->make_clone;
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

#define IS_QUALIFIER(a) (a==8 || a==1 || a==2)
#define NUMBER_OF_QUALIFIERS_PER_CATEGORY 256
#define NUMBER_OF_QUALIFIERS_CATEGORIES 3

static int number_of_qualifiers=NUMBER_OF_QUALIFIERS_PER_CATEGORY*NUMBER_OF_QUALIFIERS_CATEGORIES;

static GRIB_INLINE int significanceQualifierIndex(int X,int Y)
{
    int a[]={-1,0,1,-1,-1,-1,-1,-1,2};
    return Y+a[X]*NUMBER_OF_QUALIFIERS_PER_CATEGORY;
}

static long init_length(grib_accessor* a)
{
    grib_accessor_bufr_data *self =(grib_accessor_bufr_data*)a;
    long section4Length=0;

    grib_handle* h=grib_handle_of_accessor(a);

    grib_get_long(h,self->section4LengthName,&section4Length);

    return section4Length-4;
}

static int get_elements_and_decode(grib_accessor* a);

static void init(grib_accessor* a,const long v, grib_arguments* params)
{
    grib_accessor_bufr_data *self =(grib_accessor_bufr_data*)a;
    int n = 0;

    a->sub_section           = grib_section_create(grib_handle_of_accessor(a),a);

    self->offsetSection4Name = grib_arguments_get_name(grib_handle_of_accessor(a),params,n++);
    self->offsetBeforeDataName = grib_arguments_get_name(grib_handle_of_accessor(a),params,n++);
    self->offsetEndSection4Name = grib_arguments_get_name(grib_handle_of_accessor(a),params,n++);
    self->section4LengthName = grib_arguments_get_name(grib_handle_of_accessor(a),params,n++);
    self->numberOfDataSubsetsName = grib_arguments_get_name(grib_handle_of_accessor(a),params,n++);
    self->subsetNumberName    = grib_arguments_get_name(grib_handle_of_accessor(a),params,n++);
    self->expandedDescriptorsName = grib_arguments_get_name(grib_handle_of_accessor(a),params,n++);
    self->abbreviationName = grib_arguments_get_name(grib_handle_of_accessor(a),params,n++);
    self->typeName = grib_arguments_get_name(grib_handle_of_accessor(a),params,n++);
    self->nameName = grib_arguments_get_name(grib_handle_of_accessor(a),params,n++);
    self->unitName = grib_arguments_get_name(grib_handle_of_accessor(a),params,n++);
    self->referenceName = grib_arguments_get_name(grib_handle_of_accessor(a),params,n++);
    self->scaleName = grib_arguments_get_name(grib_handle_of_accessor(a),params,n++);
    self->widthName = grib_arguments_get_name(grib_handle_of_accessor(a),params,n++);
    self->codeFlags = grib_arguments_get_name(grib_handle_of_accessor(a),params,n++);
    self->bitmapNumberName = grib_arguments_get_name(grib_handle_of_accessor(a),params,n++);
    self->associatedBitmapNumberName = grib_arguments_get_name(grib_handle_of_accessor(a),params,n++);
    self->associatedBitmapIndexName = grib_arguments_get_name(grib_handle_of_accessor(a),params,n++);
    self->associatedInfoNumberName = grib_arguments_get_name(grib_handle_of_accessor(a),params,n++);
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
    self->do_compute=1;

    a->length = init_length(a);

    /* Assert(a->length>=0); */
}

static void self_clear(grib_context* c,grib_accessor_bufr_data* self)
{
    int i=0;
    for (i=0;i<self->numberOfDescriptors;i++) {
        grib_context_free(c,self->abbreviation[i]);
        grib_context_free(c,self->type[i]);
        grib_context_free(c,self->names[i]);
        grib_context_free(c,self->units[i]);
    }
    grib_context_free(c,self->expandedDescriptors);
    grib_context_free(c,self->abbreviation);
    grib_context_free(c,self->type);
    grib_context_free(c,self->names);
    grib_context_free(c,self->units);
    grib_context_free(c,self->reference);
    grib_context_free(c,self->width);
    grib_context_free(c,self->bitmapNumber);
    grib_context_free(c,self->associatedBitmapNumber);
    grib_context_free(c,self->associatedBitmapIndex);
    grib_context_free(c,self->associatedInfoNumber);
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
    grib_context_log(a->context,GRIB_LOG_DEBUG,"updating size of %s old %ld new %ld",a->name,a->length,s);
    a->length = s;
    Assert(a->length>=0);
}

static int compare(grib_accessor* a, grib_accessor* b) {
    return GRIB_NOT_IMPLEMENTED;
}

static int pack_long(grib_accessor* a, const long* val, size_t *len)
{
    grib_accessor_bufr_data *self =(grib_accessor_bufr_data*)a;
    self->do_compute=1;

    return GRIB_NOT_IMPLEMENTED;
}

static int pack_double(grib_accessor* a, const double* val, size_t *len)
{
    grib_accessor_bufr_data *self =(grib_accessor_bufr_data*)a;
    self->do_compute=1;
    return GRIB_NOT_IMPLEMENTED;
}

static int get_descriptors(grib_accessor* a)
{
    int err=0;
    size_t size=0;
    long* scale=0;
    double* factor=0;
    int i=0;
    grib_accessor_bufr_data *self =(grib_accessor_bufr_data*)a;
    grib_accessor* expandedDescriptors=0;
    grib_handle* h=grib_handle_of_accessor(a);
    grib_context* c=a->context;

    expandedDescriptors=grib_find_accessor(grib_handle_of_accessor(a),self->expandedDescriptorsName);
    if (!expandedDescriptors) {
        grib_context_log(a->context,GRIB_LOG_ERROR,
                "unable to find accessor %s",self->expandedDescriptorsName);
        return GRIB_NOT_FOUND;
    }

    if (self->numberOfDescriptors) self_clear(c,self);
    err=_grib_get_size(grib_handle_of_accessor(a),expandedDescriptors,&(self->numberOfDescriptors));
    if (err) return err;

    self->expandedDescriptors=(long*)grib_context_malloc_clear(a->context,sizeof(long)*self->numberOfDescriptors);
    if (!self->expandedDescriptors) {
        grib_context_log(a->context,GRIB_LOG_FATAL,
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

    self->reference=(long*)grib_context_malloc_clear(c,size*sizeof(long));
    err=grib_get_long_array(h,self->referenceName,self->reference,&size);

    scale=(long*)grib_context_malloc_clear(c,size*sizeof(long));
    factor=(double*)grib_context_malloc_clear(c,size*sizeof(double));
    err=grib_get_long_array(h,self->scaleName,scale,&size);
    for (i=0;i<size;i++) {
        if (scale[i]==GRIB_MISSING_LONG) {
            grib_context_log(c,GRIB_LOG_FATAL,"Missing table entry for descriptor %ld",self->expandedDescriptors[i]);
            return GRIB_MISSING_BUFR_ENTRY;
        }
        factor[i]=grib_power(-scale[i],10);
    }
    self->factor=factor;
    grib_context_free(c,scale);

    self->width=(long*)grib_context_malloc_clear(c,size*sizeof(long));
    err=grib_get_long_array(h,self->widthName,self->width,&size);

    self->bitmapNumber=(long*)grib_context_malloc_clear(c,size*sizeof(long));
    err=grib_get_long_array(h,self->bitmapNumberName,self->bitmapNumber,&size);

    self->associatedBitmapNumber=(long*)grib_context_malloc_clear(c,size*sizeof(long));
    err=grib_get_long_array(h,self->associatedBitmapNumberName,self->associatedBitmapNumber,&size);

    self->associatedBitmapIndex=(long*)grib_context_malloc_clear(c,size*sizeof(long));
    err=grib_get_long_array(h,self->associatedBitmapIndexName,self->associatedBitmapIndex,&size);

    self->associatedInfoNumber=(long*)grib_context_malloc_clear(c,size*sizeof(long));
    err=grib_get_long_array(h,self->associatedInfoNumberName,self->associatedInfoNumber,&size);

    err=grib_get_long(h,self->numberOfDataSubsetsName,&(self->numberOfDataSubsets));

    return err;
}

static void push_units_accessor(grib_section* section,grib_accessor* a,long groupNumber,long i)
{
    char buf[1024]={0,};
    grib_accessor_bufr_data *self =(grib_accessor_bufr_data*)a;
    grib_accessor* gaUnits=0;
    grib_action creatorUnits = {0, };
    grib_context* c=a->context;

    if (self->units[i][0]=='C' && ( !strcmp(self->units[i],"CCITTIA5") ||
            !strncmp(self->units[i],"COMMON",6)) )
        return;
    if (self->units[i][0]=='N' &&  !strcmp(self->units[i],"NUMERIC") )
        return;

    creatorUnits.op         = (char*)"variable";
    creatorUnits.name_space = (char*)"";
    creatorUnits.set        = 0;
    creatorUnits.flags      = GRIB_ACCESSOR_FLAG_DUMP;

    sprintf(buf,"%sUnits",self->abbreviation[i]);
    creatorUnits.name=buf;
    gaUnits = grib_accessor_factory(section, &creatorUnits, 0, NULL);
    gaUnits->bufr_group_number=groupNumber;
    ((grib_accessor_constant*)gaUnits)->type=GRIB_TYPE_STRING;

    if ((self->units[i][0]=='C' && !strncmp(self->units[i],"CODE",4)) ||
            ( self->units[i][0]=='F' && !strncmp(self->units[i],"FLAG",4) ) ) {
        sprintf(buf,"codetable %ld",self->expandedDescriptors[i]);
        ((grib_accessor_constant*)gaUnits)->cval=grib_context_strdup(c,buf);
    } else {
        ((grib_accessor_constant*)gaUnits)->cval=grib_context_strdup(c,self->units[i]);
    }

    grib_push_accessor(gaUnits,section->block);
}

static long computeBitmapStartElement(grib_accessor_bufr_data* self,long length, grib_iarray* index)
{
    int i,startIndex;
    int idx;

    startIndex=index->n-1;
    /* printf("======== BITMAP\n"); */
    for (i=0;i<length;i++) {
        idx=grib_iarray_get(index,startIndex);
        if (self->expandedDescriptors[idx]/100000!=2) {
            startIndex--;
            /* printf("==> %d %.6ld %s\n",startIndex,self->expandedDescriptors[idx],self->abbreviation[idx]); */
        }
    }

    startIndex++;
    return startIndex;
}

static grib_section* find_section_of_associated_group(grib_section** groupNumberSection,
        long* bitmapStartElement,grib_iarray* groupNumberIndex,long bitmapNumber)
{
    grib_section* section=0;
    long indexOfGroupNumber=0;
    long jj=bitmapStartElement[bitmapNumber];
    indexOfGroupNumber=grib_iarray_get(groupNumberIndex,jj);
    section=groupNumberSection[indexOfGroupNumber+1];
    return section;
}

static GRIB_INLINE void reset_qualifiers(grib_accessor* significanceQualifierGroup[])
{
    int i;
    for (i=0;i<number_of_qualifiers;i++)
        significanceQualifierGroup[i]=0;
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

static int decode_elements(grib_accessor* a)
{
    int err=0;
    grib_accessor_bufr_data *self =(grib_accessor_bufr_data*)a;
    unsigned char* data =NULL;
    int forceGroupClosure=0;
    long i=0,k=0,jj=0;
    long pos=0;
    long localReference=0,width,j;
    long numberOfValuesPerElement;
    double* dvalues;
    grib_sarray* svalues = NULL;
    size_t n,nn;
    int *F,*X,*Y;
    char* sval;
    long lval = 0;
    long groupNumber;
    int numberOfElementsToRepeat=0,numberOfRepetitions=0;
    int startRepetition=0;
    int depth;
    int bitmapIndex[10];
    /*long bitmapLength[10]={0,};*/
    long bitmapStartElement[10]={0,};
    grib_section* section=0;
    grib_section* sectionUp=0;
    grib_section* groupSection=0;
    grib_iarray* index=0;
    grib_iarray* bitmap=0;
    /* int count; */
    long indexOfReferringElement;
    long indexOfGroupNumber;
    grib_accessor* gaGroup=0;
    grib_action creatorGroup = {0, };
    grib_iarray* groupNumberIndex=0;
    grib_section* groupNumberSection[1000]={0,};
    grib_accessor* significanceQualifierGroup[NUMBER_OF_QUALIFIERS_PER_CATEGORY*NUMBER_OF_QUALIFIERS_CATEGORIES]={0,};
    int significanceQualifierDepth[NUMBER_OF_QUALIFIERS_PER_CATEGORY*NUMBER_OF_QUALIFIERS_CATEGORIES]={0,};

    grib_handle* h=grib_handle_of_accessor(a);
    grib_context* c=h->context;

    grib_accessor* ga=0;
    grib_action creator = {0, };
    creator.op         = (char*)"bufr_data_element";
    creator.name_space = (char*)"";
    creator.flags     = GRIB_ACCESSOR_FLAG_DUMP;
    creator.set        = 0;

    creatorGroup.op         = (char*)"bufr_group";
    creatorGroup.name       = (char*)"groupNumber";
    creatorGroup.name_space = (char*)"";
    creatorGroup.flags      = GRIB_ACCESSOR_FLAG_DUMP;
    creatorGroup.set        = 0;

    data = (unsigned char*)h->buffer->data;
    pos=a->offset*8;

    for (i=0;i<10;i++) bitmapIndex[i]=-1;

    err=get_descriptors(a);
    if (err) return err;

    index=grib_iarray_new(c,self->numberOfDescriptors,100);
    groupNumberIndex=grib_iarray_new(c,self->numberOfDescriptors,100);
    bitmap=grib_iarray_new(c,self->numberOfDescriptors,100);

    /*totalNumberOfValues=self->numberOfDescriptors*self->numberOfDataSubsets;*/
    numberOfValuesPerElement=self->numberOfDataSubsets;

    F=(int*)grib_context_malloc_clear(c,sizeof(int)*self->numberOfDescriptors);
    X=(int*)grib_context_malloc_clear(c,sizeof(int)*self->numberOfDescriptors);
    Y=(int*)grib_context_malloc_clear(c,sizeof(int)*self->numberOfDescriptors);
    for (i=0;i<self->numberOfDescriptors;i++) {
        F[i]=self->expandedDescriptors[i]/100000;
        X[i]=(self->expandedDescriptors[i]-F[i]*100000)/1000;
        Y[i]=(self->expandedDescriptors[i]-F[i]*100000)%1000;
    }

    nn=0;
    k=-1;
    groupNumber=1;

    gaGroup = grib_accessor_factory(a->sub_section, &creatorGroup, 0, NULL);
    gaGroup->bufr_group_number=groupNumber;
    gaGroup->sub_section=grib_section_create(grib_handle_of_accessor(a),gaGroup);
    section=gaGroup->sub_section;
    sectionUp=a->sub_section;
    ((grib_accessor_constant*)gaGroup)->type=GRIB_TYPE_LONG;
    ((grib_accessor_constant*)gaGroup)->dval=groupNumber;
    grib_push_accessor(gaGroup,a->sub_section->block);

    forceGroupClosure=0;
    indexOfGroupNumber=0;
    depth=0;
    /* count=1; */
    /* printf("=================================\n"); */
    for (i=0;i<self->numberOfDescriptors;i++) {

        if (F[i]==0 && IS_QUALIFIER(X[i]) && self->associatedInfoNumber[i]==0) {
            int idx=significanceQualifierIndex(X[i],Y[i]);
            groupNumber++;

            if (significanceQualifierGroup[idx]) {
                groupSection=significanceQualifierGroup[idx]->parent;
                depth=significanceQualifierDepth[idx];
                reset_deeper_qualifiers(significanceQualifierGroup,significanceQualifierDepth,depth);
                forceGroupClosure=0;
            } else {
                if (forceGroupClosure) {
                    groupSection=sectionUp;
                    forceGroupClosure=0;
                    depth=0;
                } else {
                    groupSection=section;
                    depth++;
                }
            }

            gaGroup = grib_accessor_factory(groupSection, &creatorGroup, 0, NULL);
            gaGroup->sub_section=grib_section_create(grib_handle_of_accessor(a),gaGroup);
            gaGroup->bufr_group_number=groupNumber;
            ((grib_accessor_constant*)gaGroup)->type=GRIB_TYPE_LONG;
            ((grib_accessor_constant*)gaGroup)->dval=groupNumber;
            grib_push_accessor(gaGroup,groupSection->block);

            section=gaGroup->sub_section;
            sectionUp=gaGroup->parent;

            significanceQualifierGroup[idx]=gaGroup;
            significanceQualifierDepth[idx]=depth;
        }

        if (F[i]==1) {
            /* delayed replication */
            if (! IS_QUALIFIER(X[i-1])) {
                forceGroupClosure=1;
                reset_qualifiers(significanceQualifierGroup);
            }
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
            if (self->bitmapNumber[i+1]) {
                /*bitmapLength[self->bitmapNumber[i+1]]=numberOfRepetitions;*/
                bitmapStartElement[self->bitmapNumber[i+1]]=computeBitmapStartElement(self,numberOfRepetitions,index);
            }
            grib_iarray_push(index,i);
            grib_iarray_push(groupNumberIndex,groupNumber);
            groupNumberSection[groupNumber]=section;
            indexOfGroupNumber=0;
            continue;
        }

        if (self->bitmapNumber[i]) {
            /* bitmap extraction */
            lval=grib_decode_unsigned_long(data,&pos,1);
            width=grib_decode_unsigned_long(data,&pos,6);
            if (width) {Assert(0);}

            if(bitmapIndex[self->bitmapNumber[i]]==-1)
                bitmapIndex[self->bitmapNumber[i]]=grib_iarray_used_size(bitmap);
            grib_iarray_push(index,i);
            grib_iarray_push(groupNumberIndex,groupNumber);
            groupNumberSection[groupNumber]=section;
            grib_iarray_push(bitmap,lval) ;
            k=-1;
            jj=0;
        } else if (self->associatedBitmapNumber[i]) {
            /* bitmap application */
            char name[1024]={0,};

            if (k==-1) {
                k=bitmapIndex[self->associatedBitmapNumber[i]];
                jj=bitmapStartElement[self->associatedBitmapNumber[i]];
            } else {
                k++;
            }

            indexOfReferringElement=grib_iarray_get(index,jj);
            indexOfGroupNumber=grib_iarray_get(groupNumberIndex,jj);
            sprintf(name,"%sOf-%s",self->abbreviation[i],self->abbreviation[indexOfReferringElement]);
            jj++;
            if (grib_iarray_get(bitmap,k)==0) {
                creator.name = name;
                ga = grib_accessor_factory(section, &creator, 0, NULL);
                ga->bufr_group_number=indexOfGroupNumber;
                ((grib_accessor_bufr_data_element*)ga)->type=GRIB_TYPE_DOUBLE;
                lval=grib_decode_unsigned_long(data,&pos,self->width[indexOfReferringElement]);
                localReference=lval+self->reference[indexOfReferringElement];
                width=grib_decode_unsigned_long(data,&pos,6);
                if (width) {
                    ((grib_accessor_bufr_data_element*)ga)->isConstant=0;
                    n=numberOfValuesPerElement;
                } else {
                    ((grib_accessor_bufr_data_element*)ga)->isConstant=1;
                    n=1;
                }
                dvalues=(double*)grib_context_malloc_clear(c,sizeof(double)*n);
                if (((grib_accessor_bufr_data_element*)ga)->isConstant) {
                    if (!grib_is_all_bits_one(lval,self->width[indexOfReferringElement])) {
                        dvalues[0]=localReference*self->factor[indexOfReferringElement];
                    } else dvalues[0]=GRIB_MISSING_DOUBLE;
                } else {
                    for (j=0;j<n;j++) {
                        lval=grib_decode_unsigned_long(data,&pos,width);
                        if (!grib_is_all_bits_one(lval,width)) {
                            dvalues[j]=(localReference+lval)*self->factor[indexOfReferringElement];
                        } else dvalues[j]=GRIB_MISSING_DOUBLE;
                    }
                }
                ((grib_accessor_bufr_data_element*)ga)->type=GRIB_TYPE_DOUBLE;
                ((grib_accessor_bufr_data_element*)ga)->dvalues=dvalues;
                ((grib_accessor_bufr_data_element*)ga)->numberOfValues=numberOfValuesPerElement;

                grib_iarray_push(index,i);
                grib_iarray_push(groupNumberIndex,ga->bufr_group_number);
                grib_push_accessor(ga,groupNumberSection[ga->bufr_group_number]->block);
            } else {
                i--;
                continue;
            }
        } else {

            if (*(self->type[i])=='s') {
                svalues=grib_sarray_new(c,numberOfValuesPerElement,100);
                sval=(char*)grib_context_malloc_clear(c,self->width[i]/8+1);
                grib_decode_string(data,&pos,self->width[i]/8,sval);
                grib_sarray_push(c,svalues,sval);
            } else {
                lval=grib_decode_unsigned_long(data,&pos,self->width[i]);
                localReference=lval+self->reference[i];
            }

            width=grib_decode_unsigned_long(data,&pos,6);
            creator.name = self->abbreviation[i];
            ga = grib_accessor_factory(section, &creator, 0, NULL);
            ga->bufr_group_number=groupNumber;
            ((grib_accessor_bufr_data_element*)ga)->numberOfValues=numberOfValuesPerElement;


            if (width) {
                ((grib_accessor_bufr_data_element*)ga)->isConstant=0;
                n=numberOfValuesPerElement;
            } else {
                ((grib_accessor_bufr_data_element*)ga)->isConstant=1;
                n=1;
            }
            if (*(self->type[i])=='s') {
                /* string element */
                if (!width) {
                    width=self->width[i];
                    n=1;
                }
                for (j=1;j<n;j++) {
                    sval=(char*)grib_context_malloc_clear(c,width/8+1);
                    grib_decode_string(data,&pos,width/8,sval);
                    grib_sarray_push(c,svalues,sval);
                }
                ((grib_accessor_bufr_data_element*)ga)->type=GRIB_TYPE_STRING;
                ((grib_accessor_bufr_data_element*)ga)->svalues=svalues;
            } else {
                dvalues=(double*)grib_context_malloc_clear(c,sizeof(double)*n);
                if (((grib_accessor_bufr_data_element*)ga)->isConstant) {
                    if (!grib_is_all_bits_one(lval,self->width[i])) {
                        dvalues[0]=localReference*self->factor[i];
                    } else dvalues[0]=GRIB_MISSING_DOUBLE;
                } else {
                    for (j=0;j<n;j++) {
                        lval=grib_decode_unsigned_long(data,&pos,width);
                        if (!grib_is_all_bits_one(lval,width)) {
                            dvalues[j]=(localReference+lval)*self->factor[i];
                        } else dvalues[j]=GRIB_MISSING_DOUBLE;
                    }
                }
                ((grib_accessor_bufr_data_element*)ga)->type=GRIB_TYPE_DOUBLE;
                ((grib_accessor_bufr_data_element*)ga)->dvalues=dvalues;
                if (*(self->type[i])=='l')
                    ((grib_accessor_bufr_data_element*)ga)->type=GRIB_TYPE_LONG;
            }
            grib_iarray_push(index,i);
            grib_iarray_push(groupNumberIndex,groupNumber);
            groupNumberSection[groupNumber]=section;
            /* printf("==== %s %ld\n",ga->name,ga->bufr_group_number); */
            if (self->associatedInfoNumber[i]) {
                grib_section* sectionOfAssociatedGroup=find_section_of_associated_group(groupNumberSection,
                        bitmapStartElement,groupNumberIndex,self->associatedInfoNumber[i]);
                if (!sectionOfAssociatedGroup) sectionOfAssociatedGroup=section;
                push_units_accessor(sectionOfAssociatedGroup,a,sectionOfAssociatedGroup->block->first->bufr_group_number,i);
                ga->bufr_group_number=sectionOfAssociatedGroup->block->first->bufr_group_number;
                grib_push_accessor(ga,sectionOfAssociatedGroup->block);
            } else {
                push_units_accessor(section,a,groupNumber,i);
                grib_push_accessor(ga,section->block);
            }

            k=-1;
            jj=0;
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
    grib_iarray_delete(index);
    grib_iarray_delete(bitmap);
    grib_iarray_delete(groupNumberIndex);
    grib_context_free(c,F);
    grib_context_free(c,X);
    grib_context_free(c,Y);
    return err;
}

static void dump(grib_accessor* a, grib_dumper* dumper)
{
    get_elements_and_decode(a);

    grib_dump_section(dumper,a,a->sub_section->block);

    return;
}

static int value_count(grib_accessor* a,long* count)
{
    int err=0;
    grib_accessor_bufr_data *self =(grib_accessor_bufr_data*)a;
    grib_handle* h=grib_handle_of_accessor(a);

    err=get_descriptors(a);
    if (err) return err;

    grib_get_long(h,self->numberOfDataSubsetsName,&(self->numberOfDataSubsets));

    *count=self->numberOfElements*self->numberOfDataSubsets;
    return err;
}

static int unpack_double(grib_accessor* a, double* val, size_t *len)
{
    int err=0;

    err=get_elements_and_decode(a);
    if (err) return err;

    if (!val) return GRIB_SUCCESS;
    else return GRIB_NOT_IMPLEMENTED;
}

static int get_elements_and_decode(grib_accessor* a)
{
    grib_accessor_bufr_data *self =(grib_accessor_bufr_data*)a;
    int err=0;
    if (self->do_compute) {
        err=decode_elements(a);
        self->do_compute=0;
    }
    return err;
}

static void destroy(grib_context* c,grib_accessor* a)
{
    grib_accessor_bufr_data *self =(grib_accessor_bufr_data*)a;
    self_clear(c,self);
    grib_section_delete(c,a->sub_section);
}
