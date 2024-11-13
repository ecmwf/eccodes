/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_nearest_class_polar_stereographic.h"

eccodes::geo_nearest::PolarStereographic _grib_nearest_polar_stereographic{};
eccodes::geo_nearest::Nearest* grib_nearest_polar_stereographic = &_grib_nearest_polar_stereographic;

namespace eccodes::geo_nearest {

int PolarStereographic::init(grib_handle* h, grib_arguments* args)
{
    int ret = GRIB_SUCCESS;
    if ((ret = Gen::init(h, args) != GRIB_SUCCESS))
        return ret;

    Ni_ = grib_arguments_get_name(h, args, cargs_++);
    Nj_ = grib_arguments_get_name(h, args, cargs_++);
    i_  = (size_t*)grib_context_malloc(h->context, 2 * sizeof(size_t));
    j_  = (size_t*)grib_context_malloc(h->context, 2 * sizeof(size_t));

    return ret;
}

int PolarStereographic::find(grib_handle* h,
                double inlat, double inlon, unsigned long flags,
                double* outlats, double* outlons,
                double* values, double* distances, int* indexes, size_t* len)
{
    return grib_nearest_find_generic(
        h, inlat, inlon, flags, /* inputs */

        values_key_,   /* outputs to set the 'self' object */
        &(lats_),
        &(lats_count_),
        &(lons_),
        &(lons_count_),
        &(distances_),

        outlats, outlons,  /* outputs of the find function */
        values, distances, indexes, len);
}

}  // namespace eccodes::geo_nearest
