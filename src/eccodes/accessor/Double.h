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

class Double : public Gen
{
public:
    Double() :
        Gen() { class_name_ = "double"; }
    // grib_accessor* create_empty_accessor() override { return new Double{}; }
    long get_native_type() override;
    int pack_missing() override;
    int unpack_string(char*, size_t* len) override;
    void dump(eccodes::Dumper*) override;
    int compare(grib_accessor*) override;
};

}  // namespace eccodes::accessor
