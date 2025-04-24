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

#include "Long.h"

namespace eccodes::accessor
{

class NumberOfPoints : public Long
{
public:
    NumberOfPoints() :
        Long() { class_name_ = "number_of_points"; }
    grib_accessor* create_empty_accessor() override { return new NumberOfPoints{}; }
    int unpack_long(long* val, size_t* len) override;
    void init(const long, grib_arguments*) override;

private:
    const char* ni_ = nullptr;
    const char* nj_ = nullptr;
    const char* plpresent_ = nullptr;
    const char* pl_ = nullptr;
};

}  // namespace eccodes::accessor
