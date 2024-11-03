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

#include "grib_nearest_class_gen.h"

namespace eccodes::geo_nearest {

class Healpix : public Gen {
public:
    Healpix() {
        class_name_ = "healpix";
    }
    Nearest* create() override { return new Healpix(); };
    int init(grib_handle*, grib_arguments*) override;
    int find(grib_handle*, double, double, unsigned long, double*, double*, double*, double*, int*, size_t*) override;
    int destroy() override;

private:
    double* lats_ = nullptr;
    int lats_count_ = 0;
    double* lons_ = nullptr;
    int lons_count_ = 0;
    double* distances_ = nullptr;
    int* k_ = nullptr;
    int* i_ = nullptr;
    int* j_ = nullptr;
    const char* Ni_ = nullptr;
    const char* Nj_ = nullptr;
};

}  // namespace eccodes::geo_nearest
