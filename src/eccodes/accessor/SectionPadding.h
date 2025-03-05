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

class SectionPadding : public Padding
{
public:
    SectionPadding() :
        Padding() { class_name_ = "section_padding"; }
    grib_accessor* create_empty_accessor() override { return new SectionPadding{}; }
    void init(const long, grib_arguments*) override;
    size_t preferred_size(int) override;

private:
    int preserve_ = 0;
};

}  // namespace eccodes::accessor
