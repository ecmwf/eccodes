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
#include "md5.h"
/*
   This is used by make_class.pl

   START_CLASS_DEF
   CLASS      = accessor
   SUPER      = grib_accessor_class_gen
   IMPLEMENTS = get_native_type;init
   IMPLEMENTS = compare;unpack_string;value_count;destroy
   MEMBERS = const char* offset
   MEMBERS = grib_expression* length
   MEMBERS = grib_string_list* blacklist
   END_CLASS_DEF

 */

/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "accessor.class" and rerun ./make_class.pl

*/

static int  get_native_type(grib_accessor*);
static int unpack_string (grib_accessor*, char*, size_t *len);
static int value_count(grib_accessor*,long*);
static void destroy(grib_context*,grib_accessor*);
static void init(grib_accessor*,const long, grib_arguments* );
static void init_class(grib_accessor_class*);
static int compare(grib_accessor*, grib_accessor*);

typedef struct grib_accessor_md5 {
    grib_accessor          att;
/* Members defined in gen */
/* Members defined in md5 */
	const char* offset;
	grib_expression* length;
	grib_string_list* blacklist;
} grib_accessor_md5;

extern grib_accessor_class* grib_accessor_class_gen;

static grib_accessor_class _grib_accessor_class_md5 = {
    &grib_accessor_class_gen,                      /* super                     */
    "md5",                      /* name                      */
    sizeof(grib_accessor_md5),  /* size                      */
    0,                           /* inited */
    &init_class,                 /* init_class */
    &init,                       /* init                      */
    0,                  /* post_init                      */
    &destroy,                    /* free mem                       */
    0,                       /* describes himself         */
    0,                /* get length of section     */
    0,              /* get length of string      */
    &value_count,                /* get number of values      */
    0,                 /* get number of bytes      */
    0,                /* get offset to bytes           */
    &get_native_type,            /* get native type               */
    0,                /* get sub_section                */
    0,               /* grib_pack procedures long      */
    0,                 /* grib_pack procedures long      */
    0,                  /* grib_pack procedures long      */
    0,                /* grib_unpack procedures long    */
    0,                /* grib_pack procedures double    */
    0,              /* grib_unpack procedures double  */
    0,                /* grib_pack procedures string    */
    &unpack_string,              /* grib_unpack procedures string  */
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


grib_accessor_class* grib_accessor_class_md5 = &_grib_accessor_class_md5;


static void init_class(grib_accessor_class* c)
{
	c->dump	=	(*(c->super))->dump;
	c->next_offset	=	(*(c->super))->next_offset;
	c->string_length	=	(*(c->super))->string_length;
	c->byte_count	=	(*(c->super))->byte_count;
	c->byte_offset	=	(*(c->super))->byte_offset;
	c->sub_section	=	(*(c->super))->sub_section;
	c->pack_missing	=	(*(c->super))->pack_missing;
	c->is_missing	=	(*(c->super))->is_missing;
	c->pack_long	=	(*(c->super))->pack_long;
	c->unpack_long	=	(*(c->super))->unpack_long;
	c->pack_double	=	(*(c->super))->pack_double;
	c->unpack_double	=	(*(c->super))->unpack_double;
	c->pack_string	=	(*(c->super))->pack_string;
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

static void init(grib_accessor* a, const long len , grib_arguments* arg )
{
    grib_accessor_md5* self = (grib_accessor_md5*)a;
    char* b=0;
    int n=0;
    grib_string_list* current=0;
    grib_context* context=a->context;

    self->offset = grib_arguments_get_name(grib_handle_of_accessor(a),arg,n++);
    self->length = grib_arguments_get_expression(grib_handle_of_accessor(a),arg,n++);
    self->blacklist=0;
    while ( (b=(char*)grib_arguments_get_name(grib_handle_of_accessor(a),arg,n++)) !=NULL) {
        if (! self->blacklist) {
            self->blacklist=(grib_string_list*)grib_context_malloc_clear(context,sizeof(grib_string_list));
            self->blacklist->value=grib_context_strdup(context,b);
            current=self->blacklist;
        } else {
            current->next=(grib_string_list*)grib_context_malloc_clear(context,sizeof(grib_string_list));
            current->next->value=grib_context_strdup(context,b);
            current=current->next;
        }
    }
    a->length = 0;
    a->flags |= GRIB_ACCESSOR_FLAG_READ_ONLY;
    a->flags |= GRIB_ACCESSOR_FLAG_EDITION_SPECIFIC;
}

static int  get_native_type(grib_accessor* a)
{
    return GRIB_TYPE_STRING;
}

static int compare(grib_accessor* a, grib_accessor* b)
{
    int retval=GRIB_SUCCESS;

    size_t alen = 0;
    size_t blen = 0;
    int err=0;
    long count=0;

    err=grib_value_count(a,&count);
    if (err) return err;
    alen=count;

    err=grib_value_count(b,&count);
    if (err) return err;
    blen=count;

    if (alen != blen) return GRIB_COUNT_MISMATCH;

    return retval;
}

static int unpack_string(grib_accessor*a , char*  v, size_t *len)
{
    grib_accessor_md5* self = (grib_accessor_md5*)a;
    unsigned mess_len;
    unsigned char* mess;
    unsigned char* p;
    long offset,length;
    grib_string_list* blacklist=NULL;
    grib_accessor* b=NULL;
    int ret=0;
    int i=0;
    struct grib_md5_state md5c;

    if (*len <32 ) {
        grib_context_log(a->context,GRIB_LOG_ERROR,"md5: array too small");
        return GRIB_ARRAY_TOO_SMALL;
    }

    if((ret = grib_get_long_internal(grib_handle_of_accessor(a),self->offset,&offset))
            != GRIB_SUCCESS)
        return ret;
    if((ret = grib_expression_evaluate_long(grib_handle_of_accessor(a),self->length,&length))
            != GRIB_SUCCESS)
        return ret;

    mess=(unsigned char*)grib_context_malloc(a->context,length);
    memcpy(mess,grib_handle_of_accessor(a)->buffer->data+offset,length);
    mess_len=length;

    blacklist=a->context->blacklist;
    /* passed blacklist overrides context blacklist.
     Consider to modify following line to extend context blacklist.
     */
    if (self->blacklist) blacklist=self->blacklist;
    while (blacklist && blacklist->value) {
        b=grib_find_accessor(grib_handle_of_accessor(a),blacklist->value);
        if (!b) {
            grib_context_free(a->context,mess);
            return GRIB_NOT_FOUND;
        }

        p=mess+b->offset-offset;
        for (i=0;i<b->length;i++) *(p++)=0;

        blacklist=blacklist->next;
    }

    grib_md5_init(&md5c);
    grib_md5_add(&md5c,mess,mess_len);
    grib_md5_end(&md5c,v);
    grib_context_free(a->context,mess);

    return ret;
}

static void destroy(grib_context* c,grib_accessor* a)
{
    grib_accessor_md5* self = (grib_accessor_md5*)a;
    if (self->blacklist) {
        grib_string_list* next=self->blacklist;
        grib_string_list* cur=self->blacklist;
        while(next) {
            cur=next;
            next=next->next;
            grib_context_free(c,cur->value);
            grib_context_free(c,cur);
        }
    }
}

static int value_count(grib_accessor* a,long* count)
{
    *count=16;
    return 0;
}
