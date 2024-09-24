/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#pragma once

#include "grib_iterator_class_gen.h"

namespace eccodes {
namespace grib {
namespace geo {

class LambertAzimuthalEqualArea : public Gen
{
public:
    LambertAzimuthalEqualArea() : Gen()
    {
        class_name_ = "lambert_azimuthal_equal_area";
    }

    int init(grib_handle*, grib_arguments*) override;
    int next(double*, double*, double*) override;
    int previous(double*, double*, double*) override;
    int destroy() override;

public:
    double *lats_;
    double *lons_;
    long Nj_;
};

} // namespace geo
} // namespace grib
} // namespace eccodes
