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

class Uint64LittleEndian : public Gen
{
public:
    Uint64LittleEndian() :
        Gen() { class_name_ = "uint64_little_endian"; }
    grib_accessor* create_empty_accessor() override { return new Uint64LittleEndian{}; }
    long get_native_type() override;
    int unpack_long(long* val, size_t* len) override;
};

}  // namespace eccodes::accessor
