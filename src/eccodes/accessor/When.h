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

class When : public Gen
{
public:
    When() :
        Gen() { class_name_ = "when"; }
    grib_accessor* create_empty_accessor() override { return new When{}; }
    long get_native_type() override;
    void dump(eccodes::Dumper*) override;
    void init(const long, grib_arguments*) override;
    int notify_change(grib_accessor* changed) override;
};

}  // namespace eccodes::accessor
