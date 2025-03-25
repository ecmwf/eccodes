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
        Gen() { }
    const AccessorType& accessor_type() const override {{ return accessor_type_; }}

public:
    // TODO(maee): make private
    long* v_ = nullptr;
    long pack_index_ = 0;
    int number_of_elements_ = 0;
private:

public:
    static inline const AccessorType accessor_type_{"abstract_long_vector"};
};

}  // namespace eccodes::accessor
