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
#include <limits.h>
/*
   This is used by make_class.pl

   START_CLASS_DEF
   CLASS      = accessor
   SUPER      = grib_accessor_class_gen
   IMPLEMENTS = unpack_double;pack_double
   IMPLEMENTS = unpack_string;pack_string;string_length
   IMPLEMENTS = unpack_long;pack_long;destroy;byte_count
   IMPLEMENTS = init;dump;value_count;get_native_type
   IMPLEMENTS = compare; make_clone
   MEMBERS=double dval
   MEMBERS=char*  cval
   MEMBERS=int    type
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
static int unpack_double(grib_accessor*, double* val,size_t *len);
static int unpack_long(grib_accessor*, long* val,size_t *len);
static int unpack_string (grib_accessor*, char*, size_t *len);
static size_t string_length(grib_accessor*);
static long byte_count(grib_accessor*);
static int value_count(grib_accessor*,long*);
static void destroy(grib_context*,grib_accessor*);
static void dump(grib_accessor*, grib_dumper*);
static void init(grib_accessor*,const long, grib_arguments* );
static void init_class(grib_accessor_class*);
static int compare(grib_accessor*, grib_accessor*);
static grib_accessor* make_clone(grib_accessor*,grib_section*,int*);

typedef struct grib_accessor_variable {
    grib_accessor          att;
/* Members defined in gen */
/* Members defined in variable */
	double dval;
	char*  cval;
	int    type;
} grib_accessor_variable;

extern grib_accessor_class* grib_accessor_class_gen;

static grib_accessor_class _grib_accessor_class_variable = {
    &grib_accessor_class_gen,                      /* super                     */
    "variable",                      /* name                      */
    sizeof(grib_accessor_variable),  /* size                      */
    0,                           /* inited */
    &init_class,                 /* init_class */
    &init,                       /* init                      */
    0,                  /* post_init                      */
    &destroy,                    /* free mem                       */
    &dump,                       /* describes himself         */
    0,                /* get length of section     */
    &string_length,              /* get length of string      */
    &value_count,                /* get number of values      */
    &byte_count,                 /* get number of bytes      */
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
    &make_clone,               		/* clone accessor          */
};


grib_accessor_class* grib_accessor_class_variable = &_grib_accessor_class_variable;


static void init_class(grib_accessor_class* c)
{
	c->next_offset	=	(*(c->super))->next_offset;
	c->byte_offset	=	(*(c->super))->byte_offset;
	c->sub_section	=	(*(c->super))->sub_section;
	c->pack_missing	=	(*(c->super))->pack_missing;
	c->is_missing	=	(*(c->super))->is_missing;
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
}

/* END_CLASS_IMP */

#define MAX_VARIABLE_STRING_LENGTH 255

static void init(grib_accessor* a, const long length , grib_arguments* args )
{
    grib_accessor_variable* self = (grib_accessor_variable*)a;
    grib_handle* hand = grib_handle_of_accessor(a);
    grib_expression *expression = grib_arguments_get_expression(hand,args,0);
    const char* p = 0;
    size_t len = 1;
    long l;
    int ret=0;
    double d;

    a->length = 0;
    if (self->type==GRIB_TYPE_UNDEFINED && expression) {
        self->type = grib_expression_native_type(hand,expression);

        switch(self->type)
        {
            case GRIB_TYPE_DOUBLE:
                grib_expression_evaluate_double(hand,expression,&d);
                pack_double(a,&d,&len);
                break;

            case GRIB_TYPE_LONG:
                grib_expression_evaluate_long(hand,expression,&l);
                pack_long(a,&l,&len);
                break;

            default: {
                char tmp[1024];
                len = sizeof(tmp);
                p = grib_expression_evaluate_string(hand,expression,tmp,&len,&ret);
                if (ret != GRIB_SUCCESS) {
                    grib_context_log(a->context,GRIB_LOG_ERROR,"unable to evaluate %s as string",a->name);
                    Assert(0);
                }
                len = strlen(p)+1;
                pack_string(a,p,&len);
                break;
            }
        }
    }
}

void accessor_variable_set_type(grib_accessor* a,int type) {
    grib_accessor_variable *self = (grib_accessor_variable*)a;
    self->type=type;
}

static void dump(grib_accessor* a, grib_dumper* dumper)
{
    grib_accessor_variable *self = (grib_accessor_variable*)a;
    switch(self->type)
    {
    case GRIB_TYPE_DOUBLE:
        grib_dump_double(dumper,a,NULL);
        break;

    case GRIB_TYPE_LONG:
        grib_dump_long(dumper,a,NULL);
        break;

    default:
        grib_dump_string(dumper,a,NULL);
        break;
    }
}

static int pack_double(grib_accessor* a, const double* val, size_t *len)
{
    grib_accessor_variable *self = (grib_accessor_variable*)a;

    if(*len != 1) {
        grib_context_log(a->context, GRIB_LOG_ERROR, "Wrong size for %s it contains %d values ", a->name , 1 );
        *len = 1;
        return GRIB_ARRAY_TOO_SMALL;
    }

    self->dval = *val;
    if ( *val < (double) LONG_MIN || *val > (double) LONG_MAX )
        self->type = GRIB_TYPE_DOUBLE;
    else
        self->type = ((long)*val == *val) ? GRIB_TYPE_LONG : GRIB_TYPE_DOUBLE;

    return GRIB_SUCCESS;
}

static int pack_long(grib_accessor* a, const long* val, size_t *len)
{
    grib_accessor_variable *self = (grib_accessor_variable*)a;

    if(*len != 1) {
        grib_context_log(a->context, GRIB_LOG_ERROR, "Wrong size for %s it contains %d values ", a->name , 1 );
        *len = 1;
        return GRIB_ARRAY_TOO_SMALL;
    }

    self->dval = *val;
    self->type = GRIB_TYPE_LONG;

    return GRIB_SUCCESS;
}

static int unpack_double(grib_accessor* a, double* val, size_t *len)
{
    grib_accessor_variable *ac = (grib_accessor_variable*)a;

    if(*len < 1) {
        grib_context_log(a->context, GRIB_LOG_ERROR, "Wrong size for %s it contains %d values ", a->name , 1 );
        *len = 0;
        return GRIB_ARRAY_TOO_SMALL;
    }
    *val = ac->dval;
    *len = 1;
    return GRIB_SUCCESS;

}
static int unpack_long(grib_accessor* a, long* val, size_t *len)
{
    grib_accessor_variable *ac = (grib_accessor_variable*)a;

    if(*len < 1) {
        grib_context_log(a->context, GRIB_LOG_ERROR, "Wrong size for %s it contains %d values ", a->name , 1 );
        *len = 0;
        return GRIB_ARRAY_TOO_SMALL;
    }
    *val = (long)ac->dval;
    *len = 1;
    return GRIB_SUCCESS;

}

static int get_native_type(grib_accessor* a)
{
    grib_accessor_variable *self = (grib_accessor_variable*)a;
    return self->type;
}

static void destroy(grib_context* c,grib_accessor* a)
{
    grib_accessor_variable *self = (grib_accessor_variable*)a;
    grib_context_free(c,self->cval);
}

static int unpack_string(grib_accessor* a, char* val, size_t *len){
    grib_accessor_variable *self = (grib_accessor_variable*)a;

    char buf[80];
    char *p = buf;
    size_t slen ;

    if(self->type == GRIB_TYPE_STRING) {
        p = self->cval;
    } else {
        sprintf(p,"%g",self->dval);
    }

    slen = strlen(p) +1;
    if(*len < slen)
    {
        grib_context_log(a->context, GRIB_LOG_ERROR, "Variable unpack_string Wrong size for %s it is %d bytes big (len=%d)", a->name , slen ,*len);
        *len = slen;
        return GRIB_BUFFER_TOO_SMALL;
    }
    strcpy(val,p);
    *len = slen;

    return GRIB_SUCCESS;
}

static int pack_string(grib_accessor* a, const char* val, size_t *len)
{
    grib_accessor_variable *self = (grib_accessor_variable*)a;
    grib_context *c = a->context;

    grib_context_free(c,self->cval);
    self->cval = grib_context_strdup(c,val);
    self->dval = atof(val);
    self->type = GRIB_TYPE_STRING;
    return GRIB_SUCCESS;
}

static int value_count(grib_accessor* a,long* count)
{
    *count=1;
    return 0;
}

static size_t string_length(grib_accessor* a)
{
    grib_accessor_variable *self = (grib_accessor_variable*)a;
    if(self->type == GRIB_TYPE_STRING)
        return strlen(self->cval);
    else
        return MAX_VARIABLE_STRING_LENGTH;
}

static long byte_count(grib_accessor* a)
{
    return a->length;
}

/* NOT ANY MORE
static long byte_count(grib_accessor* a) {
  grib_accessor_variable *self = (grib_accessor_variable*)a;
  char buf[80]={0,};

  if(self->type == GRIB_TYPE_STRING) {
    return strlen(self->cval) +1;
  } else {
    sprintf(buf,"%g",self->dval);
	printf("========> \"%s\"\n",buf);
    return strlen(buf)+1;
  }
}
 */

static int compare(grib_accessor* a, grib_accessor* b)
{
    int retval=0;
    double *aval=0;
    double *bval=0;

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

    aval=(double*)grib_context_malloc(a->context,alen*sizeof(double));
    bval=(double*)grib_context_malloc(b->context,blen*sizeof(double));

    grib_unpack_double(a,aval,&alen);
    grib_unpack_double(b,bval,&blen);

    retval = GRIB_SUCCESS;
    while (alen != 0) {
        if (*bval != *aval) retval = GRIB_DOUBLE_VALUE_MISMATCH;
        alen--;
    }

    grib_context_free(a->context,aval);
    grib_context_free(b->context,bval);

    return retval;
}

static grib_accessor* make_clone(grib_accessor* a,grib_section* s,int* err)
{
    grib_accessor* the_clone=NULL;
    grib_accessor_variable *self = (grib_accessor_variable*)a;
    grib_accessor_variable* variableAccessor=NULL;
    grib_action creator = {0, };
    creator.op         = "variable";
    creator.name_space = "";
    creator.set        = 0;

    creator.name=grib_context_strdup(a->context,a->name);
    the_clone=grib_accessor_factory(s, &creator, 0, NULL);
    the_clone->parent=NULL;
    the_clone->h=s->h;
    the_clone->flags=a->flags;
    variableAccessor=(grib_accessor_variable*)the_clone;

    *err=0;
    variableAccessor->type=self->type;
    if(self->type == GRIB_TYPE_STRING && self->cval!=NULL) {
        variableAccessor->cval=grib_context_strdup(a->context,self->cval);
    } else {
        variableAccessor->dval=self->dval;
    }

    return the_clone;
}
