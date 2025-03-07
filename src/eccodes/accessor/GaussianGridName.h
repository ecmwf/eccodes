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

namespace eccodes::accessor
{

class GaussianGridName : public Gen
{
public:
    GaussianGridName() :
        Gen() { class_name_ = "gaussian_grid_name"; }
    grib_accessor* create_empty_accessor() override { return new GaussianGridName{}; }
    long get_native_type() override;
    int unpack_string(char*, size_t* len) override;
    size_t string_length() override;
    void init(const long, grib_arguments*) override;

private:
    const char* N_ = nullptr;
    const char* Ni_ = nullptr;
    const char* isOctahedral_ = nullptr;
};

}  // namespace eccodes::accessor
