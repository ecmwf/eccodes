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

class UnsignedBits : public Long
{
public:
    UnsignedBits() :
        Long() { class_name_ = "unsigned_bits"; }
    int pack_long(const long* val, size_t* len) override;
    int unpack_long(long* val, size_t* len) override;
    long byte_count() override;
    long byte_offset() override;
    long next_offset() override;
    int value_count(long*) override;
    void init(const long, grib_arguments*) override;
    void update_size(size_t) override;

private:
    const char* numberOfBits_ = nullptr;
    const char* numberOfElements_ = nullptr;

    long compute_byte_count();
};

}  // namespace eccodes::accessor
