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

class Mercator : public Gen
{
public:
    Mercator() { class_name_ = "mercator"; }
    Iterator* create() const override { return new Mercator(); }
    
    int init(grib_handle*,grib_arguments*) override;
    int next(double *lat, double *lon, double *val) override;
    int destroy() override;

private:
    double *lats_;
    double *lons_;
    long Nj_;

    int init_mercator(grib_handle*,
                      size_t, long, long,
                      double, double,
                      double, double,
                      double, double,
                      double, double,
                      double, double);
};

} // namespace geo
} // namespace grib
} // namespace eccodes
