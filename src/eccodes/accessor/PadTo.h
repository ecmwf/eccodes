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

#include "Padding.h"

namespace eccodes::accessor
{

class PadTo : public Padding
{
public:
    PadTo() :
        Padding() { class_name_ = "padto"; }
    grib_accessor* create_empty_accessor() override { return new PadTo{}; }
    void dump(eccodes::Dumper*) override;
    void init(const long, grib_arguments*) override;
    size_t preferred_size(int) override;

private:
    grib_expression* expression_ = nullptr;
};

}  // namespace eccodes::accessor
