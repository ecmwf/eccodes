/*
 * Copyright 2005-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

/*************************************************
 * Enrico Fucile
 ***********************************************/

#include "grib_api_internal.h"
#include <math.h>
/*
   This is used by make_class.pl

   START_CLASS_DEF
   CLASS      = accessor
   SUPER      = grib_accessor_class_long
   IMPLEMENTS = unpack_long;pack_long
   IMPLEMENTS = init
   MEMBERS=const char*                  N
   MEMBERS=const char*                  Ni
   MEMBERS=const char*                  di
   MEMBERS=const char*                  latfirst
   MEMBERS=const char*                  lonfirst
   MEMBERS=const char*                  latlast
   MEMBERS=const char*                  lonlast
   MEMBERS=const char*                  plpresent
   MEMBERS=const char*                  pl
   MEMBERS=const char*                  basic_angle
   MEMBERS=const char*                  subdivision
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
static void init(grib_accessor*,const long, grib_arguments* );
static void init_class(grib_accessor_class*);

typedef struct grib_accessor_global_gaussian {
    grib_accessor          att;
/* Members defined in gen */
/* Members defined in long */
/* Members defined in global_gaussian */
	const char*                  N;
	const char*                  Ni;
	const char*                  di;
	const char*                  latfirst;
	const char*                  lonfirst;
	const char*                  latlast;
	const char*                  lonlast;
	const char*                  plpresent;
	const char*                  pl;
	const char*                  basic_angle;
	const char*                  subdivision;
} grib_accessor_global_gaussian;

extern grib_accessor_class* grib_accessor_class_long;

static grib_accessor_class _grib_accessor_class_global_gaussian = {
    &grib_accessor_class_long,                      /* super                     */
    "global_gaussian",                      /* name                      */
    sizeof(grib_accessor_global_gaussian),  /* size                      */
    0,                           /* inited */
    &init_class,                 /* init_class */
    &init,                       /* init                      */
    0,                  /* post_init                      */
    0,                    /* free mem                       */
    0,                       /* describes himself         */
    0,                /* get length of section     */
    0,              /* get length of string      */
    0,                /* get number of values      */
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
    0,              		/* clear          */
    0,               		/* clone accessor          */
};


grib_accessor_class* grib_accessor_class_global_gaussian = &_grib_accessor_class_global_gaussian;


static void init_class(grib_accessor_class* c)
{
	c->dump	=	(*(c->super))->dump;
	c->next_offset	=	(*(c->super))->next_offset;
	c->string_length	=	(*(c->super))->string_length;
	c->value_count	=	(*(c->super))->value_count;
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
	c->make_clone	=	(*(c->super))->make_clone;
}

/* END_CLASS_IMP */

static void init(grib_accessor* a,const long l, grib_arguments* c)
{
    grib_accessor_global_gaussian* self = (grib_accessor_global_gaussian*)a;
    int n = 0;
    grib_handle* h = grib_handle_of_accessor(a);

    self->N            = grib_arguments_get_name(h,c,n++);
    self->Ni           = grib_arguments_get_name(h,c,n++);
    self->di           = grib_arguments_get_name(h,c,n++);
    self->latfirst     = grib_arguments_get_name(h,c,n++);
    self->lonfirst     = grib_arguments_get_name(h,c,n++);
    self->latlast      = grib_arguments_get_name(h,c,n++);
    self->lonlast      = grib_arguments_get_name(h,c,n++);
    self->plpresent    = grib_arguments_get_name(h,c,n++);
    self->pl           = grib_arguments_get_name(h,c,n++);
    self->basic_angle  = grib_arguments_get_name(h,c,n++);
    self->subdivision  = grib_arguments_get_name(h,c,n++);
}

static int unpack_long(grib_accessor* a, long* val, size_t *len)
{
    grib_accessor_global_gaussian* self = (grib_accessor_global_gaussian*)a;
    int ret = GRIB_SUCCESS;
    long latfirst,latlast,lonfirst,lonlast,basic_angle,subdivision,N,Ni;
    double dlatfirst,dlatlast,dlonfirst,dlonlast;
    double angular_precision = 0;
    double* lats = NULL;
    long factor=1000, plpresent=0;
    long max_pl=0; /* max. element of pl array */
    grib_context* c=a->context;
    grib_handle* h = grib_handle_of_accessor(a);

    if (self->basic_angle && self->subdivision) {
        factor=1000000;
        if((ret = grib_get_long_internal(h, self->basic_angle,&basic_angle)) != GRIB_SUCCESS)
            return ret;

        if((ret = grib_get_long_internal(h, self->subdivision,&subdivision)) != GRIB_SUCCESS)
            return ret;

        if ( (basic_angle !=0 && basic_angle != GRIB_MISSING_LONG) ||
                ( subdivision  !=0 && subdivision != GRIB_MISSING_LONG) ) {
            *val=0;
            return ret;
        }
    } else {
        factor=1000;
    }
    angular_precision = 1.0/factor;

    if((ret = grib_get_long_internal(h, self->N,&N)) != GRIB_SUCCESS)
        return ret;

    if((ret = grib_get_long_internal(h, self->Ni,&Ni)) != GRIB_SUCCESS)
        return ret;

    if((ret = grib_get_long_internal(h, self->latfirst,&latfirst)) != GRIB_SUCCESS)
        return ret;

    if((ret = grib_get_long_internal(h, self->lonfirst,&lonfirst)) != GRIB_SUCCESS)
        return ret;

    if((ret = grib_get_long_internal(h, self->latlast,&latlast)) != GRIB_SUCCESS)
        return ret;

    if((ret = grib_get_long_internal(h, self->lonlast,&lonlast)) != GRIB_SUCCESS)
        return ret;

    if((ret = grib_get_long_internal(h, self->plpresent,&plpresent)) != GRIB_SUCCESS)
        return ret;

    dlatfirst=((double)latfirst)/factor;
    dlatlast=((double)latlast)/factor;
    dlonfirst=((double)lonfirst)/factor;
    dlonlast=((double)lonlast)/factor;

    lats=(double*)grib_context_malloc(c,sizeof(double)*N*2);
    if (!lats) {
        grib_context_log(c,GRIB_LOG_FATAL,
                "global_gaussian: unable to allocate %d bytes",sizeof(double)*N*2);
    }
    if((ret = grib_get_gaussian_latitudes(N, lats)) != GRIB_SUCCESS)
        return ret;

    /* GRIB-704: Work out the maximum element in pl array, if present */
    max_pl = 4*N; /* default */
    if (plpresent) {
        size_t plsize=0, i=0;
        long* pl=NULL; /* pl array */
        if((ret = grib_get_size(h,self->pl,&plsize)) != GRIB_SUCCESS)
            return ret;
        Assert(plsize);
        pl=(long*)grib_context_malloc_clear(c,sizeof(long)*plsize);
        grib_get_long_array_internal(h,self->pl,pl, &plsize);

        max_pl = pl[0];
        for (i=1; i<plsize; i++) {
            if (pl[i] > max_pl) max_pl = pl[i];
        }
        grib_context_free(c, pl);
    }

    /* If Ni is missing, then this is a reduced gaussian grid */
    if (Ni == GRIB_MISSING_LONG ) Ni=max_pl;

    if (is_gaussian_global(dlatfirst,dlatlast,dlonfirst,dlonlast,Ni,lats,angular_precision)) {
        *val=1;  /* global */
    } else {
        *val=0;  /* not global */
    }

    grib_context_free(c,lats);

    return ret;
}

static int pack_long(grib_accessor* a, const long* val, size_t *len)
{
    grib_accessor_global_gaussian* self = (grib_accessor_global_gaussian*)a;
    int ret=GRIB_SUCCESS;
    long latfirst,latlast,lonfirst,lonlast,di,diold,basic_angle=0,N,Ni;
    long factor;
    double* lats;
    double ddi,dlonlast;
    double dfactor,dNi;
    long plpresent=0;
    grib_context* c=a->context;
    grib_handle* h = grib_handle_of_accessor(a);

    if (*val == 0) return ret;

    if (self->basic_angle)  {
        factor=1000000;
        if((ret = grib_set_missing(h, self->subdivision)) != GRIB_SUCCESS)
            return ret;

        if((ret = grib_set_long_internal(h, self->basic_angle,basic_angle)) != GRIB_SUCCESS)
            return ret;
    } else factor=1000;

    if((ret = grib_get_long_internal(h, self->N,&N)) != GRIB_SUCCESS)
        return ret;
    if (N==0) return ret;

    if((ret = grib_get_long_internal(h, self->Ni,&Ni)) != GRIB_SUCCESS)
        return ret;
    if (Ni == GRIB_MISSING_LONG ) Ni=N*4;
    if (Ni==0) return ret;

    if((ret = grib_get_long_internal(h, self->di,&diold)) != GRIB_SUCCESS)
        return ret;

    lats=(double*)grib_context_malloc(c,sizeof(double)*N*2);
    if (!lats) {
        grib_context_log(c,GRIB_LOG_FATAL,
                "global_gaussian: unable to allocate %d bytes",sizeof(double)*N*2);
    }
    if((ret = grib_get_gaussian_latitudes(N, lats)) != GRIB_SUCCESS)
        return ret;

    if((ret = grib_get_long_internal(h, self->plpresent,&plpresent)) != GRIB_SUCCESS)
        return ret;

    /* GRIB-854: For octahedral grids, get max of pl array */
    if (plpresent) {
        size_t plsize=0, i=0;
        long* pl=NULL; /* pl array */
        long max_pl=0; /* max. element of pl array */

        if((ret = grib_get_size(h,self->pl,&plsize)) != GRIB_SUCCESS)
            return ret;
        Assert(plsize);
        pl=(long*)grib_context_malloc_clear(c,sizeof(long)*plsize);
        grib_get_long_array_internal(h,self->pl,pl, &plsize);

        max_pl = pl[0];
        for (i=1; i<plsize; i++) {
            Assert( pl[i] > 0 );
            if (pl[i] > max_pl) max_pl = pl[i];
        }
        grib_context_free(c, pl);
        Ni = max_pl;
    }

    /* rounding */
    latfirst=(long)(lats[0]*factor+0.5);
    latlast=-latfirst;
    lonfirst=0;
    dfactor=(double)factor;
    dNi=(double)Ni;
    ddi=(360.0*dfactor)/dNi;
    dlonlast=(360.0*dfactor)-ddi+0.5;
    ddi=ddi+0.5;
    di=ddi;
    lonlast=dlonlast;

    grib_context_free(c,lats);

    if((ret = grib_set_long_internal(h, self->latfirst,latfirst)) != GRIB_SUCCESS)
        return ret;

    if((ret = grib_set_long_internal(h, self->lonfirst,lonfirst)) != GRIB_SUCCESS)
        return ret;

    if((ret = grib_set_long_internal(h, self->latlast,latlast)) != GRIB_SUCCESS)
        return ret;

    if((ret = grib_set_long_internal(h, self->lonlast,lonlast)) != GRIB_SUCCESS)
        return ret;

    if (diold != GRIB_MISSING_LONG)
        if((ret = grib_set_long_internal(h, self->di,di)) != GRIB_SUCCESS)
            return ret;

    return GRIB_SUCCESS;
}
