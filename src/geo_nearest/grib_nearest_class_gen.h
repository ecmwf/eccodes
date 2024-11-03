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

#include "grib_nearest.h"

namespace eccodes::geo_nearest {

class Gen : public Nearest {
public:
    Gen() { class_name_ = "gen"; }
    int init(grib_handle*, grib_arguments*) override;
    int find(grib_handle*, double, double, unsigned long, double*, double*, double*, double*, int*, size_t*) override;
    int destroy() override;

protected:
    int cargs_ = 0;
    const char* values_key_ = nullptr;

private:
    const char* radius_ = nullptr;
};

int grib_nearest_find_generic(
    Nearest* nearest, grib_handle* h,
    double inlat, double inlon, unsigned long flags,

    const char* values_keyname,
    double** out_lats,
    int* out_lats_count,
    double** out_lons,
    int* out_lons_count,
    double** out_distances,

    double* outlats, double* outlons,
    double* values, double* distances, int* indexes, size_t* len);

}  // namespace eccodes::geo_nearest
