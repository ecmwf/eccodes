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

class ProjString : public Gen
{
public:
    ProjString() :
        Gen() { class_name_ = "proj_string"; }
    grib_accessor* create_empty_accessor() override { return new ProjString{}; }
    long get_native_type() override;
    int unpack_string(char*, size_t* len) override;
    void init(const long, grib_arguments*) override;

private:
    const char* grid_type_ = nullptr;
    int endpoint_ = 0;
};

}  // namespace eccodes::accessor
