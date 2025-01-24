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

class BufrExtractSubsets : public Gen
{
public:
    BufrExtractSubsets() :
        Gen() { class_name_ = "bufr_extract_subsets"; }
    grib_accessor* create_empty_accessor() override { return new BufrExtractSubsets{}; }
    long get_native_type() override;
    int pack_long(const long* val, size_t* len) override;
    void init(const long, grib_arguments*) override;

private:
    const char* numericValues_ = nullptr;
    const char* pack_ = nullptr;
    grib_accessor* numericValuesAccessor_ = nullptr;
    grib_accessor* packAccessor_ = nullptr;

    void get_accessors();
};

}  // namespace eccodes::accessor
