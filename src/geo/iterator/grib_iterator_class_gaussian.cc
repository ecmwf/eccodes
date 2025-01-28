/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_iterator_class_gaussian.h"
#include "grib_iterator_factory.h"

namespace eccodes::geo_iterator
{

static FactoryBuilderGeneric<Gaussian> __builder("gaussian");

static void binary_search_gaussian_latitudes(const double array[], unsigned long n, double x, long* j);

Gaussian::Gaussian(grib_handle* h, grib_arguments* args, unsigned long flags, int& err) :
    Regular(h, args, flags, err)
{
    if (err != GRIB_SUCCESS) {
        return;
    }

    double* lats = nullptr;
    double laf; /* latitude of first point in degrees */
    double lal; /* latitude of last point in degrees */
    long trunc; /* number of parallels between a pole and the equator */
    long lai              = 0;
    long jScansPositively = 0;
    int size              = 0;
    double start;
    long istart = 0;

    const char* latofirst          = args->get_name(h, carg_++);
    const char* latoflast          = args->get_name(h, carg_++);
    const char* numtrunc           = args->get_name(h, carg_++);
    const char* s_jScansPositively = args->get_name(h, carg_++);

    if ((err = grib_get_double_internal(h, latofirst, &laf))) {
        return;
    }
    if ((err = grib_get_double_internal(h, latoflast, &lal))) {
        return;
    }
    if ((err = grib_get_long_internal(h, numtrunc, &trunc))) {
        return;
    }
    if ((err = grib_get_long_internal(h, s_jScansPositively, &jScansPositively))) {
        return;
    }

    start = laf;

    size = trunc * 2;

    lats = (double*)grib_context_malloc(h->context, size * sizeof(double));

    if ((err = grib_get_gaussian_latitudes(trunc, lats)) != GRIB_SUCCESS) {
        grib_context_log(h->context, GRIB_LOG_ERROR, "Error calculating gaussian points: %s", grib_get_error_message(err));
        return;
    }
    /*
     for(loi=(trunc*2)-1;loi>=0;loi--)
       if(fabs(lats[loi] - lal) < glatPrecision) break;
     for(j=(trunc*2)-1;j>0;j--) {
       if(fabs(lats[j] - laf) < glatPrecision) break;
     }
     */

    binary_search_gaussian_latitudes(lats, size - 1, start, &istart);
    if (istart < 0 || istart >= size) {
        grib_context_log(h->context, GRIB_LOG_ERROR, "Failed to find index for latitude=%g", start);
        err = GRIB_GEOCALCULUS_PROBLEM;
        return;
    }

    if (jScansPositively) {
        for (lai = 0; lai < Nj_; lai++) {
            DEBUG_ASSERT(istart >= 0);
            lats_[lai] = lats[istart--];
            if (istart < 0) {
                istart = size - 1;
            }
        }
    }
    else {
        for (lai = 0; lai < Nj_; lai++) {
            lats_[lai] = lats[istart++];
            if (istart > size - 1) {
                istart = 0;
            }
        }
    }

    grib_context_free(h->context, lats);
}

#define EPSILON 1e-3
/* Note: the argument 'n' is NOT the size of the 'xx' array but its LAST index i.e. size of xx - 1 */
static void binary_search_gaussian_latitudes(const double array[], unsigned long n, double x, long* j)
{
    unsigned long low  = 0;
    unsigned long high = n;
    unsigned long mid;
    const int descending = (array[n] < array[0]);
    ECCODES_ASSERT(descending); /* Gaussian latitudes should be in descending order */
    while (low <= high) {
        mid = (high + low) / 2;

        if (fabs(x - array[mid]) < EPSILON) {
            *j = mid;
            return;
        }

        if (x < array[mid]) {
            low = mid + 1;
        }
        else {
            high = mid - 1;
        }
    }
    *j = -1; /* Not found */
}

// static void binary_search_old(const double array[], unsigned long n, double x, long* j)
// {
//     /*This routine works only on descending ordered arrays*/
//     unsigned long ju, jm, jl;
//     jl = 0;
//     ju = n;
//     if (fabs(x - array[0]) < EPSILON) {
//         *j = 0;
//         return;
//     }
//     if (fabs(x - array[n]) < EPSILON) {
//         *j = n;
//         return;
//     }
//     while (ju - jl > 1) {
//         jm = (ju + jl) >> 1;
//         if (fabs(x - array[jm]) < EPSILON) {
//             /* found something close enough. We're done */
//             *j = jm;
//             return;
//         }
//         if (x < array[jm])
//             jl = jm;
//         else
//             ju = jm;
//     }
//     *j = jl;
// }

}  // namespace eccodes::geo_iterator
