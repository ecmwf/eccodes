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
   IMPLEMENTS = init; get_native_type
   IMPLEMENTS = pack_long;
   MEMBERS    = const char* doExtractSubsets
   MEMBERS    = const char* numberOfSubsets
   MEMBERS    = const char* extractSubsetList
   MEMBERS    = const char* extractAreaWestLongitude
   MEMBERS    = const char* extractAreaEastLongitude
   MEMBERS    = const char* extractAreaNorthLatitude
   MEMBERS    = const char* extractAreaSouthLatitude
   MEMBERS    = const char* extractAreaLongitudeRank
   MEMBERS    = const char* extractAreaLatitudeRank
   MEMBERS    = const char* extractedAreaNumberOfSubsets
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
static void init(grib_accessor*,const long, grib_arguments* );
static void init_class(grib_accessor_class*);

typedef struct grib_accessor_bufr_extract_area_subsets {
    grib_accessor          att;
/* Members defined in gen */
/* Members defined in bufr_extract_area_subsets */
	const char* doExtractSubsets;
	const char* numberOfSubsets;
	const char* extractSubsetList;
	const char* extractAreaWestLongitude;
	const char* extractAreaEastLongitude;
	const char* extractAreaNorthLatitude;
	const char* extractAreaSouthLatitude;
	const char* extractAreaLongitudeRank;
	const char* extractAreaLatitudeRank;
	const char* extractedAreaNumberOfSubsets;
} grib_accessor_bufr_extract_area_subsets;

extern grib_accessor_class* grib_accessor_class_gen;

static grib_accessor_class _grib_accessor_class_bufr_extract_area_subsets = {
    &grib_accessor_class_gen,                      /* super                     */
    "bufr_extract_area_subsets",                      /* name                      */
    sizeof(grib_accessor_bufr_extract_area_subsets),  /* size                      */
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
    &get_native_type,            /* get native type               */
    0,                /* get sub_section                */
    0,               /* grib_pack procedures long      */
    0,                 /* grib_pack procedures long      */
    &pack_long,                  /* grib_pack procedures long      */
    0,                /* grib_unpack procedures long    */
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


grib_accessor_class* grib_accessor_class_bufr_extract_area_subsets = &_grib_accessor_class_bufr_extract_area_subsets;


static void init_class(grib_accessor_class* c)
{
	c->dump	=	(*(c->super))->dump;
	c->next_offset	=	(*(c->super))->next_offset;
	c->string_length	=	(*(c->super))->string_length;
	c->value_count	=	(*(c->super))->value_count;
	c->byte_count	=	(*(c->super))->byte_count;
	c->byte_offset	=	(*(c->super))->byte_offset;
	c->sub_section	=	(*(c->super))->sub_section;
	c->pack_missing	=	(*(c->super))->pack_missing;
	c->is_missing	=	(*(c->super))->is_missing;
	c->unpack_long	=	(*(c->super))->unpack_long;
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

static void init(grib_accessor* a, const long len , grib_arguments* arg )
{
    int n=0;
    grib_accessor_bufr_extract_area_subsets *self =(grib_accessor_bufr_extract_area_subsets*)a;

    a->length=0;
    self->doExtractSubsets = grib_arguments_get_name(grib_handle_of_accessor(a),arg,n++);
    self->numberOfSubsets = grib_arguments_get_name(grib_handle_of_accessor(a),arg,n++);
    self->extractSubsetList = grib_arguments_get_name(grib_handle_of_accessor(a),arg,n++);
    self->extractAreaWestLongitude = grib_arguments_get_name(grib_handle_of_accessor(a),arg,n++);
    self->extractAreaEastLongitude = grib_arguments_get_name(grib_handle_of_accessor(a),arg,n++);
    self->extractAreaNorthLatitude = grib_arguments_get_name(grib_handle_of_accessor(a),arg,n++);
    self->extractAreaSouthLatitude = grib_arguments_get_name(grib_handle_of_accessor(a),arg,n++);
    self->extractAreaLongitudeRank = grib_arguments_get_name(grib_handle_of_accessor(a),arg,n++);
    self->extractAreaLatitudeRank = grib_arguments_get_name(grib_handle_of_accessor(a),arg,n++);
    self->extractedAreaNumberOfSubsets = grib_arguments_get_name(grib_handle_of_accessor(a),arg,n++);

    a->flags |= GRIB_ACCESSOR_FLAG_FUNCTION;
}

static int get_native_type(grib_accessor* a)
{
    return GRIB_TYPE_LONG;
}

/* Copy first element of array into all others */
static void fill_in(double a[], long length)
{
    long i;
    for (i=1; i<length; ++i) a[i] = a[0];
}
static int select_area(grib_accessor* a)
{
    int ret=0;
    long compressed=0;
    grib_accessor_bufr_extract_area_subsets *self =(grib_accessor_bufr_extract_area_subsets*)a;
    grib_handle* h=grib_handle_of_accessor(a);
    grib_context* c=h->context;

    double *lat=0;
    double *lon=0;
    size_t n;
    double lonWest,lonEast,latNorth,latSouth;
    long numberOfSubsets,i,latRank,lonRank;
    grib_iarray* subsets;
    long *subsets_ar=0;
    size_t nsubsets=0;
    char latstr[20]={0,};
    char lonstr[20]={0,};

    ret=grib_get_long(h,"compressedData",&compressed);
    if (ret) return ret;

    ret=grib_get_long(h,self->numberOfSubsets,&numberOfSubsets);
    if (ret) return ret;

    subsets=grib_iarray_new(c,numberOfSubsets,10);

    ret=grib_set_long(h,"unpack",1);
    if (ret) return ret;

    if (compressed) {
        ret=grib_get_long(h,self->extractAreaLongitudeRank,&lonRank);
        if (ret) return ret;
        sprintf(lonstr,"#%ld#longitude",lonRank);
        ret=grib_get_long(h,self->extractAreaLatitudeRank,&latRank);
        if (ret) return ret;
        sprintf(latstr,"#%ld#latitude",latRank);
    }

    /* Latitudes */
    n=numberOfSubsets;
    lat=(double*)grib_context_malloc_clear(c,sizeof(double)*numberOfSubsets);
    if (compressed) {
        ret=grib_get_double_array(h,latstr,lat,&n);
        if (ret) return ret;
        if (! (n==1 || n==numberOfSubsets) ) {
            /* n can be 1 if all latitudes are the same */
            return GRIB_INTERNAL_ERROR;
        }
        if (n == 1) {
            fill_in(lat, numberOfSubsets);
        }
    } else {
        size_t values_len=0;
        for(i=0; i<numberOfSubsets; ++i) {
            sprintf(latstr,"#%ld#latitude", i+1);
            ret = grib_get_size(h, latstr, &values_len); if (ret) return ret;
            if (values_len>1)
                return GRIB_NOT_IMPLEMENTED;
            ret=grib_get_double(h,latstr,&(lat[i]));
            if (ret) return ret;
        }
    }

    /* Longitudes */
    n=numberOfSubsets;
    lon=(double*)grib_context_malloc_clear(c,sizeof(double)*numberOfSubsets);
    if (compressed) {
        ret=grib_get_double_array(h,lonstr,lon,&n);
        if (ret) return ret;
        if (! (n==1 || n==numberOfSubsets) ) {
            /* n can be 1 if all longitudes are the same */
            return GRIB_INTERNAL_ERROR;
        }
        if (n == 1) {
            fill_in(lon, numberOfSubsets);
        }
    } else {
        size_t values_len=0;
        for(i=0; i<numberOfSubsets; ++i) {
            sprintf(lonstr,"#%ld#longitude", i+1);
            ret = grib_get_size(h, lonstr, &values_len); if (ret) return ret;
            if (values_len>1)
                return GRIB_NOT_IMPLEMENTED;
            ret=grib_get_double(h,lonstr,&(lon[i]));
            if (ret) return ret;
        }
    }

    ret=grib_get_double(h,self->extractAreaWestLongitude,&lonWest);
    if (ret) return ret;
    ret=grib_get_double(h,self->extractAreaEastLongitude,&lonEast);
    if (ret) return ret;
    ret=grib_get_double(h,self->extractAreaNorthLatitude,&latNorth);
    if (ret) return ret;
    ret=grib_get_double(h,self->extractAreaSouthLatitude,&latSouth);
    if (ret) return ret;

    for (i=0;i<numberOfSubsets;i++) {
        /* printf("++++++ lat: %g <= %g <= %g lon: %g <= %g <= %g \n",latSouth,lat[i],latNorth,lonWest,lon[i],lonEast); */
        if (lat[i]>=latSouth && lat[i]<=latNorth && lon[i]>=lonWest && lon[i]<=lonEast) {
            grib_iarray_push(subsets,i+1);
            /* printf("++++++++ %ld\n",i+1); */
        }
    }

    nsubsets=grib_iarray_used_size(subsets);
    ret=grib_set_long(h,self->extractedAreaNumberOfSubsets,nsubsets);
    if (ret) return ret;

    if (nsubsets!=0) {
        subsets_ar=grib_iarray_get_array(subsets);
        ret=grib_set_long_array(h,self->extractSubsetList,subsets_ar,nsubsets);
        if (ret) return ret;

        ret=grib_set_long(h,self->doExtractSubsets,1);
        if (ret) return ret;
    }

    grib_context_free(c,lat);
    grib_context_free(c,lon);
    grib_iarray_delete(subsets);
    subsets=0;

    return ret;
}

static int pack_long(grib_accessor* a, const long* val, size_t *len)
{
    int err=0;
    /*grib_accessor_bufr_extract_area_subsets *self =(grib_accessor_bufr_extract_area_subsets*)a;*/

    if (*len==0) return GRIB_SUCCESS;
    err=select_area(a);
    if (err) return err;

    return err;
}
