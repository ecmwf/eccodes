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

#include "Variable.h"

namespace eccodes::accessor
{

class Transient : public Variable
{
public:
    void init(const long l, grib_arguments* args) override;
    Transient() :
        Variable() { class_name_ = "transient"; }
    grib_accessor* create_empty_accessor() override { return new Transient{}; }
};

}  // namespace eccodes::accessor
