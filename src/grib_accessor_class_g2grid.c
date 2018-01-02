/*
 * Copyright 2005-2018 ECMWF.
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
   SUPER      = grib_accessor_class_double
   IMPLEMENTS = unpack_double;pack_double;value_count
   IMPLEMENTS = init
   MEMBERS=const char*                  latitude_first
   MEMBERS=const char*                  longitude_first
   MEMBERS=const char*                  latitude_last
   MEMBERS=const char*                  longitude_last
   MEMBERS=const char*                  i_increment
   MEMBERS=const char*                  j_increment
   MEMBERS=const char*                  basic_angle
   MEMBERS=const char*                  sub_division
   END_CLASS_DEF

 */

/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "accessor.class" and rerun ./make_class.pl

*/

static int pack_double(grib_accessor*, const double* val,size_t *len);
static int unpack_double(grib_accessor*, double* val,size_t *len);
static int value_count(grib_accessor*,long*);
static void init(grib_accessor*,const long, grib_arguments* );
static void init_class(grib_accessor_class*);

typedef struct grib_accessor_g2grid {
    grib_accessor          att;
/* Members defined in gen */
/* Members defined in double */
/* Members defined in g2grid */
	const char*                  latitude_first;
	const char*                  longitude_first;
	const char*                  latitude_last;
	const char*                  longitude_last;
	const char*                  i_increment;
	const char*                  j_increment;
	const char*                  basic_angle;
	const char*                  sub_division;
} grib_accessor_g2grid;

extern grib_accessor_class* grib_accessor_class_double;

static grib_accessor_class _grib_accessor_class_g2grid = {
    &grib_accessor_class_double,                      /* super                     */
    "g2grid",                      /* name                      */
    sizeof(grib_accessor_g2grid),  /* size                      */
    0,                           /* inited */
    &init_class,                 /* init_class */
    &init,                       /* init                      */
    0,                  /* post_init                      */
    0,                    /* free mem                       */
    0,                       /* describes himself         */
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
    0,                    /* compare vs. another accessor   */
    0,     /* unpack only ith value          */
    0,     /* unpack a subarray         */
    0,              		/* clear          */
    0,               		/* clone accessor          */
};


grib_accessor_class* grib_accessor_class_g2grid = &_grib_accessor_class_g2grid;


static void init_class(grib_accessor_class* c)
{
	c->dump	=	(*(c->super))->dump;
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

#ifdef ECCODES_ON_WINDOWS
#define round(a) ( (a) >=0 ? ((a)+0.5) : ((a)-0.5) )
#endif

static void init(grib_accessor* a,const long l, grib_arguments* c)
{
    grib_accessor_g2grid* self = (grib_accessor_g2grid*)a;
    int n = 0;

    self->latitude_first      = grib_arguments_get_name(grib_handle_of_accessor(a),c,n++);
    self->longitude_first     = grib_arguments_get_name(grib_handle_of_accessor(a),c,n++);
    self->latitude_last       = grib_arguments_get_name(grib_handle_of_accessor(a),c,n++);
    self->longitude_last      = grib_arguments_get_name(grib_handle_of_accessor(a),c,n++);
    self->i_increment         = grib_arguments_get_name(grib_handle_of_accessor(a),c,n++);
    self->j_increment         = grib_arguments_get_name(grib_handle_of_accessor(a),c,n++);
    self->basic_angle         = grib_arguments_get_name(grib_handle_of_accessor(a),c,n++);
    self->sub_division        = grib_arguments_get_name(grib_handle_of_accessor(a),c,n++);

    a->flags |=
            GRIB_ACCESSOR_FLAG_EDITION_SPECIFIC |
            GRIB_ACCESSOR_FLAG_READ_ONLY ;
}

static int value_count(grib_accessor* a,long* count)
{
    *count=6;
    return 0;
}

static int unpack_double(grib_accessor* a, double* val, size_t *len)
{
    grib_accessor_g2grid* self = (grib_accessor_g2grid*)a;
    int ret = 0;

    long basic_angle  = 0;
    long sub_division = 0;
    int n = 0;
    long v[6];
    int i;

    if(*len < 6){
        ret = GRIB_ARRAY_TOO_SMALL;
        return ret;
    }

    if((ret = grib_get_long_internal(grib_handle_of_accessor(a), self->basic_angle,&basic_angle)) != GRIB_SUCCESS)
        return ret;

    if((ret = grib_get_long_internal(grib_handle_of_accessor(a), self->sub_division,&sub_division)) != GRIB_SUCCESS)
        return ret;


    if(sub_division == GRIB_MISSING_LONG || sub_division == 0)
        sub_division = 1000000;

    if(basic_angle == 0)
        basic_angle = 1;

    n = 0;
    if((ret = grib_get_long_internal(grib_handle_of_accessor(a), self->latitude_first,&v[n++])) != GRIB_SUCCESS)
        return ret;

    if((ret = grib_get_long_internal(grib_handle_of_accessor(a), self->longitude_first,&v[n++])) != GRIB_SUCCESS)
        return ret;

    if((ret = grib_get_long_internal(grib_handle_of_accessor(a), self->latitude_last,&v[n++])) != GRIB_SUCCESS)
        return ret;

    if((ret = grib_get_long_internal(grib_handle_of_accessor(a), self->longitude_last,&v[n++])) != GRIB_SUCCESS)
        return ret;

    if(!self->i_increment)  v[n++] = GRIB_MISSING_LONG;
    else
        if((ret = grib_get_long_internal(grib_handle_of_accessor(a), self->i_increment,&v[n++])) != GRIB_SUCCESS)
            return ret;

    if(!self->j_increment)  v[n++] = GRIB_MISSING_LONG;
    else
        if(self->j_increment)
            if((ret = grib_get_long_internal(grib_handle_of_accessor(a), self->j_increment,&v[n++])) != GRIB_SUCCESS)
                return ret;

    for(i = 0 ; i < n ; i++)
        if(v[i] == GRIB_MISSING_LONG)
            val[i] = GRIB_MISSING_DOUBLE;
        else
            val[i] = (double)v[i] / (double)sub_division * (double)basic_angle;

    return GRIB_SUCCESS;
}

static long gcd(long a,long b)
{
    if(b>a)    return gcd(b,a);
    if(b == 0) return a;
    return gcd(b,a%b);
}

static long lcm(long a,long b)
{
    return a*b/gcd(a,b);
}

static int is_ok(const double* val,long v[6],double basic_angle,double sub_division)
{
    int i;
    int ok = 1;

    for(i = 0; i < 6 ; i++)
    {
        if(val[i] == GRIB_MISSING_DOUBLE)
            v[i] = GRIB_MISSING_LONG;
        else
        {
            double d = (val[i] * sub_division) / basic_angle;
            double e;
            d = round(d); /* GRIB-941 */
            v[i]     = d;
            e        = (v[i] * basic_angle) / sub_division ;

            /* if(fabs(e - val[i]) >= 1e-6) */
            if(fabs(e - val[i]) > 0)
            {
                /* printf("e=%g val=%g diff=%g\n",e,val[i],e-val[i]); */
                ok = 0;
            }
        }
    }
    return ok;
}

static int trial(const double* val,long v[6],long* basic_angle,long* sub_division)
{
    int i = 0;
    long ni, nj;

    for(i = 0; i < 6 ; i++)
        if(val[i] == GRIB_MISSING_DOUBLE)
            return 0;

    if(val[4] == 0) return 0;
    if(val[5] == 0) return 0;

    ni = (long)(0.5+fabs((val[0] - val[2])/val[4])) + 1;
    nj = (long)(0.5+fabs((val[1] - val[3])/val[5])) + 1;

    *basic_angle  = 360;
    *sub_division = lcm(ni,nj);

#if 0
    printf("ni = %ld, nj = %ld , basic_angle=%ld sub_division = %ld\n",
            ni,nj,
            *basic_angle,*sub_division);
#endif
    if(*sub_division < 0)
        return 0;

    Assert(*sub_division >= 0);

    return is_ok(val,v,*basic_angle,*sub_division);
}

static int pack_double(grib_accessor* a, const double* val, size_t *len)
{
    grib_accessor_g2grid* self = (grib_accessor_g2grid*)a;
    int ret;
    long v[6];
    int n;
    long basic_angle;
    long sub_division;

    if(*len < 6){
        ret = GRIB_ARRAY_TOO_SMALL;
        return ret;
    }

    /* printf("pack_double %g %g %g %g %g %g\n",val[0],val[1],val[2],val[3],val[4],val[5]);*/

    if(is_ok(val,v,1,1000000))
    {
        basic_angle = 1;
        sub_division = 1000000;
    }
    else if(trial(val,v,&basic_angle,&sub_division))
    {

    }
    else
    {
        basic_angle  = 1;
        sub_division = 1000000;

        if(!is_ok(val,v,basic_angle,sub_division))
            grib_context_log(a->context,GRIB_LOG_DEBUG,"Grid cannot be coded with any loss of precision");
    }

    if(basic_angle == 1 && sub_division == 1000000)
    {
        basic_angle = 0;
        sub_division = GRIB_MISSING_LONG;
    }

    if((ret = grib_set_long_internal(grib_handle_of_accessor(a), self->basic_angle,basic_angle)) != GRIB_SUCCESS)
        return ret;

    if((ret = grib_set_long_internal(grib_handle_of_accessor(a), self->sub_division,sub_division)) != GRIB_SUCCESS)
        return ret;

    n = 0;
    if((ret = grib_set_long_internal(grib_handle_of_accessor(a), self->latitude_first,v[n++])) != GRIB_SUCCESS)
        return ret;

    if((ret = grib_set_long_internal(grib_handle_of_accessor(a), self->longitude_first,v[n++])) != GRIB_SUCCESS)
        return ret;

    if((ret = grib_set_long_internal(grib_handle_of_accessor(a), self->latitude_last,v[n++])) != GRIB_SUCCESS)
        return ret;

    if((ret = grib_set_long_internal(grib_handle_of_accessor(a), self->longitude_last,v[n++])) != GRIB_SUCCESS)
        return ret;

    if(!self->i_increment) n++;
    else
        if((ret = grib_set_long_internal(grib_handle_of_accessor(a), self->i_increment,v[n++])) != GRIB_SUCCESS)
            return ret;

    if(!self->j_increment) n++;
    else
        if((ret = grib_set_long_internal(grib_handle_of_accessor(a), self->j_increment,v[n++])) != GRIB_SUCCESS)
            return ret;

    return GRIB_SUCCESS;
}
