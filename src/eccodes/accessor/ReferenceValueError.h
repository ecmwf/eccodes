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

class ReferenceValueError : public Double
{
public:
    ReferenceValueError() :
        Double() { class_name_ = "reference_value_error"; }
    grib_accessor* create_empty_accessor() override { return new ReferenceValueError{}; }
    int unpack_double(double* val, size_t* len) override;
    void init(const long, grib_arguments*) override;

private:
    const char* referenceValue_ = nullptr;
    const char* floatType_ = nullptr;
};

}  // namespace eccodes::accessor
