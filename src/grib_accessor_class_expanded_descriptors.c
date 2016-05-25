/*
* Copyright 2005-2016 ECMWF.
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
SUPER      = grib_accessor_class_long
IMPLEMENTS = unpack_long;pack_long
IMPLEMENTS = unpack_double
IMPLEMENTS = init;dump;destroy
IMPLEMENTS = value_count; get_native_type
MEMBERS    = const char* unexpandedDescriptors
MEMBERS    = const char* sequence
MEMBERS    = const char* expandedName
MEMBERS    = const char* tablesAccessorName
MEMBERS    = bufr_descriptors_array* expanded
MEMBERS    = int rank
MEMBERS    = grib_accessor* expandedAccessor
MEMBERS    = int do_expand
MEMBERS    = grib_accessor* tablesAccessor

END_CLASS_DEF

*/

/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "accessor.class" and rerun ./make_class.pl

*/

static int  get_native_type(grib_accessor*);
static int pack_long(grib_accessor*, const long* val,size_t *len);
static int unpack_double(grib_accessor*, double* val,size_t *len);
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
	const char* expandedName;
	const char* tablesAccessorName;
	bufr_descriptors_array* expanded;
	int rank;
	grib_accessor* expandedAccessor;
	int do_expand;
	grib_accessor* tablesAccessor;
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
    &get_native_type,            /* get native type               */
    0,                /* get sub_section                */
    0,               /* grib_pack procedures long      */
    0,                 /* grib_pack procedures long      */
    &pack_long,                  /* grib_pack procedures long      */
    &unpack_long,                /* grib_unpack procedures long    */
    0,                /* grib_pack procedures double    */
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
    0,                    /* compare vs. another accessor   */
    0,     /* unpack only ith value          */
    0,     /* unpack a subarray         */
    0,              		/* clear          */
    0,               		/* clone accessor          */
};


grib_accessor_class* grib_accessor_class_expanded_descriptors = &_grib_accessor_class_expanded_descriptors;


static void init_class(grib_accessor_class* c)
{
	c->next_offset	=	(*(c->super))->next_offset;
	c->string_length	=	(*(c->super))->string_length;
	c->byte_count	=	(*(c->super))->byte_count;
	c->byte_offset	=	(*(c->super))->byte_offset;
	c->sub_section	=	(*(c->super))->sub_section;
	c->pack_missing	=	(*(c->super))->pack_missing;
	c->is_missing	=	(*(c->super))->is_missing;
	c->pack_double	=	(*(c->super))->pack_double;
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
	c->make_clone	=	(*(c->super))->make_clone;
}

/* END_CLASS_IMP */

#define MYDEBUG 0

typedef struct change_coding_params {
    int associatedFieldWidth;
    int localDescriptorWidth;
    int extraWidth;
    int extraScale;
    int newStringWidth;
    double referenceFactor;
} change_coding_params ;

static void init(grib_accessor* a, const long len , grib_arguments* args )
{
    grib_accessor_expanded_descriptors* self = (grib_accessor_expanded_descriptors*)a;
    int n=0;
    self->tablesAccessorName=grib_arguments_get_name(grib_handle_of_accessor(a),args,n++);
    self->expandedName=grib_arguments_get_name(grib_handle_of_accessor(a),args,n++);
    self->rank=grib_arguments_get_long(grib_handle_of_accessor(a),args,n++);
    if (self->rank!=0) {
        self->expandedAccessor=grib_find_accessor(grib_handle_of_accessor(a),self->expandedName);
    } else {
        self->expandedAccessor=0;
    }
    self->unexpandedDescriptors=grib_arguments_get_name(grib_handle_of_accessor(a),args,n++);
    self->sequence=grib_arguments_get_name(grib_handle_of_accessor(a),args,n++);
    self->do_expand=1;
    self->expanded=0;
    a->length = 0;
}

static void dump(grib_accessor* a, grib_dumper* dumper)
{
    grib_dump_long(dumper,a,NULL);
}

static bufr_descriptors_array* do_expand(grib_accessor* a,bufr_descriptors_array* unexpanded,change_coding_params* ccp,int *err);

static int depth=-1;

static size_t __expand(grib_accessor* a, bufr_descriptors_array* unexpanded, bufr_descriptors_array* expanded,
        change_coding_params* ccp, int* err)
{
    int k,j,i;
    grib_accessor_expanded_descriptors* self = (grib_accessor_expanded_descriptors*)a;
    size_t size=0;
    long* v=NULL;
    bufr_descriptor* u=NULL;
    bufr_descriptor* vv=NULL;
    bufr_descriptor** ur=NULL;
    bufr_descriptor* urc=NULL;
    int idx;
    bufr_descriptor* u0=NULL;
    grib_context* c=a->context;
    bufr_descriptor* us=NULL;
    bufr_descriptors_array* inner_expanded=NULL;
    bufr_descriptors_array* inner_unexpanded=NULL;
#if MYDEBUG
    int idepth;
#endif


    if (grib_bufr_descriptors_array_used_size(unexpanded)==0) return 0;

    us=grib_bufr_descriptor_clone(grib_bufr_descriptors_array_get(unexpanded,0));

    *err=0;
#if MYDEBUG
    for (idepth=0;idepth<depth;idepth++) printf("\t");
    printf("expanding ==> %d-%02d-%03d\n",us->F,us->X,us->Y);
#endif
    switch (us->F) {
    case 3:
        /* sequence */
        u=grib_bufr_descriptors_array_pop_front(unexpanded);
#if MYDEBUG
        for (idepth=0;idepth<depth;idepth++) printf("\t");
        printf("+++ pop  %06ld\n",u->code);
#endif
        /*this is to get the sequence elements of the sequence unexpanded[i] */
        *err=grib_set_long(grib_handle_of_accessor(a),self->sequence,u->code);
        *err=grib_get_size(grib_handle_of_accessor(a),self->sequence,&size);
        grib_bufr_descriptor_delete(u);
        if (*err) return 0;
        v=(long*)grib_context_malloc_clear(c,sizeof(long)*size);
        *err=grib_get_long_array(grib_handle_of_accessor(a),self->sequence,v,&size);
        if (*err) return 0;
        inner_unexpanded=grib_bufr_descriptors_array_new(c,100,100);
        for (i=0;i<size;i++) {
            vv=grib_bufr_descriptor_new(self->tablesAccessor,v[i],err);
            inner_unexpanded=grib_bufr_descriptors_array_push(inner_unexpanded,vv);
        }
        grib_context_free(c,v);
        inner_expanded=do_expand(a,inner_unexpanded,ccp,err);
        grib_bufr_descriptors_array_delete(inner_unexpanded);
#if MYDEBUG
        for (i=0;i<inner_expanded->n;i++) {
            for (idepth=0;idepth<depth;idepth++) printf("\t");
            printf("+++ push %06ld\n",inner_expanded->v[i]->code);
        }
#endif
        size=grib_bufr_descriptors_array_used_size(inner_expanded);
        expanded=grib_bufr_descriptors_array_append(expanded,inner_expanded);
        break;

    case 1:
        if (us->Y==0) {
            /* delayed replication */
            bufr_descriptor* uidx=0;
            u=grib_bufr_descriptors_array_pop_front(unexpanded);
#if MYDEBUG
            for (idepth=0;idepth<depth;idepth++) printf("\t");
            printf("+++ pop  %06ld\n",u->code);
            for (idepth=0;idepth<depth;idepth++) printf("\t");
            printf("+++ push %06ld\n",u->code);
#endif
            grib_bufr_descriptors_array_push(expanded,u);
            idx=expanded->n-1;
            size=0;
            inner_unexpanded=grib_bufr_descriptors_array_new(c,100,100);
            inner_expanded=grib_bufr_descriptors_array_new(c,100,100);
            for (j=0;j<us->X+1;j++) {
                u0=grib_bufr_descriptors_array_pop_front(unexpanded);
                grib_bufr_descriptors_array_push(inner_unexpanded,u0);
#if MYDEBUG
                for (idepth=0;idepth<depth;idepth++) printf("\t");
                printf("+++ pop  %06ld\n",u0->code);
#endif
            }
            inner_expanded=do_expand(a,inner_unexpanded,ccp,err);
            size=grib_bufr_descriptors_array_used_size(inner_expanded);
#if MYDEBUG
            for (i=0;i<inner_expanded->n;i++) {
                for (idepth=0;idepth<depth;idepth++) printf("\t");
                printf("+++ push %06ld\n",inner_expanded->v[i]->code);
            }
#endif
            expanded=grib_bufr_descriptors_array_append(expanded,inner_expanded);
            uidx=grib_bufr_descriptors_array_get(expanded,idx);
            grib_bufr_descriptor_set_code(0,(size-1)*1000+100000,uidx);
            size++;
        } else {
            u=grib_bufr_descriptors_array_pop_front(unexpanded);
#if MYDEBUG
            for (idepth=0;idepth<depth;idepth++) printf("\t");
            printf("+++ pop  %06ld\n",u->code);
#endif
            grib_bufr_descriptor_delete(u);
            size=us->X*us->Y;
            ur=(bufr_descriptor**)grib_context_malloc_clear(c,us->X*sizeof(bufr_descriptor));
            for (j=0;j<us->X;j++) {
                ur[j]=grib_bufr_descriptors_array_pop_front(unexpanded);
#if MYDEBUG
                for (idepth=0;idepth<depth;idepth++) printf("\t");
                printf("+++ pop  %06ld\n",ur[j]->code);
#endif
            }
            inner_unexpanded=grib_bufr_descriptors_array_new(c,100,100);
            for (j=0;j<us->X;j++) {
                urc=grib_bufr_descriptor_clone(ur[j]);
                grib_bufr_descriptors_array_push(inner_unexpanded,urc);
            }
            for (k=1;k<us->Y;k++) {
                for (j=0;j<us->X;j++) {
                    urc=grib_bufr_descriptor_clone(ur[j]);
                    grib_bufr_descriptors_array_push(inner_unexpanded,urc);
                }
            }
            for (i=0;i<us->X;i++) grib_bufr_descriptor_delete(ur[i]);
            grib_context_free(c,ur);
            inner_expanded=do_expand(a,inner_unexpanded,ccp,err);
            grib_bufr_descriptors_array_delete(inner_unexpanded);
#if MYDEBUG
            for (i=0;i<inner_expanded->n;i++) {
                for (idepth=0;idepth<depth;idepth++) printf("\t");
                printf("+++ push %06ld\n",inner_expanded->v[i]->code);
            }
#endif
            size=grib_bufr_descriptors_array_used_size(inner_expanded);
            expanded=grib_bufr_descriptors_array_append(expanded,inner_expanded);
        }
        break;

    case 0:
        u=grib_bufr_descriptors_array_pop_front(unexpanded);
        size=1;
        if (ccp->associatedFieldWidth && u->X!=31) {
            bufr_descriptor* au=grib_bufr_descriptor_new(self->tablesAccessor,999999,err);
            au->width=ccp->associatedFieldWidth;
            grib_bufr_descriptor_set_scale(au,0);
            au->shortName=grib_context_strdup(c,"associatedField");
            au->name=grib_context_strdup(c,"associated field");
            au->units=grib_context_strdup(c,"associated units");
#if MYDEBUG
            for (idepth=0;idepth<depth;idepth++) printf("\t");
            printf("+++ push %06ld (%ld %g %ld)",au->code,au->scale,au->reference,au->width);
#endif
            grib_bufr_descriptors_array_push(expanded,au);
            size++;
        }
#if MYDEBUG
        for (idepth=0;idepth<depth;idepth++) printf("\t");
        printf("+++ pop  %06ld\n",u->code);
        for (idepth=0;idepth<depth;idepth++) printf("\t");
        printf("+++ push %06ld [type=%d] (%ld %g %ld)",u->code,
                u->type,u->scale,u->reference,u->width);
#endif
        if ( u->type!=BUFR_DESCRIPTOR_TYPE_FLAG  &&
                u->type!=BUFR_DESCRIPTOR_TYPE_TABLE &&
                u->type!=BUFR_DESCRIPTOR_TYPE_STRING  ) {
            if (ccp->localDescriptorWidth>0) {
                u->width=ccp->localDescriptorWidth;
                u->reference=0;
                grib_bufr_descriptor_set_scale(u,0);
                ccp->localDescriptorWidth=0;
            } else {
                u->width += ccp->extraWidth;
                u->reference *= ccp->referenceFactor;
                grib_bufr_descriptor_set_scale(u,u->scale+ccp->extraScale);
            }
        } else if (u->type==BUFR_DESCRIPTOR_TYPE_STRING && ccp->newStringWidth!=0) {
            u->width=ccp->newStringWidth;
        }
#if MYDEBUG
        printf("->(%ld %g %ld)\n",u->scale,u->reference,u->width);
#endif
        grib_bufr_descriptors_array_push(expanded,u);
        break;

    case 2:
        u=grib_bufr_descriptors_array_pop_front(unexpanded);
#if MYDEBUG
        for (idepth=0;idepth<depth;idepth++) printf("\t");
        printf("+++ pop  %06ld\n",u->code);
#endif
        switch(us->X) {
        case 1:
            ccp->extraWidth = us->Y ? us->Y-128 : 0;
            size=0;
            grib_bufr_descriptor_delete(u);
            break;
        case 2:
            ccp->extraScale = us->Y ? us->Y-128 : 0;
            size=0;
            grib_bufr_descriptor_delete(u);
            break;
        case 4:
            /* associated field*/
            ccp->associatedFieldWidth=us->Y;
            grib_bufr_descriptor_delete(u);
            break;
        case 6:
            /*signify data width*/
            ccp->localDescriptorWidth=us->Y;
            size=0;
            grib_bufr_descriptor_delete(u);
            break;
        case 7:
            if (us->Y) {
                ccp->extraScale = us->Y;
                ccp->referenceFactor=grib_power(us->Y,10);
                ccp->extraWidth=((10*us->Y)+2)/3;
            } else {
                ccp->extraWidth=0;
                ccp->extraScale=0;
                ccp->referenceFactor=1;
            }
            size=0;
            grib_bufr_descriptor_delete(u);
            break;
        case 8:
            ccp->newStringWidth = us->Y * 8 ;
            break;
        default:
#if MYDEBUG
            for (idepth=0;idepth<depth;idepth++) printf("\t");
            printf("+++ push %06ld\n",u->code);
#endif
            grib_bufr_descriptors_array_push(expanded,u);
            size=1;
        }
        break;

        default:
            u=grib_bufr_descriptors_array_pop_front(unexpanded);
#if MYDEBUG
            for (idepth=0;idepth<depth;idepth++) printf("\t");
            printf("+++ pop  %06ld\n",u->code);
            for (idepth=0;idepth<depth;idepth++) printf("\t");
            printf("+++ push %06ld\n",u->code);
#endif
            grib_bufr_descriptors_array_push(expanded,u);
            size=1;
    }
#if MYDEBUG
    for (idepth=0;idepth<depth;idepth++) printf("\t");
    printf("expanding <== %d-%.2d-%.3d (size=%ld)\n\n",us->F,us->X,us->Y,size);
#endif
    if (us) grib_bufr_descriptor_delete(us);
    return size;
}

static bufr_descriptors_array* do_expand(grib_accessor* a,bufr_descriptors_array* unexpanded,change_coding_params* ccp,int *err)
{
    bufr_descriptors_array* expanded=NULL;
    grib_context* c=a->context;
#if MYDEBUG
    int idepth;
#endif

    depth++;

    expanded=grib_bufr_descriptors_array_new(c,100,100);

#if MYDEBUG
    {
        int i;
        for (idepth=0;idepth<depth;idepth++) printf("\t");
        printf("to be expanded ==> \n");
        for (i=0;i<unexpanded->n;i++) {
            bufr_descriptor* xx=grib_bufr_descriptors_array_get(unexpanded,i);
            for (idepth=0;idepth<depth;idepth++) printf("\t");
            printf("%06ld\n",xx->code);
        }
        for (idepth=0;idepth<depth;idepth++) printf("\t");
        printf("to be expanded <== \n\n");
    }
#endif
    while (unexpanded->n) {
        __expand(a,unexpanded,expanded,ccp,err);
    }
#if MYDEBUG
    {
        int i;
        for (idepth=0;idepth<depth;idepth++) printf("\t");
        printf("expanded ==> \n");
        for (i=0;i<expanded->n;i++) {
            bufr_descriptor* xx=grib_bufr_descriptors_array_get(expanded,i);
            for (idepth=0;idepth<depth;idepth++) printf("\t");
            printf("==  %-6d== %06ld ",i,xx->code);
            printf("%ld %g %ld",xx->scale,xx->reference,xx->width);
            printf("\n");
        }
        for (idepth=0;idepth<depth;idepth++) printf("\t");
        printf("expanded <== \n\n");
    }
#endif

    depth--;

    return expanded;
}

static int expand(grib_accessor* a)
{
    grib_accessor_expanded_descriptors* self = (grib_accessor_expanded_descriptors*)a;
    int err=0;
    size_t unexpandedSize=0;
    int i;
    long* u=0;
    change_coding_params ccp;
    bufr_descriptors_array* unexpanded=NULL;
    grib_context* c=a->context;

    if (!self->do_expand) {
        return err;
    }
    self->do_expand=0;
    if (!self->tablesAccessor) {
        self->tablesAccessor=grib_find_accessor(grib_handle_of_accessor(a),self->tablesAccessorName);
        Assert(self->tablesAccessor);
    }

    if (self->rank!=0) {
        err=expand(self->expandedAccessor);
        self->expanded=((grib_accessor_expanded_descriptors*)self->expandedAccessor)->expanded;
        return err;
    }

    grib_bufr_descriptors_array_delete(self->expanded);
    err=grib_get_size(grib_handle_of_accessor(a),self->unexpandedDescriptors,&unexpandedSize);
    if (err) return err;
    u=(long*)grib_context_malloc_clear(c,sizeof(long)*unexpandedSize);
    if (!u) {err=GRIB_OUT_OF_MEMORY; return err;}
    err=grib_get_long_array(grib_handle_of_accessor(a),self->unexpandedDescriptors,u,&unexpandedSize);
    if (err) return err;

    unexpanded=grib_bufr_descriptors_array_new(c,unexpandedSize,100);
    for (i=0;i<unexpandedSize;i++) {
        grib_bufr_descriptors_array_push(unexpanded,grib_bufr_descriptor_new(self->tablesAccessor,u[i],&err));
    }

    grib_context_free(c,u);

    ccp.extraWidth=0;
    ccp.localDescriptorWidth=-1;
    ccp.extraScale=0;
    ccp.referenceFactor=1;
    ccp.associatedFieldWidth=0;
    ccp.newStringWidth=0;
    self->expanded=do_expand(a,unexpanded,&ccp,&err);
    grib_bufr_descriptors_array_delete(unexpanded);

    return err;

}

int grib_accessor_class_expanded_descriptors_set_do_expand(grib_accessor* a,long do_expand)
{
    grib_accessor_expanded_descriptors* self = (grib_accessor_expanded_descriptors*)a;
    self->do_expand=do_expand;
    return 0;
}

bufr_descriptors_array* grib_accessor_class_expanded_descriptors_get_expanded(grib_accessor* a,int* err)
{
    grib_accessor_expanded_descriptors* self = (grib_accessor_expanded_descriptors*)a;
    *err=expand(a);
    return self->expanded;
}

static int    unpack_double   (grib_accessor* a, double* val, size_t *len)
{
    grib_accessor_expanded_descriptors* self = (grib_accessor_expanded_descriptors*)a;
    int ret=0;
    int i;
    size_t expandedSize;

    if (self->rank!=2) {
        long* lval=(long*)grib_context_malloc_clear(a->context,*len*sizeof(long));
        ret=unpack_long(a,lval,len);
        if (ret) return ret;
        for (i=0;i<*len;i++) val[i]=(double)lval[i];
        grib_context_free(a->context,lval);
    } else {
        ret=expand(a);
        if (ret) return ret;

        expandedSize=grib_bufr_descriptors_array_used_size(self->expanded);
        if(*len < expandedSize)
        {
            grib_context_log(a->context, GRIB_LOG_ERROR,
                    " wrong size (%ld) for %s it contains %d values ",*len, a->name , expandedSize);
            *len = 0;
            return GRIB_ARRAY_TOO_SMALL;
        }
        *len = expandedSize;
        for (i=0;i<*len;i++) val[i]=self->expanded->v[i]->reference;
    }
    return ret;
}

static int    unpack_long   (grib_accessor* a, long* val, size_t *len)
{
    grib_accessor_expanded_descriptors* self = (grib_accessor_expanded_descriptors*)a;
    int ret=0;
    size_t rlen=0;
    size_t i;

    ret=expand(a);
    rlen=grib_bufr_descriptors_array_used_size(self->expanded);
    if (ret) return ret;

    if(*len < rlen)
    {
        grib_context_log(a->context, GRIB_LOG_ERROR,
                " wrong size (%ld) for %s it contains %d values ",*len, a->name , rlen);
        *len = 0;
        return GRIB_ARRAY_TOO_SMALL;
    }

    *len = rlen;
    switch (self->rank) {
    case 0:
        for (i=0;i<*len;i++) val[i]=self->expanded->v[i]->code;
        break;
    case 1:
        for (i=0;i<*len;i++) val[i]=self->expanded->v[i]->scale;
        break;
    case 2:
        return GRIB_INVALID_TYPE;
        break;
    case 3:
        for (i=0;i<*len;i++) val[i]=self->expanded->v[i]->width;
        break;
    case 4:
        for (i=0;i<*len;i++) val[i]=self->expanded->v[i]->type;
        break;
    }

    return GRIB_SUCCESS;
}

static int    pack_long   (grib_accessor* a, const long* val, size_t *len)
{
    grib_accessor_expanded_descriptors* self = (grib_accessor_expanded_descriptors*)a;
    self->do_expand=1;
    return GRIB_NOT_IMPLEMENTED;
}

static int value_count(grib_accessor* a,long* rlen)
{
    grib_accessor_expanded_descriptors* self = (grib_accessor_expanded_descriptors*)a;
    int ret=0;
    grib_context* c=a->context;
    *rlen=0;

    ret=expand(a);
    *rlen=grib_bufr_descriptors_array_used_size(self->expanded);
    if (ret) {
        grib_context_log(c,GRIB_LOG_ERROR,"%s unable to compute size",a->name);
        return ret;
    }

    return ret;
}

static void destroy(grib_context* c,grib_accessor* a)
{
    grib_accessor_expanded_descriptors* self = (grib_accessor_expanded_descriptors*)a;
    if (self->rank==0 && self->expanded) {
        grib_bufr_descriptors_array_delete(self->expanded);
    }
}

static int  get_native_type(grib_accessor* a)
{
    grib_accessor_expanded_descriptors* self = (grib_accessor_expanded_descriptors*)a;
    if (self->rank==2) return GRIB_TYPE_DOUBLE;
    else return GRIB_TYPE_LONG;
}
