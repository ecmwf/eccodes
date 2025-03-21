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

class NumberOfCodedValues : public Long
{
public:
    NumberOfCodedValues() :
        Long() { class_name_ = "number_of_coded_values"; }
    int unpack_long(long* val, size_t* len) override;
    void init(const long, grib_arguments*) override;

private:
    const char* numberOfValues_ = nullptr;
    const char* bitsPerValue_ = nullptr;
    const char* offsetBeforeData_ = nullptr;
    const char* offsetAfterData_ = nullptr;
    const char* unusedBits_ = nullptr;
};

}  // namespace eccodes::accessor
