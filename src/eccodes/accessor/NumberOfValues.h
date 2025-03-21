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

class NumberOfValues : public Long
{
public:
    NumberOfValues() :
        Long() { class_name_ = "number_of_values"; }
    int unpack_long(long* val, size_t* len) override;
    void init(const long, grib_arguments*) override;

private:
    const char* values_ = nullptr;
    const char* bitsPerValue_ = nullptr;
    const char* numberOfPoints_ = nullptr;
    const char* bitmapPresent_ = nullptr;
    const char* bitmap_ = nullptr;
    const char* numberOfCodedValues_ = nullptr;
};

}  // namespace eccodes::accessor
