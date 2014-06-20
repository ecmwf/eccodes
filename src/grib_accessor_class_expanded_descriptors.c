/*
* Copyright 2005-2014 ECMWF.
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
expanded descriptors cannot contain sequences and only delayed replication
can appear
*/

#include "grib_api_internal.h"
/*
This is used by make_class.pl

START_CLASS_DEF
CLASS      = accessor
SUPER      = grib_accessor_class_long
IMPLEMENTS = unpack_long;pack_long
IMPLEMENTS = init;dump;destroy
IMPLEMENTS = value_count
MEMBERS    = const char* unexpandedDescriptors
MEMBERS    = const char* sequence
MEMBERS    = long* expanded
MEMBERS    = size_t expandedSize
MEMBERS    = int dirty

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
static int value_count(grib_accessor*,long*);
static void destroy(grib_context*,grib_accessor*);
static void dump(grib_accessor*, grib_dumper*);
static void init(grib_accessor*,const long, grib_arguments* );
static void init_class(grib_accessor_class*);

typedef struct grib_accessor_expanded_descriptors {
    grib_accessor          att;
/* Members defined in gen */
/* Members defined in long */
/* Members defined in expanded_descriptors */
	const char* unexpandedDescriptors;
	const char* sequence;
	long* expanded;
	size_t expandedSize;
	int dirty;
} grib_accessor_expanded_descriptors;

extern grib_accessor_class* grib_accessor_class_long;

static grib_accessor_class _grib_accessor_class_expanded_descriptors = {
    &grib_accessor_class_long,                      /* super                     */
    "expanded_descriptors",                      /* name                      */
    sizeof(grib_accessor_expanded_descriptors),  /* size                      */
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
    &pack_long,                  /* grib_pack procedures long      */
    &unpack_long,                /* grib_unpack procedures long    */
    0,                /* grib_pack procedures double    */
    0,              /* grib_unpack procedures double  */
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
    0,                    /* compare vs. another accessor   */
    0,     /* unpack only ith value          */
    0,     /* unpack a subarray         */
    0,             		/* clear          */
};


grib_accessor_class* grib_accessor_class_expanded_descriptors = &_grib_accessor_class_expanded_descriptors;


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
	c->pack_double	=	(*(c->super))->pack_double;
	c->unpack_double	=	(*(c->super))->unpack_double;
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
	c->compare	=	(*(c->super))->compare;
	c->unpack_double_element	=	(*(c->super))->unpack_double_element;
	c->unpack_double_subarray	=	(*(c->super))->unpack_double_subarray;
	c->clear	=	(*(c->super))->clear;
}

/* END_CLASS_IMP */

static void init(grib_accessor* a, const long len , grib_arguments* args )
{
  grib_accessor_expanded_descriptors* self = (grib_accessor_expanded_descriptors*)a;
  int n=0;
  self->unexpandedDescriptors=grib_arguments_get_name(a->parent->h,args,n++);
  self->sequence=grib_arguments_get_name(a->parent->h,args,n++);
  self->dirty=1;
  self->expanded=0;
  a->length = 0;
}

static void dump(grib_accessor* a, grib_dumper* dumper)
{
  grib_dump_long(dumper,a,NULL);
}

size_t __expand(grib_accessor* a,grib_iarray* unexpanded,grib_iarray* expanded, int* err) {
  int FF,F,X,Y,k,j;
  grib_accessor_expanded_descriptors* self = (grib_accessor_expanded_descriptors*)a;
  size_t size;
  long* v;
  long u;
  long* ur;
  int idx;
  long u0;
  grib_context* c=a->parent->h->context;
  long us;

  if (grib_iarray_get_used_size(unexpanded)==0) return 0;

  us=grib_iarray_get(unexpanded,0);

  *err=0;
  F=us/100000;
  X=(us-F*100000)/1000;
  Y=(us-F*100000)%1000;
  /* printf("\n__expand ==> %d-%.2d-%.3d\n",F,X,Y); */
  switch (F) {
    case 3:
      /* sequence */
      u=grib_iarray_pop_front(unexpanded);
      /* printf("+++ pop %ld\n",u); */
      /*this is to get the sequence elements of the sequence unexpanded[i] */
      *err=grib_set_long(a->parent->h,self->sequence,u);
      *err=grib_get_size(a->parent->h,self->sequence,&size);
      if (*err) return 0;
      v=grib_context_malloc_clear(c,sizeof(long)*size);
      *err=grib_get_long_array(a->parent->h,self->sequence,v,&size);
      if (*err) return 0;
      expanded=grib_iarray_push_array(expanded,v,size);
      /* printf("=== push array \n"); */
      /* for (i=0;i<size;i++) printf("=== push %ld\n",v[i]); */
      /* for (i=0;i<expanded->n;i++) printf("=== %ld\n",grib_iarray_get(expanded,i)); */
      grib_context_free(c,v);
      break;

    case 1:
      if (Y==0) {
        u=grib_iarray_pop_front(unexpanded);
        /* printf("+++ pop %ld\n",u); */
        /* printf("=== push %ld\n",u); */
        grib_iarray_push(expanded,u);
        idx=expanded->n-1;
        u=grib_iarray_pop_front(unexpanded);
        /* printf("+++ pop %ld\n",u); */
        /* printf("=== push %ld\n",u); */
        grib_iarray_push(expanded,u);
        size=0;
        for (j=0;j<X;j++) {
          u0=grib_iarray_get(unexpanded,0);
          if ((FF=u0/100000)==1)
              j+=(u0-FF*100000)/1000;
          size+=__expand(a,unexpanded,expanded,err);
        }
        grib_iarray_set(expanded,idx,size*1000+100000);
        size++;
      } else {
        u=grib_iarray_pop_front(unexpanded);
        /* printf("+++ pop %ld\n",u); */
        size=X*Y;
        ur=grib_context_malloc_clear(c,X*sizeof(long));
        for (j=0;j<X;j++) {
          ur[j]=grib_iarray_pop_front(unexpanded);
          /* printf("+++ pop front %ld\n",ur[j]); */
        }
        for (k=0;k<Y;k++) {
              for (j=0;j<X;j++) {
                  /* printf("=== push %ld\n",ur[j]); */
                  grib_iarray_push(expanded,ur[j]);
              }
        }
        grib_context_free(c,ur);
      }
      break;

    default:
      u=grib_iarray_pop_front(unexpanded);
      /* printf("+++ pop front %ld\n",u); */
      /* printf("=== push %ld\n",u); */
      grib_iarray_push(expanded,u);
      size=1;
  }
    /* printf("__expand <== %d-%.2d-%.3d (%ld)\n",F,X,Y,size); */
    return size;
}

static long* _expand(grib_accessor* a,long* u,size_t unexpandedSize,size_t *size,int *err)
{
  grib_iarray* expanded=NULL;
  long *expanded_array=0;
  long *ar=0;
  size_t i=0;
  grib_iarray* unexpanded;
  grib_context* c=a->parent->h->context;
  expanded=grib_iarray_new(c,unexpandedSize,100);
  unexpanded=grib_iarray_new_from_array(c,u,unexpandedSize);

  *size=0;
  while (unexpanded->n)
    __expand(a,unexpanded,expanded,err);

  ar=grib_iarray_get_array(expanded);
  expanded_array=grib_context_malloc_clear(c,sizeof(long)*expanded->n);
  for (i=0;i<expanded->n;i++) expanded_array[i]=ar[i];
  *size=expanded->n;
  grib_iarray_delete(expanded);
  grib_context_free(c,ar);
  grib_iarray_delete(unexpanded);
  return expanded_array;
}

static long* expand(grib_accessor* a,size_t *expandedSize,int *err)
{
  grib_accessor_expanded_descriptors* self = (grib_accessor_expanded_descriptors*)a;
  long *expanded=0;
  size_t unexpandedSize=0;
  long* unexpanded=NULL;
  grib_context* c=a->parent->h->context;

  *err=grib_get_size(a->parent->h,self->unexpandedDescriptors,&unexpandedSize);
  if (*err) return NULL;
  unexpanded=grib_context_malloc_clear(c,sizeof(long)*unexpandedSize);
  if (!unexpanded) {*err=GRIB_OUT_OF_MEMORY; return NULL;}
  *err=grib_get_long_array(a->parent->h,self->unexpandedDescriptors,unexpanded,&unexpandedSize);
  if (*err) return NULL;

  *expandedSize=unexpandedSize;
  while (1) {
    expanded=_expand(a,unexpanded,unexpandedSize,expandedSize,err);
    /* printf("unexpandedSize=%ld expandedSize=%ld\n",unexpandedSize,*expandedSize); */
    grib_context_free(c,unexpanded);
    if (unexpandedSize==*expandedSize) break;
    unexpanded=expanded;
    expanded=0;
    unexpandedSize=*expandedSize;
  }

  return expanded;

}

static int    unpack_long   (grib_accessor* a, long* val, size_t *len)
{
  grib_accessor_expanded_descriptors* self = (grib_accessor_expanded_descriptors*)a;
  int ret=0;
  size_t rlen=0;
  size_t i;

  if (self->dirty) {
    if (self->expanded)
        grib_context_free(a->parent->h->context,self->expanded);
    self->expanded=expand(a,&rlen,&ret);
    self->expandedSize=rlen;
    self->dirty=0;
    if (ret) return ret;
  }


  if(*len < rlen)
  {
    grib_context_log(a->parent->h->context, GRIB_LOG_ERROR,
		    " wrong size (%ld) for %s it contains %d values ",*len, a->name , rlen);
    *len = 0;
    return GRIB_ARRAY_TOO_SMALL;
  }

  *len = self->expandedSize;
  for (i=0;i<*len;i++) val[i]=self->expanded[i];

  return GRIB_SUCCESS;
}

static int    pack_long   (grib_accessor* a, const long* val, size_t *len)
{
  grib_accessor_expanded_descriptors* self = (grib_accessor_expanded_descriptors*)a;
  self->dirty=1;
  return GRIB_NOT_IMPLEMENTED;
  
}


static int value_count(grib_accessor* a,long* rlen)
{
  int ret=0;
  grib_context* c=a->parent->h->context;
  grib_accessor_expanded_descriptors* self = (grib_accessor_expanded_descriptors*)a;
  *rlen=0;

  if (self->expanded) grib_context_free(c,self->expanded);
  self->expanded=expand(a,&(self->expandedSize),&ret);
  self->dirty=0;
  if (ret) {
    grib_context_log(a->parent->h->context,GRIB_LOG_ERROR,
		    "%s unable to compute size",a->name);
		    return ret;
  }

  *rlen=self->expandedSize;

  return ret;
}

static void destroy(grib_context* c,grib_accessor* a) {
  grib_accessor_expanded_descriptors* self = (grib_accessor_expanded_descriptors*)a;
  grib_context_free(c,self->expanded);
}
