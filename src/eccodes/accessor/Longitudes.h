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

class Longitudes : public Double
{
public:
    Longitudes() :
        Double() { class_name_ = "longitudes"; }
    grib_accessor* create_empty_accessor() override { return new Longitudes{}; }
    int unpack_double(double* val, size_t* len) override;
    int value_count(long*) override;
    void init(const long, grib_arguments*) override;

private:
    const char* values_ = nullptr;
    long distinct_ = 0;
    double* lons_ = nullptr;
    long size_ = 0;
    int save_ = 0;
};

}  // namespace eccodes::accessor
