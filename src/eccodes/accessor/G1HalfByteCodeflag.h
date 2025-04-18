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

class G1HalfByteCodeflag : public Gen
{
public:
    G1HalfByteCodeflag() :
        Gen() { class_name_ = "g1_half_byte_codeflag"; }
    grib_accessor* create_empty_accessor() override { return new G1HalfByteCodeflag{}; }
    long get_native_type() override;
    int pack_long(const long* val, size_t* len) override;
    int unpack_long(long* val, size_t* len) override;
    void dump(eccodes::Dumper*) override;
    void init(const long, grib_arguments*) override;
};

}  // namespace eccodes::accessor
