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

#include "Gen.h"

namespace eccodes::geo_nearest {

class Regular : public Gen {
public:
    Regular() {
        class_name_ = "regular";
    }
    Nearest* create() override { return new Regular(); }
    int init(grib_handle*, grib_arguments*) override;
    int find(grib_handle*, double, double, unsigned long, double*, double*, double*, double*, int*, size_t*) override;
};

}  // namespace eccodes::geo_nearest
