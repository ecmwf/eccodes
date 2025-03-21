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

class SecondOrderBitsPerValue : public Long
{
public:
    SecondOrderBitsPerValue() :
        Long() { class_name_ = "second_order_bits_per_value"; }
    int pack_long(const long* val, size_t* len) override;
    int unpack_long(long* val, size_t* len) override;
    void init(const long, grib_arguments*) override;

private:
    const char* values_ = nullptr;
    const char* binaryScaleFactor_ = nullptr;
    const char* decimalScaleFactor_ = nullptr;
    long bitsPerValue_ = 0;
};

}  // namespace eccodes::accessor
