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

class FromScaleFactorScaledValue : public Double
{
public:
    FromScaleFactorScaledValue() :
        Double() { class_name_ = "from_scale_factor_scaled_value"; }
    int is_missing() override;
    int pack_double(const double* val, size_t* len) override;
    int unpack_double(double* val, size_t* len) override;
    int value_count(long*) override;
    void init(const long, grib_arguments*) override;

private:
    const char* scaleFactor_ = nullptr;
    const char* scaledValue_ = nullptr;
};

}  // namespace eccodes::accessor
