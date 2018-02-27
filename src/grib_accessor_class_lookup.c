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
#include <ctype.h>
/*
   This is used by make_class.pl

   START_CLASS_DEF
   CLASS      = accessor
   SUPER      = grib_accessor_class_long
   IMPLEMENTS = unpack_long; unpack_string
   IMPLEMENTS = init;post_init;dump
   IMPLEMENTS = byte_offset;byte_count
   IMPLEMENTS = pack_long
   IMPLEMENTS = notify_change
   MEMBERS    = long llength
   MEMBERS    = long loffset
   MEMBERS    = grib_expression* real_name
   END_CLASS_DEF

 */

/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "accessor.class" and rerun ./make_class.pl

*/

static int pack_long(grib_accessor*, const long* val,size_t *len);
static int unpack_long(grib_accessor*, long* val,size_t *len);
static int unpack_string (grib_accessor*, char*, size_t *len);
static long byte_count(grib_accessor*);
static long byte_offset(grib_accessor*);
static void dump(grib_accessor*, grib_dumper*);
static void init(grib_accessor*,const long, grib_arguments* );
static void post_init(grib_accessor*);
static void init_class(grib_accessor_class*);
static int notify_change(grib_accessor*,grib_accessor*);

typedef struct grib_accessor_lookup {
    grib_accessor          att;
/* Members defined in gen */
/* Members defined in long */
/* Members defined in lookup */
	long llength;
	long loffset;
	grib_expression* real_name;
} grib_accessor_lookup;

extern grib_accessor_class* grib_accessor_class_long;

static grib_accessor_class _grib_accessor_class_lookup = {
    &grib_accessor_class_long,                      /* super                     */
    "lookup",                      /* name                      */
    sizeof(grib_accessor_lookup),  /* size                      */
    0,                           /* inited */
    &init_class,                 /* init_class */
    &init,                       /* init                      */
    &post_init,                  /* post_init                      */
    0,                    /* free mem                       */
    &dump,                       /* describes himself         */
    0,                /* get length of section     */
    0,              /* get length of string      */
    0,                /* get number of values      */
    &byte_count,                 /* get number of bytes      */
    &byte_offset,                /* get offset to bytes           */
    0,            /* get native type               */
    0,                /* get sub_section                */
    0,               /* grib_pack procedures long      */
    0,                 /* grib_pack procedures long      */
    &pack_long,                  /* grib_pack procedures long      */
    &unpack_long,                /* grib_unpack procedures long    */
    0,                /* grib_pack procedures double    */
    0,              /* grib_unpack procedures double  */
    0,                /* grib_pack procedures string    */
    &unpack_string,              /* grib_unpack procedures string  */
    0,          /* grib_pack array procedures string    */
    0,        /* grib_unpack array procedures string  */
    0,                 /* grib_pack procedures bytes     */
    0,               /* grib_unpack procedures bytes   */
    0,            /* pack_expression */
    &notify_change,              /* notify_change   */
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


grib_accessor_class* grib_accessor_class_lookup = &_grib_accessor_class_lookup;


static void init_class(grib_accessor_class* c)
{
	c->next_offset	=	(*(c->super))->next_offset;
	c->string_length	=	(*(c->super))->string_length;
	c->value_count	=	(*(c->super))->value_count;
	c->get_native_type	=	(*(c->super))->get_native_type;
	c->sub_section	=	(*(c->super))->sub_section;
	c->pack_missing	=	(*(c->super))->pack_missing;
	c->is_missing	=	(*(c->super))->is_missing;
	c->pack_double	=	(*(c->super))->pack_double;
	c->unpack_double	=	(*(c->super))->unpack_double;
	c->pack_string	=	(*(c->super))->pack_string;
	c->pack_string_array	=	(*(c->super))->pack_string_array;
	c->unpack_string_array	=	(*(c->super))->unpack_string_array;
	c->pack_bytes	=	(*(c->super))->pack_bytes;
	c->unpack_bytes	=	(*(c->super))->unpack_bytes;
	c->pack_expression	=	(*(c->super))->pack_expression;
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

static void init(grib_accessor* a, const long len, grib_arguments *arg )
{
    grib_accessor_lookup* self = (grib_accessor_lookup*)a;
    a->length = 0;
    self->llength = len;
    self->loffset = grib_arguments_get_long(grib_handle_of_accessor(a),arg,0);
    a->flags |= GRIB_ACCESSOR_FLAG_READ_ONLY;
    self->real_name = grib_arguments_get_expression(grib_handle_of_accessor(a),arg,1);
}

static void post_init(grib_accessor* a)
{
    grib_accessor_lookup* self = (grib_accessor_lookup*)a;
    if(self->real_name ) {
        grib_dependency_observe_expression(a,self->real_name );
    }
}

static void dump(grib_accessor* a,grib_dumper* dumper)
{
    grib_accessor_lookup* self = (grib_accessor_lookup*)a;
    unsigned char bytes[1024] = {0,};
    char msg[1024]= {0,};
    char buf[1024];
    int i;
    unsigned long v = 0;

    size_t llen = self->llength;
    grib_unpack_bytes(a, bytes, &llen); /* TODO: Unpack byte unpack the wrong offset */

    bytes[llen] = 0;
    for(i = 0; i < llen; i++)
    {
        msg[i] = isprint(bytes[i]) ? bytes[i] : '?';
        v <<= 8;
        v |= bytes[i];
    }

    msg[llen] = 0;

    sprintf(buf,"%s %lu %ld-%ld", msg, v, (long)a->offset+self->loffset, (long)self->llength);

    grib_dump_long(dumper,a,buf);
}

static int unpack_string(grib_accessor*a , char*  v, size_t *len)
{
    grib_accessor_lookup* self = (grib_accessor_lookup*)a;
    unsigned char bytes[1024] = {0,};
    int i;

    size_t llen = self->llength;
    grib_unpack_bytes(a, bytes, &llen); /* TODO: Unpack byte unpack the wrong offset */

    bytes[llen] = 0;

    for(i = 0; i < llen; i++)
    {
        v[i] = isprint(bytes[i]) ? bytes[i] : '?';
    }

    v[llen] = 0;
    if (llen==1 && v[0] == '?') {
        /* Try unpack as long */
        size_t length = 10;
        long lval = 0;
        int err = unpack_long(a, &lval, &length);
        if (!err) {
            char str[5];
            int conv = sprintf(str, "%ld", lval);
            if (conv == 1) {
                v[0] = str[0];
            }
        }
    }

    return GRIB_SUCCESS;
}

static int unpack_long(grib_accessor* a, long* val, size_t *len)
{
    grib_accessor_lookup* al = (grib_accessor_lookup*)a;
    grib_handle *h = grib_handle_of_accessor(a);

    long pos = (a->offset+al->loffset)*8;

    if(len[0] < 1)
    {
        grib_context_log(a->context, GRIB_LOG_ERROR, "Wrong size for %s it contains %d values ", a->name , 1 );
        len[0] = 0;
        return GRIB_ARRAY_TOO_SMALL;
    }

    /* This is used when reparsing or rebuilding */
    if(h->loader) {
        Assert(*len == 1);
        return h->loader->lookup_long(h->context,h->loader,a->name,val);
    }

    val[0] = grib_decode_unsigned_long(h->buffer->data , &pos, al->llength*8);
    len[0] = 1;

    /*printf("###########lookup unpack_long: %s %ld %ld\n",a->name, pos/8, val[0]);*/

    return GRIB_SUCCESS;
}

static int pack_long(grib_accessor* a, const long* val, size_t *len)
{
    return GRIB_NOT_IMPLEMENTED;
}

static long byte_count(grib_accessor* a)
{
    grib_accessor_lookup* al = (grib_accessor_lookup*)a;
    return al->llength;
}

static long byte_offset(grib_accessor* a)
{
    grib_accessor_lookup* al = (grib_accessor_lookup*)a;
    return al->loffset;
}

static int notify_change(grib_accessor* self,grib_accessor* changed)
{
    /* Forward changes */
    return grib_dependency_notify_change(self);
}
