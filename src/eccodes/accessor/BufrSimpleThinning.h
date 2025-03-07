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

class BufrSimpleThinning : public Gen
{
public:
    BufrSimpleThinning() :
        Gen() { class_name_ = "bufr_simple_thinning"; }
    grib_accessor* create_empty_accessor() override { return new BufrSimpleThinning{}; }
    long get_native_type() override;
    int pack_long(const long* val, size_t* len) override;
    void init(const long, grib_arguments*) override;

private:
    const char* doExtractSubsets_ = nullptr;
    const char* numberOfSubsets_ = nullptr;
    const char* extractSubsetList_ = nullptr;
    const char* simpleThinningStart_ = nullptr;
    const char* simpleThinningMissingRadius_ = nullptr;
    const char* simpleThinningSkip_ = nullptr;

    int apply_thinning();
};

}  // namespace eccodes::accessor
