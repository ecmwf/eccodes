/*
 * Copyright 2005-2018 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

/*****************************************
 *  Enrico Fucile
 ****************************************/

#include "grib_api_internal.h"
#include <ctype.h>

/*
   This is used by make_class.pl

   START_CLASS_DEF
   CLASS      = accessor
   SUPER      = grib_accessor_class_gen
   IMPLEMENTS = init;dump
   IMPLEMENTS = unpack_string;unpack_string_array;unpack_long; unpack_double
   IMPLEMENTS = unpack_double_element
   IMPLEMENTS = pack_long; pack_double ; pack_string_array; pack_string
   IMPLEMENTS = value_count; get_native_type; make_clone; destroy
   MEMBERS    = long index
   MEMBERS    = int type
   MEMBERS    = long compressedData
   MEMBERS    = long subsetNumber
   MEMBERS    = long numberOfSubsets
   MEMBERS    = bufr_descriptors_array* descriptors
   MEMBERS    = grib_vdarray* numericValues
   MEMBERS    = grib_vsarray* stringValues
   MEMBERS    = grib_viarray* elementsDescriptorsIndex

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
static int pack_string(grib_accessor*, const char*, size_t *len);
static int pack_string_array(grib_accessor*, const char**, size_t *len);
static int unpack_double(grib_accessor*, double* val,size_t *len);
static int unpack_long(grib_accessor*, long* val,size_t *len);
static int unpack_string (grib_accessor*, char*, size_t *len);
static int unpack_string_array (grib_accessor*, char**, size_t *len);
static int value_count(grib_accessor*,long*);
static void destroy(grib_context*,grib_accessor*);
static void dump(grib_accessor*, grib_dumper*);
static void init(grib_accessor*,const long, grib_arguments* );
static void init_class(grib_accessor_class*);
static int unpack_double_element(grib_accessor*,size_t i, double* val);
static grib_accessor* make_clone(grib_accessor*,grib_section*,int*);

typedef struct grib_accessor_bufr_data_element {
    grib_accessor          att;
/* Members defined in gen */
/* Members defined in bufr_data_element */
	long index;
	int type;
	long compressedData;
	long subsetNumber;
	long numberOfSubsets;
	bufr_descriptors_array* descriptors;
	grib_vdarray* numericValues;
	grib_vsarray* stringValues;
	grib_viarray* elementsDescriptorsIndex;
} grib_accessor_bufr_data_element;

extern grib_accessor_class* grib_accessor_class_gen;

static grib_accessor_class _grib_accessor_class_bufr_data_element = {
    &grib_accessor_class_gen,                      /* super                     */
    "bufr_data_element",                      /* name                      */
    sizeof(grib_accessor_bufr_data_element),  /* size                      */
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
    &pack_string,                /* grib_pack procedures string    */
    &unpack_string,              /* grib_unpack procedures string  */
    &pack_string_array,          /* grib_pack array procedures string    */
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
    &unpack_double_element,     /* unpack only ith value          */
    0,     /* unpack a subarray         */
    0,              		/* clear          */
    &make_clone,               		/* clone accessor          */
};


grib_accessor_class* grib_accessor_class_bufr_data_element = &_grib_accessor_class_bufr_data_element;


static void init_class(grib_accessor_class* c)
{
	c->next_offset	=	(*(c->super))->next_offset;
	c->string_length	=	(*(c->super))->string_length;
	c->byte_count	=	(*(c->super))->byte_count;
	c->byte_offset	=	(*(c->super))->byte_offset;
	c->sub_section	=	(*(c->super))->sub_section;
	c->pack_missing	=	(*(c->super))->pack_missing;
	c->is_missing	=	(*(c->super))->is_missing;
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
}

/* END_CLASS_IMP */

static grib_accessor* make_clone(grib_accessor* a,grib_section* s,int* err)
{
    grib_accessor* the_clone=NULL;
    grib_accessor* attribute=NULL;
    grib_accessor_bufr_data_element* elementAccessor;
    grib_accessor_bufr_data_element* self;
    int i;
    grib_action creator = {0, };
    creator.op         = "bufr_data_element";
    creator.name_space = "";
    creator.set        = 0;
    creator.name="unknown";
    if (strcmp(a->cclass->name,"bufr_data_element")) {
        grib_context_log(a->context,GRIB_LOG_FATAL,"wrong accessor type: '%s' should be '%s'",a->cclass->name,"bufr_data_element");
    }
    *err=0;

    the_clone = grib_accessor_factory(s, &creator, 0, NULL);
    the_clone->name=grib_context_strdup(a->context,a->name);
    elementAccessor=(grib_accessor_bufr_data_element*)the_clone;
    self=(grib_accessor_bufr_data_element*)a;
    the_clone->flags=a->flags;
    the_clone->parent=NULL;
    the_clone->h=s->h;
    elementAccessor->index=self->index;
    elementAccessor->type=self->type;
    elementAccessor->numberOfSubsets=self->numberOfSubsets;
    elementAccessor->subsetNumber=self->subsetNumber;
    elementAccessor->compressedData=self->compressedData;
    elementAccessor->descriptors=self->descriptors;
    elementAccessor->numericValues=self->numericValues;
    elementAccessor->stringValues=self->stringValues;
    elementAccessor->elementsDescriptorsIndex=self->elementsDescriptorsIndex;

    i=0;
    while (a->attributes[i]) {
        attribute=grib_accessor_clone(a->attributes[i],s,err);
        /* attribute->parent=a->parent; */
        grib_accessor_add_attribute(the_clone,attribute,0);
        i++;
    }

    return the_clone;
}

void accessor_bufr_data_element_set_index(grib_accessor* a,long index)
{
    grib_accessor_bufr_data_element* self = (grib_accessor_bufr_data_element*)a;
    self->index=index;
}

void accessor_bufr_data_element_set_type(grib_accessor* a,int type)
{
    grib_accessor_bufr_data_element* self = (grib_accessor_bufr_data_element*)a;
    self->type=type;
}

void accessor_bufr_data_element_set_numberOfSubsets(grib_accessor* a,long numberOfSubsets)
{
    grib_accessor_bufr_data_element* self = (grib_accessor_bufr_data_element*)a;
    self->numberOfSubsets=numberOfSubsets;
}

void accessor_bufr_data_element_set_subsetNumber(grib_accessor* a,long subsetNumber)
{
    grib_accessor_bufr_data_element* self = (grib_accessor_bufr_data_element*)a;
    self->subsetNumber=subsetNumber;
}

void accessor_bufr_data_element_set_compressedData(grib_accessor* a,int compressedData)
{
    grib_accessor_bufr_data_element* self = (grib_accessor_bufr_data_element*)a;
    self->compressedData=compressedData;
}

void accessor_bufr_data_element_set_descriptors(grib_accessor* a,bufr_descriptors_array* descriptors)
{
    grib_accessor_bufr_data_element* self = (grib_accessor_bufr_data_element*)a;
    self->descriptors=descriptors;
}

void accessor_bufr_data_element_set_numericValues(grib_accessor* a,grib_vdarray* numericValues)
{
    grib_accessor_bufr_data_element* self = (grib_accessor_bufr_data_element*)a;
    self->numericValues=numericValues;
}

void accessor_bufr_data_element_set_stringValues(grib_accessor* a,grib_vsarray* stringValues)
{
    grib_accessor_bufr_data_element* self = (grib_accessor_bufr_data_element*)a;
    self->stringValues=stringValues;
}

void accessor_bufr_data_element_set_elementsDescriptorsIndex(grib_accessor* a,grib_viarray* elementsDescriptorsIndex)
{
    grib_accessor_bufr_data_element* self = (grib_accessor_bufr_data_element*)a;
    self->elementsDescriptorsIndex=elementsDescriptorsIndex;
}

static void init(grib_accessor* a, const long len, grib_arguments* params)
{

    a->length = 0;
    a->flags |= GRIB_ACCESSOR_FLAG_BUFR_DATA;
    /* a->flags |= GRIB_ACCESSOR_FLAG_READ_ONLY; */
}

static void dump(grib_accessor* a, grib_dumper* dumper)
{
    int type=get_native_type(a);

    switch (type) {
    case GRIB_TYPE_LONG:
        grib_dump_long(dumper,a,NULL);
        break;
    case GRIB_TYPE_DOUBLE:
        grib_dump_values(dumper,a);
        break;
    case GRIB_TYPE_STRING:
        grib_dump_string_array(dumper,a,NULL);
        break;
    }
}

static int unpack_string_array (grib_accessor* a, char** val, size_t *len)
{
    grib_accessor_bufr_data_element* self = (grib_accessor_bufr_data_element*)a;

    int ret=0,i,idx;
    long count=0;
    grib_context* c=a->context;

    if (self->compressedData) {
        DebugAssert(self->index < self->numericValues->n);
        idx=((int)self->numericValues->v[self->index]->v[0]/1000-1)/self->numberOfSubsets;
        DebugAssert(idx < self->stringValues->n);
        count=grib_sarray_used_size(self->stringValues->v[idx]);
        for (i=0;i<count;i++) {
            val[i]=grib_context_strdup(c,self->stringValues->v[idx]->v[i]);
        }
        *len=count;
    } else {
        DebugAssert(self->subsetNumber < self->numericValues->n);
        DebugAssert(self->index < self->numericValues->v[self->subsetNumber]->n);
        idx=(int)self->numericValues->v[self->subsetNumber]->v[self->index]/1000-1;
        val[0]=grib_context_strdup(c,self->stringValues->v[idx]->v[0]);
        *len=1;
    }

    return ret;
}

static int pack_string_array(grib_accessor*a , const char**  v, size_t *len)
{
    grib_accessor_bufr_data_element* self = (grib_accessor_bufr_data_element*)a;

    int ret=0,i,idx;
    char* s=NULL;
    grib_context* c=a->context;

    if (self->compressedData) {
        idx=((int)self->numericValues->v[self->index]->v[0]/1000-1)/self->numberOfSubsets;
        if (*len!=1 && *len!=self->numberOfSubsets) {
            grib_context_log(c,GRIB_LOG_ERROR,"Number of values mismatch for %s: are %ld should be %ld",
                    self->descriptors->v[self->elementsDescriptorsIndex->v[0]->v[idx]]->shortName,*len,self->numberOfSubsets);
            return GRIB_ARRAY_TOO_SMALL;
        }
        grib_sarray_delete(c,self->stringValues->v[idx]);
        self->stringValues->v[idx]=grib_sarray_new(c,*len,1);
        for (i=0;i<*len;i++) {
            s=grib_context_strdup(c,v[i]);
            grib_sarray_push(c,self->stringValues->v[idx],s);
        }
    } else {
        ret=GRIB_NOT_IMPLEMENTED;
    }

    return ret;
}

static int unpack_string (grib_accessor* a, char* val, size_t *len)
{
    grib_accessor_bufr_data_element* self = (grib_accessor_bufr_data_element*)a;
    char* str=NULL;
    char* p=0;
    size_t slen=0;
    char sval[100]={0,};
    double dval=0;
    size_t dlen=1;

    int ret=0,idx;
    grib_context* c=a->context;

    if (self->type != BUFR_DESCRIPTOR_TYPE_STRING) {
        unpack_double(a,&dval,&dlen);
        sprintf(sval,"%g",dval);
        slen=strlen(sval);
        if (*len < slen) return GRIB_ARRAY_TOO_SMALL;
        strcpy(val,sval);
        return GRIB_SUCCESS;
    }

    if (self->compressedData) {
        DebugAssert(self->index < self->numericValues->n);
        idx=((int)self->numericValues->v[self->index]->v[0]/1000-1)/self->numberOfSubsets;
        if (idx < 0) return GRIB_INTERNAL_ERROR;
        str=grib_context_strdup(c,self->stringValues->v[idx]->v[0]);
    } else {
        DebugAssert(self->subsetNumber < self->numericValues->n);
        idx=(int)self->numericValues->v[self->subsetNumber]->v[self->index]/1000-1;
        if (idx < 0) return GRIB_INTERNAL_ERROR;
        DebugAssert(idx < self->stringValues->n);
        str=grib_context_strdup(c,self->stringValues->v[idx]->v[0]);
    }

    if (str==0 || strlen(str)==0) {
        *len=0;
        *val=0;
        return ret;
    }

    p=str;
    while (*p!=0) p++;
    p--;
    while (p!=str) {
        if (*p!=' ') break;
        else *p=0;
        p--;
    }
    slen=strlen(str);
    if (slen>*len) return GRIB_ARRAY_TOO_SMALL;

    strcpy(val,str);
    grib_context_free(c,str);
    *len=slen;

    return ret;
}

static int pack_string(grib_accessor* a, const char* val, size_t *len)
{
    grib_accessor_bufr_data_element* self = (grib_accessor_bufr_data_element*)a;

    int ret=0,idx;
    char* s=NULL;
    grib_context* c=a->context;

    if (self->compressedData) {
      idx=((int)self->numericValues->v[self->index]->v[0]/1000-1)/self->numberOfSubsets;
    } else {
      idx=(int)self->numericValues->v[self->subsetNumber]->v[self->index]/1000-1;
    }
    grib_sarray_delete(c,self->stringValues->v[idx]);
    self->stringValues->v[idx]=grib_sarray_new(c,1,1);
    s=grib_context_strdup(c,val);
    grib_sarray_push(c,self->stringValues->v[idx],s);

    return ret;
}

static int unpack_long (grib_accessor* a, long* val, size_t *len)
{
    grib_accessor_bufr_data_element* self = (grib_accessor_bufr_data_element*)a;
    int ret=0,i;
    long count=0;

    value_count(a,&count);

    if (*len<count) return GRIB_ARRAY_TOO_SMALL;

    if (self->compressedData) {
        for (i=0;i<count;i++) {
            DebugAssert(self->index < self->numericValues->n);
            DebugAssert(i < self->numericValues->v[self->index]->n);
            val[i]= self->numericValues->v[self->index]->v[i] == GRIB_MISSING_DOUBLE ?
                    GRIB_MISSING_LONG : (long)self->numericValues->v[self->index]->v[i];
        }
        *len=count;
    } else {
        DebugAssert(self->subsetNumber < self->numericValues->n);
        DebugAssert(self->index < self->numericValues->v[self->subsetNumber]->n);
        val[0]= self->numericValues->v[self->subsetNumber]->v[self->index] == GRIB_MISSING_DOUBLE ?
                GRIB_MISSING_LONG : (long)self->numericValues->v[self->subsetNumber]->v[self->index];
        *len=1;
    }

    return ret;
}

static int unpack_double (grib_accessor* a, double* val, size_t *len)
{
    grib_accessor_bufr_data_element* self = (grib_accessor_bufr_data_element*)a;
    int ret=0,i;
    long count=0;

    value_count(a,&count);

    if (*len<count) return GRIB_ARRAY_TOO_SMALL;

    if (self->compressedData) {
        for (i=0;i<count;i++) {
            DebugAssert(self->index < self->numericValues->n);
            DebugAssert(i < self->numericValues->v[self->index]->n);
            val[i]=self->numericValues->v[self->index]->v[i];
        }
        *len=count;
    } else {
        DebugAssert(self->subsetNumber < self->numericValues->n);
        DebugAssert(self->index < self->numericValues->v[self->subsetNumber]->n);
        val[0]=self->numericValues->v[self->subsetNumber]->v[self->index];
        *len=1;
    }

    return ret;
}

static int pack_double(grib_accessor* a, const double* val, size_t *len)
{
    grib_accessor_bufr_data_element* self = (grib_accessor_bufr_data_element*)a;
    int ret=0,i;
    long count=1;
    grib_context* c=a->context;

    if (self->compressedData) {
        count=*len;
        if (count!=1 && count!=self->numberOfSubsets) {
            grib_context_log(c,GRIB_LOG_ERROR,"Number of values mismatch for %s: are %ld should be %ld",
                    self->descriptors->v[self->elementsDescriptorsIndex->v[0]->v[self->index]]->shortName,count,self->numberOfSubsets);
            return GRIB_ARRAY_TOO_SMALL;
        }
        grib_darray_delete(a->context,self->numericValues->v[self->index]);
        self->numericValues->v[self->index]=grib_darray_new(a->context,count,1);

        for (i=0;i<count;i++)
            grib_darray_push(a->context,self->numericValues->v[self->index],val[i]);

        *len=count;
    } else {
        self->numericValues->v[self->subsetNumber]->v[self->index]=val[0];
        *len=1;
    }

    return ret;
}

static int pack_long(grib_accessor* a, const long* val, size_t *len)
{
    grib_accessor_bufr_data_element* self = (grib_accessor_bufr_data_element*)a;
    int ret=0,i;
    long count=1;
    grib_context* c=a->context;

    if (self->compressedData) {
        count=*len;
        if (count!=1 && count!=self->numberOfSubsets) {
            grib_context_log(c,GRIB_LOG_ERROR,"Number of values mismatch for %s: are %ld should be %ld",
                    self->descriptors->v[self->elementsDescriptorsIndex->v[0]->v[self->index]]->shortName,count,self->numberOfSubsets);
            return GRIB_ARRAY_TOO_SMALL;
        }
        grib_darray_delete(a->context,self->numericValues->v[self->index]);
        self->numericValues->v[self->index]=grib_darray_new(a->context,count,1);

        for (i=0;i<count;i++) {
            grib_darray_push(a->context,self->numericValues->v[self->index], val[i] ==  GRIB_MISSING_LONG ? GRIB_MISSING_DOUBLE : val[i]);
        }
        *len=count;
    } else {
        self->numericValues->v[self->subsetNumber]->v[self->index] = val[0] ==  GRIB_MISSING_LONG ? GRIB_MISSING_DOUBLE : val[0];
        *len=1;
    }

    return ret;
}

static int value_count(grib_accessor* a,long* count)
{
    int ret=0,size,type,idx;
    grib_accessor_bufr_data_element* self = (grib_accessor_bufr_data_element*)a;

    if (!self->compressedData) {
        *count=1;
        return 0;
    }
    type=get_native_type(a);

    if (type==GRIB_TYPE_STRING) {
        DebugAssert(self->index < self->numericValues->n);
        idx=((int)self->numericValues->v[self->index]->v[0]/1000-1)/self->numberOfSubsets;
        size=grib_sarray_used_size(self->stringValues->v[idx]);
    } else {
        DebugAssert(self->index < self->numericValues->n);
        size=grib_darray_used_size(self->numericValues->v[self->index]);
    }

    *count = size == 1 ? 1 : self->numberOfSubsets;

    return ret;
}

static int unpack_double_element(grib_accessor* a, size_t idx, double* val)
{
    /* ECC-415 */
    grib_accessor_bufr_data_element* self = (grib_accessor_bufr_data_element*)a;
    int ret = GRIB_SUCCESS;
    long count = 0;

    value_count(a, &count);
    if (idx >= count) {
        return GRIB_INTERNAL_ERROR;
    }

    if (self->compressedData) {
        *val = self->numericValues->v[self->index]->v[idx];
    } else {
        ret = GRIB_NOT_IMPLEMENTED;
    }
    return ret;
}

static int  get_native_type(grib_accessor* a)
{
    grib_accessor_bufr_data_element* self = (grib_accessor_bufr_data_element*)a;
    int ret=GRIB_TYPE_DOUBLE;

    switch (self->type) {
    case BUFR_DESCRIPTOR_TYPE_STRING:
        ret=GRIB_TYPE_STRING;
        break;
    case BUFR_DESCRIPTOR_TYPE_DOUBLE:
        ret=GRIB_TYPE_DOUBLE;
        break;
    case BUFR_DESCRIPTOR_TYPE_LONG:
        ret=GRIB_TYPE_LONG;
        break;
    case BUFR_DESCRIPTOR_TYPE_TABLE:
        ret=GRIB_TYPE_LONG;
        break;
    case BUFR_DESCRIPTOR_TYPE_FLAG:
        ret=GRIB_TYPE_LONG;
        break;
    }

    return ret;
}

static void destroy(grib_context* ct, grib_accessor* a)
{
    int i=0;
    while (i<MAX_ACCESSOR_ATTRIBUTES && a->attributes[i]) {
        /*grib_context_log(ct,GRIB_LOG_DEBUG,"deleting attribute %s->%s",a->name,a->attributes[i]->name);*/
        /* printf("+++++ %s\n",a->attributes[i]->name); */
        grib_accessor_delete(ct,a->attributes[i]);
        a->attributes[i]=NULL;
        i++;
    }
}
