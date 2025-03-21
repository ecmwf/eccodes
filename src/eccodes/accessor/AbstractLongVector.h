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

class AbstractLongVector : public Gen
{
public:
    AbstractLongVector() :
        Gen() { class_name_ = "abstract_long_vector"; }

public:
    // TODO(maee): make private
    long* v_ = nullptr;
    long pack_index_ = 0;
    int number_of_elements_ = 0;
};

}  // namespace eccodes::accessor
