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

class DataSecondaryBitmap : public Gen
{
public:
    DataSecondaryBitmap() :
        Gen() { class_name_ = "data_secondary_bitmap"; }
    // grib_accessor* create_empty_accessor() override { return new DataSecondaryBitmap{}; }
    long get_native_type() override;
    int unpack_double(double* val, size_t* len) override;
    void dump(eccodes::Dumper*) override;
    void init(const long, grib_arguments*) override;

protected:
    const char* primary_bitmap_ = nullptr;
    const char* secondary_bitmap_ = nullptr;
    const char* missing_value_ = nullptr;
    const char* expand_by_ = nullptr;
};

}  // namespace eccodes::accessor
