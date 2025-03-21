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

class Values : public Gen
{
public:
    Values() :
        Gen() { class_name_ = "values"; }
    long get_native_type() override;
    int pack_long(const long* val, size_t* len) override;
    long byte_count() override;
    long byte_offset() override;
    long next_offset() override;
    void dump(eccodes::Dumper*) override;
    void init(const long, grib_arguments*) override;
    void update_size(size_t) override;
    int compare(grib_accessor*) override;

protected:
    int carg_ = 0;
    const char* seclen_ = nullptr;
    const char* offsetdata_ = nullptr;
    const char* offsetsection_ = nullptr;
    int values_dirty_ = 0;

private:
    long init_length();
};

}  // namespace eccodes::accessor
