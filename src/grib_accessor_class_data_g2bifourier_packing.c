/*
 * Copyright 2005-2019 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

/********************************
 *   philippe.marguinaud@meteo.fr
 *******************************/

#include "grib_api_internal.h"
#include "grib_optimize_decimal_factor.h"
#include <math.h>

/*
   This is used by make_class.pl

   START_CLASS_DEF
   CLASS      = accessor
   SUPER      = grib_accessor_class_data_simple_packing
   IMPLEMENTS = init
   IMPLEMENTS = unpack_double
   IMPLEMENTS = pack_double
   IMPLEMENTS = value_count
   MEMBERS= const char*  ieee_floats
   MEMBERS= const char*  laplacianOperatorIsSet
   MEMBERS= const char*  laplacianOperator
   MEMBERS= const char*  biFourierTruncationType
   MEMBERS= const char*  sub_i
   MEMBERS= const char*  sub_j
   MEMBERS= const char*  bif_i
   MEMBERS= const char*  bif_j
   MEMBERS= const char*  biFourierSubTruncationType
   MEMBERS= const char*  biFourierDoNotPackAxes
   MEMBERS= const char*  biFourierMakeTemplate
   MEMBERS= const char*  totalNumberOfValuesInUnpackedSubset
   MEMBERS=const char*  numberOfValues
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

typedef struct grib_accessor_data_g2bifourier_packing {
    grib_accessor          att;
/* Members defined in gen */
/* Members defined in values */
	int  carg;
	const char* seclen;
	const char* offsetdata;
	const char* offsetsection;
	int dirty;
/* Members defined in data_simple_packing */
	int edition;
	const char*  units_factor;
	const char*  units_bias;
	const char*  changing_precision;
	const char*  number_of_values;
	const char*  bits_per_value;
	const char*  reference_value;
	const char*  binary_scale_factor;
	const char*  decimal_scale_factor;
	const char*  optimize_scaling_factor;
/* Members defined in data_g2bifourier_packing */
	const char*  ieee_floats;
	const char*  laplacianOperatorIsSet;
	const char*  laplacianOperator;
	const char*  biFourierTruncationType;
	const char*  sub_i;
	const char*  sub_j;
	const char*  bif_i;
	const char*  bif_j;
	const char*  biFourierSubTruncationType;
	const char*  biFourierDoNotPackAxes;
	const char*  biFourierMakeTemplate;
	const char*  totalNumberOfValuesInUnpackedSubset;
	const char*  numberOfValues;
} grib_accessor_data_g2bifourier_packing;

extern grib_accessor_class* grib_accessor_class_data_simple_packing;

static grib_accessor_class _grib_accessor_class_data_g2bifourier_packing = {
    &grib_accessor_class_data_simple_packing,                      /* super                     */
    "data_g2bifourier_packing",                      /* name                      */
    sizeof(grib_accessor_data_g2bifourier_packing),  /* size                      */
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


grib_accessor_class* grib_accessor_class_data_g2bifourier_packing = &_grib_accessor_class_data_g2bifourier_packing;


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

typedef unsigned long (*encode_float_proc)(double);
typedef double        (*decode_float_proc)(unsigned long);

static void init(grib_accessor* a,const long v, grib_arguments* args)
{
    grib_accessor_data_g2bifourier_packing *self =(grib_accessor_data_g2bifourier_packing*)a;
    grib_handle* gh = grib_handle_of_accessor(a);

    self->ieee_floats                = grib_arguments_get_name(gh,args,self->carg++);
    self->laplacianOperatorIsSet     = grib_arguments_get_name(gh,args,self->carg++);
    self->laplacianOperator          = grib_arguments_get_name(gh,args,self->carg++);
    self->biFourierTruncationType    = grib_arguments_get_name(gh,args,self->carg++);
    self->sub_i                      = grib_arguments_get_name(gh,args,self->carg++);
    self->sub_j                      = grib_arguments_get_name(gh,args,self->carg++);
    self->bif_i                      = grib_arguments_get_name(gh,args,self->carg++);
    self->bif_j                      = grib_arguments_get_name(gh,args,self->carg++);
    self->biFourierSubTruncationType = grib_arguments_get_name(gh,args,self->carg++);
    self->biFourierDoNotPackAxes     = grib_arguments_get_name(gh,args,self->carg++);
    self->biFourierMakeTemplate      = grib_arguments_get_name(gh,args,self->carg++);
    self->totalNumberOfValuesInUnpackedSubset = grib_arguments_get_name(gh,args,self->carg++);
    /*self->numberOfValues             = grib_arguments_get_name(gh,args,self->carg++);*/

    a->flags |= GRIB_ACCESSOR_FLAG_DATA;
    self->dirty=1;
}

static int value_count(grib_accessor* a,long* numberOfValues)
{
    grib_accessor_data_g2bifourier_packing* self =  (grib_accessor_data_g2bifourier_packing*)a;
    grib_handle* gh = grib_handle_of_accessor(a);
    *numberOfValues=0;

    return grib_get_long_internal(gh,self->number_of_values,numberOfValues);
}

#define MAX(x,y) ((x) > (y) ? (x) : (y))
#define MIN(x,y) ((x) < (y) ? (x) : (y))

static void ellipse (long ni, long nj, long itrunc[], long jtrunc[])
{
    const double zeps = 1.E-10;
    const double zauxil=0.;
    int i, j;
    double zi, zj;

    /*
     * 1. Computing meridional limit wavenumbers along zonal wavenumbers
     */

    for (j = 1; j < nj; j++)
    {
        zi = (double)ni / (double)nj * sqrt (MAX (zauxil, (double)(nj * nj - j * j)));
        itrunc[j] = (int)(zi + zeps);
    }

    if (nj == 0)
    {
        itrunc[0] = ni;
    }
    else
    {
        itrunc[0] = ni;
        itrunc[nj] = 0;
    }

    /*
     * 2. Computing zonal limit wavenumbers along meridional wavenumbers
     */

    for (i = 1; i < ni; i++)
    {
        zj = (double)nj / (double)ni * sqrt (MAX (zauxil, (double)(ni * ni - i * i)));
        jtrunc[i] = (int)(zj + zeps);
    }

    if (ni == 0)
    {
        jtrunc[0] = nj;
    }
    else
    {
        jtrunc[0] = nj;
        jtrunc[ni] = 0;
    }
}

static void rectangle (long ni, long nj, long itrunc[], long jtrunc[])
{
    int i, j;

    /*
     * 1. Computing meridional limit wavenumbers along zonal wavenumbers
     */

    for (j = 0; j <= nj; j++)
        itrunc[j] = ni;

    /*
     * 2. Computing zonal limit wavenumbers along meridional wavenumbers
     */

    for (i = 0; i <= ni; i++)
        jtrunc[i] = nj;

}

static void diamond (long ni, long nj, long itrunc[], long jtrunc[])
{
    int i, j;

    if (nj == 0)
        itrunc[0] = -1;
    else
        for (j = 0; j <= nj; j++)
            itrunc[j] = ni - (j * ni) / nj;

    if (ni == 0)
        jtrunc[0] = -1;
    else
        for (i = 0; i <= ni; i++)
            jtrunc[i] = nj - (i * nj) / ni;

}

#define scals(i,j) pow((double)((i)*(i)+(j)*(j)),bt->laplacianOperator)


#define for_ij() \
        for (j = 0; j <= bt->bif_j; j++) \
        for (i = 0; i <= bt->itruncation_bif[j]; i++)

#define calc_insub() \
        do {                                                                         \
            insub = (i <= bt->sub_i) && (j <= bt->sub_j);                            \
            if (insub)                                                               \
            {                                                                        \
                int insubi = (i <= bt->itruncation_sub[j]);                          \
                int insubj = (j <= bt->jtruncation_sub[i]);                          \
                insub = insubi && insubj;                                            \
            }                                                                        \
            if (bt->keepaxes)                                                        \
            insub = insub || (i == 0) || (j == 0);                                   \
        } while (0)

typedef struct bif_trunc_t
{
    long bits_per_value;
    long decimal_scale_factor;
    long binary_scale_factor;
    long ieee_floats;
    long laplacianOperatorIsSet;
    double laplacianOperator;
    double reference_value;
    long sub_i, sub_j, bif_i, bif_j;
    long biFourierTruncationType;
    long biFourierSubTruncationType;
    long keepaxes;
    long maketemplate;
    decode_float_proc decode_float;
    encode_float_proc encode_float;
    int bytes;
    long * itruncation_bif;
    long * jtruncation_bif;
    long * itruncation_sub;
    long * jtruncation_sub;
    size_t n_vals_bif, n_vals_sub;
} bif_trunc_t;

/*
 * Total number of coefficients
 */
static size_t size_bif (bif_trunc_t * bt)
{
    size_t n_vals = 0;
    int j;
    for (j = 0; j <= bt->bif_j; j++)
        n_vals += 4 * (bt->itruncation_bif[j] + 1);
    return n_vals;
}

/*
 * Number of unpacked coefficients
 */
static size_t size_sub (bif_trunc_t * bt)
{
    size_t n_vals = 0;
    int i, j;
    for_ij ()
    {
        int insub;

        calc_insub ();

        if (insub)
            n_vals += 4;
    }
    return n_vals;
}

static double laplam (bif_trunc_t * bt, const double val[])
{
    /*
     * For bi-Fourier spectral fields, the Laplacian operator is a multiplication by (i*i+j*j)
     */

    const double zeps = 1E-15;
    double * znorm = NULL, * zw = NULL;
    int kmax = 1 + bt->bif_i * bt->bif_i + bt->bif_j * bt->bif_j, lmax;
    int * itab1 = NULL, * itab2 = NULL;
    int i, j, k, l, isp;
    double zxmw, zymw, zwsum, zx, zy, zsum1, zsum2, zbeta1, zp;

    itab1 = (int *)malloc (sizeof (int) * kmax);
    itab2 = (int *)malloc (sizeof (int) * ((1 + bt->bif_i) * (1 + bt->bif_j)));

    for (k = 0; k < kmax; k++)
        itab1[k] = 0;

    /*
     * Keep record of the possible values of i**2+j**2 outside the non-packed truncation
     */
    for_ij ()
    {
        int insub;

        calc_insub ();

        if (! insub)
        {
            int k = i*i+j*j;
            itab1[k] = 1;
        }
    }

    l = 0;
    for (k = 0; k < kmax; k++)
        if (itab1[k])
        {
            itab2[l] = k;
            itab1[k] = l;
            l++;
        }
    lmax = l;

    /*
     * Now, itab2 contains all possible values of i*i+j*j, and itab1 contains
     * the rank of all i*i+j*j
     */
    znorm = (double *)malloc (sizeof (double) * lmax);
    zw    = (double *)malloc (sizeof (double) * lmax);

    /*
     * Compute norms of input field, gathered by values of i**2+j**2; we have to
     * go through the unpacked truncation again
     */
    for (l = 0; l < lmax; l++)
        znorm[l] = 0.;

    isp = 0;
    for_ij ()
    {
        int insub;

        calc_insub ();

        if (insub)
        {
            isp += 4;
        }
        else
        {
            int m, l = itab1[i*i+j*j];
            for (m = 0; m < 4; m++, isp++)
                znorm[l] = MAX (znorm[l], fabs (val[isp]));
        }
    }

    /*
     * Compute weights, fix very small norms to avoid problems with log function
     */
    for (l = 0; l < lmax; l++)
    {
        zw[l] = (double)lmax / (double)(l + 1);
        if (znorm[l] < zeps)
        {
            znorm[l] = zeps;
            zw[l] = 100. * zeps;
        }
    }

    /*
     * Sum weights
     */
    zxmw = 0.;
    zymw = 0.;
    zwsum = 0.;

    for (l = 0; l < lmax; l++)
    {
        zx = log (itab2[l]);
        zy = log (znorm[l]);
        zxmw += zx * zw[l];
        zymw += zy * zw[l];
        zwsum += zw[l];
    }

    /*
     * Least square regression
     */
    zxmw = zxmw / zwsum;
    zymw = zymw / zwsum;
    zsum1 = 0.;
    zsum2 = 0.;

    for (l = 0; l < lmax; l++)
    {
        zx = log (itab2[l]);
        zy = log (znorm[l]);
        zsum1 += zw[l] * (zy - zymw) * (zx - zxmw);
        zsum2 += zw[l] * (zx - zxmw) * (zx - zxmw);
    }

    zbeta1 = zsum1 / zsum2;
    zp = -zbeta1;
    zp = MAX (-9.999, MIN (9.999, zp));

    free (itab1);
    free (itab2);

    free (znorm);
    free (zw);

    /*zp = ((long)(zp * 1000.)) / 1000.; FAPULA rounds Laplacian power to 1/1000th*/

    return zp;
}

static void free_bif_trunc (bif_trunc_t * bt, grib_accessor * a)
{
    grib_handle* gh = grib_handle_of_accessor(a);
    if (bt == NULL)
        return;
    if (bt->itruncation_bif != NULL) free (bt->itruncation_bif);
    if (bt->jtruncation_bif != NULL) free (bt->jtruncation_bif);
    if (bt->itruncation_sub != NULL) free (bt->itruncation_sub);
    if (bt->jtruncation_sub != NULL) free (bt->jtruncation_sub);
    memset (bt, 0, sizeof (bif_trunc_t));
    grib_context_free (gh->context, bt);
}

static bif_trunc_t * new_bif_trunc (grib_accessor * a, grib_accessor_data_g2bifourier_packing * self)
{
    int ret;
    grib_handle* gh = grib_handle_of_accessor(a);
    bif_trunc_t * bt = (bif_trunc_t *)grib_context_malloc (gh->context, sizeof (bif_trunc_t));

    memset (bt, 0, sizeof (bif_trunc_t));

    if ((ret = grib_get_double_internal (gh, self->reference_value, &bt->reference_value)) != GRIB_SUCCESS)
        goto cleanup;
    if ((ret = grib_get_long_internal (gh, self->bits_per_value, &bt->bits_per_value)) != GRIB_SUCCESS)
        goto cleanup;
    if ((ret = grib_get_long_internal (gh, self->binary_scale_factor, &bt->binary_scale_factor)) != GRIB_SUCCESS)
        goto cleanup;
    if ((ret = grib_get_long_internal (gh, self->decimal_scale_factor, &bt->decimal_scale_factor)) != GRIB_SUCCESS)
        goto cleanup;
    if ((ret = grib_get_long_internal (gh, self->ieee_floats, &bt->ieee_floats)) != GRIB_SUCCESS)
        goto cleanup;
    if ((ret = grib_get_long_internal (gh, self->laplacianOperatorIsSet, &bt->laplacianOperatorIsSet)) != GRIB_SUCCESS)
        goto cleanup;
    if ((ret = grib_get_double_internal (gh, self->laplacianOperator, &bt->laplacianOperator)) != GRIB_SUCCESS)
        goto cleanup;
    if ((ret = grib_get_long_internal (gh, self->sub_i, &bt->sub_i)) != GRIB_SUCCESS)
        goto cleanup;
    if ((ret = grib_get_long_internal (gh, self->sub_j, &bt->sub_j)) != GRIB_SUCCESS)
        goto cleanup;
    if ((ret = grib_get_long_internal (gh, self->bif_i, &bt->bif_i)) != GRIB_SUCCESS)
        goto cleanup;
    if ((ret = grib_get_long_internal (gh, self->bif_j, &bt->bif_j)) != GRIB_SUCCESS)
        goto cleanup;
    if ((ret = grib_get_long_internal (gh, self->biFourierTruncationType, &bt->biFourierTruncationType)) != GRIB_SUCCESS)
        goto cleanup;
    if ((ret = grib_get_long_internal (gh, self->biFourierSubTruncationType, &bt->biFourierSubTruncationType)) != GRIB_SUCCESS)
        goto cleanup;
    if ((ret = grib_get_long_internal (gh, self->biFourierDoNotPackAxes, &bt->keepaxes)) != GRIB_SUCCESS)
        goto cleanup;
    if ((ret = grib_get_long_internal (gh, self->biFourierMakeTemplate, &bt->maketemplate)) != GRIB_SUCCESS)
        goto cleanup;


    switch (bt->ieee_floats)
    {
    case 0:
        bt->decode_float = grib_long_to_ibm;
        bt->encode_float = grib_ibm_to_long;
        bt->bytes        = 4;
        break;
    case 1:
        bt->decode_float = grib_long_to_ieee;
        bt->encode_float = grib_ieee_to_long;
        bt->bytes        = 4;
        break;
    case 2:
        bt->decode_float = grib_long_to_ieee64;
        bt->encode_float = grib_ieee64_to_long;
        bt->bytes        = 8;
        break;
    default:
        ret = GRIB_NOT_IMPLEMENTED;
        goto cleanup;
    }

    bt->itruncation_sub = (long *)grib_context_malloc(gh->context, sizeof(long)*(1+bt->sub_j));
    bt->jtruncation_sub = (long *)grib_context_malloc(gh->context, sizeof(long)*(1+bt->sub_i));
    bt->itruncation_bif = (long *)grib_context_malloc(gh->context, sizeof(long)*(1+bt->bif_j));
    bt->jtruncation_bif = (long *)grib_context_malloc(gh->context, sizeof(long)*(1+bt->bif_i));

#define RECTANGLE 77
#define ELLIPSE   88
#define DIAMOND   99

    switch (bt->biFourierTruncationType)
    {
    case RECTANGLE: rectangle (bt->bif_i, bt->bif_j, bt->itruncation_bif, bt->jtruncation_bif); break;
    case ELLIPSE  : ellipse   (bt->bif_i, bt->bif_j, bt->itruncation_bif, bt->jtruncation_bif); break;
    case DIAMOND  : diamond   (bt->bif_i, bt->bif_j, bt->itruncation_bif, bt->jtruncation_bif); break;
    default:
        ret = GRIB_INVALID_KEY_VALUE;
        goto cleanup;
    }
    switch (bt->biFourierSubTruncationType)
    {
    case RECTANGLE: rectangle (bt->sub_i, bt->sub_j, bt->itruncation_sub, bt->jtruncation_sub); break;
    case ELLIPSE  : ellipse   (bt->sub_i, bt->sub_j, bt->itruncation_sub, bt->jtruncation_sub); break;
    case DIAMOND  : diamond   (bt->sub_i, bt->sub_j, bt->itruncation_sub, bt->jtruncation_sub); break;
    default:
        ret = GRIB_INVALID_KEY_VALUE;
        goto cleanup;
    }

    bt->n_vals_bif = size_bif (bt);
    bt->n_vals_sub = size_sub (bt);

    return bt;

cleanup:

    free_bif_trunc (bt, a);

    return NULL;
}

static int unpack_double(grib_accessor* a, double* val, size_t *len)
{
    grib_accessor_data_g2bifourier_packing* self =  (grib_accessor_data_g2bifourier_packing*)a;
    grib_handle* gh = grib_handle_of_accessor(a);

    unsigned char* buf  = (unsigned char*)gh->buffer->data;
    unsigned char* hres = NULL;
    unsigned char* lres = NULL;
    unsigned long packed_offset;

    long   hpos = 0;
    long   lpos = 0;

    int isp;

    bif_trunc_t * bt = NULL;

    long count = 0;

    long   offsetdata = 0;

    double s = 0;
    double d = 0;
    int ret = GRIB_SUCCESS;
    int i, j, k;

    if ((ret = grib_value_count (a, &count)) != GRIB_SUCCESS)
        goto cleanup;

    bt = new_bif_trunc (a, self);

    if (bt == NULL)
    {
        ret = GRIB_INTERNAL_ERROR;
        goto cleanup;
    }

    if (bt->n_vals_bif != count)
    {
        ret = GRIB_INTERNAL_ERROR;
        goto cleanup;
    }

    if ((ret = grib_get_long_internal (gh, self->offsetdata, &offsetdata)) != GRIB_SUCCESS)
        goto cleanup;
    if (*len < bt->n_vals_bif)
    {
        *len = (long)bt->n_vals_bif;
        ret = GRIB_ARRAY_TOO_SMALL;
        goto cleanup;
    }

    self->dirty = 0;

    buf = (unsigned char*)gh->buffer->data;
    buf  += grib_byte_offset (a);

    s = grib_power (bt->binary_scale_factor, 2);
    d = grib_power (-bt->decimal_scale_factor, 10);

    /*
     * Decode data
     */
    hres = buf;
    lres = buf;

    packed_offset = grib_byte_offset (a) +  bt->bytes * bt->n_vals_sub;

    lpos = 8 * (packed_offset - offsetdata);
    hpos = 0;

    isp = 0;
    for_ij ()
    {
        int insub;

        calc_insub ();

        if (insub)
            for (k = 0; k < 4; k++)
            {
                val[isp+k] = bt->decode_float (grib_decode_unsigned_long (hres, &hpos, 8 * bt->bytes));
            }
        else
            for (k = 0; k < 4; k++)
            {
                double S = scals (i, j);
                long dec_val = grib_decode_unsigned_long (lres, &lpos, bt->bits_per_value);
                val[isp+k] = (double)(((dec_val * s) + bt->reference_value) * d)/ S;
            }

        isp += 4;
    }

    Assert (*len >= isp);
    *len = isp;

cleanup:

    free_bif_trunc (bt, a);

    return ret;
}

static int pack_double(grib_accessor* a, const double* val, size_t *len)
{
    grib_accessor_data_g2bifourier_packing* self =  (grib_accessor_data_g2bifourier_packing*)a;
    grib_handle* gh = grib_handle_of_accessor(a);
    size_t         buflen = 0;
    size_t         hsize  = 0;
    size_t         lsize  = 0;
    unsigned char* buf  = NULL;
    unsigned char* hres = NULL;
    unsigned char* lres = NULL;
    long   hpos = 0;
    long   lpos = 0;
    int isp;
    bif_trunc_t * bt = NULL;

    double max = 0;
    double min = 0;
    /*grib_context* c = gh->context;*/

    int ret = GRIB_SUCCESS;
    int i, j, k;
    int minmax_set;
    double d = 0., s = 0.;

    if (*len == 0)
    {
        ret = GRIB_NO_VALUES;
        goto cleanup;
    }

    bt = new_bif_trunc (a, self);

    if (bt == NULL)
    {
        long makeTemplate = 0;
        if ((ret = grib_get_long_internal (gh, self->biFourierMakeTemplate, &makeTemplate)) != GRIB_SUCCESS)
            goto cleanup;
        if (! makeTemplate)
        {
            ret = GRIB_INTERNAL_ERROR;
            goto cleanup;
        }
        else
        {
            printf ("Assuming we are creating a template\n");
            ret = GRIB_SUCCESS;
            goto cleanup;
        }
    }

    self->dirty = 1;

    if (*len != bt->n_vals_bif)
    {
        grib_context_log(gh->context,GRIB_LOG_ERROR,"BIFOURIER_PACKING : wrong number of values, expected %d - got %d", bt->n_vals_bif, *len);
        ret = GRIB_INTERNAL_ERROR;
        goto cleanup;
    }

    if (! bt->laplacianOperatorIsSet)
    {
        bt->laplacianOperator = laplam (bt, val);

        if ((ret = grib_set_double_internal (gh, self->laplacianOperator, bt->laplacianOperator)) != GRIB_SUCCESS)
            goto cleanup;

        grib_get_double_internal (gh, self->laplacianOperator, &bt->laplacianOperator);
    }

    /*
     * Scan all values that will be truncated and find their minimum and maximum
     */
    minmax_set = 0;

    isp = 0;
    for_ij ()
    {
        int insub;

        calc_insub ();

        if (! insub)
        {
            for (k = 0; k < 4; k++)
            {
                double current_val = val[isp+k] * scals (i, j);
                if (! minmax_set)
                {
                    min = current_val;
                    max = current_val;
                    minmax_set++;
                }
                if (current_val < min) min = current_val;
                if (current_val > max) max = current_val;
            }
        }

        isp += 4;
    }

    if (bt->n_vals_bif != bt->n_vals_sub)
    {
        ret = grib_optimize_decimal_factor (a, self->reference_value,
                max, min, bt->bits_per_value, 0, 1,
                &bt->decimal_scale_factor,
                &bt->binary_scale_factor,
                &bt->reference_value);
        if (ret != GRIB_SUCCESS)
            goto cleanup;

        s = grib_power (-bt->binary_scale_factor,   2);
        d = grib_power (+bt->decimal_scale_factor, 10);
    }
    else
    {
        bt->decimal_scale_factor = 0;
        bt->binary_scale_factor  = 0;
        bt->reference_value      = 0.;
    }

    /*
     * Encode values
     */
    hsize = bt->bytes * bt->n_vals_sub;
    lsize = ((bt->n_vals_bif - bt->n_vals_sub) * bt->bits_per_value) / 8;

    buflen = hsize + lsize;

    buf  = (unsigned char*)grib_context_malloc (gh->context, buflen);

    hres = buf;
    lres = buf + hsize;

    lpos = 0;
    hpos = 0;

    isp = 0;

    for_ij ()
    {
        double current_val;
        int insub;

        calc_insub ();

        if (insub)
            for (k = 0; k < 4; k++)
            {
                current_val = val[isp+k];
                grib_encode_unsigned_long (hres, bt->encode_float (current_val) , &hpos, 8 * bt->bytes);
            }
        else
            for (k = 0; k < 4; k++)
            {
                double S = scals (i, j);
                current_val = (((((val[isp+k] * d) * S) - bt->reference_value) * s) + 0.5);

                if (current_val < 0)
                    grib_context_log (gh->context, GRIB_LOG_ERROR, "BIFOURIER_PACKING : negative coput before packing (%g)", current_val);

                if (bt->bits_per_value % 8)
                    grib_encode_unsigned_longb (lres, current_val, &lpos, bt->bits_per_value);
                else
                    grib_encode_unsigned_long (lres, current_val, &lpos, bt->bits_per_value);
            }

        isp += 4;
    }

    if (((hpos / 8) != hsize) && ((lpos / 8) != lsize))
    {
        grib_context_log (gh->context, GRIB_LOG_ERROR, "BIFOURIER_PACKING : Mismatch in packing between high resolution and low resolution part");
        ret = GRIB_INTERNAL_ERROR;
        goto cleanup;
    }

    buflen = ((hpos + lpos)/8);

    if ((ret = grib_set_double_internal (gh, self->reference_value, bt->reference_value)) != GRIB_SUCCESS)
        goto cleanup;

    {
        /* Make sure we can decode it again */
        double ref = 1e-100;
        grib_get_double_internal (gh,self->reference_value,&ref);
        Assert (ref == bt->reference_value);
    }

    if ((ret = grib_set_long_internal (gh, self->binary_scale_factor, bt->binary_scale_factor)) != GRIB_SUCCESS)
        goto cleanup;
    if ((ret = grib_set_long_internal (gh, self->decimal_scale_factor, bt->decimal_scale_factor)) != GRIB_SUCCESS)
        goto cleanup;

    grib_buffer_replace (a, buf, buflen, 1, 1);

    if ((ret = grib_set_long_internal (gh, self->totalNumberOfValuesInUnpackedSubset, bt->n_vals_sub)) != GRIB_SUCCESS)
        goto cleanup;

    if ((ret = grib_set_long_internal (gh, self->number_of_values, bt->n_vals_bif)) != GRIB_SUCCESS)
        goto cleanup;

cleanup:

    free_bif_trunc (bt, a);

    if (buf != NULL)
        grib_context_free (gh->context, buf);

    return ret;
}
