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

#include "Double.h"

namespace eccodes::accessor
{

class AbstractVector : public Double
{
public:
    AbstractVector() :
        Double() { class_name_ = "abstract_vector"; }
    // grib_accessor* create_empty_accessor() override { return new AbstractVector{}; }

public:
    // TODO(maee): make private
    double* v_ = nullptr;
    int number_of_elements_ = 0;
};

}  // namespace eccodes::accessor
