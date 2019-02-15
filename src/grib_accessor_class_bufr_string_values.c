/*
* Copyright 2005-2018 ECMWF.
*
* This software is licensed under the terms of the Apache Licence Version 2.0
* which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
*
* In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
* virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
*/

/* Sequences and replication (not delayed) are resolved in this class.
   Number of elements to which a delayed replication applies are recomputed to
   take account of the expansion.
   Expanded descriptors cannot contain sequences and only delayed replication
   can appear
*/

#include "grib_api_internal.h"
/*
This is used by make_class.pl

START_CLASS_DEF
CLASS      = accessor
SUPER      = grib_accessor_class_ascii
IMPLEMENTS = unpack_string_array; pack_string_array
IMPLEMENTS = unpack_string; pack_string
IMPLEMENTS = init;dump;destroy
IMPLEMENTS = value_count
MEMBERS    = const char* dataAccessorName
MEMBERS    = grib_accessor* dataAccessor

END_CLASS_DEF

*/

/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "accessor.class" and rerun ./make_class.pl

*/

static int pack_string(grib_accessor*, const char*, size_t *len);
static int pack_string_array(grib_accessor*, const char**, size_t *len);
static int unpack_string (grib_accessor*, char*, size_t *len);
static int unpack_string_array (grib_accessor*, char**, size_t *len);
static int value_count(grib_accessor*,long*);
static void destroy(grib_context*,grib_accessor*);
static void dump(grib_accessor*, grib_dumper*);
static void init(grib_accessor*,const long, grib_arguments* );
static void init_class(grib_accessor_class*);

typedef struct grib_accessor_bufr_string_values {
    grib_accessor          att;
/* Members defined in gen */
/* Members defined in ascii */
/* Members defined in bufr_string_values */
	const char* dataAccessorName;
	grib_accessor* dataAccessor;
} grib_accessor_bufr_string_values;

extern grib_accessor_class* grib_accessor_class_ascii;

static grib_accessor_class _grib_accessor_class_bufr_string_values = {
    &grib_accessor_class_ascii,                      /* super                     */
    "bufr_string_values",                      /* name                      */
    sizeof(grib_accessor_bufr_string_values),  /* size                      */
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
    0,            /* get native type               */
    0,                /* get sub_section                */
    0,               /* grib_pack procedures long      */
    0,                 /* grib_pack procedures long      */
    0,                  /* grib_pack procedures long      */
    0,                /* grib_unpack procedures long    */
    0,                /* grib_pack procedures double    */
    0,              /* grib_unpack procedures double  */
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
    0,     /* unpack only ith value          */
    0,     /* unpack a subarray         */
    0,              		/* clear          */
    0,               		/* clone accessor          */
};


grib_accessor_class* grib_accessor_class_bufr_string_values = &_grib_accessor_class_bufr_string_values;


static void init_class(grib_accessor_class* c)
{
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
	c->pack_double	=	(*(c->super))->pack_double;
	c->unpack_double	=	(*(c->super))->unpack_double;
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

static void init(grib_accessor* a, const long len , grib_arguments* args )
{
    grib_accessor_bufr_string_values* self = (grib_accessor_bufr_string_values*)a;
    int n=0;
    self->dataAccessorName=grib_arguments_get_name(grib_handle_of_accessor(a),args,n++);
    self->dataAccessor=NULL;
    a->length = 0;
    a->flags |= GRIB_ACCESSOR_FLAG_READ_ONLY;
}

static void dump(grib_accessor* a, grib_dumper* dumper)
{
    grib_dump_string_array(dumper,a,NULL);
}

static grib_accessor* get_accessor(grib_accessor* a)
{
    grib_accessor_bufr_string_values* self = (grib_accessor_bufr_string_values*)a;
    if (!self->dataAccessor) {
        self->dataAccessor=grib_find_accessor(grib_handle_of_accessor(a),self->dataAccessorName);
    }
    return self->dataAccessor;
}

static int unpack_string_array (grib_accessor* a, char** buffer, size_t *len)
{
    grib_accessor* data=0;
    grib_context* c=a->context;
    grib_vsarray* stringValues=NULL;
    long l=0,n=0,tl;
    size_t i,j;
    /*char buf[25]={0,};*/
    char** b=buffer;

    data=get_accessor(a);
    if (!data) return GRIB_NOT_FOUND;

    stringValues=accessor_bufr_data_array_get_stringValues(data);

    n=grib_vsarray_used_size(stringValues);

    tl=0;
    for (j=0;j<n;j++) {
        l=grib_sarray_used_size(stringValues->v[j]);
        tl+=l;

        if (tl>*len) return GRIB_ARRAY_TOO_SMALL;

        for (i=0;i<l;i++) {
            *(b++)=grib_context_strdup(c,stringValues->v[j]->v[i]);
        }
    }
    *len=tl;

    return GRIB_SUCCESS;
}

static int pack_string_array(grib_accessor*a , const char**  v, size_t *len)
{
    return GRIB_NOT_IMPLEMENTED;
}

static int unpack_string(grib_accessor* a, char* val, size_t *len)
{
    return GRIB_NOT_IMPLEMENTED;
}

static int pack_string(grib_accessor* a, const char* val, size_t *len)
{
    return GRIB_NOT_IMPLEMENTED;
}

static int value_count(grib_accessor* a,long* rlen)
{
    grib_accessor* descriptors=get_accessor(a);
    return grib_value_count(descriptors,rlen);
}

static void destroy(grib_context* c,grib_accessor* a)
{
}
