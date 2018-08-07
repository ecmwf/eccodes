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
   CLASS      = iterator
   IMPLEMENTS = destroy
   IMPLEMENTS = has_next
   IMPLEMENTS = init
   IMPLEMENTS = reset
   MEMBERS     =   long carg
   MEMBERS     =  const char* missingValue;
   END_CLASS_DEF

 */

/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "iterator.class" and rerun ./make_class.pl

*/


static void init_class              (grib_iterator_class*);

static int init               (grib_iterator* i,grib_handle*,grib_arguments*);
static int destroy            (grib_iterator* i);
static int reset              (grib_iterator* i);
static long has_next          (grib_iterator* i);


typedef struct grib_iterator_gen{
  grib_iterator it;
/* Members defined in gen */
	long carg;
	const char* missingValue;
} grib_iterator_gen;


static grib_iterator_class _grib_iterator_class_gen = {
    0,                    /* super                     */
    "gen",                    /* name                      */
    sizeof(grib_iterator_gen),/* size of instance          */
    0,                           /* inited */
    &init_class,                 /* init_class */
    &init,                     /* constructor               */
    &destroy,                  /* destructor                */
    0,                     /* Next Value                */
    0,                 /*  Previous Value           */
    &reset,                    /* Reset the counter         */
    &has_next,                 /* has next values           */
};

grib_iterator_class* grib_iterator_class_gen = &_grib_iterator_class_gen;


static void init_class(grib_iterator_class* c)
{
}
/* END_CLASS_IMP */

/*
 * Return pointer to data at (i,j) (Fortran convention)
 */
static double* pointer_to_data(unsigned int i, unsigned int j,
        long iScansNegatively, long jScansPositively, long jPointsAreConsecutive, long alternativeRowScanning,
        unsigned int nx, unsigned int ny, double *data)
{
    /* Regular grid */
    if (nx > 0 && ny > 0) {
        if (i >= nx || j >= ny) return NULL;
        j = (jScansPositively) ? j : ny - 1 - j;
        i = ((alternativeRowScanning) && (j % 2 == 1)) ?  nx - 1 - i : i;
        i = (iScansNegatively) ? nx - 1 - i : i;

        return (jPointsAreConsecutive) ?  data + j + i*ny : data + i + nx*j;
    }

    /* Reduced or other data not on a grid */
    return NULL;
}

/* Apply the scanning mode flags which may require data array to be transformed */
/* to standard west-to-east south-to-north mode */
int transform_iterator_data(grib_handle* h, double* data,
        long iScansNegatively, long jScansPositively, long jPointsAreConsecutive, long alternativeRowScanning,
        size_t numPoints, long nx, long ny)
{
    double* data2;
    double *pData0, *pData1, *pData2;
    unsigned long ix, iy;

    if ( !iScansNegatively && jScansPositively && !jPointsAreConsecutive && !alternativeRowScanning )
    {
        /* Already +i and +j. No need to change */
        return GRIB_SUCCESS;
    }

    if ( !iScansNegatively && !jScansPositively && !jPointsAreConsecutive && !alternativeRowScanning &&
         nx > 0 && ny > 0)
    {
        /* regular grid +i -j: convert from we:ns to we:sn */
        size_t row_size = ((size_t) nx) * sizeof(double);
        data2 = (double*)grib_context_malloc(h->context, row_size);
        if (!data2) {
            grib_context_log(h->context,GRIB_LOG_ERROR, "Unable to allocate %ld bytes", row_size);
            return GRIB_OUT_OF_MEMORY;
        }
        for (iy = 0; iy < ny/2; iy++) {
            memcpy(data2, data + ((size_t) iy) * nx, row_size);
            memcpy(data + iy*nx, data + (ny-1-iy) * ((size_t) nx), row_size);
            memcpy(data + (ny-1-iy) * ((size_t) nx), data2, row_size);
        }
        grib_context_free(h->context, data2);
        return GRIB_SUCCESS;
    }

    if (nx < 1 || ny < 1) {
        grib_context_log(h->context,GRIB_LOG_ERROR, "Invalid values for Nx and/or Ny");
        return GRIB_GEOCALCULUS_PROBLEM;
    }
    data2 = (double*)grib_context_malloc(h->context, numPoints*sizeof(double));
    if (!data2) {
        grib_context_log(h->context,GRIB_LOG_ERROR, "Unable to allocate %ld bytes",numPoints*sizeof(double));
        return GRIB_OUT_OF_MEMORY;
    }
    pData0 = data2;
    for (iy = 0; iy < ny; iy++) {
        long deltaX = 0;
        pData1 = pointer_to_data(0, iy, iScansNegatively, jScansPositively, jPointsAreConsecutive, alternativeRowScanning, nx,ny, data);
        if (!pData1) return GRIB_GEOCALCULUS_PROBLEM;
        pData2 = pointer_to_data(1, iy, iScansNegatively, jScansPositively, jPointsAreConsecutive, alternativeRowScanning, nx,ny, data);
        if (!pData2) return GRIB_GEOCALCULUS_PROBLEM;
        deltaX = pData2 - pData1;
        for (ix = 0; ix < nx; ix++) {
            *pData0++ = *pData1;
            pData1 += deltaX;
        }
    }
    memcpy(data, data2, ((size_t)numPoints) * sizeof(double));
    grib_context_free(h->context, data2);

    return GRIB_SUCCESS;
}

static int init(grib_iterator* iter, grib_handle *h, grib_arguments* args)
{
    grib_iterator_gen* self = (grib_iterator_gen*) iter;
    size_t dli=0;
    int err = GRIB_SUCCESS;
    const char* s_rawData  = NULL;
    const char* s_numPoints=NULL;
    long numberOfPoints=0;
    self->carg = 1;

    s_numPoints = grib_arguments_get_name(h,args,self->carg++);
    self->missingValue  = grib_arguments_get_name(h,args,self->carg++);
    s_rawData      = grib_arguments_get_name(h,args,self->carg++);

    iter->h    = h; /* We may not need to keep them */
    iter->args = args;
    if( (err =  grib_get_size(h,s_rawData,&dli))!= GRIB_SUCCESS) return err;

    if( (err =  grib_get_long_internal(h,s_numPoints,&numberOfPoints))
            != GRIB_SUCCESS)
        return err;

    if (numberOfPoints!=dli) {
        grib_context_log(h->context,GRIB_LOG_ERROR,"%s != size(%s) (%ld!=%ld)",
                s_numPoints,s_rawData,numberOfPoints,dli);
        return GRIB_WRONG_GRID;
    }
    iter->nv = dli;
    if (iter->nv==0) {
        grib_context_log(h->context,GRIB_LOG_ERROR,"size(%s) is %ld", s_rawData, dli);
        return GRIB_WRONG_GRID;
    }
    iter->data = (double*)grib_context_malloc(h->context,(iter->nv)*sizeof(double));

    if( (err = grib_get_double_array_internal(h,s_rawData,iter->data ,&(iter->nv))))
        return err;

    iter->e = -1;

    return err;
}

static int reset(grib_iterator* iter)
{
    iter->e = -1;
    return 0;
}

static int destroy(grib_iterator* iter)
{
    const grib_context *c = iter->h->context;
    grib_context_free(c, iter->data);
    return 1;
}

static long has_next(grib_iterator* iter)
{
    if(iter->data == NULL) return 0;
    return   iter->nv - iter->e;
}
