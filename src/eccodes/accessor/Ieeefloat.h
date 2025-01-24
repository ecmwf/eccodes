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

#include "Double.h"

namespace eccodes::accessor
{

class Ieeefloat : public Double
{
public:
    Ieeefloat() :
        Double() { class_name_ = "ieeefloat"; }
    grib_accessor* create_empty_accessor() override { return new Ieeefloat{}; }
    int pack_double(const double* val, size_t* len) override;
    int unpack_double(double* val, size_t* len) override;
    int unpack_float(float* val, size_t* len) override;
    int value_count(long*) override;
    void init(const long, grib_arguments*) override;
    void update_size(size_t) override;
    int nearest_smaller_value(double val, double* nearest) override;

private:
    grib_arguments* arg_ = nullptr;
};

}  // namespace eccodes::accessor
