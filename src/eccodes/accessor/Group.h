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

class Group : public Gen
{
public:
    Group() :
        Gen() { class_name_ = "group"; }
    grib_accessor* create_empty_accessor() override { return new Group{}; }
    long get_native_type() override;
    int unpack_double(double* val, size_t* len) override;
    int unpack_long(long* val, size_t* len) override;
    int unpack_string(char*, size_t* len) override;
    size_t string_length() override;
    long next_offset() override;
    int value_count(long*) override;
    void dump(eccodes::Dumper*) override;
    void init(const long, grib_arguments*) override;
    int compare(grib_accessor*) override;

private:
    char endCharacter_ = 0;
};

}  // namespace eccodes::accessor
