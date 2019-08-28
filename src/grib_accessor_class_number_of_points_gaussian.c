/*
 * Copyright 2005-2019 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

/**************************************
 * Enrico Fucile
 ************************************/

#include "grib_api_internal.h"


/*
   This is used by make_class.pl

   START_CLASS_DEF
   CLASS      = accessor
   SUPER      = grib_accessor_class_long
   IMPLEMENTS = unpack_long
   IMPLEMENTS = init
   MEMBERS = const char* ni
   MEMBERS = const char* nj
   MEMBERS = const char* plpresent
   MEMBERS = const char* pl
   MEMBERS = const char* order
   MEMBERS = const char* lat_first
   MEMBERS = const char* lon_first
   MEMBERS = const char* lat_last
   MEMBERS = const char* lon_last
   MEMBERS = const char* support_legacy
   END_CLASS_DEF
 */

/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "accessor.class" and rerun ./make_class.pl

*/

static int unpack_long(grib_accessor*, long* val,size_t *len);
static void init(grib_accessor*,const long, grib_arguments* );
static void init_class(grib_accessor_class*);

typedef struct grib_accessor_number_of_points_gaussian {
    grib_accessor          att;
/* Members defined in gen */
/* Members defined in long */
/* Members defined in number_of_points_gaussian */
	const char* ni;
	const char* nj;
	const char* plpresent;
	const char* pl;
	const char* order;
	const char* lat_first;
	const char* lon_first;
	const char* lat_last;
	const char* lon_last;
	const char* support_legacy;
} grib_accessor_number_of_points_gaussian;

extern grib_accessor_class* grib_accessor_class_long;

static grib_accessor_class _grib_accessor_class_number_of_points_gaussian = {
    &grib_accessor_class_long,                      /* super                     */
    "number_of_points_gaussian",                      /* name                      */
    sizeof(grib_accessor_number_of_points_gaussian),  /* size                      */
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
    0,                  /* grib_pack procedures long      */
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


grib_accessor_class* grib_accessor_class_number_of_points_gaussian = &_grib_accessor_class_number_of_points_gaussian;


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
	c->pack_long	=	(*(c->super))->pack_long;
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

#define EFDEBUG 0

static void init(grib_accessor* a,const long l, grib_arguments* c)
{
    int n=0;
    grib_handle* h = grib_handle_of_accessor(a);
    grib_accessor_number_of_points_gaussian* self = (grib_accessor_number_of_points_gaussian*)a;
    self->ni = grib_arguments_get_name(h,c,n++);
    self->nj = grib_arguments_get_name(h,c,n++);
    self->plpresent = grib_arguments_get_name(h,c,n++);
    self->pl = grib_arguments_get_name(h,c,n++);
    self->order = grib_arguments_get_name(h,c,n++);
    self->lat_first = grib_arguments_get_name(h,c,n++);
    self->lon_first = grib_arguments_get_name(h,c,n++);
    self->lat_last = grib_arguments_get_name(h,c,n++);
    self->lon_last = grib_arguments_get_name(h,c,n++);
    self->support_legacy = grib_arguments_get_name(h,c,n++);
    a->flags  |= GRIB_ACCESSOR_FLAG_READ_ONLY;
    a->flags |= GRIB_ACCESSOR_FLAG_FUNCTION;
    a->length=0;
}
#if 0
/*Legacy mode*/
static long num_points_reduced_gauss_old(grib_handle* h, long nj, long pl[],
                                         long max_pl, double lats[],
                                         double angular_precision,
                                         double lat_first, double lat_last,
                                         double lon_first, double lon_last)
{
    long result=0;
    int is_global=0;
    size_t plsize=0;
    long ilon_first=0,ilon_last=0;
    double lon_first_row=0,lon_last_row=0;
    float d = 0;
    is_global=is_gaussian_global(lat_first,lat_last,lon_first,lon_last,max_pl,lats,angular_precision);
    d=fabs(lats[0]-lats[1]);
    if ( !is_global ) {
        long j = 0;
        /*sub area*/
        (void)d;
#if EFDEBUG
        printf("-------- subarea fabs(lat_first-lats[0])=%g d=%g\n",fabs(lat_first-lats[0]),d);
        printf("-------- subarea fabs(lat_last+lats[0])=%g d=%g\n",fabs(lat_last+lats[0]),d);
        printf("-------- subarea lon_last=%g order=%ld 360.0-90.0/order=%g\n",
                lon_last,order,360.0-90.0/order);
        printf("-------- subarea lon_first=%g fabs(lon_last  -( 360.0-90.0/order))=%g 90.0/order=%g\n",
                lon_first,fabs(lon_last  - (360.0-90.0/order)),90.0/order);
#endif
        for (j=0;j<nj;j++) {
            long row_count=0;
#if EFDEBUG
            printf("--  %d ",j);
#endif
            grib_get_reduced_row(pl[j],lon_first,lon_last,&row_count,&ilon_first,&ilon_last);
            lon_first_row=((ilon_first)*360.0)/pl[j];
            lon_last_row=((ilon_last)*360.0)/pl[j];
            result += row_count;
            (void)lon_last_row;
            (void)lon_first_row;
#if EFDEBUG
            printf("        ilon_first=%ld lon_first=%.10e ilon_last=%ld lon_last=%.10e count=%ld row_count=%ld\n",
                    ilon_first,lon_first_row,ilon_last,lon_last_row,result,row_count);
#endif
        }
    } else {
        int i = 0;
        result=0;
        for (i=0;i<plsize;i++) result += pl[i];
    }
    return result;
}

/* New MIR compatible way */
static long num_points_reduced_gauss_new(grib_handle* h, long nj, long pl[], double lon_first, double lon_last)
{
    long result = 0;
    long j;
    /* Always assume sub area */
    for (j=0;j<nj;j++) {
        long row_count=0;
        long ilon_first=0,ilon_last=0;
        grib_get_reduced_row2(pl[j], lon_first, lon_last, &row_count, &ilon_first, &ilon_last);
        result += row_count;
    }
    return result;
}
#endif

static int angleApproximatelyEqual(double A, double B, double angular_precision)
{
    return angular_precision > 0 ? (fabs(A-B)<=angular_precision) : (A == B);
}

static double longitude_normalise(double lon, double minimum)
{
    while (lon < minimum) {
        lon += 360;
    }
    while (lon >= minimum + 360) {
        lon -= 360;
    }
    return lon;
}

static void correctWestEast(long max_pl, double angular_precision, double* pWest, double* pEast)
{
    double w, e;
    const double inc = 360.0/max_pl; /*smallest increment*/
    if (*pWest > *pEast) *pEast += 360;

    w = *pWest;
    e = *pEast;

    if (angleApproximatelyEqual(0, w, angular_precision)) {
        const int cond1 = angleApproximatelyEqual(360 - inc, e - w, angular_precision);
        const int cond2 = (360 - inc < e - w);
        const int cond3 = (e != w);
        const int cond4 = longitude_normalise(e,w)==w;    /* e.normalise(w) == w */
        if ( cond1 || cond2 || (cond3 && cond4) ) {
            *pWest = 0;
            *pEast = 360 - inc;
        }
    }
}

static int get_number_of_data_values(grib_handle* h, size_t* numDataValues)
{
    int err = 0;
    long bpv=0, bitmapPresent=0;
    size_t bitmapLength = 0;

    if ( (err = grib_get_long(h, "bitsPerValue", &bpv)) ) return err;

    if (bpv != 0) {
        if (grib_get_size(h, "values", numDataValues)==GRIB_SUCCESS) {
            return GRIB_SUCCESS;
        }
    } else {
        /* Constant field (with or without bitmap) */
        if ( (err = grib_get_long(h,"bitmapPresent", &bitmapPresent)) ) return err;
        if (bitmapPresent) {
            if ( (err = grib_get_size(h,"bitmap", &bitmapLength)) ) return err;
            *numDataValues = bitmapLength;
            return GRIB_SUCCESS;
        } else {
            err = GRIB_NO_VALUES; /* Cannot determine number of values */
        }
    }

    return err;
}

static int unpack_long_with_legacy_support(grib_accessor* a, long* val, size_t *len);
static int unpack_long_new(grib_accessor* a, long* val, size_t *len);

static int unpack_long(grib_accessor* a, long* val, size_t *len)
{
    int ret=GRIB_SUCCESS;
    long support_legacy=1;
    grib_accessor_number_of_points_gaussian* self = (grib_accessor_number_of_points_gaussian*)a;
    grib_handle* h = grib_handle_of_accessor(a);

    if((ret = grib_get_long_internal(h, self->support_legacy,&support_legacy)) != GRIB_SUCCESS)
        return ret;
    if (support_legacy==1) return unpack_long_with_legacy_support(a, val, len);
    else                   return unpack_long_new(a, val, len);
}

/* New algorithm */
static int unpack_long_new(grib_accessor* a, long* val, size_t *len)
{
    int ret=GRIB_SUCCESS;
    int is_global = 0;
    long ni=0,nj=0,plpresent=0,order=0;
    size_t plsize=0;
    double* lats={0,};
    double lat_first,lat_last,lon_first,lon_last;
    long* pl=NULL;
    long* plsave=NULL;
    long row_count;
    long ilon_first=0,ilon_last=0;
    double angular_precision = 1.0/1000000.0;
    long editionNumber = 0;
    grib_handle* h = grib_handle_of_accessor(a);

    grib_accessor_number_of_points_gaussian* self = (grib_accessor_number_of_points_gaussian*)a;
    grib_context* c=a->context;

    if((ret = grib_get_long_internal(h, self->ni,&ni)) != GRIB_SUCCESS)
        return ret;

    if((ret = grib_get_long_internal(h, self->nj,&nj)) != GRIB_SUCCESS)
        return ret;

    if((ret = grib_get_long_internal(h, self->plpresent,&plpresent)) != GRIB_SUCCESS)
        return ret;

    if (nj == 0) return GRIB_GEOCALCULUS_PROBLEM;

    if (grib_get_long(h, "editionNumber", &editionNumber)==GRIB_SUCCESS) {
        if (editionNumber == 1) angular_precision = 1.0/1000;
    }

    if (plpresent) {
        long max_pl=0;
        float d = 0;
        int j=0;
        double lon_first_row=0,lon_last_row=0;

        /*reduced*/
        if((ret = grib_get_long_internal(h, self->order,&order)) != GRIB_SUCCESS)
            return ret;
        if((ret = grib_get_double_internal(h, self->lat_first,&lat_first)) != GRIB_SUCCESS)
            return ret;
        if((ret = grib_get_double_internal(h, self->lon_first,&lon_first)) != GRIB_SUCCESS)
            return ret;
        if((ret = grib_get_double_internal(h, self->lat_last,&lat_last)) != GRIB_SUCCESS)
            return ret;
        if((ret = grib_get_double_internal(h, self->lon_last,&lon_last)) != GRIB_SUCCESS)
            return ret;

        lats=(double*)grib_context_malloc(a->context,sizeof(double)*order*2);
        if((ret = grib_get_gaussian_latitudes(order, lats)) != GRIB_SUCCESS)
            return ret;

        if((ret = grib_get_size(h,self->pl,&plsize)) != GRIB_SUCCESS)
            return ret;

        pl=(long*)grib_context_malloc_clear(c,sizeof(long)*plsize);
        plsave=pl;
        grib_get_long_array_internal(h,self->pl,pl, &plsize);

        if (lon_last<0) lon_last+=360;
        if (lon_first<0) lon_first+=360;

        /* Find the maximum element of "pl" array, do not assume it's 4*N! */
        /* This could be an Octahedral Gaussian Grid */
        max_pl = pl[0];
        for (j=1; j<plsize; j++) {
            if (pl[j] > max_pl) max_pl = pl[j];
        }

        d=fabs(lats[0]-lats[1]);
        is_global = 0; /* ECC-445 */

        correctWestEast(max_pl, angular_precision, &lon_first, &lon_last);

        if ( !is_global ) {
            /*sub area*/
            (void)d;
            *val=0;
            for (j=0;j<nj;j++) {
                row_count=0;
                grib_get_reduced_row_wrapper(h, pl[j],lon_first,lon_last,&row_count,&ilon_first,&ilon_last);
                lon_first_row=((ilon_first)*360.0)/pl[j];
                lon_last_row=((ilon_last)*360.0)/pl[j];
                *val+=row_count;
                (void)lon_last_row;
                (void)lon_first_row;
            }
        } else {
            int i = 0;
            *val=0;
            for (i=0;i<plsize;i++) *val+=pl[i];
        }
    } else {
        /*regular*/
        *val=ni*nj;
    }
    if (lats) grib_context_free(c,lats);
    if (plsave) grib_context_free(c,plsave);

    return ret;
}


/* With Legacy support */
static int unpack_long_with_legacy_support(grib_accessor* a, long* val, size_t *len)
{
    int ret=GRIB_SUCCESS;
    int is_global = 0;
    long ni=0,nj=0,plpresent=0,order=0;
    size_t plsize=0;
    double* lats={0,};
    double lat_first,lat_last,lon_first,lon_last;
    long* pl=NULL;
    long* plsave=NULL;
    long row_count;
    long ilon_first=0,ilon_last=0;
    double angular_precision = 1.0/1000000.0;
    long editionNumber = 0;
    grib_handle* h = grib_handle_of_accessor(a);
    size_t numDataValues=0;

    grib_accessor_number_of_points_gaussian* self = (grib_accessor_number_of_points_gaussian*)a;
    grib_context* c=a->context;

    if((ret = grib_get_long_internal(h, self->ni,&ni)) != GRIB_SUCCESS)
        return ret;

    if((ret = grib_get_long_internal(h, self->nj,&nj)) != GRIB_SUCCESS)
        return ret;

    if((ret = grib_get_long_internal(h, self->plpresent,&plpresent)) != GRIB_SUCCESS)
        return ret;

    if (nj == 0) return GRIB_GEOCALCULUS_PROBLEM;

    if (grib_get_long(h, "editionNumber", &editionNumber)==GRIB_SUCCESS) {
        if (editionNumber == 1) angular_precision = 1.0/1000;
    }

    if (plpresent) {
        long max_pl=0;
        float d = 0;
        int j=0;
        double lon_first_row=0,lon_last_row=0;

        /*reduced*/
        if((ret = grib_get_long_internal(h, self->order,&order)) != GRIB_SUCCESS)
            return ret;
        if((ret = grib_get_double_internal(h, self->lat_first,&lat_first)) != GRIB_SUCCESS)
            return ret;
        if((ret = grib_get_double_internal(h, self->lon_first,&lon_first)) != GRIB_SUCCESS)
            return ret;
        if((ret = grib_get_double_internal(h, self->lat_last,&lat_last)) != GRIB_SUCCESS)
            return ret;
        if((ret = grib_get_double_internal(h, self->lon_last,&lon_last)) != GRIB_SUCCESS)
            return ret;

        lats=(double*)grib_context_malloc(a->context,sizeof(double)*order*2);
        if((ret = grib_get_gaussian_latitudes(order, lats)) != GRIB_SUCCESS)
            return ret;

        if((ret = grib_get_size(h,self->pl,&plsize)) != GRIB_SUCCESS)
            return ret;

        pl=(long*)grib_context_malloc_clear(c,sizeof(long)*plsize);
        plsave=pl;
        grib_get_long_array_internal(h,self->pl,pl, &plsize);

        if (lon_last<0) lon_last+=360;
        if (lon_first<0) lon_first+=360;

        /* Find the maximum element of "pl" array, do not assume it's 4*N! */
        /* This could be an Octahedral Gaussian Grid */
        max_pl = pl[0];
        for (j=1; j<plsize; j++) {
            if (pl[j] > max_pl) max_pl = pl[j];
        }

        /*is_global=is_gaussian_global(lat_first,lat_last,lon_first,lon_last,max_pl,lats,angular_precision);*/
        d=fabs(lats[0]-lats[1]);
        is_global = 0; /* ECC-445 */

        correctWestEast(max_pl, angular_precision, &lon_first, &lon_last);

        if ( !is_global ) {
            /*sub area*/
            (void)d;
#if EFDEBUG
            printf("-------- subarea fabs(lat_first-lats[0])=%g d=%g\n",fabs(lat_first-lats[0]),d);
            printf("-------- subarea fabs(lat_last+lats[0])=%g d=%g\n",fabs(lat_last+lats[0]),d);
            printf("-------- subarea lon_last=%g order=%ld 360.0-90.0/order=%g\n",
                    lon_last,order,360.0-90.0/order);
            printf("-------- subarea lon_first=%g fabs(lon_last  -( 360.0-90.0/order))=%g 90.0/order=%g\n",
                    lon_first,fabs(lon_last  - (360.0-90.0/order)),90.0/order);
#endif
            *val=0;
            for (j=0;j<nj;j++) {
                row_count=0;
#if EFDEBUG
                printf("--  %d ",j);
#endif
                grib_get_reduced_row_wrapper(h, pl[j],lon_first,lon_last,&row_count,&ilon_first,&ilon_last);
#if 0
                if ( row_count != pl[j] ) {
                    printf("oops...... rc=%ld but pl[%d]=%ld\n", row_count, j,pl[j]);
                }
#endif
                lon_first_row=((ilon_first)*360.0)/pl[j];
                lon_last_row=((ilon_last)*360.0)/pl[j];
                *val+=row_count;
                (void)lon_last_row;
                (void)lon_first_row;
#if EFDEBUG
                printf("        ilon_first=%ld lon_first=%.10e ilon_last=%ld lon_last=%.10e count=%ld row_count=%ld\n",
                        ilon_first,lon_first_row,ilon_last,lon_last_row,*val,row_count);
#endif
            }
        } else {
            int i = 0;
            *val=0;
            for (i=0;i<plsize;i++) *val+=pl[i];
        }
    } else {
        /*regular*/
        *val=ni*nj;
    }
#if EFDEBUG
    printf("DEBUG:     number_of_points_gaussian=%ld plpresent=%ld plsize=%d\n",*val,plpresent,plsize);
    for (i=0;i<plsize;i++) printf(" DEBUG: pl[%d]=%ld\n",i,pl[i]);
#endif
    if (lats) grib_context_free(c,lats);
    if (plsave) grib_context_free(c,plsave);

    /* ECC-756: Now decide whether this is legacy GRIB1 message. */
    /* Query data values to see if there is a mismatch */
    if (get_number_of_data_values(h, &numDataValues) == GRIB_SUCCESS) {
        if (*val != numDataValues) {
            if (h->context->debug)
                fprintf(stderr, "ECCODES DEBUG number_of_points_gaussian: LEGACY MODE activated. "
                       "Count(=%ld) changed to num values(=%ld)\n",*val,(long)numDataValues);
            *val = numDataValues;
        }
    }

    return ret;
}
