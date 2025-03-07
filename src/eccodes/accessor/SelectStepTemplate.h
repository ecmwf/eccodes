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

#include "Unsigned.h"

namespace eccodes::accessor
{

class SelectStepTemplate : public Unsigned
{
public:
    SelectStepTemplate() :
        Unsigned() { class_name_ = "select_step_template"; }
    grib_accessor* create_empty_accessor() override { return new SelectStepTemplate{}; }
    int pack_long(const long* val, size_t* len) override;
    int unpack_long(long* val, size_t* len) override;
    int value_count(long*) override;
    void init(const long, grib_arguments*) override;

private:
    const char* productDefinitionTemplateNumber_ = nullptr;
    int instant_ = 0;
};

}  // namespace eccodes::accessor
