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
SUPER      = grib_accessor_class_long
IMPLEMENTS = unpack_long;pack_long
IMPLEMENTS = unpack_double;pack_double
IMPLEMENTS = unpack_string_array
IMPLEMENTS = init;dump;destroy
IMPLEMENTS = value_count;get_native_type
MEMBERS    = const char* expandedDescriptors
MEMBERS    = const char* expandedScale
MEMBERS    = const char* expandedReference
MEMBERS    = const char* expandedWidth
MEMBERS    = const char* abbreviation
MEMBERS    = const char* type
MEMBERS    = const char* name
MEMBERS    = const char* unit
MEMBERS    = int index
MEMBERS    = long* expandedAO
MEMBERS    = long* scaleAO
MEMBERS    = double* referenceAO
MEMBERS    = long* widthAO
MEMBERS    = long* bitmapNumber
MEMBERS    = long* associatedBitmapNumber
MEMBERS    = long* associatedBitmapIndex
MEMBERS    = long* associatedInfoNumber
MEMBERS    = char** abbreviations
MEMBERS    = char** types
MEMBERS    = char** names
MEMBERS    = char** units
MEMBERS    = size_t expandedAOSize
MEMBERS    = int do_compute

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
static int unpack_long(grib_accessor*, long* val,size_t *len);
static int unpack_string_array (grib_accessor*, char**, size_t *len);
static int value_count(grib_accessor*,long*);
static void destroy(grib_context*,grib_accessor*);
static void dump(grib_accessor*, grib_dumper*);
static void init(grib_accessor*,const long, grib_arguments* );
static void init_class(grib_accessor_class*);

typedef struct grib_accessor_apply_operators {
    grib_accessor          att;
/* Members defined in gen */
/* Members defined in long */
/* Members defined in apply_operators */
	const char* expandedDescriptors;
	const char* expandedScale;
	const char* expandedReference;
	const char* expandedWidth;
	const char* abbreviation;
	const char* type;
	const char* name;
	const char* unit;
	int index;
	long* expandedAO;
	long* scaleAO;
	double* referenceAO;
	long* widthAO;
	long* bitmapNumber;
	long* associatedBitmapNumber;
	long* associatedBitmapIndex;
	long* associatedInfoNumber;
	char** abbreviations;
	char** types;
	char** names;
	char** units;
	size_t expandedAOSize;
	int do_compute;
} grib_accessor_apply_operators;

extern grib_accessor_class* grib_accessor_class_long;

static grib_accessor_class _grib_accessor_class_apply_operators = {
    &grib_accessor_class_long,                      /* super                     */
    "apply_operators",                      /* name                      */
    sizeof(grib_accessor_apply_operators),  /* size                      */
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
    &pack_long,                  /* grib_pack procedures long      */
    &unpack_long,                /* grib_unpack procedures long    */
    &pack_double,                /* grib_pack procedures double    */
    &unpack_double,              /* grib_unpack procedures double  */
    0,                /* grib_pack procedures string    */
    0,              /* grib_unpack procedures string  */
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
    0,              		/* clear          */
    0,               		/* clone accessor          */
};


grib_accessor_class* grib_accessor_class_apply_operators = &_grib_accessor_class_apply_operators;


static void init_class(grib_accessor_class* c)
{
	c->next_offset	=	(*(c->super))->next_offset;
	c->string_length	=	(*(c->super))->string_length;
	c->byte_count	=	(*(c->super))->byte_count;
	c->byte_offset	=	(*(c->super))->byte_offset;
	c->sub_section	=	(*(c->super))->sub_section;
	c->pack_missing	=	(*(c->super))->pack_missing;
	c->is_missing	=	(*(c->super))->is_missing;
	c->pack_string	=	(*(c->super))->pack_string;
	c->unpack_string	=	(*(c->super))->unpack_string;
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
	c->make_clone	=	(*(c->super))->make_clone;
}

/* END_CLASS_IMP */

#define MAX_DELAYED_REPLICATION_COUNT 1000

static void self_clear(grib_context* c,grib_accessor_apply_operators* self) {
  size_t i=0;
  if (!self->expandedAOSize) return;

  grib_context_free(c,self->expandedAO);
  grib_context_free(c,self->scaleAO);
  grib_context_free(c,self->referenceAO);
  grib_context_free(c,self->widthAO);
  grib_context_free(c,self->bitmapNumber);
  grib_context_free(c,self->associatedBitmapNumber);
  grib_context_free(c,self->associatedBitmapIndex);
  grib_context_free(c,self->associatedInfoNumber);

  for (i=0;i<self->expandedAOSize;i++) {
    if (!self->abbreviations[i]) break;
    grib_context_free(c,self->abbreviations[i]);
    grib_context_free(c,self->types[i]);
    grib_context_free(c,self->names[i]);
    grib_context_free(c,self->units[i]);
  }
  grib_context_free(c,self->abbreviations);
  grib_context_free(c,self->types);
  grib_context_free(c,self->names);
  grib_context_free(c,self->units);
}

static void init(grib_accessor* a, const long len , grib_arguments* args )
{
  grib_accessor_apply_operators* self = (grib_accessor_apply_operators*)a;
  int n=0;
  self->expandedDescriptors=grib_arguments_get_name(grib_handle_of_accessor(a),args,n++);
  self->abbreviation=grib_arguments_get_name(grib_handle_of_accessor(a),args,n++);
  self->type=grib_arguments_get_name(grib_handle_of_accessor(a),args,n++);
  self->name=grib_arguments_get_name(grib_handle_of_accessor(a),args,n++);
  self->unit=grib_arguments_get_name(grib_handle_of_accessor(a),args,n++);
  self->expandedScale=grib_arguments_get_name(grib_handle_of_accessor(a),args,n++);
  self->expandedReference=grib_arguments_get_name(grib_handle_of_accessor(a),args,n++);
  self->expandedWidth=grib_arguments_get_name(grib_handle_of_accessor(a),args,n++);
  self->index=grib_arguments_get_long(grib_handle_of_accessor(a),args,n++);
  self->do_compute=1;
  a->length = 0;
  self->scaleAO=0;
  self->expandedAOSize=0;
}


static void dump(grib_accessor* a, grib_dumper* dumper)
{
  int type=get_native_type(a);

  switch (type) {
    case GRIB_TYPE_LONG:
      grib_dump_long(dumper,a,NULL);
      break;
    case GRIB_TYPE_DOUBLE:
      grib_dump_double(dumper,a,NULL);
      break;
    case GRIB_TYPE_STRING:
      grib_dump_string_array(dumper,a,NULL);
      break;
  }
}

static int  get_native_type(grib_accessor* a){
  int type=GRIB_TYPE_LONG;
  if (a->flags & GRIB_ACCESSOR_FLAG_STRING_TYPE)
    type=GRIB_TYPE_STRING;
  return type;
}

size_t compute_size_AO(long* descriptors,size_t numberOfDescriptors) {
        int i=0;
        size_t sizeAO=numberOfDescriptors;
        int extraElement=0;
        int X,F,Y;

	for (i=0;i<numberOfDescriptors;i++) {
		F=descriptors[i]/100000;
		X=(descriptors[i]-F*100000)/1000;
		Y=(descriptors[i]-F*100000)%1000;
                if (F==0 && X!=31) sizeAO+=extraElement;
                if (F==2 && X==4) extraElement= Y ? 1 : 0;
        }
        return sizeAO;

}

static void set_bitmap_number(long* descriptors,int *useDefinedBitmap,size_t* i,int* bitmapNumber) {
  switch (descriptors[(*i)+1]) {
    case 236000:
      *useDefinedBitmap=1;
      (*bitmapNumber)++;
      (*i)++;
      break;
    case 237000:
      if (! *useDefinedBitmap) (*bitmapNumber)++;
      (*i)++;
      break;
    case 237255:
      *useDefinedBitmap=0;
      break;
    default:
      (*bitmapNumber)++;
  }
}

static void computeDelayedReplication(grib_accessor_apply_operators* self,
    long* delayedReplication,long* delayedReplicationIndex,long delayedReplicationCount,long index) {
  long i=0;
  for (i=0;i<delayedReplicationCount;i++) {
    if (delayedReplication[i]>0) {
      self->expandedAO[delayedReplicationIndex[i]]-=1000;
      delayedReplication[i]--;
    }
  }
}

static int apply_operators(grib_accessor* a) {
	grib_accessor_apply_operators* self = (grib_accessor_apply_operators*)a;
	grib_context* c=a->context;
	grib_handle* h=grib_handle_of_accessor(a);
  int useDefinedBitmap;
	long* descriptors=0;
	long* scale=0;
	long* width=0;
	long* reference=0;
  char** abbreviations=0;
  char** names=0;
  char** types=0;
  char** units=0;
  int bitmapNumber=0;
  double referenceValueFactor=1;
  int X,F,Y;
  long extraWidth=0,extraScale=0,localDescriptorWidth=0;
	size_t size=0,sizeAO=0,iq=0,numberOfDescriptors;
	size_t i=0,j=0,ielement=0;
	int err=0;
  char* missing="???";
	long associatedFieldWidth=0;
	char* firstOrderStatisticsAbbreviation="firstOrderStatistics";
	char* differenceStatisticsAbbreviation="differenceStatistics";
	char* associatedFieldAbbreviation="associatedField";
	char* substitutedValueAbbreviation="substitutedValue";
	char* associatedFieldName="associated field";
	char* associatedFieldType="long";
	char* associatedFieldUnit="CODE TABLE 31021";
	long associatedFieldDescriptor=99999;
  long delayedReplication[MAX_DELAYED_REPLICATION_COUNT]={0,};
  long delayedReplicationIndex[MAX_DELAYED_REPLICATION_COUNT]={0,};
  long delayedReplicationCount=0;
  long iassociatedInfoNumber=0;
  int id=0;


  if (!self->do_compute) return GRIB_SUCCESS;
  self_clear(c,self);

	err=grib_get_size(grib_handle_of_accessor(a),self->expandedDescriptors,&size);
	if (err) return err;
  numberOfDescriptors=size;

	descriptors=(long*)grib_context_malloc_clear(c,sizeof(long)*size);
	scale=(long*)grib_context_malloc_clear(c,sizeof(long)*size);
	reference=(long*)grib_context_malloc_clear(c,sizeof(long)*size);
	width=(long*)grib_context_malloc_clear(c,sizeof(long)*size);

	abbreviations=(char**)grib_context_malloc_clear(c,sizeof(char*)*size);
	types=(char**)grib_context_malloc_clear(c,sizeof(char*)*size);
	names=(char**)grib_context_malloc_clear(c,sizeof(char*)*size);
	units=(char**)grib_context_malloc_clear(c,sizeof(char*)*size);

	err=grib_get_string_array(h,self->abbreviation,abbreviations,&size);
	if (err) return err;
	err=grib_get_string_array(h,self->type,types,&size);
	if (err) return err;
	err=grib_get_string_array(h,self->name,names,&size);
	if (err) return err;
	err=grib_get_string_array(h,self->unit,units,&size);
	if (err) return err;

	err=grib_get_long_array(h,self->expandedDescriptors,descriptors,&size);
	if (err) return err;
	err=grib_get_long_array(h,self->expandedScale,scale,&size);
	if (err) return err;
	err=grib_get_long_array(h,self->expandedReference,reference,&size);
	if (err) return err;
	err=grib_get_long_array(h,self->expandedWidth,width,&size);
	if (err) return err;

  sizeAO=compute_size_AO(descriptors,numberOfDescriptors);
	self->expandedAO=(long*)grib_context_malloc_clear(c,sizeof(long)*sizeAO);
	self->scaleAO=(long*)grib_context_malloc_clear(c,sizeof(long)*sizeAO);
	self->referenceAO=(double*)grib_context_malloc_clear(c,sizeof(double)*sizeAO);
	self->widthAO=(long*)grib_context_malloc_clear(c,sizeof(long)*sizeAO);
	self->bitmapNumber=(long*)grib_context_malloc_clear(c,sizeof(long)*sizeAO);
	self->associatedBitmapNumber=(long*)grib_context_malloc_clear(c,sizeof(long)*sizeAO);
	self->associatedBitmapIndex=(long*)grib_context_malloc_clear(c,sizeof(long)*sizeAO);
	self->associatedInfoNumber=(long*)grib_context_malloc_clear(c,sizeof(long)*sizeAO);
	self->abbreviations=(char**)grib_context_malloc_clear(c,sizeof(char*)*sizeAO);
	self->types=(char**)grib_context_malloc_clear(c,sizeof(char*)*sizeAO);
	self->names=(char**)grib_context_malloc_clear(c,sizeof(char*)*sizeAO);
	self->units=(char**)grib_context_malloc_clear(c,sizeof(char*)*sizeAO);

	j=0;
  ielement=0;
  iassociatedInfoNumber=0;
	for (i=0;i<numberOfDescriptors;i++) {
		F=descriptors[i]/100000;
		X=(descriptors[i]-F*100000)/1000;
		Y=(descriptors[i]-F*100000)%1000;

		if ( F==2 ) {
		    /*operators*/
		    switch (X) {
          case 1:
            extraWidth = Y ? Y-128 : 0;
            computeDelayedReplication(self,delayedReplication,delayedReplicationIndex,delayedReplicationCount,i);
            break;
          case 2:
            extraScale = Y ? Y-128 : 0;
            computeDelayedReplication(self,delayedReplication,delayedReplicationIndex,delayedReplicationCount,i);
            break;
          case 4:
            /* associated field*/
            associatedFieldWidth=Y;
            computeDelayedReplication(self,delayedReplication,delayedReplicationIndex,delayedReplicationCount,i);
            break;
          case 6:
            /*signify data width*/
            localDescriptorWidth=Y;
            computeDelayedReplication(self,delayedReplication,delayedReplicationIndex,delayedReplicationCount,i);
            break;
          case 7:
            if (Y) {
              extraScale = Y;
              referenceValueFactor=grib_power(Y,10);
              extraWidth=((10*Y)+2)/3;
            } else {
              extraWidth=0;
              extraScale=0;
              referenceValueFactor=1;
            }
            computeDelayedReplication(self,delayedReplication,delayedReplicationIndex,delayedReplicationCount,i);
            break;
          case 22:
            /*quality information follows*/
            iq=1;
            set_bitmap_number(descriptors,&useDefinedBitmap,&i,&bitmapNumber);
            iassociatedInfoNumber=bitmapNumber;
            computeDelayedReplication(self,delayedReplication,delayedReplicationIndex,delayedReplicationCount,i);
            break;
          case 23:
            /*substituted values*/
            switch (Y) {
              case 0:
                iq=1;
                set_bitmap_number(descriptors,&useDefinedBitmap,&i,&bitmapNumber);
                iassociatedInfoNumber=bitmapNumber;
                break;
              case 255:
                self->expandedAO[j]=descriptors[i];
                self->associatedBitmapIndex[j]=iq;
                self->associatedBitmapNumber[j]=bitmapNumber;
                self->abbreviations[j]=grib_context_strdup(c,substitutedValueAbbreviation);
                self->types[j]=grib_context_strdup(c,missing);
                self->names[j]=grib_context_strdup(c,missing);
                self->units[j]=grib_context_strdup(c,missing);
                iq++;
                j++;
                break;
              default :
                grib_context_log(c,GRIB_LOG_FATAL,"operator %d not implemented", (int)descriptors[i]);
            }
            break;
          case 24:
            /*first order statistical operators values*/
            switch (Y) {
              case 0:
                iq=1;
                set_bitmap_number(descriptors,&useDefinedBitmap,&i,&bitmapNumber);
                iassociatedInfoNumber=bitmapNumber;
                break;
              case 255:
                self->expandedAO[j]=descriptors[i];
                self->associatedBitmapIndex[j]=iq;
                self->associatedBitmapNumber[j]=bitmapNumber;
                self->abbreviations[j]=grib_context_strdup(c,firstOrderStatisticsAbbreviation);
                self->types[j]=grib_context_strdup(c,missing);
                self->names[j]=grib_context_strdup(c,missing);
                self->units[j]=grib_context_strdup(c,missing);
                iq++;
                j++;
                break;
              default :
                grib_context_log(c,GRIB_LOG_FATAL,"operator %d not implemented", (int)descriptors[i]);
            }
            break;
          case 25:
            /*difference statistical operators values*/
            switch (Y) {
              case 0:
                iq=1;
                set_bitmap_number(descriptors,&useDefinedBitmap,&i,&bitmapNumber);
                iassociatedInfoNumber=bitmapNumber;
                break;
              case 255:
                self->expandedAO[j]=descriptors[i];
                self->associatedBitmapIndex[j]=iq;
                self->associatedBitmapNumber[j]=bitmapNumber;
                self->abbreviations[j]=grib_context_strdup(c,differenceStatisticsAbbreviation);
                self->types[j]=grib_context_strdup(c,missing);
                self->names[j]=grib_context_strdup(c,missing);
                self->units[j]=grib_context_strdup(c,missing);
                iq++;
                j++;
                break;
              default :
                grib_context_log(c,GRIB_LOG_FATAL,"operator %d not implemented", (int)descriptors[i]);
            }
            break;
          default:
            grib_context_log(c,GRIB_LOG_ERROR,"operator %d not implemented",(int)descriptors[i]);
		    }
		} else if ( F==1 ) {
			self->expandedAO[j]=descriptors[i];
      self->abbreviations[j]=grib_context_strdup(c,missing);
      self->types[j]=grib_context_strdup(c,missing);
      self->names[j]=grib_context_strdup(c,missing);
      self->units[j]=grib_context_strdup(c,missing);
      delayedReplication[delayedReplicationCount]=X;
      delayedReplicationIndex[delayedReplicationCount]=j;
      delayedReplicationCount++;
      Assert(delayedReplicationCount<=MAX_DELAYED_REPLICATION_COUNT);
      j++;
		} else {
      if (localDescriptorWidth) {
        if (scale[ielement]==GRIB_MISSING_LONG) scale[ielement] = 0 ;
        if (reference[ielement]==GRIB_MISSING_LONG) reference[ielement]=0;
        if (width[ielement]==GRIB_MISSING_LONG) width[ielement]=localDescriptorWidth;
        localDescriptorWidth=0;
      }
			if (associatedFieldWidth && X!=31) {
				self->scaleAO[j]=0;
				self->referenceAO[j]=0;
				self->widthAO[j]=associatedFieldWidth;
				self->abbreviations[j]=grib_context_strdup(c,associatedFieldAbbreviation);
				self->names[j]=grib_context_strdup(c,associatedFieldName);
				self->types[j]=grib_context_strdup(c,associatedFieldType);
				self->units[j]=grib_context_strdup(c,associatedFieldUnit);
			  self->expandedAO[j]=associatedFieldDescriptor;

        for (id=0;id<delayedReplicationCount;id++) delayedReplication[id]--;
				j++;
			}
      if (!strncmp(units[ielement],"CODE",4) || !strncmp(units[ielement],"FLAG",4)) {
        self->scaleAO[j]=scale[ielement];
        self->associatedInfoNumber[j]=iassociatedInfoNumber;
        self->referenceAO[j]=reference[ielement];
        self->widthAO[j]=width[ielement];
      } else {
        self->scaleAO[j]=extraScale+scale[ielement];
        self->associatedInfoNumber[j]=iassociatedInfoNumber;
        self->referenceAO[j]=reference[ielement]*referenceValueFactor;
        self->widthAO[j]=width[ielement]+extraWidth;
      }
        self->abbreviations[j]=abbreviations[ielement];
        self->names[j]=names[ielement];
        self->types[j]=types[ielement];
        self->units[j]=units[ielement];
        self->expandedAO[j]=descriptors[i];
      /* quality information*/
      if (X==33) {
        self->associatedBitmapIndex[j]=iq;
        self->associatedBitmapNumber[j]=bitmapNumber;
        iq++;
      }
      /* bitmap */
      if (X==31 && Y==31) { self->bitmapNumber[j]=bitmapNumber; }

      for (id=0;id<delayedReplicationCount;id++) delayedReplication[id]--;
      j++;
      ielement++;
    }

	}
  self->expandedAOSize=j;

  /*{
  int mycount=1;
  printf("+++++++++++++++++++++++++++++\n");
  for (i=0;i<self->expandedAOSize;i++) {
    printf("%.6ld ",self->expandedAO[i]);
    if (mycount>15) {mycount=0;printf("\n");}
    mycount++;
  }
  printf("\n");
  }
  */

	grib_context_free(c,abbreviations);
	grib_context_free(c,names);
	grib_context_free(c,types);
	grib_context_free(c,units);
	grib_context_free(c,descriptors);
	grib_context_free(c,scale);
	grib_context_free(c,reference);
	grib_context_free(c,width);
  self->do_compute=0;

	return err;
}

static void long_to_string(grib_context* c,long* v,size_t len,char** val) {
      size_t i;
      char tmp[1024]={0,};
      for (i=0;i<len;i++) {
           sprintf(tmp,"%ld",v[i]);
           val[i]=grib_context_strdup(c,tmp);
      }
}

static void double_to_string(grib_context* c,double* v,size_t len,char** val) {
      size_t i;
      char tmp[1024]={0,};
      for (i=0;i<len;i++) {
           sprintf(tmp,"%g",v[i]);
           val[i]=grib_context_strdup(c,tmp);
      }
}

static int unpack_string_array (grib_accessor* a, char** val, size_t *len)
{
  grib_accessor_apply_operators* self = (grib_accessor_apply_operators*)a;
  int ret=0;
  int i=0;
  grib_context* c=a->context; 

  ret=apply_operators(a);
  if (ret) return ret;

  if(*len < self->expandedAOSize)
  {
    grib_context_log(a->context, GRIB_LOG_ERROR,
		    " wrong size (%ld) for %s it contains %d values ",*len, a->name , self->expandedAOSize);
    *len = 0;
    return GRIB_ARRAY_TOO_SMALL;
  }

  *len = self->expandedAOSize;
  switch (self->index) {
    case 0:
            long_to_string(a->context,self->expandedAO,self->expandedAOSize,val);
            break;
    case 1:
            long_to_string(a->context,self->scaleAO,self->expandedAOSize,val);
            break;
    case 2:
            double_to_string(a->context,self->referenceAO,self->expandedAOSize,val);
            break;
    case 3:
            long_to_string(a->context,self->widthAO,self->expandedAOSize,val);
            break;
    case 4:
            long_to_string(a->context,self->bitmapNumber,self->expandedAOSize,val);
            break;
    case 5:
            long_to_string(a->context,self->associatedBitmapNumber,self->expandedAOSize,val);
            break;
    case 6:
            long_to_string(a->context,self->associatedBitmapIndex,self->expandedAOSize,val);
            break;
    case 7:
          for (i=0;i<self->expandedAOSize;i++)
            val[i]=grib_context_strdup(c,self->abbreviations[i]);
          break;
    case 8:
          for (i=0;i<self->expandedAOSize;i++)
            val[i]=grib_context_strdup(c,self->types[i]);
          break;
    case 9:
          for (i=0;i<self->expandedAOSize;i++)
            val[i]=grib_context_strdup(c,self->names[i]);
          break;
    case 10:
          for (i=0;i<self->expandedAOSize;i++)
            val[i]=grib_context_strdup(c,self->units[i]);
          break;
    case 11:
            long_to_string(a->context,self->associatedInfoNumber,self->expandedAOSize,val);
            break;
	default:
          Assert(0);
  }

  return GRIB_SUCCESS;
}

static int    unpack_long   (grib_accessor* a, long* val, size_t *len)
{
  grib_accessor_apply_operators* self = (grib_accessor_apply_operators*)a;
  int ret=0;
  size_t i;

  ret=apply_operators(a);
  if (ret) return ret;

  if(*len < self->expandedAOSize)
  {
    grib_context_log(a->context, GRIB_LOG_ERROR,
		    " wrong size (%ld) for %s it contains %d values ",*len, a->name , self->expandedAOSize);
    *len = 0;
    return GRIB_ARRAY_TOO_SMALL;
  }

  *len = self->expandedAOSize;
  switch (self->index) {
    case 0:
      for (i=0;i<*len;i++) val[i]=self->expandedAO[i];
		  break;
    case 1:
      for (i=0;i<*len;i++) val[i]=self->scaleAO[i];
		  break;
    case 2:
      for (i=0;i<*len;i++) val[i]=self->referenceAO[i];
		  break;
    case 3:
      for (i=0;i<*len;i++) val[i]=self->widthAO[i];
		  break;
    case 4:
      for (i=0;i<*len;i++) val[i]=self->bitmapNumber[i];
		  break;
    case 5:
      for (i=0;i<*len;i++) val[i]=self->associatedBitmapNumber[i];
		  break;
    case 6:
      for (i=0;i<*len;i++) val[i]=self->associatedBitmapIndex[i];
		  break;
    case 11:
      for (i=0;i<*len;i++) val[i]=self->associatedInfoNumber[i];
		  break;
	default:
		Assert(0);
  }

  return GRIB_SUCCESS;
}

static int    unpack_double   (grib_accessor* a, double* val, size_t *len)
{
  grib_accessor_apply_operators* self = (grib_accessor_apply_operators*)a;
  int ret=0;
  size_t i;

  ret=apply_operators(a);
  if (ret) return ret;

  if(*len < self->expandedAOSize)
  {
    grib_context_log(a->context, GRIB_LOG_ERROR,
		    " wrong size (%ld) for %s it contains %d values ",*len, a->name , self->expandedAOSize);
    *len = 0;
    return GRIB_ARRAY_TOO_SMALL;
  }

  *len = self->expandedAOSize;
  switch (self->index) {
	case 0:
                  for (i=0;i<*len;i++) val[i]=self->expandedAO[i];
		  break;
	case 1:
                  for (i=0;i<*len;i++) val[i]=self->scaleAO[i];
		  break;
	case 2:
                  for (i=0;i<*len;i++) val[i]=self->referenceAO[i];
		  break;
	case 3:
                  for (i=0;i<*len;i++) val[i]=self->widthAO[i];
		  break;
	case 4:
                  for (i=0;i<*len;i++) val[i]=self->bitmapNumber[i];
		  break;
	case 5:
                  for (i=0;i<*len;i++) val[i]=self->associatedBitmapNumber[i];
		  break;
	case 6:
                  for (i=0;i<*len;i++) val[i]=self->associatedBitmapIndex[i];
		  break;
	default: 
		Assert(0);
  }

  return GRIB_SUCCESS;
}

static int    pack_long   (grib_accessor* a, const long* val, size_t *len)
{
  grib_accessor_apply_operators* self = (grib_accessor_apply_operators*)a;
  self->do_compute=1;
  return GRIB_NOT_IMPLEMENTED;
}

static int    pack_double   (grib_accessor* a, const double* val, size_t *len)
{
  grib_accessor_apply_operators* self = (grib_accessor_apply_operators*)a;
  self->do_compute=1;
  return GRIB_NOT_IMPLEMENTED;
}

static int value_count(grib_accessor* a,long* rlen)
{
  int ret=0;
  grib_accessor_apply_operators* self = (grib_accessor_apply_operators*)a;
  *rlen=0;

  ret=apply_operators(a);

  if (ret) {
    grib_context_log(a->context,GRIB_LOG_ERROR,
		    "%s unable to compute size",a->name);
		    return ret;
  }

  *rlen=self->expandedAOSize;

  return ret;
}

static void destroy(grib_context* c,grib_accessor* a)
{
  grib_accessor_apply_operators* self = (grib_accessor_apply_operators*)a; 
  self_clear(c,self);
}
