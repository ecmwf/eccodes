/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_iterator_class_gen.h"

namespace eccodes {
namespace grib {
namespace geo {

//Gen _grib_iterator_class_gen{};
//Iterator* grib_iterator_class_gen = &_grib_iterator_class_gen;


/*
 * Return pointer to data at (i,j) (Fortran convention)
 */
static double* pointer_to_data(unsigned int i, unsigned int j,
                               long iScansNegatively, long jScansPositively,
                               long jPointsAreConsecutive, long alternativeRowScanning,
                               unsigned int nx, unsigned int ny, double* data)
{
    /* Regular grid */
    if (nx > 0 && ny > 0) {
        if (i >= nx || j >= ny)
            return NULL;
        j = (jScansPositively) ? j : ny - 1 - j;
        i = ((alternativeRowScanning) && (j % 2 == 1)) ? nx - 1 - i : i;
        i = (iScansNegatively) ? nx - 1 - i : i;

        return (jPointsAreConsecutive) ? data + j + i * ny : data + i + nx * j;
    }

    /* Reduced or other data not on a grid */
    return NULL;
}

/* Apply the scanning mode flags which may require data array to be transformed
 * to standard west-to-east (+i) south-to-north (+j) mode.
 * The data array passed in should have 'numPoints' elements.
*/
int transform_iterator_data(grib_context* context, double* data,
                            long iScansNegatively, long jScansPositively,
                            long jPointsAreConsecutive, long alternativeRowScanning,
                            size_t numPoints, long nx, long ny)
{
    double* data2;
    double *pData0, *pData1, *pData2;
    long ix, iy;

    if (!iScansNegatively && jScansPositively && !jPointsAreConsecutive && !alternativeRowScanning) {
        /* Already +i and +j. No need to change */
        return GRIB_SUCCESS;
    }
    if (!data) return GRIB_SUCCESS;

    if (!context) context = grib_context_get_default();

    if (!iScansNegatively && !jScansPositively && !jPointsAreConsecutive && !alternativeRowScanning &&
        nx > 0 && ny > 0) {
        /* Regular grid +i -j: convert from we:ns to we:sn */
        size_t row_size = ((size_t)nx) * sizeof(double);
        data2           = (double*)grib_context_malloc(context, row_size);
        if (!data2) {
            grib_context_log(context, GRIB_LOG_ERROR, "Geoiterator data: Error allocating %ld bytes", row_size);
            return GRIB_OUT_OF_MEMORY;
        }
        for (iy = 0; iy < ny / 2; iy++) {
            memcpy(data2, data + ((size_t)iy) * nx, row_size);
            memcpy(data + iy * nx, data + (ny - 1 - iy) * ((size_t)nx), row_size);
            memcpy(data + (ny - 1 - iy) * ((size_t)nx), data2, row_size);
        }
        grib_context_free(context, data2);
        return GRIB_SUCCESS;
    }

    if (nx < 1 || ny < 1) {
        grib_context_log(context, GRIB_LOG_ERROR, "Geoiterator data: Invalid values for Nx and/or Ny");
        return GRIB_GEOCALCULUS_PROBLEM;
    }
    data2 = (double*)grib_context_malloc(context, numPoints * sizeof(double));
    if (!data2) {
        grib_context_log(context, GRIB_LOG_ERROR, "Geoiterator data: Error allocating %ld bytes", numPoints * sizeof(double));
        return GRIB_OUT_OF_MEMORY;
    }
    pData0 = data2;
    for (iy = 0; iy < ny; iy++) {
        long deltaX = 0;
        pData1 = pointer_to_data(0, iy, iScansNegatively, jScansPositively, jPointsAreConsecutive, alternativeRowScanning, nx, ny, data);
        if (!pData1) {
            grib_context_free(context, data2);
            return GRIB_GEOCALCULUS_PROBLEM;
        }
        pData2 = pointer_to_data(1, iy, iScansNegatively, jScansPositively, jPointsAreConsecutive, alternativeRowScanning, nx, ny, data);
        if (!pData2) {
            grib_context_free(context, data2);
            return GRIB_GEOCALCULUS_PROBLEM;
        }
        deltaX = pData2 - pData1;
        for (ix = 0; ix < nx; ix++) {
            *pData0++ = *pData1;
            pData1 += deltaX;
        }
    }
    memcpy(data, data2, ((size_t)numPoints) * sizeof(double));
    grib_context_free(context, data2);

    return GRIB_SUCCESS;
}

int Gen::init(grib_handle* h, grib_arguments* args)
{
    Iterator::init(h, args);

    size_t dli              = 0;
    int err                 = GRIB_SUCCESS;
    const char* s_rawData   = NULL;
    const char* s_numPoints = NULL;
    long numberOfPoints     = 0;
    carg_              = 1;

    s_numPoints        = grib_arguments_get_name(h, args, carg_++);
    missingValue_ = grib_arguments_get_name(h, args, carg_++);
    s_rawData          = grib_arguments_get_name(h, args, carg_++);

    data_ = NULL;
    h    = h; /* We may not need to keep them */
    args = args;
    if ((err = grib_get_size(h, s_rawData, &dli)) != GRIB_SUCCESS)
        return err;

    if ((err = grib_get_long_internal(h, s_numPoints, &numberOfPoints)) != GRIB_SUCCESS)
        return err;

    // See ECC-1792. If we don't want to decode the Data Section, we should not do a check
    // to see if it is consistent with the Grid Section
    if (flags_ & GRIB_GEOITERATOR_NO_VALUES) {
        // Iterator's number of values taken from the Grid Section
        nv_ = numberOfPoints;
    } else {
        // Check for consistency between the Grid and Data Sections
        if (numberOfPoints != dli) {
            grib_context_log(h->context, GRIB_LOG_ERROR, "Geoiterator: %s != size(%s) (%ld!=%ld)",
                             s_numPoints, s_rawData, numberOfPoints, dli);
            return GRIB_WRONG_GRID;
        }
        nv_ = dli;
    }

    if (nv_ == 0) {
        grib_context_log(h->context, GRIB_LOG_ERROR, "Geoiterator: size(%s) is %ld", s_rawData, dli);
        return GRIB_WRONG_GRID;
    }

    if ((flags_ & GRIB_GEOITERATOR_NO_VALUES) == 0) {
        // ECC-1525
        // When the NO_VALUES flag is unset, decode the values and store them in the iterator.
        // By default (and legacy) flags==0, so we decode
        data_ = (double*)grib_context_malloc(h->context, (nv_) * sizeof(double));

        if ((err = grib_get_double_array_internal(h, s_rawData, data_, &(nv_)))) {
            return err;
        }
    }
    e_ = -1;

    return err;
}

int Gen::reset()
{
    e_ = -1;
    return 0;
}

int Gen::destroy()
{
    const grib_context* c = h_->context;
    grib_context_free(c, data_);

    Iterator::destroy();
    return GRIB_SUCCESS;
}

long Gen::has_next()
{
    if (flags_ == 0 && data_ == NULL)
        return 0;
    if (e_ >= (long)(nv_ - 1))
        return 0;
    return 1;
}


int Gen::previous(double*, double*, double*) {
    return GRIB_NOT_IMPLEMENTED;
}

int Gen::next(double*, double*, double*) {
    return GRIB_NOT_IMPLEMENTED;
}


int Gen::get(double* lat, double* lon, double* val)
{
    if (e_ >= (long)(nv_ - 1))
        return GRIB_END_OF_ITERATION;

    e_++;
    if (lat) *lat = 0;
    if (lon) *lon = 0;
    if (val) *val = 0;

    return 1;
}

} // namespace geo
} // namespace grib
} // namespace eccodes
