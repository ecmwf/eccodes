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

class CountMissing : public Long
{
public:
    CountMissing() :
        Long() { class_name_ = "count_missing"; }
    grib_accessor* create_empty_accessor() override { return new CountMissing{}; }
    int unpack_long(long* val, size_t* len) override;
    int value_count(long*) override;
    void init(const long, grib_arguments*) override;

private:
    const char* bitmap_ = nullptr;
    const char* unusedBitsInBitmap_ = nullptr;
    const char* numberOfDataPoints_ = nullptr;
    const char* missingValueManagementUsed_ = nullptr;
};

}  // namespace eccodes::accessor
