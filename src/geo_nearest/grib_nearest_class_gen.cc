/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_nearest_class_gen.h"

namespace eccodes::geo_nearest {

int Gen::init(grib_handle* h, grib_arguments* args)
{
    int ret = GRIB_SUCCESS;
    if ((ret = Nearest::init(h, args) != GRIB_SUCCESS))
        return ret;

    cargs_ = 1;

    values_key_ = grib_arguments_get_name(h, args, cargs_++);
    radius_     = grib_arguments_get_name(h, args, cargs_++);
    values_  = NULL;

    return ret;
}

int Gen::destroy()
{
    grib_context* c = grib_context_get_default();
    if (values_)
        grib_context_free(c, values_);

    return Nearest::destroy();
}

int Gen::find(grib_handle* h,
                double inlat, double inlon, unsigned long flags,
                double* outlats, double* outlons, double* values,
                double* distances, int* indexes, size_t* len)
{
    return GRIB_NOT_IMPLEMENTED;
}

}  // namespace eccodes::geo_nearest
