/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_iterator_class_regular.h"

eccodes::grib::geo::Regular _grib_iterator_regular{};
eccodes::grib::geo::Iterator* grib_iterator_regular = &_grib_iterator_regular;

namespace eccodes
{
namespace grib
{
namespace geo
{

#define ITER "Regular grid Geoiterator"

int Regular::next(double* lat, double* lon, double* val) const
{
    if ((long)e_ >= (long)(nv_ - 1))
        return 0;

    e_++;

    *lat = las_[(long)floor(e_ / Ni_)];
    *lon = los_[(long)e_ % Ni_];
    if (val && data_) {
        *val = data_[e_];
    }
    return 1;
}

int Regular::previous(double* lat, double* lon, double* val) const
{
    if (e_ < 0)
        return 0;
    *lat = las_[(long)floor(e_ / Ni_)];
    *lon = los_[e_ % Ni_];
    if (val && data_) {
        *val = data_[e_];
    }
    e_--;

    return 1;
}

int Regular::destroy()
{
    const grib_context* c = h_->context;
    grib_context_free(c, las_);
    grib_context_free(c, los_);

    return Gen::destroy();
}

int Regular::init(grib_handle* h, grib_arguments* args)
{
    int ret = GRIB_SUCCESS;
    if ((ret = Gen::init(h, args)) != GRIB_SUCCESS)
        return ret;

    long Ni; /* Number of points along a parallel = Nx */
    long Nj; /* Number of points along a meridian = Ny */
    double idir, idir_coded, lon1, lon2;
    long loi;

    const char* s_lon1      = grib_arguments_get_name(h, args, carg_++);
    const char* s_idir      = grib_arguments_get_name(h, args, carg_++);
    const char* s_Ni        = grib_arguments_get_name(h, args, carg_++);
    const char* s_Nj        = grib_arguments_get_name(h, args, carg_++);
    const char* s_iScansNeg = grib_arguments_get_name(h, args, carg_++);

    if ((ret = grib_get_double_internal(h, s_lon1, &lon1)))
        return ret;
    if ((ret = grib_get_double_internal(h, "longitudeOfLastGridPointInDegrees", &lon2)))
        return ret;
    if ((ret = grib_get_double_internal(h, s_idir, &idir)))  // can be GRIB_MISSING_DOUBLE
        return ret;
    idir_coded = idir;
    if ((ret = grib_get_long_internal(h, s_Ni, &Ni)))
        return ret;
    if (grib_is_missing(h, s_Ni, &ret) && ret == GRIB_SUCCESS) {
        grib_context_log(h->context, GRIB_LOG_ERROR, "%s: Key %s cannot be 'missing' for a regular grid!", ITER, s_Ni);
        return GRIB_WRONG_GRID;
    }

    if ((ret = grib_get_long_internal(h, s_Nj, &Nj)))
        return ret;
    if (grib_is_missing(h, s_Nj, &ret) && ret == GRIB_SUCCESS) {
        grib_context_log(h->context, GRIB_LOG_ERROR, "%s: Key %s cannot be 'missing' for a regular grid!", ITER, s_Nj);
        return GRIB_WRONG_GRID;
    }

    if (Ni * Nj != nv_) {
        grib_context_log(h->context, GRIB_LOG_ERROR, "%s: Ni*Nj!=numberOfDataPoints (%ld*%ld!=%zu)", ITER, Ni, Nj, nv_);
        return GRIB_WRONG_GRID;
    }

    if ((ret = grib_get_long_internal(h, s_iScansNeg, &iScansNegatively_)))
        return ret;

    /* GRIB-801: Careful of case with a single point! Ni==1 */
    if (Ni > 1) {
        /* Note: If first and last longitudes are equal I assume you wanna go round the globe */
        if (iScansNegatively_) {
            if (lon1 > lon2) {
                idir = (lon1 - lon2) / (Ni - 1);
            }
            else {
                idir = (lon1 + 360.0 - lon2) / (Ni - 1);
            }
        }
        else {
            if (lon2 > lon1) {
                idir = (lon2 - lon1) / (Ni - 1);
            }
            else {
                idir = (lon2 + 360.0 - lon1) / (Ni - 1);
            }
        }
    }
    if (iScansNegatively_) {
        idir = -idir;
    }
    else {
        if (lon1 + (Ni - 2) * idir > 360)
            lon1 -= 360;
        /*See ECC-704, GRIB-396*/
        /*else if ( (lon1+(Ni-1)*idir)-360 > epsilon ){
            idir=360.0/(float)Ni;
        }*/
    }

    Ni_ = Ni;
    Nj_ = Nj;

    las_ = (double*)grib_context_malloc(h->context, Nj * sizeof(double));
    los_ = (double*)grib_context_malloc(h->context, Ni * sizeof(double));

    if (idir != idir_coded) {
        grib_context_log(h->context, GRIB_LOG_DEBUG, "%s: Using idir=%g (coded value=%g)", ITER, idir, idir_coded);
    }

    for (loi = 0; loi < Ni; loi++) {
        los_[loi] = lon1;
        lon1 += idir;
    }

    // ECC-1406: Due to rounding, errors can accumulate.
    // So we ensure the last longitude is longitudeOfLastGridPointInDegrees
    // Also see ECC-1671, ECC-1708
    if (lon2 > 0) {
        lon2 = normalise_longitude_in_degrees(lon2);
    }
    los_[Ni - 1] = lon2;

    return ret;
}

}  // namespace geo
}  // namespace grib
}  // namespace eccodes
