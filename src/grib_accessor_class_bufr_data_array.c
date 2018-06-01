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
   IMPLEMENTS = compare
   IMPLEMENTS = pack_long; unpack_double; pack_double
   MEMBERS    = const char* bufrDataEncodedName
   MEMBERS    = const char* numberOfSubsetsName
   MEMBERS    = const char* expandedDescriptorsName
   MEMBERS    = const char* flagsName
   MEMBERS    = const char* unitsName
   MEMBERS    = const char* elementsDescriptorsIndexName
   MEMBERS    = const char* compressedDataName
   MEMBERS    = bufr_descriptors_array* expanded
   MEMBERS    = grib_accessor* expandedAccessor
   MEMBERS    = int* canBeMissing
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
   MEMBERS    = grib_section* dataKeys
   MEMBERS    = double* inputBitmap
   MEMBERS    = int nInputBitmap
   MEMBERS    = int iInputBitmap
   MEMBERS    = long* inputReplications
   MEMBERS    = int nInputReplications
   MEMBERS    = int iInputReplications
   MEMBERS    = long* inputExtendedReplications
   MEMBERS    = int nInputExtendedReplications
   MEMBERS    = int iInputExtendedReplications
   MEMBERS    = long* inputShortReplications
   MEMBERS    = int nInputShortReplications
   MEMBERS    = int iInputShortReplications
   MEMBERS    = grib_iarray* iss_list
   MEMBERS    = grib_trie_with_rank* dataAccessorsTrie
   MEMBERS    = grib_sarray* tempStrings
   MEMBERS    = int change_ref_value_operand
   MEMBERS    = size_t refValListSize
   MEMBERS    = long* refValList
   MEMBERS    = long refValIndex
   MEMBERS    = bufr_tableb_override* tableb_override

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
static int compare(grib_accessor*, grib_accessor*);

typedef struct grib_accessor_bufr_data_array {
    grib_accessor          att;
/* Members defined in gen */
/* Members defined in bufr_data_array */
	const char* bufrDataEncodedName;
	const char* numberOfSubsetsName;
	const char* expandedDescriptorsName;
	const char* flagsName;
	const char* unitsName;
	const char* elementsDescriptorsIndexName;
	const char* compressedDataName;
	bufr_descriptors_array* expanded;
	grib_accessor* expandedAccessor;
	int* canBeMissing;
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
	double* inputBitmap;
	int nInputBitmap;
	int iInputBitmap;
	long* inputReplications;
	int nInputReplications;
	int iInputReplications;
	long* inputExtendedReplications;
	int nInputExtendedReplications;
	int iInputExtendedReplications;
	long* inputShortReplications;
	int nInputShortReplications;
	int iInputShortReplications;
	grib_iarray* iss_list;
	grib_trie_with_rank* dataAccessorsTrie;
	grib_sarray* tempStrings;
	int change_ref_value_operand;
	size_t refValListSize;
	long* refValList;
	long refValIndex;
	bufr_tableb_override* tableb_override;
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
    0,                /* update_size   */
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
	c->update_size	=	(*(c->super))->update_size;
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

#define MAX_NESTED_REPLICATIONS 8

#define PROCESS_DECODE     0
#define PROCESS_NEW_DATA   1
#define PROCESS_ENCODE     2

#define OVERRIDDEN_REFERENCE_VALUES_KEY "inputOverriddenReferenceValues"

#ifdef ECCODES_ON_WINDOWS
#define round(a) ( (a) >=0 ? ((a)+0.5) : ((a)-0.5) )
#endif

/* Set the error code, if it is bad and we should fail (default case), return */
/* variable 'err' is assumed to be pointer to int */
/* If BUFRDC mode is enabled, then we tolerate problems like wrong data section length */
#define CHECK_END_DATA_RETURN(ctx,b,size,retval) {*err=check_end_data(ctx,b,size); if (*err!=0 && ctx->bufrdc_mode==0) return retval; }

static int process_elements(grib_accessor* a,int flag,long onlySubset,long startSubset,long endSubset);

typedef int (*codec_element_proc) (grib_context* c,grib_accessor_bufr_data_array* self,int subsetIndex, grib_buffer* b,unsigned char* data,long *pos,int i,bufr_descriptor* descriptor,long elementIndex,grib_darray* dval,grib_sarray* sval); 
typedef int (*codec_replication_proc) (grib_context* c,grib_accessor_bufr_data_array* self,int subsetIndex,grib_buffer* buff,unsigned char* data,long *pos,int i,long elementIndex,grib_darray* dval,long* numberOfRepetitions);

static int create_keys(grib_accessor* a,long onlySubset,long startSubset,long endSubset);

static void restart_bitmap(grib_accessor_bufr_data_array *self)
{
    self->bitmapCurrent=-1;
    self->bitmapCurrentElementsDescriptorsIndex=self->bitmapStartElementsDescriptorsIndex-1;
}

static void cancel_bitmap(grib_accessor_bufr_data_array *self)
{
    self->bitmapCurrent=-1;
    self->bitmapStart=-1;
}

static int is_bitmap_start_defined(grib_accessor_bufr_data_array *self)
{
    return self->bitmapStart==-1 ? 0 : 1;
}

int accessor_bufr_data_array_create_keys(grib_accessor* a,long onlySubset,long startSubset,long endSubset)
{
    return create_keys(a,onlySubset,startSubset,endSubset);
}

int accessor_bufr_data_array_process_elements(grib_accessor* a,int flag,long onlySubset,long startSubset,long endSubset)
{
    return process_elements(a,flag,onlySubset,startSubset,endSubset);
}

static size_t get_length(grib_accessor* a)
{
    grib_accessor_bufr_data_array *self =(grib_accessor_bufr_data_array*)a;
    size_t len=0;

    grib_handle* h=grib_handle_of_accessor(a);

    grib_get_size(h,self->bufrDataEncodedName,&len);

    return len;
}

/* Operator 203YYY: Store the TableB code and changed reference value in linked list */
static void tableB_override_store_ref_val(grib_context* c, grib_accessor_bufr_data_array *self, int code, long new_ref_val)
{
    bufr_tableb_override* tb=(bufr_tableb_override*)grib_context_malloc_clear(c, sizeof(bufr_tableb_override));
    tb->code = code;
    tb->new_ref_val = new_ref_val;
    if (!self->tableb_override) {
        self->tableb_override = tb;
    } else {
        /*Add to end of linked list*/
        bufr_tableb_override* q = self->tableb_override;
        while(q->next) q=q->next;
        q->next = tb;
    }
}
/* Operator 203YYY: Retrieve changed reference value from linked list */
static int tableB_override_get_ref_val(grib_accessor_bufr_data_array *self, int code, long* out_ref_val)
{
    bufr_tableb_override* p = self->tableb_override;
    while (p) {
        if (p->code == code) {
            *out_ref_val = p->new_ref_val;
            return GRIB_SUCCESS;
        }
        p = p->next;
    }
    return GRIB_NOT_FOUND;
}
/* Operator 203YYY: Clear and free linked list */
static void tableB_override_clear(grib_context* c, grib_accessor_bufr_data_array *self)
{
    bufr_tableb_override* tb = self->tableb_override;
    while(tb) {
        bufr_tableb_override* n = tb->next;
        grib_context_free(c,tb);
        tb = n;
    }
    self->tableb_override=NULL;
}
/* Operator 203YYY: Copy contents of linked list to the transient array key */
static int tableB_override_set_key(grib_handle* h, grib_accessor_bufr_data_array *self)
{
    int err = GRIB_SUCCESS;
    size_t size = 0;
    long* refVals = NULL;
    grib_iarray* refValArray = grib_iarray_new(h->context, 10, 10);
    bufr_tableb_override* p = self->tableb_override;
    while (p) {
        grib_iarray_push(refValArray, p->new_ref_val);
        p = p->next;
    }
    size = grib_iarray_used_size(refValArray);
    if (size > 0) {
        refVals = grib_iarray_get_array(refValArray);
        err=grib_set_long_array(h, OVERRIDDEN_REFERENCE_VALUES_KEY, refVals, size);
        grib_context_free(h->context, refVals);
    }
    grib_iarray_delete(refValArray);
    return err;
}
/* Check numBits is sufficient for entries in the overridden reference values list*/
static int check_overridden_reference_values(const grib_context *c, long* refValList, size_t refValListSize, int numBits)
{
    const long maxval = (1 << (numBits-1))-1;
    const long minval = -(1 << (numBits-1));
    size_t i=0;
    for(i=0; i<refValListSize; ++i) {
        grib_context_log(c, GRIB_LOG_DEBUG, "check_overridden_reference_values: refValList[%ld]=%ld", i,refValList[i]);
        if (refValList[i] < minval || refValList[i] > maxval) {
            grib_context_log(c, GRIB_LOG_ERROR, "Overridden reference value: entry %d (%ld) does not fit in %d bits (specified by operator 203)",
                    refValList[i], i, numBits);
            return GRIB_OUT_OF_RANGE;
        }
    }
    return GRIB_SUCCESS;
}
/*
static void tableB_override_dump(grib_accessor_bufr_data_array *self)
{
    bufr_tableb_override* p = self->tableb_override;
    int i = 1;
    while (p) {
        printf("ECCODES DEBUG: Table B Override: [%d] code=%d, rv=%ld\n", i, p->code, p->new_ref_val);
        p = p->next;
        ++i;
    }
}
*/

#define DYN_ARRAY_SIZE_INIT 1000 /* Initial size for grib_iarray_new and grib_darray_new */
#define DYN_ARRAY_SIZE_INCR 1000 /* Increment size for grib_iarray_new and grib_darray_new */

static void init(grib_accessor* a,const long v, grib_arguments* params)
{
    grib_accessor_bufr_data_array *self =(grib_accessor_bufr_data_array*)a;
    int n = 0;
    const char* dataKeysName=NULL;
    grib_accessor* dataKeysAcc=NULL;

    self->bufrDataEncodedName = grib_arguments_get_name(grib_handle_of_accessor(a),params,n++);
    self->numberOfSubsetsName = grib_arguments_get_name(grib_handle_of_accessor(a),params,n++);
    self->expandedDescriptorsName = grib_arguments_get_name(grib_handle_of_accessor(a),params,n++);
    self->flagsName = grib_arguments_get_name(grib_handle_of_accessor(a),params,n++);
    self->elementsDescriptorsIndexName = grib_arguments_get_name(grib_handle_of_accessor(a),params,n++);
    self->compressedDataName = grib_arguments_get_name(grib_handle_of_accessor(a),params,n++);
    dataKeysName = grib_arguments_get_name(grib_handle_of_accessor(a),params,n++);

    dataKeysAcc=grib_find_accessor(grib_handle_of_accessor(a),dataKeysName);
    self->dataKeys=dataKeysAcc->parent;
    self->do_decode=1;
    self->elementsDescriptorsIndex=0;
    self->numericValues=0;
    self->stringValues=0;
    cancel_bitmap(self);
    self->expanded=0;
    self->expandedAccessor=0;
    self->dataAccessorsTrie=0;
    self->change_ref_value_operand=0; /* Operator 203YYY: 0, 255 or YYY */
    self->refValListSize = 0;         /* Operator 203YYY: size of overridden reference values array */
    self->refValList=NULL;            /* Operator 203YYY: overridden reference values array */
    self->refValIndex=0;              /* Operator 203YYY: index into overridden reference values array */
    self->tableb_override = NULL;     /* Operator 203YYY: Table B lookup linked list */

    a->length=0;
    self->bitsToEndData=get_length(a)*8;
    self->unpackMode=CODES_BUFR_UNPACK_STRUCTURE;

    /* Assert(a->length>=0); */
}

/*
static void clean_string(char* s,int len)
{
    int i=len-1;
    while (i) {
        if (!isalnum(s[i])) s[i]=0;
        else break;
        i--;
    }
}
 */

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
        grib_vsarray_delete_content(c,self->stringValues);
        grib_vsarray_delete(c,self->stringValues);
    }
    grib_viarray_delete_content(c,self->elementsDescriptorsIndex);
    grib_viarray_delete(c,self->elementsDescriptorsIndex);
    if (self->inputReplications) grib_context_free(c,self->inputReplications);
    if (self->inputExtendedReplications) grib_context_free(c,self->inputExtendedReplications);
    if (self->inputShortReplications) grib_context_free(c,self->inputShortReplications);
    self->change_ref_value_operand = 0;
    self->refValListSize = 0;
    if (self->refValList) grib_context_free(c, self->refValList);
    self->refValIndex=0;
    tableB_override_clear(c, self);
}

static int  get_native_type(grib_accessor* a)
{
    return GRIB_TYPE_DOUBLE;
}

static long byte_count(grib_accessor* a)
{
    return 0;
}

static long byte_offset(grib_accessor* a)
{
    return a->offset;
}

static long next_offset(grib_accessor* a)
{
    return a->offset;
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
    return process_elements(a,PROCESS_ENCODE,0,0,0);
}

grib_vsarray* accessor_bufr_data_array_get_stringValues(grib_accessor* a)
{
    grib_accessor_bufr_data_array *self =(grib_accessor_bufr_data_array*)a;
    process_elements(a,PROCESS_DECODE,0,0,0);

    return self->stringValues;
}

grib_accessors_list* accessor_bufr_data_array_get_dataAccessors(grib_accessor* a)
{
    grib_accessor_bufr_data_array *self =(grib_accessor_bufr_data_array*)a;
    return self->dataAccessors;
}

grib_trie_with_rank* accessor_bufr_data_array_get_dataAccessorsTrie(grib_accessor* a)
{
    grib_accessor_bufr_data_array *self =(grib_accessor_bufr_data_array*)a;
    return self->dataAccessorsTrie;
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
    grib_handle* h=grib_handle_of_accessor(a);
    grib_context* c=a->context;

    if (!self->expandedAccessor)
        self->expandedAccessor=grib_find_accessor(grib_handle_of_accessor(a),self->expandedDescriptorsName);
    self->expanded=grib_accessor_class_expanded_descriptors_get_expanded(self->expandedAccessor,&ret);
    if (ret != GRIB_SUCCESS) return ret;

    numberOfDescriptors=grib_bufr_descriptors_array_used_size(self->expanded);
    self->canBeMissing=(int*)grib_context_malloc_clear(c,numberOfDescriptors*sizeof(int));
    for (i=0;i<numberOfDescriptors;i++)
        self->canBeMissing[i]=grib_bufr_descriptor_can_be_missing(self->expanded->v[i]);

    ret=grib_get_long(h,self->numberOfSubsetsName,&(self->numberOfSubsets));
    if (ret != GRIB_SUCCESS) return ret;
    ret=grib_get_long(h,self->compressedDataName,&(self->compressedData));

    return ret;
}

static int decode_string_array(grib_context* c, unsigned char* data, long* pos, bufr_descriptor* bd,
        grib_accessor_bufr_data_array* self)
{
    int ret=0;
    int* err = &ret;
    char* sval=0;
    int j,modifiedWidth,width;
    grib_sarray* sa=grib_sarray_new(c,self->numberOfSubsets,10);

    modifiedWidth= bd->width;

    sval=(char*)grib_context_malloc_clear(c,modifiedWidth/8+1);
    CHECK_END_DATA_RETURN(c, self, modifiedWidth, *err);
    if (*err) {
      grib_sarray_push(c,sa,sval);
      grib_vsarray_push(c,self->stringValues,sa);
      return ret;
    }
    grib_decode_string(data,pos,modifiedWidth/8,sval);
    CHECK_END_DATA_RETURN(c, self, 6, *err);
    if (*err) {
      grib_sarray_push(c,sa,sval);
      grib_vsarray_push(c,self->stringValues,sa);
      return ret;
    }
    width=grib_decode_unsigned_long(data,pos,6);
    if (width) {
        CHECK_END_DATA_RETURN(c, self, width*8*self->numberOfSubsets, *err);
      if (*err) {
        grib_sarray_push(c,sa,sval);
        grib_vsarray_push(c,self->stringValues,sa);
        return ret;
      }
      grib_context_free(c,sval);
      for (j=0;j<self->numberOfSubsets;j++) {
        sval=(char*)grib_context_malloc_clear(c,width+1);
        grib_decode_string(data,pos,width,sval);
        grib_sarray_push(c,sa,sval);
      }
    } else {
      grib_sarray_push(c,sa,sval);
    }
    grib_vsarray_push(c,self->stringValues,sa);
    return ret;
}

static grib_darray* decode_double_array(grib_context* c,unsigned char* data,long* pos,
        bufr_descriptor* bd,int canBeMissing,
        grib_accessor_bufr_data_array* self,int *err)
{
    grib_darray* ret=NULL;
    int j;
    unsigned long lval;
    int localReference,localWidth,modifiedWidth,modifiedReference;
    double modifiedFactor,dval;

    *err=0;

    modifiedReference= bd->reference;
    modifiedFactor= bd->factor;
    modifiedWidth= bd->width;

    CHECK_END_DATA_RETURN(c, self, modifiedWidth+6, NULL);
    if (*err) {
      dval=GRIB_MISSING_DOUBLE;
      lval=0;
      grib_context_log(c, GRIB_LOG_DEBUG," modifiedWidth=%ld lval=%ld dval=%g", modifiedWidth,lval,dval);
      ret=grib_darray_new(c,DYN_ARRAY_SIZE_INIT,DYN_ARRAY_SIZE_INCR);
      grib_darray_push(c,ret,dval);
      *err=0;
      return ret;
    }
    lval=grib_decode_unsigned_long(data,pos,modifiedWidth);
    localReference=(long)lval+modifiedReference;
    localWidth=grib_decode_unsigned_long(data,pos,6);
    grib_context_log(c, GRIB_LOG_DEBUG,"BUFR data decoding: \tlocalWidth=%ld",localWidth);
    ret=grib_darray_new(c,DYN_ARRAY_SIZE_INIT,DYN_ARRAY_SIZE_INCR);
    if (localWidth) {
        CHECK_END_DATA_RETURN(c, self, localWidth*self->numberOfSubsets, NULL);
        if (*err) {
          dval=GRIB_MISSING_DOUBLE;
          lval=0;
          grib_context_log(c, GRIB_LOG_DEBUG," modifiedWidth=%ld lval=%ld dval=%g", modifiedWidth,lval,dval);
          ret=grib_darray_new(c,DYN_ARRAY_SIZE_INIT,DYN_ARRAY_SIZE_INCR);
          grib_darray_push(c,ret,dval);
          *err=0;
          return ret;
        }
        for (j=0;j<self->numberOfSubsets;j++) {
            lval=grib_decode_unsigned_long(data,pos,localWidth);
            if (grib_is_all_bits_one(lval,localWidth) && canBeMissing) {
                dval=GRIB_MISSING_DOUBLE;
            } else {
                dval=((long)lval+localReference)*modifiedFactor;
            }
            grib_darray_push(c,ret,dval);
        }
    } else {
        if (grib_is_all_bits_one(lval,modifiedWidth) && canBeMissing) {
            dval=GRIB_MISSING_DOUBLE;
        } else {
            dval=localReference*modifiedFactor;
        }
        grib_context_log(c, GRIB_LOG_DEBUG," modifiedWidth=%ld lval=%ld dval=%g", modifiedWidth,lval,dval);
        grib_darray_push(c,ret,dval);
    }

    return ret;
}

static int encode_string_array(grib_context* c,grib_buffer* buff,long* pos, bufr_descriptor* bd,
        grib_accessor_bufr_data_array* self,grib_sarray* stringValues)
{
    int err=0,n,ival;
    int k,j,modifiedWidth,width;

    if (self->iss_list==NULL) {
        grib_context_log(c, GRIB_LOG_ERROR,"encode_string_array: self->iss_list==NULL");
        return GRIB_INTERNAL_ERROR;
    }

    n=grib_iarray_used_size(self->iss_list);

    if (n<=0) return GRIB_NO_VALUES;

    if (grib_sarray_used_size(stringValues)==1) {
      n=1;
      ival=0;
    } else {
      ival=self->iss_list->v[0];
    }

    if (n>grib_sarray_used_size(stringValues))
        return GRIB_ARRAY_TOO_SMALL;

    modifiedWidth= bd->width;

    grib_buffer_set_ulength_bits(c,buff,buff->ulength_bits+modifiedWidth);
    grib_encode_string(buff->data,pos,modifiedWidth/8,stringValues->v[ival]);
    width= n > 1 ? modifiedWidth : 0;

    grib_buffer_set_ulength_bits(c,buff,buff->ulength_bits+6);
    grib_encode_unsigned_longb(buff->data,width/8,pos,6);
    if (width) {
        grib_buffer_set_ulength_bits(c,buff,buff->ulength_bits+width*n);
        for (j=0;j<n;j++) {
            k=self->iss_list->v[j];
            grib_encode_string(buff->data,pos,width/8,stringValues->v[k]);
        }
    }
    return err;
}

static void set_missing_long_to_double(grib_darray* dvalues)
{
    size_t i,n=grib_darray_used_size(dvalues);
    for (i=0;i<n;i++) {
        if (dvalues->v[i]==GRIB_MISSING_LONG) dvalues->v[i]=GRIB_MISSING_DOUBLE;
    }
}

static int encode_double_array(grib_context* c,grib_buffer* buff,long* pos, bufr_descriptor* bd,
        grib_accessor_bufr_data_array* self,grib_darray* dvalues)
{
    int err=0;
    int j,i;
    unsigned long lval;
    long localReference=0,localWidth=0,modifiedWidth,modifiedReference;
    long reference,allone;
    double localRange,modifiedFactor,inverseFactor;
    size_t ii;
    int nvals = 0;
    double min=0,max=0,maxAllowed,minAllowed;
    double* v=NULL;
    double* values=NULL;
    int thereIsAMissing=0;
    int is_constant;
    double val0;
    const int dont_fail_if_out_of_range = c->bufr_set_to_missing_if_out_of_range;/* ECC-379 */

    if (self->iss_list==NULL) {
        grib_context_log(c, GRIB_LOG_ERROR,"encode_double_array: self->iss_list==NULL");
        return GRIB_INTERNAL_ERROR;
    }

    modifiedReference= bd->reference;
    modifiedFactor= bd->factor;
    inverseFactor= grib_power(bd->scale,10);
    modifiedWidth= bd->width;

    maxAllowed=(grib_power(modifiedWidth,2)+modifiedReference)*modifiedFactor;
    minAllowed=modifiedReference*modifiedFactor;

    nvals=grib_iarray_used_size(self->iss_list);
    if (nvals<=0) return GRIB_NO_VALUES;

    set_missing_long_to_double(dvalues);

    v=dvalues->v;

    /* is constant */
    if (grib_darray_is_constant(dvalues,modifiedFactor*.5)) {
        localWidth=0;
        grib_buffer_set_ulength_bits(c,buff,buff->ulength_bits+modifiedWidth);
        if (*v == GRIB_MISSING_DOUBLE) {
            grib_set_bits_on(buff->data,pos,modifiedWidth);
        } else {
            if (*v > maxAllowed || *v < minAllowed) {
                return GRIB_OUT_OF_RANGE; /* ECC-611 */
            }
            lval=round(*v * inverseFactor)-modifiedReference;
            grib_encode_unsigned_longb(buff->data,lval,pos,modifiedWidth);
        }
        grib_buffer_set_ulength_bits(c,buff,buff->ulength_bits+6);
        grib_encode_unsigned_longb(buff->data,localWidth,pos,6);
        return err;
    }

    if (nvals>grib_darray_used_size(dvalues)) return GRIB_ARRAY_TOO_SMALL;
    values=(double*)grib_context_malloc_clear(c,sizeof(double)*nvals);
    val0=dvalues->v[self->iss_list->v[0]];
    is_constant=1;
    for (i=0;i<nvals;i++) {
      values[i]=dvalues->v[self->iss_list->v[i]];
      if (val0 != values[i]) is_constant=0;
    }
    v=values;

    /* encoding a range with constant values*/
    if (is_constant==1) {
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
    /* ECC-379: First determine if we need to change any out-of-range value */
    if (dont_fail_if_out_of_range) {
        while (ii<nvals) {
            /* Turn out-of-range values into 'missing' */
            if (*v!=GRIB_MISSING_DOUBLE && (*v < minAllowed || *v > maxAllowed)) {
                grib_context_log(c, GRIB_LOG_ERROR, "encode_double_array: %s. Value at index %ld (%g) out of range (minAllowed=%g, maxAllowed=%g)."
                                                    "Setting it to missing value\n",
                                                    bd->shortName, (long)ii, *v, minAllowed, maxAllowed);
                *v = GRIB_MISSING_DOUBLE;
            }
            ii++;
            v++;
        }
    }
    /* Determine min and max values. */
    /* Note: value[0] could be missing so cannot set min/max to this.
     * Find first value which is non-missing as a starting point */
    for (i=0;i<nvals;i++) {
        if (values[i] != GRIB_MISSING_DOUBLE) {
            min = max = values[i];
            break;
        }
    }
    ii=0;
    v=values;
    while (ii<nvals) {
        if (*v<min && *v!=GRIB_MISSING_DOUBLE) min=*v;
        if (*v>max && *v!=GRIB_MISSING_DOUBLE) max=*v;
        if (*v == GRIB_MISSING_DOUBLE) thereIsAMissing=1;
        ii++;
        v++;
    }
    if (max>maxAllowed && max!=GRIB_MISSING_DOUBLE)
        return GRIB_OUT_OF_RANGE;
    if (min<minAllowed && min!=GRIB_MISSING_DOUBLE)
        return GRIB_OUT_OF_RANGE;

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
        if (localWidth == 1) localWidth++;
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
        grib_buffer_set_ulength_bits(c,buff,buff->ulength_bits+nvals*localWidth);
        for (j=0;j<nvals;j++) {
            if (values[j]==GRIB_MISSING_DOUBLE) {
                grib_set_bits_on(buff->data,pos,localWidth);
            } else {
                lval=round(values[j]*inverseFactor)-reference;
                grib_encode_unsigned_longb(buff->data,lval,pos,localWidth);
            }
        }
    }

    grib_context_free(c,values);

    return err;
}

static int encode_double_value(grib_context* c,grib_buffer* buff,long* pos,bufr_descriptor* bd,
        grib_accessor_bufr_data_array* self,double value)
{
    unsigned long lval;
    double maxAllowed,minAllowed;
    int err=0;
    int modifiedWidth,modifiedReference;
    double modifiedFactor;
    const int dont_fail_if_out_of_range = c->bufr_set_to_missing_if_out_of_range; /* ECC-379 */

    modifiedReference= bd->reference;
    modifiedFactor= bd->factor;
    modifiedWidth= bd->width;
    maxAllowed=(grib_power(modifiedWidth,2)+modifiedReference)*modifiedFactor;
    minAllowed=modifiedReference*modifiedFactor;

    grib_buffer_set_ulength_bits(c,buff,buff->ulength_bits+modifiedWidth);
    if (value==GRIB_MISSING_DOUBLE) {
        grib_set_bits_on(buff->data,pos,modifiedWidth);
    }
    else if (value>maxAllowed || value<minAllowed) {
        if (dont_fail_if_out_of_range) {
            grib_context_log(c, GRIB_LOG_ERROR, "encode_double_value: %s. Value (%g) out of range (minAllowed=%g, maxAllowed=%g)."
                                                "Setting it to missing value\n",
                                                bd->shortName, value, minAllowed, maxAllowed);
            value = GRIB_MISSING_DOUBLE;  /* Ignore the bad value and instead use 'missing' */
            grib_set_bits_on(buff->data,pos,modifiedWidth);
        } else {
            grib_context_log(c, GRIB_LOG_DEBUG, "encode_double_value: %s. Value (%g) out of range (minAllowed=%g, maxAllowed=%g).",
                             bd->shortName, value, minAllowed, maxAllowed);
            return GRIB_OUT_OF_RANGE;
        }
    }
    else {
        lval=round(value/modifiedFactor)-modifiedReference;
        grib_encode_unsigned_longb(buff->data,lval,pos,modifiedWidth);
    }

    return err;
}

static int encode_string_value(grib_context* c,grib_buffer* buff,long* pos, bufr_descriptor* bd,
        grib_accessor_bufr_data_array* self,char* sval)
{
    int err=0;
    int len;

    len= bd->width / 8;
    grib_buffer_set_ulength_bits(c,buff,buff->ulength_bits+bd->width);

    grib_encode_string(buff->data,pos,len,sval);

    return err;
}

static char* decode_string_value(grib_context* c,unsigned char* data,long* pos, bufr_descriptor* bd,
        grib_accessor_bufr_data_array* self,int *err)
{
    char* sval=0;
    int len;

    *err=0;

    len= bd->width / 8;

    CHECK_END_DATA_RETURN(c, self, bd->width, NULL);
    sval=(char*)grib_context_malloc_clear(c,len+1);
    if (*err) {*err=0; return sval;}
    grib_decode_string(data,pos,len,sval);

    /* clean_string(sval,len); */

    return sval;
}

static double decode_double_value(grib_context* c,unsigned char* data,long* pos,
        bufr_descriptor* bd,int canBeMissing,
        grib_accessor_bufr_data_array* self,int* err)
{
    unsigned long lval;
    int modifiedWidth,modifiedReference;
    double modifiedFactor;
    double dval=0;

    *err=0;

    modifiedReference= bd->reference;
    modifiedFactor= bd->factor;
    modifiedWidth= bd->width;

    CHECK_END_DATA_RETURN(c, self, modifiedWidth, 0);
    if (*err) {*err=0; return GRIB_MISSING_DOUBLE;}

    lval=grib_decode_unsigned_long(data,pos,modifiedWidth);
    if (grib_is_all_bits_one(lval,modifiedWidth) && canBeMissing) {
        dval=GRIB_MISSING_DOUBLE;
    } else {
        dval=((long)lval+modifiedReference)*modifiedFactor;
    }
    return dval;
}

static int decode_element(grib_context* c,grib_accessor_bufr_data_array* self,int subsetIndex,
        grib_buffer* b,unsigned char* data,long *pos,int i,bufr_descriptor* descriptor,long elementIndex,
        grib_darray* dval,grib_sarray* sval)
{
    grib_accessor* a=(grib_accessor*)self;
    grib_darray* dar=0;
    grib_sarray* sar=0;
    int index=0,ii,stringValuesLen;
    char* csval=0;
    double cdval=0,x;
    int err=0;
    bufr_descriptor* bd = descriptor==NULL ? self->expanded->v[i] : descriptor ;
    /* Assert( b->data == data); */

    if (self->change_ref_value_operand > 0 && self->change_ref_value_operand != 255) {
        /* Operator 203YYY: Change Reference Values: Definition phase */
        const int number_of_bits = self->change_ref_value_operand;
        long new_ref_val = grib_decode_signed_longb(data, pos, number_of_bits);
        grib_context_log(c, GRIB_LOG_DEBUG,"BUFR data decoding: -**- \tcode=203YYY width=%ld pos=%ld -> %ld",
                number_of_bits,(long)*pos,(long)(*pos-a->offset*8));
        grib_context_log(c, GRIB_LOG_DEBUG, "Operator 203YYY: Store for code %6.6ld => new ref val %ld", bd->code, new_ref_val);
        tableB_override_store_ref_val(c, self, bd->code, new_ref_val);
        bd->nokey=1;
        err=check_end_data(c, self, number_of_bits); /*advance bitsToEnd*/
        return err;
    }
    grib_context_log(c, GRIB_LOG_DEBUG,"BUFR data decoding: -%ld- \tcode=%6.6ld width=%ld scale=%ld ref=%ld (pos=%ld -> %ld)",
            i, bd->code, bd->width, bd->scale, bd->reference,
            (long)*pos, (long)(*pos-a->offset*8));
    if (bd->type==BUFR_DESCRIPTOR_TYPE_STRING) {
        /* string */
        if (self->compressedData) {
            err = decode_string_array(c,data,pos,bd,self);
            index=grib_vsarray_used_size(self->stringValues);
            dar=grib_darray_new(c,self->numberOfSubsets,10);
            index=self->numberOfSubsets*(index-1);
            for (ii=1;ii<=self->numberOfSubsets;ii++) {
                x=(index+ii)*1000+bd->width/8;
                grib_darray_push(c,dar,x);
            }
            grib_vdarray_push(c,self->numericValues,dar);
        } else {
            csval=decode_string_value(c,data,pos,bd,self,&err);
            grib_context_log(c, GRIB_LOG_DEBUG,"BUFR data decoding: \t %s = %s",
                    bd->shortName,csval);
            sar=grib_sarray_push(c,sar,csval);
            grib_vsarray_push(c,self->stringValues,sar);
            stringValuesLen=grib_vsarray_used_size(self->stringValues);
            index=0;
            for (ii=0;ii<stringValuesLen;ii++) {
                index+=grib_sarray_used_size(self->stringValues->v[ii]);
            }
            cdval=index*1000+bd->width / 8;
            grib_darray_push(c,dval,cdval);
        }
    } else {
        /* numeric or codetable or flagtable */
        /* Operator 203YYY: Check if we have changed ref value for this element. If so modify bd->reference */
        if (self->change_ref_value_operand!=0 && tableB_override_get_ref_val(self, bd->code, &(bd->reference)) == GRIB_SUCCESS) {
            grib_context_log(c, GRIB_LOG_DEBUG,"Operator 203YYY: For code %6.6ld, changed ref val: %ld", bd->code, bd->reference);
        }

        if (self->compressedData) {
            dar=decode_double_array(c,data,pos,bd,self->canBeMissing[i],self,&err);
            grib_vdarray_push(c,self->numericValues,dar);
        } else {
            /* Uncompressed */
            cdval=decode_double_value(c,data,pos,bd,self->canBeMissing[i],self,&err);
            grib_context_log(c, GRIB_LOG_DEBUG,"BUFR data decoding: \t %s = %g",
                    bd->shortName,cdval);
            grib_darray_push(c,dval,cdval);
        }
    }
    return err;
}

static int decode_replication(grib_context* c,grib_accessor_bufr_data_array* self,int subsetIndex,
        grib_buffer* buff,unsigned char* data,long *pos,int i,long elementIndex,grib_darray* dval,long* numberOfRepetitions)
{
    int ret=0;
    int *err;
    int localReference,width;
    bufr_descriptor** descriptors=0;
    err=&ret;
    descriptors=self->expanded->v;

    /* Assert(buff->data == data); */

    grib_context_log(c, GRIB_LOG_DEBUG,"BUFR data decoding: -%ld- \tcode=%6.6ld width=%ld ",
            i,self->expanded->v[i]->code,self->expanded->v[i]->width);
    if (self->compressedData) {
        grib_context_log(c, GRIB_LOG_DEBUG,"BUFR data decoding: \tdelayed replication localReference width=%ld", descriptors[i]->width);
        CHECK_END_DATA_RETURN(c, self, descriptors[i]->width+6, *err);
        if (*err) {
          *numberOfRepetitions=0;
        } else {
          localReference=grib_decode_unsigned_long(data,pos,descriptors[i]->width)+descriptors[i]->reference;
          grib_context_log(c, GRIB_LOG_DEBUG,"BUFR data decoding: \tdelayed replication localWidth width=6");
          width=grib_decode_unsigned_long(data,pos,6);
          if (width) {
              grib_context_log(c, GRIB_LOG_DEBUG,"BUFR data decoding: \tdelayed replication is NOT constant for compressed data!");
            /* delayed replication number is not constant. NOT IMPLEMENTED */
            return GRIB_NOT_IMPLEMENTED;
          } else {
            *numberOfRepetitions=localReference*descriptors[i]->factor;
            grib_context_log(c, GRIB_LOG_DEBUG,"BUFR data decoding: \tdelayed replication value=%ld",*numberOfRepetitions);
          }
        }
    } else {
        CHECK_END_DATA_RETURN(c, self, descriptors[i]->width, *err);
        if (*err) { 
          *numberOfRepetitions=0;
        } else {
          *numberOfRepetitions=grib_decode_unsigned_long(data,pos,descriptors[i]->width)+
            descriptors[i]->reference*descriptors[i]->factor;
          grib_context_log(c, GRIB_LOG_DEBUG,"BUFR data decoding: \tdelayed replication value=%ld",*numberOfRepetitions);
        }
    }
    if (self->compressedData) {
        dval=grib_darray_new(c,1,100);
        grib_darray_push(c,dval,(double)(*numberOfRepetitions));
        grib_vdarray_push(c,self->numericValues,dval);
    } else {
        grib_darray_push(c,dval,(double)(*numberOfRepetitions));
    }
    return ret;
}

static int encode_new_bitmap(grib_context* c,grib_buffer* buff,long *pos,int idx,grib_accessor_bufr_data_array* self)
{
    grib_darray* doubleValues=NULL;
    int err=0;
    double cdval=0;
    if (self->nInputBitmap>0) {
        if (self->nInputBitmap < self->iInputBitmap)
            return GRIB_ARRAY_TOO_SMALL;
        cdval=self->inputBitmap[self->iInputBitmap++];
    }
    if (self->compressedData) {
        doubleValues=grib_darray_new(c,1,1);
        grib_darray_push(c,doubleValues,cdval);
        err=encode_double_array(c,buff,pos,self->expanded->v[idx],self,doubleValues);
        grib_darray_delete(c,doubleValues);
    } else {
        err=encode_double_value(c,buff,pos,self->expanded->v[idx],self,cdval);
    }
    return err;
}

/* Operator 203YYY: Change Reference Values: Encoding definition phase */
static int encode_overridden_reference_value(grib_context* c, grib_accessor_bufr_data_array* self,
        grib_buffer* buff, long *pos, bufr_descriptor* bd)
{
    int err = 0;
    long currRefVal = -1;
    long numBits = self->change_ref_value_operand;
    /* We must be encoding between 203YYY and 203255 */
    Assert(self->change_ref_value_operand > 0 && self->change_ref_value_operand != 255);
    if ( self->refValListSize == 0 ) {
        grib_context_log(c, GRIB_LOG_ERROR,"encode_new_element: Overridden Reference Values array is empty!");
        return GRIB_ENCODING_ERROR;
    }
    if ( self->refValIndex >= self->refValListSize ) {
        grib_context_log(c, GRIB_LOG_ERROR,"encode_new_element: Overridden Reference Values: index=%ld, size=%ld. "
                "\nThe number of overridden reference values must be equal to "
                "number of descriptors between operator 203YYY and 203255",
                self->refValIndex, self->refValListSize);
        return GRIB_ENCODING_ERROR;
    }
    currRefVal = self->refValList[self->refValIndex];
    grib_context_log(c,GRIB_LOG_DEBUG, "encode_new_element: Operator 203YYY: writing ref val %ld (self->refValIndex=%ld)",
            currRefVal, self->refValIndex);
    grib_buffer_set_ulength_bits(c,buff,buff->ulength_bits+numBits);
    err = grib_encode_signed_longb(buff->data, currRefVal, pos, numBits);
    if (err) {
        grib_context_log(c,GRIB_LOG_ERROR,"encoding overridden reference value %ld for %s (code=%6.6ld)",
                currRefVal, bd->shortName, bd->code);
    }
    self->refValIndex++;
    return err;
}

static int encode_new_element(grib_context* c,grib_accessor_bufr_data_array* self,int subsetIndex,
        grib_buffer* buff,unsigned char* data,long *pos,int i,bufr_descriptor* descriptor,
        long elementIndex,grib_darray* dval,grib_sarray* sval)
{
    int ii;
    char* csval=0;
    unsigned char missingChar=0xFF;
    double cdval=GRIB_MISSING_DOUBLE;
    int err=0;
    size_t slen;
    bufr_descriptor* bd = descriptor==NULL ? self->expanded->v[i] : descriptor ;

    grib_context_log(c, GRIB_LOG_DEBUG,"BUFR data encoding: \tcode=%6.6ld width=%ld pos=%ld ulength=%ld ulength_bits=%ld",
            bd->code,bd->width,(long)*pos,buff->ulength,buff->ulength_bits);

    if (self->change_ref_value_operand > 0 && self->change_ref_value_operand != 255) {
        /* Operator 203YYY: Change Reference Values: Encoding definition phase */
        err = encode_overridden_reference_value(c, self, buff, pos, bd);
        return err;
    }

    if (bd->type==BUFR_DESCRIPTOR_TYPE_STRING) {
        /* string */
        slen=bd->width/8;
        csval=(char*)grib_context_malloc_clear(c,slen+1);
        for (ii=0;ii<slen;ii++) csval[ii]=missingChar;
        grib_context_log(c, GRIB_LOG_DEBUG,"BUFR data encoding: \t %s = %s",
                bd->shortName,csval);
        if (self->compressedData) {
            grib_sarray* stringValues=grib_sarray_new(c,1,1);
            grib_sarray_push(c,stringValues,csval);
            err=encode_string_array(c,buff,pos,bd,self,stringValues);
            grib_sarray_delete_content(c,stringValues);
            grib_sarray_delete(c,stringValues);
        } else {
            err=encode_string_value(c,buff,pos,bd,self,csval);
        }
    } else {
        /* numeric or codetable or flagtable */
        grib_context_log(c, GRIB_LOG_DEBUG,"BUFR data encoding: \t %s = %g",
                bd->shortName,cdval);
        if (bd->code==31031)
            return encode_new_bitmap(c,buff,pos,i,self);
        if (self->compressedData) {
            grib_darray* doubleValues=grib_darray_new(c,1,1);
            grib_darray_push(c,doubleValues,cdval);
            err=encode_double_array(c,buff,pos,bd,self,doubleValues);
            grib_darray_delete(c,doubleValues);
        } else {
            err=encode_double_value(c,buff,pos,bd,self,cdval);
        }
    }
    return err;
}

static int encode_new_replication(grib_context* c,grib_accessor_bufr_data_array* self,int subsetIndex,
        grib_buffer* buff,unsigned char* data,long *pos,int i,long elementIndex,grib_darray* dval,long* numberOfRepetitions)
{
    int err=0;
    unsigned long repetitions=1;
    bufr_descriptor** descriptors=self->expanded->v;
    /* Assert( buff->data==data); */

    switch(descriptors[i]->code) {
    case 31000:
        if (self->nInputShortReplications>=0) {
            if (self->iInputShortReplications>=self->nInputShortReplications) {
                grib_context_log(c,GRIB_LOG_FATAL,"array inputShortDelayedDescriptorReplicationFactor dimension too small");
            }
            repetitions=self->inputShortReplications[self->iInputShortReplications];
            self->iInputShortReplications++;
        }
        break;
    case 31001:
        if (self->nInputReplications>=0) {
            if (self->iInputReplications>=self->nInputReplications) {
                grib_context_log(c,GRIB_LOG_FATAL,"array inputDelayedDescriptorReplicationFactor dimension too small");
            }
            repetitions=self->inputReplications[self->iInputReplications];
            self->iInputReplications++;
        }
        break;
    case 31002:
        if (self->nInputExtendedReplications>=0) {
            if (self->iInputExtendedReplications>=self->nInputExtendedReplications) {
                grib_context_log(c,GRIB_LOG_FATAL,"array inputExtendedDelayedDescriptorReplicationFactor dimension too small");
            }
            repetitions=self->inputExtendedReplications[self->iInputExtendedReplications];
            self->iInputExtendedReplications++;
        }
        break;
    default:
        grib_context_log(c, GRIB_LOG_ERROR, "unsupported descriptor code %d\n", descriptors[i]->code);
        return GRIB_INTERNAL_ERROR;
    }

    grib_context_log(c, GRIB_LOG_DEBUG,"BUFR data encoding replication: \twidth=%ld pos=%ld ulength=%ld ulength_bits=%ld",
            (long)descriptors[i]->width,(long)*pos,(long)buff->ulength,(long)buff->ulength_bits);
    grib_buffer_set_ulength_bits(c,buff,buff->ulength_bits+descriptors[i]->width);
    grib_encode_unsigned_longb(data,repetitions,pos,descriptors[i]->width);

    *numberOfRepetitions=repetitions;

    if (self->compressedData) {
        grib_buffer_set_ulength_bits(c,buff,buff->ulength_bits+6);
        grib_encode_unsigned_longb(buff->data,0,pos,6);
    };

    return err;
}

static int encode_element(grib_context* c,grib_accessor_bufr_data_array* self,int subsetIndex,
        grib_buffer* buff,unsigned char* data,long *pos,int i,bufr_descriptor* descriptor,
        long elementIndex,grib_darray* dval,grib_sarray* sval)
{
    int idx,j;
    int err=0;
    bufr_descriptor* bd = descriptor==NULL ? self->expanded->v[i] : descriptor ;
    /* Assert( buff->data == data); */

    grib_context_log(c, GRIB_LOG_DEBUG,"BUFR data encoding: -%ld- \tcode=%6.6ld width=%ld pos=%ld ulength=%ld ulength_bits=%ld",
            i,bd->code,bd->width,(long)*pos,buff->ulength,buff->ulength_bits);

    if (self->change_ref_value_operand > 0 && self->change_ref_value_operand != 255) {
        /* Operator 203YYY: Change Reference Values: Encoding definition phase */
        err = encode_overridden_reference_value(c, self, buff, pos, bd);
        return err;
    }

    if (bd->type==BUFR_DESCRIPTOR_TYPE_STRING) {
        /* string */
        /* grib_context_log(c, GRIB_LOG_DEBUG,"BUFR data encoding: \t %s = %s",
                 bd->shortName,csval); */
        if (self->compressedData) {
            idx=((int)self->numericValues->v[elementIndex]->v[0]/1000-1)/self->numberOfSubsets;
            err=encode_string_array(c,buff,pos,bd,self,self->stringValues->v[idx]);
        } else {
            if (self->numericValues->v[subsetIndex] == NULL) {
                grib_context_log(c,GRIB_LOG_ERROR,"Invalid subset index %d (number of subsets=%ld)", subsetIndex, self->numberOfSubsets);
                return GRIB_INVALID_ARGUMENT;
            }
            idx=(int)self->numericValues->v[subsetIndex]->v[elementIndex]/1000-1;
            if (idx < 0 || idx >= self->stringValues->n) {
                grib_context_log(c,GRIB_LOG_ERROR,"encode_element: %s: Invalid index %d", bd->shortName, idx);
                return GRIB_INVALID_ARGUMENT;
            }
            err=encode_string_value(c,buff,pos,bd,self,self->stringValues->v[idx]->v[0]);
        }
    } else {
        /* numeric or codetable or flagtable */
        if (self->compressedData) {
            err=encode_double_array(c,buff,pos,bd,self,self->numericValues->v[elementIndex]);
            if (err) {
                grib_context_log(c,GRIB_LOG_ERROR,"encoding %s ( code=%6.6ld width=%ld scale=%ld reference=%ld )",
                        bd->shortName, bd->code, bd->width,
                        bd->scale, bd->reference);
                for (j=0;j<grib_darray_used_size(self->numericValues->v[elementIndex]);j++)
                    grib_context_log(c,GRIB_LOG_ERROR,"value[%d]\t= %g", j,self->numericValues->v[elementIndex]->v[j]);
            }
        } else {
            if (self->numericValues->v[subsetIndex] == NULL) {
                grib_context_log(c,GRIB_LOG_ERROR,"Invalid subset index %d (number of subsets=%ld)", subsetIndex, self->numberOfSubsets);
                return GRIB_INVALID_ARGUMENT;
            }
            err=encode_double_value(c,buff,pos,bd,self,self->numericValues->v[subsetIndex]->v[elementIndex]);
            if (err) {
                grib_context_log(c,GRIB_LOG_ERROR,"Cannot encode %s=%g (subset=%d)", /*subsetIndex starts from 0*/
                                 bd->shortName, self->numericValues->v[subsetIndex]->v[elementIndex], subsetIndex+1);
            }
        }
    }
    return err;
}

static int encode_replication(grib_context* c,grib_accessor_bufr_data_array* self,int subsetIndex,
        grib_buffer* buff,unsigned char* data,long *pos,int i,long elementIndex,grib_darray* dval,long* numberOfRepetitions)
{
    /* Assert( buff->data == data); */
    if (self->compressedData) {
        DebugAssert(grib_darray_used_size(self->numericValues->v[elementIndex])==1);
        *numberOfRepetitions=self->numericValues->v[elementIndex]->v[0];
    } else {
        *numberOfRepetitions=self->numericValues->v[subsetIndex]->v[elementIndex];
    }

    return encode_element(c,self,subsetIndex,buff,data,pos,i,0,elementIndex,dval,0);
}

static int build_bitmap(grib_accessor_bufr_data_array *self,unsigned char* data,long* pos,int iel,grib_iarray* elementsDescriptorsIndex,int iBitmapOperator)
{
    int bitmapSize=0,iDelayedReplication=0;
    int i,localReference,width,bitmapEndElementsDescriptorsIndex;
    long ppos,n;
    grib_accessor* a=(grib_accessor*)self;
    grib_context* c=a->context;
    bufr_descriptor** descriptors=self->expanded->v;
    long* edi=elementsDescriptorsIndex->v;
    /* int iel=grib_iarray_used_size(elementsDescriptorsIndex)-1; */
    int err=0;

    switch (descriptors[iBitmapOperator]->code) {
    case 222000:
    case 223000:
    case 236000:
        cancel_bitmap(self);
        if (iel < 0) { return GRIB_ENCODING_ERROR; }
        while (descriptors[edi[iel]]->code>=100000 || iel==0) {
            iel--;
            if (iel < 0) { return GRIB_ENCODING_ERROR; }
        }
        bitmapEndElementsDescriptorsIndex=iel;
        /*looking for another bitmap and pointing before it.
          This behaviour is not documented in the Manual on codes it is copied from BUFRDC
          ECC-243
         */
        while (iel>0) {
            while ( descriptors[edi[iel]]->code!=236000 && descriptors[edi[iel]]->code!=222000 && descriptors[edi[iel]]->code!=223000 && iel!=0) iel--;
            if (iel!=0) {
                while (descriptors[edi[iel]]->code>=100000 && iel!=0) iel--;
                bitmapEndElementsDescriptorsIndex=iel;
            }
        }

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
                    grib_context_log(c,GRIB_LOG_ERROR, "Delayed replication number is not constant");
                    return GRIB_NOT_IMPLEMENTED;
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
        grib_context_log(c,GRIB_LOG_ERROR,"build_bitmap: unsupported operator %d\n",
                descriptors[iBitmapOperator]->code);
        return GRIB_INTERNAL_ERROR;
    }
    return GRIB_SUCCESS;
}

static int consume_bitmap(grib_accessor_bufr_data_array *self,int iBitmapOperator)
{
    int bitmapSize=0,iDelayedReplication;
    int i;
    bufr_descriptor** descriptors=self->expanded->v;

    i=iBitmapOperator+1;
    if (descriptors[i]->code==101000)  {
        iDelayedReplication=iBitmapOperator+2;
        switch (descriptors[iDelayedReplication]->code) {
        case 31001:
            bitmapSize=self->inputReplications[self->iInputReplications];
            break;
        case 31002:
            bitmapSize=self->inputExtendedReplications[self->iInputExtendedReplications];
            break;
        default :
            Assert(0);
        }
    } else if (descriptors[i]->code==31031){
        bitmapSize=0;
        while (descriptors[i]->code==31031) {bitmapSize++;i++;}
    }
    self->bitmapCurrent+=bitmapSize;
    return GRIB_SUCCESS;
}

static int build_bitmap_new_data(grib_accessor_bufr_data_array *self,unsigned char* data,long* pos,int iel,grib_iarray* elementsDescriptorsIndex,int iBitmapOperator)
{
    int bitmapSize=0,iDelayedReplication=0;
    int i,bitmapEndElementsDescriptorsIndex;
    long n;
    grib_accessor* a=(grib_accessor*)self;
    grib_context* c=a->context;
    bufr_descriptor** descriptors=self->expanded->v;
    long* edi=elementsDescriptorsIndex->v;

    switch (descriptors[iBitmapOperator]->code) {
    case 222000:
    case 223000:
    case 236000:
        if (iel < 0) { return GRIB_ENCODING_ERROR; }
        while (descriptors[edi[iel]]->code>=100000) {
            iel--;
            if (iel < 0) { return GRIB_ENCODING_ERROR; }
        }
        bitmapEndElementsDescriptorsIndex=iel;
        /*looking for another bitmap and pointing before it.
          This behaviour is not documented in the Manual on codes it is copied from BUFRDC
          ECC-243
         */
        while (iel>0) {
            while ( descriptors[edi[iel]]->code!=236000 && descriptors[edi[iel]]->code!=222000 && descriptors[edi[iel]]->code!=223000 && iel!=0) iel--;
            if (iel!=0) {
                while (descriptors[edi[iel]]->code>=100000 && iel!=0) iel--;
                bitmapEndElementsDescriptorsIndex=iel;
            }
        }

        i=iBitmapOperator+1;
        if (descriptors[i]->code==101000)  {
            iDelayedReplication=iBitmapOperator+2;
            switch (descriptors[iDelayedReplication]->code) {
            case 31001:
                bitmapSize=self->inputReplications[self->iInputReplications];
                break;
            case 31002:
                bitmapSize=self->inputExtendedReplications[self->iInputExtendedReplications];
                break;
            default :
                Assert(0);
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
        self->bitmapCurrentElementsDescriptorsIndex=iel-1;
        break;
    default :
        grib_context_log(c,GRIB_LOG_ERROR,"build_bitmap_new_data: unsupported operator %d\n",
                descriptors[iBitmapOperator]->code);
        return GRIB_INTERNAL_ERROR;
    }
    return GRIB_SUCCESS;
}

static int get_next_bitmap_descriptor_index_new_bitmap(grib_accessor_bufr_data_array *self,grib_iarray* elementsDescriptorsIndex,int compressedData)
{
    int i;
    bufr_descriptor** descriptors=self->expanded->v;

    self->bitmapCurrent++;
    self->bitmapCurrentElementsDescriptorsIndex++;
    i=self->bitmapCurrent;

    if (self->compressedData) {
        DebugAssert(i<self->nInputBitmap);
        if (i>=self->nInputBitmap) return GRIB_WRONG_BITMAP_SIZE;
        while (self->inputBitmap[i]==1) {
            self->bitmapCurrent++;
            self->bitmapCurrentElementsDescriptorsIndex++;
            while (descriptors[elementsDescriptorsIndex->v[self->bitmapCurrentElementsDescriptorsIndex]]->code
                    >100000)
                self->bitmapCurrentElementsDescriptorsIndex++;
            i++;
        }
    } else {
        if (i>=self->nInputBitmap) return GRIB_WRONG_BITMAP_SIZE;
        while (self->inputBitmap[i]==1) {
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

static int get_next_bitmap_descriptor_index(grib_accessor_bufr_data_array *self,grib_iarray* elementsDescriptorsIndex,grib_darray* numericValues)
{
    int i;
    bufr_descriptor** descriptors=self->expanded->v;

    if (self->compressedData) {
        if (self->numericValues->n==0)
            return get_next_bitmap_descriptor_index_new_bitmap(self,elementsDescriptorsIndex,1);

        self->bitmapCurrent++;
        self->bitmapCurrentElementsDescriptorsIndex++;
        i=self->bitmapCurrent+self->bitmapStart;
        DebugAssert(i<self->numericValues->n);
        while (self->numericValues->v[i]->v[0]==1) {
            self->bitmapCurrent++;
            self->bitmapCurrentElementsDescriptorsIndex++;
            while (descriptors[elementsDescriptorsIndex->v[self->bitmapCurrentElementsDescriptorsIndex]]->code
                    >100000)
                self->bitmapCurrentElementsDescriptorsIndex++;
            i++;
        }
    } else {
        if (numericValues->n==0)
            return get_next_bitmap_descriptor_index_new_bitmap(self,elementsDescriptorsIndex,0);

        self->bitmapCurrent++;
        self->bitmapCurrentElementsDescriptorsIndex++;
        i=self->bitmapCurrent+self->bitmapStart;
        DebugAssert(i<numericValues->n);
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
    grib_context* c=a->context;
    if (self->compressedData) {
        d=grib_darray_new(c,1,100);
        grib_darray_push(c,d,0);
        grib_vdarray_push(c,self->numericValues,d);
    } else {
        grib_darray_push(c,dval,0);
    }
}

static grib_accessor* create_attribute_variable(char* name,grib_section* section,int type,char* sval,double dval,long lval,unsigned long flags)
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
    a->parent=NULL;
    a->h=section->h;
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
        if (!sval) 
            return NULL;
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
        creator->name = (char*)"qualityInformationFollows";
        break;
    case 223000:
        creator->name = (char*)"substitutedValuesOperator";
        break;
    case 223255:
        creator->name = (char*)"substitutedValue";
        break;
    case 224000:
        creator->name = (char*)"firstOrderStatiticalValuesFollow";
        break;
    case 224255:
        creator->name = (char*)"firstOrderStatisticalValue";
        break;
    case 225000:
        creator->name = (char*)"differenceStatisticalValuesFollow";
        break;
    case 225255:
        creator->name = (char*)"differenceStatisticalValue";
        break;
    case 232000:
        creator->name = (char*)"replacedRetainedValuesFollow";
        break;
    case 232255:
        creator->name = (char*)"replacedRetainedValue";
        break;
    case 235000:
        creator->name = (char*)"cancelBackwardDataReference";
        break;
    case 236000:
        creator->name = (char*)"defineDataPresentBitmap";
        break;
    case 237000:
        creator->name = (char*)"useDefinedDataPresentBitmap";
        break;
    case 237255:
        creator->name = (char*)"cancelUseDefinedDataPresentBitmap";
        break;
    case 241000:
        creator->name = (char*)"defineEvent";
        break;
    case 241255:
        creator->name = (char*)"cancelDefineEvent";
        break;
    case 242000:
        creator->name = (char*)"defineConditioningEvent";
        break;
    case 242255:
        creator->name = (char*)"canceDefineConditioningEvent";
        break;
    case 243000:
        creator->name = (char*)"categoricalForecastValuesFollow";
        break;
    case 243255:
        creator->name = (char*)"cancelCategoricalForecastValuesFollow";
        break;
    case 999999:
        creator->name = (char*)"associatedField";
        break;
    default :
        if (code > 204999 && code < 206000) creator->name =  (char*)"text";
        else creator->name = (char*)"operator";
        break;
    }
}

static grib_accessor* create_accessor_from_descriptor(grib_accessor* a,grib_accessor* attribute,grib_section* section,long ide,long subset,int dump,int count)
{
    grib_accessor_bufr_data_array *self =(grib_accessor_bufr_data_array*)a;
    char code[10]={0,};
    char* temp_str = NULL;
    int idx=0;
    unsigned long flags=GRIB_ACCESSOR_FLAG_READ_ONLY;
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

    if(attribute) { DebugAssert(attribute->parent==NULL); }

    if (dump) {
        creator.flags    = GRIB_ACCESSOR_FLAG_DUMP;
        operatorCreator.flags  |= GRIB_ACCESSOR_FLAG_DUMP;
    }

    idx = self->compressedData ? self->elementsDescriptorsIndex->v[0]->v[ide] :
            self->elementsDescriptorsIndex->v[subset]->v[ide] ;

    switch (self->expanded->v[idx]->F) {
    case 0:
    case 1:
        creator.name=grib_context_strdup(a->context, self->expanded->v[idx]->shortName);
        /* ECC-325: store alloc'd string (due to strdup) for clean up later */
        grib_sarray_push(a->context, self->tempStrings, creator.name);
        elementAccessor = grib_accessor_factory(section, &creator, 0, NULL);
        if (self->canBeMissing[idx]) elementAccessor->flags |= GRIB_ACCESSOR_FLAG_CAN_BE_MISSING;
        if (self->expanded->v[idx]->code == 31000 || self->expanded->v[idx]->code == 31001 || self->expanded->v[idx]->code == 31002 || self->expanded->v[idx]->code == 31031)
            elementAccessor->flags |= GRIB_ACCESSOR_FLAG_READ_ONLY;
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
            /* attribute->parent=elementAccessor->parent; */
            /*
          for (i=0;i<MAX_ACCESSOR_ATTRIBUTES;i++) {
            if (attribute->attributes[i]) attribute->attributes[i]->parent=elementAccessor->parent;
          }
             */
            grib_accessor_add_attribute(elementAccessor,attribute,0);
        }

        attribute=create_attribute_variable("index",section,GRIB_TYPE_LONG,0,0,count,flags);
        if (!attribute) return NULL;
        grib_accessor_add_attribute(elementAccessor,attribute,0);

        sprintf(code,"%06ld",self->expanded->v[idx]->code);
        temp_str = grib_context_strdup(a->context,code);
        attribute=create_attribute_variable("code",section,GRIB_TYPE_STRING,temp_str,0,0,flags);
        if (!attribute) return NULL;
        grib_sarray_push(a->context, self->tempStrings, temp_str);/* ECC-325: store alloc'd string (due to strdup) for clean up later */
        grib_accessor_add_attribute(elementAccessor,attribute,0);

        attribute=create_attribute_variable("units",section,GRIB_TYPE_STRING,self->expanded->v[idx]->units,0,0,GRIB_ACCESSOR_FLAG_DUMP | flags);
        if (!attribute) return NULL;
        grib_accessor_add_attribute(elementAccessor,attribute,0);

        attribute=create_attribute_variable("scale",section,GRIB_TYPE_LONG,0,0,self->expanded->v[idx]->scale,flags);
        if (!attribute) return NULL;
        grib_accessor_add_attribute(elementAccessor,attribute,0);

        attribute=create_attribute_variable("reference",section,GRIB_TYPE_DOUBLE,0,self->expanded->v[idx]->reference,0,flags);
        if (!attribute) return NULL;
        grib_accessor_add_attribute(elementAccessor,attribute,0);

        attribute=create_attribute_variable("width",section,GRIB_TYPE_LONG,0,0,self->expanded->v[idx]->width,flags);
        if (!attribute) return NULL;
        grib_accessor_add_attribute(elementAccessor,attribute,0);
        break;
    case 2:
        set_creator_name(&creator,self->expanded->v[idx]->code);
        if (bufr_descriptor_is_marker(self->expanded->v[idx]) ) {
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

            attribute=create_attribute_variable("index",section,GRIB_TYPE_LONG,0,0,count,flags);
            if (!attribute) return NULL;
            grib_accessor_add_attribute(elementAccessor,attribute,0);

        } else {
            elementAccessor = grib_accessor_factory(section, &operatorCreator, 0, NULL);
            accessor_variable_set_type(elementAccessor,GRIB_TYPE_LONG);

            attribute=create_attribute_variable("index",section,GRIB_TYPE_LONG,0,0,count,flags);
            if (!attribute) return NULL;
            grib_accessor_add_attribute(elementAccessor,attribute,0);

            sprintf(code,"%06ld",self->expanded->v[idx]->code);
            attribute=create_attribute_variable("code",section,GRIB_TYPE_STRING,code,0,0,flags);
            if (!attribute) return NULL;
            grib_accessor_add_attribute(elementAccessor,attribute,0);
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

        attribute=create_attribute_variable("index",section,GRIB_TYPE_LONG,0,0,count,flags);
        if (!attribute) return NULL;
        grib_accessor_add_attribute(elementAccessor,attribute,0);

        sprintf(code,"%06ld",self->expanded->v[idx]->code);
        attribute=create_attribute_variable("code",section,GRIB_TYPE_STRING,code,0,0,flags);
        if (!attribute) return NULL;
        grib_accessor_add_attribute(elementAccessor,attribute,0);

        attribute=create_attribute_variable("units",section,GRIB_TYPE_STRING,self->expanded->v[idx]->units,0,0,GRIB_ACCESSOR_FLAG_DUMP);
        if (!attribute) return NULL;
        grib_accessor_add_attribute(elementAccessor,attribute,0);

        attribute=create_attribute_variable("scale",section,GRIB_TYPE_LONG,0,0,self->expanded->v[idx]->scale,flags);
        if (!attribute) return NULL;
        grib_accessor_add_attribute(elementAccessor,attribute,0);

        attribute=create_attribute_variable("reference",section,GRIB_TYPE_DOUBLE,0,self->expanded->v[idx]->reference,0,flags);
        if (!attribute) return NULL;
        grib_accessor_add_attribute(elementAccessor,attribute,0);

        attribute=create_attribute_variable("width",section,GRIB_TYPE_LONG,0,0,self->expanded->v[idx]->width,flags);
        if (!attribute) return NULL;
        grib_accessor_add_attribute(elementAccessor,attribute,0);
        break;
    }

    return elementAccessor;
}

#define IS_QUALIFIER(a) (a==8 || a==1 || a==2 || a==4 || a==5 || a==6 || a==7)
#define NUMBER_OF_QUALIFIERS_PER_CATEGORY 256
#define NUMBER_OF_QUALIFIERS_CATEGORIES 7
#define MAX_NUMBER_OF_BITMAPS 5

static const int number_of_qualifiers=NUMBER_OF_QUALIFIERS_PER_CATEGORY*NUMBER_OF_QUALIFIERS_CATEGORIES;

static GRIB_INLINE int significanceQualifierIndex(int X,int Y)
{
    int a[]={-1,0,1,-1,2,3,4,5,6};
    int ret=Y+a[X]*NUMBER_OF_QUALIFIERS_PER_CATEGORY;
    DebugAssert(ret>0);
    return ret;
}

static GRIB_INLINE void reset_deeper_qualifiers(
    grib_accessor* significanceQualifierGroup[],
    const int* const significanceQualifierDepth,
    int numElements, int depth)
{
    int i;
    for (i=0;i<numElements;i++) {
        if (significanceQualifierDepth[i]>depth) {
            significanceQualifierGroup[i]=0;
        }
    }
}

typedef struct bitmap_s {
    grib_accessors_list* cursor;
    grib_accessors_list* referredElement;
    grib_accessors_list* referredElementStart;
} bitmap_s;

static grib_accessor* get_element_from_bitmap(grib_accessor* a,bitmap_s* bitmap)
{
    int ret;
    long bitmapVal=1;
    size_t len;

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

/*
static GRIB_INLINE void reset_qualifiers(grib_accessor* significanceQualifierGroup[])
{
    int i;
    for (i=0;i<number_of_qualifiers;i++)
        significanceQualifierGroup[i]=0;
}
*/

static void grib_convert_to_attribute(grib_accessor* a)
{
    if (a->h==NULL && a->parent!=NULL) {
        a->h=grib_handle_of_accessor(a);
        a->parent=NULL;
    }
}

static grib_iarray* set_subset_list(grib_context* c,grib_accessor_bufr_data_array *self,long onlySubset,long startSubset,long endSubset,long* subsetList,size_t subsetListSize)
{
    grib_iarray* list=grib_iarray_new(c,self->numberOfSubsets,10);
    long s;

    if (startSubset>0) {
        s=startSubset;
        while (s<=endSubset) {
            grib_iarray_push(list,s-1);
            s++;
        }
    }

    if (onlySubset>0)
        grib_iarray_push(list,onlySubset-1);

    if (subsetList && subsetList[0]>0) {
        for (s=0;s<subsetListSize;s++)
            grib_iarray_push(list,subsetList[s]-1);
    }

    if (grib_iarray_used_size(list)==0) {
        for (s=0;s<self->numberOfSubsets;s++)
            grib_iarray_push(list,s);
    }

    return list;
}

static int bitmap_ref_skip(grib_accessors_list* al,int* err)
{
    grib_accessor* acode=NULL;
    long code[1];
    size_t l=1;

    if (!al || !al->accessor) return 0;

    acode=grib_accessor_get_attribute(al->accessor,"code");

    if (acode) *err=grib_unpack_long(acode,code,&l);
    else return 1;

    switch (code[0]) {
    case 222000:
    case 223000:
    case 224000:
    case 225000:
    case 232000:
    case 236000:
    case 237000:
    case 243000:
    case 31000:
    case 31001:
    case 31002:
        return 1;
    }
    return 0;
}

static int bitmap_init(grib_context* c, bitmap_s* bitmap,
                       grib_accessors_list* bitmapStart, int bitmapSize, grib_accessors_list* lastAccessorInList)
{
    int ret=0,i;
    bitmap->cursor=bitmapStart->next;
    if (bitmap->referredElementStart!=NULL) {
        bitmap->referredElement=bitmap->referredElementStart;
        return ret;
    }
    bitmap->referredElement=bitmapStart;
    while (bitmap_ref_skip(bitmap->referredElement,&ret)) bitmap->referredElement=bitmap->referredElement->prev;
    for (i=1;i<bitmapSize;i++) {
        if (bitmap->referredElement==NULL) {
            grib_context_log(c, GRIB_LOG_ERROR,"bitmap_init: bitmap->referredElement==NULL");
            return GRIB_INTERNAL_ERROR;
        }
        bitmap->referredElement=bitmap->referredElement->prev;
    }
    bitmap->referredElementStart=bitmap->referredElement;
    return ret;
}

static grib_accessor* accessor_or_attribute_with_same_name(grib_accessor* a,const char* name)
{
    if (grib_accessor_has_attributes(a)==0) {
        return a;
    } else {
        grib_accessor* ok=a;
        grib_accessor* next;
        while ((next=grib_accessor_get_attribute(ok,name))!=NULL) {
            ok=next;
        }
        return ok;
    }
}
#if 0
static int get_key_rank(grib_trie* accessorsRank,grib_accessor* a)
{
    int* r=(int*)grib_trie_get(accessorsRank,a->name);

    if (r) (*r)++;
    else {
        r=(int*)grib_context_malloc(a->context,sizeof(int));
        *r=1;
        grib_trie_insert(accessorsRank,a->name,(void*)r);
    }
    return *r;
}
#endif
static int grib_data_accessors_trie_push(grib_trie_with_rank* accessorsTrie, grib_accessor* a)
{
    return grib_trie_with_rank_insert(accessorsTrie, a->name, a);
}

static int create_keys(grib_accessor* a,long onlySubset,long startSubset,long endSubset)
{
    grib_accessor_bufr_data_array *self =(grib_accessor_bufr_data_array*)a;
    int err=0;
    int rank;
    grib_accessor* elementAccessor=0;
    grib_accessor* associatedFieldAccessor=0;
    grib_accessor* associatedFieldSignificanceAccessor=0;
    long iss,end,elementsInSubset,ide;
    grib_section* section=NULL;
    /*grib_section* rootSection=NULL;*/
    bufr_descriptor* descriptor;
    /*grib_section* sectionUp=0;*/
    grib_section* groupSection=0;
    long groupNumber=0;
    /*long indexOfGroupNumber=0;*/
    int depth;
    int max_depth = -1; /* highest value of depth */
    int idx;
    grib_context* c=a->context;
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
    grib_accessors_list* lastAccessorInList=NULL;
    int bitmapIndex=-1;
    int incrementBitmapIndex=1;
    grib_accessor* elementFromBitmap=NULL;
    /*int reuseBitmap=0;*/
    int dump=1,count=0;
    /*int forceGroupClosure=0;*/

    creatorGroup.op         = "bufr_group";
    creatorGroup.name       = "groupNumber";
    creatorGroup.name_space = "";
    creatorGroup.flags      = GRIB_ACCESSOR_FLAG_DUMP;
    creatorGroup.set        = 0;

    if (self->dataAccessors) {
        grib_accessors_list_delete(c,self->dataAccessors);
    }
    self->dataAccessors=grib_accessors_list_create(c);

    if (self->dataAccessorsTrie) {
        grib_trie_with_rank_delete(self->dataAccessorsTrie);
    }
    self->dataAccessorsTrie=grib_trie_with_rank_new(c);

    if (self->tempStrings) {
        grib_sarray_delete_content(c, self->tempStrings);
        grib_sarray_delete        (c, self->tempStrings);
        self->tempStrings=NULL;
    }
    self->tempStrings = grib_sarray_new(c, self->numberOfSubsets, 10);

    end= self->compressedData ? 1 : self->numberOfSubsets;
    groupNumber=1;

    gaGroup = grib_accessor_factory(self->dataKeys, &creatorGroup, 0, NULL);
    gaGroup->bufr_group_number=groupNumber;
    gaGroup->sub_section=grib_section_create(grib_handle_of_accessor(a),gaGroup);
    section=gaGroup->sub_section;
    /*rootSection=section;*/
    /*sectionUp=self->dataKeys;*/
    accessor_constant_set_type(gaGroup,GRIB_TYPE_LONG);
    accessor_constant_set_dval(gaGroup,groupNumber);
    self->dataKeys->block->first=0;
    self->dataKeys->block->last=0;
    grib_push_accessor(gaGroup,self->dataKeys->block);

    /*indexOfGroupNumber=0;*/
    depth=0;
    extraElement=0;

    for (iss=0;iss<end;iss++) {
        qualityPresent=0;
        /*forceGroupClosure=0;*/
        elementsInSubset= self->compressedData ? grib_iarray_used_size(self->elementsDescriptorsIndex->v[0]) :
                grib_iarray_used_size(self->elementsDescriptorsIndex->v[iss]);
        associatedFieldAccessor=NULL;
        associatedFieldSignificanceAccessor=NULL;
        for (ide=0;ide<elementsInSubset;ide++) {
            idx = self->compressedData ? self->elementsDescriptorsIndex->v[0]->v[ide] :
                    self->elementsDescriptorsIndex->v[iss]->v[ide] ;

            descriptor=self->expanded->v[idx];
            if (descriptor->nokey==1) {
                continue;  /* Descriptor does not have an associated key e.g. inside op 203YYY */
            }
            elementFromBitmap=NULL;
            if (descriptor->F==0 && IS_QUALIFIER(descriptor->X) &&
                self->unpackMode==CODES_BUFR_UNPACK_STRUCTURE) {
                int sidx=significanceQualifierIndex(descriptor->X,descriptor->Y);
                groupNumber++;

                if (significanceQualifierGroup[sidx]) {
                    groupSection=significanceQualifierGroup[sidx]->parent;
                    depth=significanceQualifierDepth[sidx];
                    if (depth < max_depth) {
                        /* If depth >= max_depth, then no entry will be deeper so no need for call */
                        reset_deeper_qualifiers(significanceQualifierGroup,significanceQualifierDepth,
                                                number_of_qualifiers,depth);
                    }
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
                gaGroup->sub_section=grib_section_create(grib_handle_of_accessor(a),gaGroup);
                gaGroup->bufr_group_number=groupNumber;
                accessor_constant_set_type(gaGroup,GRIB_TYPE_LONG);
                accessor_constant_set_dval(gaGroup,groupNumber);
                grib_push_accessor(gaGroup,groupSection->block);

                section=gaGroup->sub_section;
                /*sectionUp=gaGroup->parent;*/

                significanceQualifierGroup[sidx]=gaGroup;
                significanceQualifierDepth[sidx]=depth;
                if (depth > max_depth) max_depth = depth;
                incrementBitmapIndex=1;
                dump=1;
            } else if (descriptor->code == 31031 && incrementBitmapIndex!=0) {
                /* bitmap */
                bitmapIndex++;
                groupNumber++;
                incrementBitmapIndex=0;
                if (bitmapIndex >= MAX_NUMBER_OF_BITMAPS) {
                    grib_context_log(c, GRIB_LOG_ERROR, "Bitmap error: bitmap index=%d, max num bitmaps=%d\n", bitmapIndex, MAX_NUMBER_OF_BITMAPS);
                    err = GRIB_DECODING_ERROR;
                    return err;
                }
                bitmapStart[bitmapIndex]=grib_accessors_list_last(self->dataAccessors);
                bitmapSize[bitmapIndex]=1;
                if (self->expanded->v[idx-1]->code ==31002 || self->expanded->v[idx-1]->code==31001)
                    extraElement+=1;

                if (bitmapGroup[bitmapIndex]) {
                    groupSection=bitmapGroup[bitmapIndex]->parent;
                    depth=bitmapDepth[bitmapIndex];
                    reset_deeper_qualifiers(significanceQualifierGroup,significanceQualifierDepth,
                                            number_of_qualifiers,depth);
                    /* TODO: This branch is not reached in our tests! */
                    reset_deeper_qualifiers(bitmapGroup,bitmapDepth,MAX_NUMBER_OF_BITMAPS,depth);
                } else {
                    groupSection=section;
                    depth++;
                }
                gaGroup = grib_accessor_factory(groupSection, &creatorGroup, 0, NULL);
                gaGroup->sub_section=grib_section_create(grib_handle_of_accessor(a),gaGroup);
                gaGroup->bufr_group_number=groupNumber;
                accessor_constant_set_type(gaGroup,GRIB_TYPE_LONG);
                accessor_constant_set_dval(gaGroup,groupNumber);
                grib_push_accessor(gaGroup,groupSection->block);

                section=gaGroup->sub_section;
                /*sectionUp=gaGroup->parent;*/
                bitmapGroup[bitmapIndex]=gaGroup;
                bitmapDepth[bitmapIndex]=depth;
                dump=1;
            } else if (descriptor->code == 31031) {
                dump=1;
                bitmapSize[bitmapIndex]++;
                bitmap.cursor=0;
            } else if (descriptor->code == 222000 || descriptor->code == 223000 || descriptor->code == 224000 || descriptor->code == 225000 ) {
                bitmap.referredElement=NULL;
                qualityPresent=1;
                incrementBitmapIndex=1;
                dump=1;
                bitmap.cursor=0;
                extraElement+=1;
            } else if (descriptor->code == 236000 || descriptor->code == 237000 ) {
                bitmap.referredElement=NULL;
                bitmap.cursor=0;
                /*reuseBitmap=1;*/
                extraElement+=1;
                dump=1;
            } else if (descriptor->code == 237255 ) {
                /*reuseBitmap=0;*/
                incrementBitmapIndex=1;
                bitmap.cursor=0;
                dump=1;
            } else if ( ( descriptor->X==33 || bufr_descriptor_is_marker(descriptor) )  && qualityPresent) {
                if (!bitmap.referredElement)
                    bitmap_init(c, &bitmap,bitmapStart[bitmapIndex],bitmapSize[bitmapIndex],lastAccessorInList);
                elementFromBitmap=get_element_from_bitmap(a,&bitmap);
                dump=1;
                /* } else if ( descriptor->Y==1 && IS_QUALIFIER(self->expanded->v[idx-1]->X)==0) { */
                /* forceGroupClosure=1; */
                /* reset_qualifiers(significanceQualifierGroup); */
            } else if (descriptor->X==33 && !qualityPresent) {
                if (c->bufr_quality_without_bitmap == 1) {
                    dump=1;  /* ECC-690: percentConfidence WITHOUT a bitmap! e.g. NOAA GOES16 BUFR */
                }
            }

            if (ide==0 && !self->compressedData) {
                grib_accessor* asn=NULL;
                long subsetNumber=iss+1;
                size_t len=1;
                grib_action creatorsn = {0, };
                creatorsn.op         = "variable";
                creatorsn.name_space = "";
                creatorsn.flags     = GRIB_ACCESSOR_FLAG_READ_ONLY | GRIB_ACCESSOR_FLAG_DUMP ;
                creatorsn.set        = 0;

                creatorsn.name="subsetNumber";
                asn=grib_accessor_factory(section, &creatorsn, 0, NULL);
                accessor_variable_set_type(asn,GRIB_TYPE_LONG);
                grib_pack_long(asn,&subsetNumber,&len);

                grib_push_accessor(asn,section->block);
                rank=grib_data_accessors_trie_push(self->dataAccessorsTrie,asn);
                grib_accessors_list_push(self->dataAccessors,asn,rank);
            }
            count++;
            elementAccessor=create_accessor_from_descriptor(a,associatedFieldAccessor,section,ide,iss,dump,count);
            if (!elementAccessor) {
                err = GRIB_DECODING_ERROR;
                return err;
            }
            associatedFieldAccessor=NULL;
            if (elementFromBitmap && self->unpackMode==CODES_BUFR_UNPACK_STRUCTURE) {
                if (descriptor->code != 33007 && descriptor->code != 223255 ) {
                    grib_accessor* newAccessor=grib_accessor_clone(elementAccessor,section,&err);
                    newAccessor->parent=groupSection;
                    newAccessor->name=grib_context_strdup(c,elementFromBitmap->name);
                    grib_push_accessor(newAccessor,groupSection->block);
                    rank=grib_data_accessors_trie_push(self->dataAccessorsTrie,newAccessor);
                    grib_accessors_list_push(self->dataAccessors,newAccessor,rank);
                }

                err=grib_accessor_add_attribute(accessor_or_attribute_with_same_name(elementFromBitmap,elementAccessor->name),elementAccessor,1);
            } else if (elementAccessor) {
                int add_key = 1;
                switch (descriptor->code) {
                case 999999:
                    associatedFieldAccessor=elementAccessor;
                    grib_convert_to_attribute(associatedFieldAccessor);
                    if (associatedFieldSignificanceAccessor) {
                        grib_accessor* newAccessor=grib_accessor_clone(associatedFieldSignificanceAccessor,section,&err);
                        if (err) {
                            grib_context_log(a->context,GRIB_LOG_ERROR,"unable to clone accessor '%s'\n",associatedFieldSignificanceAccessor->name);
                            return err;
                        }
                        grib_accessor_add_attribute(associatedFieldAccessor,newAccessor,1);
                    }
                    break;
                case 31021:
                    associatedFieldSignificanceAccessor=elementAccessor;
                    break;
                /*case 33007:*/
                    /* ECC-690: See later */
                    /* break; */
                default:
                    add_key = 1;
                    /* ECC-690: percentConfidence WITHOUT a bitmap! e.g. NOAA GOES16 BUFR */
                    if(descriptor->code==33007) {
                        add_key = 0; /* Standard behaviour */
                        if (!qualityPresent) {
                            if (c->bufr_quality_without_bitmap) {
                                add_key = 1;
                            } else {
                                grib_context_log(c, GRIB_LOG_WARNING,
                                                 "create_keys: descriptor=%6.6ld: Class 33 quality information without a bitmap!",descriptor->code);
                            }
                        }
                    }
                    if (add_key) {
                        grib_push_accessor(elementAccessor,section->block);
                        rank=grib_data_accessors_trie_push(self->dataAccessorsTrie,elementAccessor);
                        grib_accessors_list_push(self->dataAccessors,elementAccessor,rank);
                        lastAccessorInList=grib_accessors_list_last(self->dataAccessors);
                    }
                }
            }
        }
    }

    return err;
}

static void set_input_replications(grib_handle* h,grib_accessor_bufr_data_array *self)
{
    size_t nInputReplications;
    size_t nInputExtendedReplications;
    size_t nInputShortReplications;
    self->nInputReplications=-1;
    self->nInputExtendedReplications=-1;
    self->nInputShortReplications=-1;
    self->iInputReplications=0;
    self->iInputExtendedReplications=0;
    self->iInputShortReplications=0;
    if (grib_get_size(h,"inputDelayedDescriptorReplicationFactor",&nInputReplications)==0 && nInputReplications!=0) {
        if (self->inputReplications) grib_context_free(h->context,self->inputReplications);
        self->inputReplications=(long*)grib_context_malloc_clear(h->context,sizeof(long)*nInputReplications);
        grib_get_long_array(h,"inputDelayedDescriptorReplicationFactor",self->inputReplications,&nInputReplications);
        /* default-> no input replications*/
        if (self->inputReplications[0]<0) self->nInputReplications=-1;
        else self->nInputReplications=nInputReplications;
    }
    if (grib_get_size(h,"inputExtendedDelayedDescriptorReplicationFactor",&nInputExtendedReplications)==0 && nInputExtendedReplications!=0) {
        if (self->inputExtendedReplications) grib_context_free(h->context,self->inputExtendedReplications);
        self->inputExtendedReplications=(long*)grib_context_malloc_clear(h->context,sizeof(long)*nInputExtendedReplications);
        grib_get_long_array(h,"inputExtendedDelayedDescriptorReplicationFactor",self->inputExtendedReplications,&nInputExtendedReplications);
        /* default-> no input replications*/
        if (self->inputExtendedReplications[0]<0) self->nInputExtendedReplications=-1;
        else self->nInputExtendedReplications=nInputExtendedReplications;
    }
    if (grib_get_size(h,"inputShortDelayedDescriptorReplicationFactor",&nInputShortReplications)==0 && nInputShortReplications!=0) {
        if (self->inputShortReplications) grib_context_free(h->context,self->inputShortReplications);
        self->inputShortReplications=(long*)grib_context_malloc_clear(h->context,sizeof(long)*nInputShortReplications);
        grib_get_long_array(h,"inputShortDelayedDescriptorReplicationFactor",self->inputShortReplications,&nInputShortReplications);
        /* default-> no input replications*/
        if (self->inputShortReplications[0]<0) self->nInputShortReplications=-1;
        else self->nInputShortReplications=nInputShortReplications;
    }
}

static void set_input_bitmap(grib_handle* h,grib_accessor_bufr_data_array *self)
{
    size_t nInputBitmap;
    self->nInputBitmap=-1;
    self->iInputBitmap=0;
    if (grib_get_size(h,"inputDataPresentIndicator",&nInputBitmap)==0 && nInputBitmap!=0) {
        if (self->inputBitmap) grib_context_free(h->context,self->inputBitmap);
        self->inputBitmap=(double*)grib_context_malloc_clear(h->context,sizeof(double)*nInputBitmap);
        grib_get_double_array(h,"inputDataPresentIndicator",self->inputBitmap,&nInputBitmap);
        /* default-> no input bitmap*/
        if (self->inputBitmap[0]<0) self->nInputBitmap=-1;
        else self->nInputBitmap=nInputBitmap;
    }
}

static int process_elements(grib_accessor* a,int flag,long onlySubset,long startSubset,long endSubset)
{
    int err=0;
    long inr,innr,ir,ip;
    long n[MAX_NESTED_REPLICATIONS]={0,};
    long nn[MAX_NESTED_REPLICATIONS]={0,};
    long numberOfElementsToRepeat[MAX_NESTED_REPLICATIONS]={0,};
    long numberOfRepetitions[MAX_NESTED_REPLICATIONS]={0,};
    long startRepetition[MAX_NESTED_REPLICATIONS]={0,};
    long numberOfNestedRepetitions=0;
    unsigned char* data=0;
    size_t subsetListSize=0;
    long* subsetList=0;
    int i;
    grib_iarray* elementsDescriptorsIndex=0;

    long pos=0,dataOffset=0;
    long iiss,iss,end,elementIndex,index;
    long numberOfDescriptors;
    long totalSize;
    bufr_descriptor** descriptors=0;
    long icount;
    int decoding=0,do_clean=1;
    grib_buffer* buffer=NULL;
    codec_element_proc codec_element;
    codec_replication_proc codec_replication;
    grib_accessor* dataAccessor=NULL;
    bufr_descriptor* bd=0;

    grib_darray* dval = NULL;
    grib_sarray* sval = NULL;
    grib_accessor_bufr_data_array *self =(grib_accessor_bufr_data_array*)a;

    grib_handle* h=grib_handle_of_accessor(a);
    grib_context* c=h->context;

    totalSize=self->bitsToEndData;

    switch (flag) {
    case PROCESS_DECODE:
        if (!self->do_decode) return 0;
        self->do_decode=0;
        buffer=h->buffer;
        decoding=1;
        do_clean=1;
        dataAccessor=grib_find_accessor(grib_handle_of_accessor(a),self->bufrDataEncodedName);
        DebugAssert(dataAccessor);
        dataOffset=accessor_raw_get_offset(dataAccessor);
        pos=dataOffset*8;
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

        set_input_replications(h,self);
        set_input_bitmap(h,self);

        break;
    case PROCESS_ENCODE:
        buffer=grib_create_growable_buffer(c);
        decoding=0;
        do_clean=0;
        self->do_decode=0;
        pos=0;
        codec_element=&encode_element;
        grib_get_long(grib_handle_of_accessor(a),"extractSubset",&onlySubset);
        grib_get_long(grib_handle_of_accessor(a),"extractSubsetIntervalStart",&startSubset);
        grib_get_long(grib_handle_of_accessor(a),"extractSubsetIntervalEnd",&endSubset);
        err=grib_get_size(grib_handle_of_accessor(a),"extractSubsetList",&subsetListSize);
        if (err) return err;
        if (subsetList) grib_context_free(c,subsetList);
        subsetList=(long*)grib_context_malloc_clear(c,subsetListSize*sizeof(long));
        err=grib_get_long_array(grib_handle_of_accessor(a),"extractSubsetList",subsetList,&subsetListSize);
        if (err) return err;

        codec_replication=&encode_replication;
        break;
    default :
        return GRIB_NOT_IMPLEMENTED;
    }
    data = buffer->data;

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

    if (flag != PROCESS_DECODE) {      /* Operator 203YYY: key OVERRIDDEN_REFERENCE_VALUES_KEY */
        err=grib_get_size(h, OVERRIDDEN_REFERENCE_VALUES_KEY, &self->refValListSize);
        if (err) return err;
        if (self->refValList) grib_context_free(c, self->refValList);
        if (self->refValListSize > 0) {
            self->refValList=(long*)grib_context_malloc_clear(c, self->refValListSize*sizeof(long));
            err=grib_get_long_array(grib_handle_of_accessor(a),OVERRIDDEN_REFERENCE_VALUES_KEY, self->refValList, &self->refValListSize);
            if (err) return err;
        }
    }

    numberOfDescriptors=grib_bufr_descriptors_array_used_size(self->expanded);

    if (self->iss_list) {
        grib_iarray_delete(self->iss_list);
        self->iss_list=0;
    }
    end= self->compressedData == 1 ? 1 : self->numberOfSubsets ;

    if (flag!=PROCESS_DECODE) {
        self->iss_list=set_subset_list(c,self,onlySubset,startSubset,endSubset,subsetList,subsetListSize);
        end= self->compressedData == 1 ? 1 : grib_iarray_used_size(self->iss_list) ;
    }

    for (iiss=0;iiss<end;iiss++) {
        icount=1;
        if (self->compressedData==0 && self->iss_list) {
            iss=self->iss_list->v[iiss];
        } else {
            iss=iiss;
        }

        grib_context_log(c, GRIB_LOG_DEBUG,"BUFR data processing: subsetNumber=%ld", iss+1);
        self->refValIndex = 0;

        if (flag!=PROCESS_ENCODE) {
            elementsDescriptorsIndex=grib_iarray_new(c,DYN_ARRAY_SIZE_INIT,DYN_ARRAY_SIZE_INCR);
            if (!self->compressedData) {
                dval=grib_darray_new(c,DYN_ARRAY_SIZE_INIT,DYN_ARRAY_SIZE_INCR);
                /* sval=grib_sarray_new(c,10,10); */
            }
        } else {
            if (self->elementsDescriptorsIndex==NULL) {
                return GRIB_ENCODING_ERROR;  /* See ECC-359 */
            }
            elementsDescriptorsIndex=self->elementsDescriptorsIndex->v[iss];
            dval=self->numericValues->v[iss];
        }
        elementIndex=0;

        numberOfNestedRepetitions=0;
        inr=0;
        for (i=0;i<numberOfDescriptors;i++) {
            grib_context_log(c, GRIB_LOG_DEBUG,"BUFR data processing: elementNumber=%ld code=%6.6ld", icount++,descriptors[i]->code);
            switch(descriptors[i]->F) {
            case 0:
                /* Table B element */
                if (flag!=PROCESS_ENCODE) grib_iarray_push(elementsDescriptorsIndex,i);
                if (descriptors[i]->code==31031 && !is_bitmap_start_defined(self)) {
                    /* self->bitmapStart=grib_iarray_used_size(elementsDescriptorsIndex)-1; */
                    self->bitmapStart=elementIndex;
                }

                err=codec_element(c,self,iss,buffer,data,&pos,i,0,elementIndex,dval,sval);
                if (err) return err;
                elementIndex++;
                break;
            case 1:
                /* Delayed replication */
                inr=numberOfNestedRepetitions;
                numberOfNestedRepetitions++;
                DebugAssert(numberOfNestedRepetitions<=MAX_NESTED_REPLICATIONS);
                numberOfElementsToRepeat[inr]=descriptors[i]->X;
                n[inr]=numberOfElementsToRepeat[inr];
                i++;

                /* ECC-517 */
                data = buffer->data;
                err=codec_replication(c,self,iss,buffer,data,&pos,i,elementIndex,dval,&(numberOfRepetitions[inr]));
                if (err) return err;

                startRepetition[inr]=i;
                nn[inr]=numberOfRepetitions[inr];
                if (flag!=PROCESS_ENCODE) grib_iarray_push(elementsDescriptorsIndex,i);
                elementIndex++;
                if (numberOfRepetitions[inr]==0) {
                    i+=numberOfElementsToRepeat[inr];
                    if (inr>0) {
                        n[inr-1]-=numberOfElementsToRepeat[inr]+2;
                        /* if the empty nested repetition is at the end of the nesting repetition
                           we need to re-point to the start of the nesting repetition */
                        ip=inr-1;
                        while (ip>=0 && n[ip]==0) {
                            nn[ip]--;
                            if (nn[ip]<=0) {
                              numberOfNestedRepetitions--;
                            } else {
                              n[ip]=numberOfElementsToRepeat[ip];
                              i=startRepetition[ip];
                            }
                            ip--;
                        }
                    }
                    numberOfNestedRepetitions--;
                }
                continue;
            case 2:
                /* Operator */
                switch(descriptors[i]->X) {

                case 3:
                    /* Change reference values */
                    if (self->compressedData == 1 && flag != PROCESS_DECODE) {
                        grib_context_log(c,GRIB_LOG_ERROR,"process_elements: operator %d not supported for encoding compressed data",descriptors[i]->X);
                        return GRIB_INTERNAL_ERROR;
                    }
                    if (descriptors[i]->Y == 255) {
                        grib_context_log(c, GRIB_LOG_DEBUG,"Operator 203YYY: Y=255, definition of new reference values is concluded");
                        self->change_ref_value_operand = 255;
                        /*if (c->debug) tableB_override_dump(self);*/
                        if (iss == 0 && flag == PROCESS_DECODE) {
                            /*Write out the contents of the TableB overridden reference values to the transient array key*/
                            err = tableB_override_set_key(h, self);
                            if (err) return err;
                        }
                        if (flag != PROCESS_DECODE) {
                            /* Encoding operator 203YYY */
                            if (self->refValIndex != self->refValListSize) {
                                grib_context_log(c, GRIB_LOG_ERROR,
                                        "process_elements: The number of overridden reference values (%ld) different from"
                                        " number of descriptors between operator 203YYY and 203255 (%ld)",
                                        self->refValListSize, self->refValIndex);
                                return GRIB_ENCODING_ERROR;
                            }
                        }
                    } else if (descriptors[i]->Y == 0) {
                        grib_context_log(c, GRIB_LOG_DEBUG,"Operator 203YYY: Y=0, clearing override of table B");
                        tableB_override_clear(c, self);
                        self->change_ref_value_operand = 0;
                    } else {
                        const int numBits = descriptors[i]->Y;
                        grib_context_log(c, GRIB_LOG_DEBUG,"Operator 203YYY: Definition phase: Num bits=%d", numBits);
                        self->change_ref_value_operand = numBits;
                        tableB_override_clear(c, self);
                        if (flag != PROCESS_DECODE) {
                            err = check_overridden_reference_values(c, self->refValList, self->refValListSize, numBits);
                            if (err) return err;
                        }
                    }
                    /*grib_iarray_push(elementsDescriptorsIndex,i);*/
                    if (decoding) push_zero_element(self,dval);
                    break;

                case 5: /* Signify character */
                    descriptors[i]->width=descriptors[i]->Y*8;
                    descriptors[i]->type=BUFR_DESCRIPTOR_TYPE_STRING;
                    err=codec_element(c,self,iss,buffer,data,&pos,i,0,elementIndex,dval,sval);
                    if (err) return err;
                    if (flag!=PROCESS_ENCODE) grib_iarray_push(elementsDescriptorsIndex,i);
                    elementIndex++;
                    break;
                case 22: /* Quality information follows */
                    if (descriptors[i]->Y==0)  {
                        if (flag==PROCESS_DECODE) {
                            grib_iarray_push(elementsDescriptorsIndex,i);
                            push_zero_element(self,dval);
                        } else if (flag==PROCESS_ENCODE) {
                            if (descriptors[i+1] && descriptors[i+1]->code!=236000 && descriptors[i+1]->code!=237000 )
                                restart_bitmap(self);
                        } else if (flag==PROCESS_NEW_DATA) {
                            grib_iarray_push(elementsDescriptorsIndex,i);
                            if (descriptors[i+1] && descriptors[i+1]->code!=236000 && descriptors[i+1]->code!=237000 )
                                consume_bitmap(self,i);
                        }
                        elementIndex++;
                    }
                    break;
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
                case 24:
                    /*first-order statistical values marker operator*/
                case 32:
                    /*replaced/retained values marker operator*/
                    if (descriptors[i]->Y==255) {
                        index=get_next_bitmap_descriptor_index(self,elementsDescriptorsIndex,dval);
                        err=codec_element(c,self,iss,buffer,data,&pos,index,0,elementIndex,dval,sval);
                        if (err) return err;
                        /* self->expanded->v[index] */
                        if (flag!=PROCESS_ENCODE) grib_iarray_push(elementsDescriptorsIndex,i);
                        elementIndex++;
                    } else {
                        if (flag!=PROCESS_ENCODE) grib_iarray_push(elementsDescriptorsIndex,i);
                        if (decoding) { 
                            push_zero_element(self,dval);
                        }
                        elementIndex++;
                    }
                    break;
                case 23:
                    if (descriptors[i]->Y==255) {
                        index=get_next_bitmap_descriptor_index(self,elementsDescriptorsIndex,dval);
                        err=codec_element(c,self,iss,buffer,data,&pos,index,0,elementIndex,dval,sval);
                        if (err) return err;
                        /* self->expanded->v[index] */
                        if (flag!=PROCESS_ENCODE) grib_iarray_push(elementsDescriptorsIndex,i);
                        elementIndex++;
                    } else {
                        if (flag==PROCESS_DECODE) {
                            grib_iarray_push(elementsDescriptorsIndex,i);
                            push_zero_element(self,dval);
                            if ( descriptors[i+1] && descriptors[i+1]->code!=236000 && descriptors[i+1]->code!=237000 ) {
                                err = build_bitmap(self,data,&pos,elementIndex,elementsDescriptorsIndex,i);
                                if (err) return err;
                            }
                        } else if (flag==PROCESS_ENCODE) {
                            if ( descriptors[i+1] && descriptors[i+1]->code!=236000 && descriptors[i+1]->code!=237000 )
                                restart_bitmap(self);
                        } else if (flag==PROCESS_NEW_DATA) {
                            grib_iarray_push(elementsDescriptorsIndex,i);
                            if ( descriptors[i+1] && descriptors[i+1]->code!=236000 && descriptors[i+1]->code!=237000 ) {
                                err = build_bitmap_new_data(self,data,&pos,elementIndex,elementsDescriptorsIndex,i);
                                if (err) return err;
                            }
                        }
                        elementIndex++;
                    }
                    break;
                case 25:
                    /*difference statistical values marker operator*/
                    if (descriptors[i]->Y==255) {
                        index=get_next_bitmap_descriptor_index(self,elementsDescriptorsIndex,dval);
                        bd=grib_bufr_descriptor_clone(self->expanded->v[index]);
                        bd->reference=-grib_power(bd->width,2);
                        bd->width++;

                        err=codec_element(c,self,iss,buffer,data,&pos,index,bd,elementIndex,dval,sval);
                        grib_bufr_descriptor_delete(bd);
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
                    if (flag==PROCESS_DECODE) {
                        grib_iarray_push(elementsDescriptorsIndex,i);
                        if (decoding) push_zero_element(self,dval);
                        err = build_bitmap(self,data,&pos,elementIndex,elementsDescriptorsIndex,i);
                        if (err) return err;
                    } else if (flag==PROCESS_ENCODE) {
                        restart_bitmap(self);
                    } else if (flag==PROCESS_NEW_DATA) {
                        grib_iarray_push(elementsDescriptorsIndex,i);
                        err = build_bitmap_new_data(self,data,&pos,elementIndex,elementsDescriptorsIndex,i);
                        if (err) return err;
                    }
                    elementIndex++;
                    break;
                case 37:
                    /* reuse defined bitmap */
                    if (flag!=PROCESS_ENCODE) {
                        grib_iarray_push(elementsDescriptorsIndex,i);
                        if (decoding) push_zero_element(self,dval);
                    }
                    if (descriptors[i]->Y==0) restart_bitmap(self);
                    /* cancel reuse */
                    else cancel_bitmap(self);
                    elementIndex++;
                    break;
                default :
                    grib_context_log(c,GRIB_LOG_ERROR,"process_elements: unsupported operator %d\n",descriptors[i]->X);
                    return GRIB_INTERNAL_ERROR;
                } /* F == 2 */
                break;
                case 9:
                    /* associated field */
                    if (descriptors[i]->X==99 && descriptors[i]->Y==999) {
                        err=codec_element(c,self,iss,buffer,data,&pos,i,0,elementIndex,dval,sval);
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
            } /* switch F */

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
                                n[ir-1]-=numberOfElementsToRepeat[ir]+1;
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

    /*if (c->debug) grib_vdarray_print("process_elements: self->numericValues", self->numericValues);*/

    if(decoding) {
        err=create_keys(a,0,0,0);
        self->bitsToEndData=totalSize;
    } else {
        self->bitsToEndData=buffer->ulength*8;
        grib_set_bytes(grib_handle_of_accessor(a),self->bufrDataEncodedName,buffer->data,&(buffer->ulength));
        grib_buffer_delete(c,buffer);
        if (self->numberOfSubsets != grib_iarray_used_size(self->iss_list) ) {
            grib_set_long(h,self->numberOfSubsetsName,grib_iarray_used_size(self->iss_list));
        }
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
    long i;
    grib_accessor_bufr_data_array *self =(grib_accessor_bufr_data_array*)a;

    err=process_elements(a,PROCESS_DECODE,0,0,0);
    if (err) return err;

    if (self->compressedData) {
        l=grib_vdarray_used_size(self->numericValues);

        *count=l*self->numberOfSubsets;
    } else {
        *count=0;
        for (i=0;i<self->numberOfSubsets;i++)
            *count+=grib_iarray_used_size(self->elementsDescriptorsIndex->v[i]);
    }

    return err;
}

static int unpack_double(grib_accessor* a, double* val, size_t *len)
{
    int err=0,i,k,ii;
    int proc_flag=PROCESS_DECODE;
    size_t l=0,elementsInSubset;
    long numberOfSubsets=0;
    grib_accessor_bufr_data_array *self =(grib_accessor_bufr_data_array*)a;

    if (self->unpackMode == CODES_BUFR_NEW_DATA) proc_flag=PROCESS_NEW_DATA;

    err=process_elements(a,proc_flag,0,0,0);
    if (err) return err;
    if (!val) return err;

    l=grib_vdarray_used_size(self->numericValues);
    err=grib_get_long(grib_handle_of_accessor(a),self->numberOfSubsetsName,&numberOfSubsets);
    if (err) return err;

    if (self->compressedData) {
        ii=0;
        for (k=0;k<numberOfSubsets;k++) {
            for (i=0;i<l;i++) {
                val[ii++]=self->numericValues->v[i]->n > 1 ? self->numericValues->v[i]->v[k] : self->numericValues->v[i]->v[0];
            }
        }
    } else {
        ii=0;
        for (k=0;k<numberOfSubsets;k++) {
            elementsInSubset=grib_iarray_used_size(self->elementsDescriptorsIndex->v[k]);
            for (i=0;i<elementsInSubset;i++) {
                val[ii++]=self->numericValues->v[k]->v[i];
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
    if (self->dataAccessorsTrie) grib_trie_with_rank_delete_container(self->dataAccessorsTrie);
    if (self->tempStrings) {
        grib_sarray_delete_content(c, self->tempStrings);
        grib_sarray_delete        (c, self->tempStrings);
    }
}
