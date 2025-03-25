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

class Pad : public Padding
{
public:
    Pad() :
        Padding() { }
    const AccessorType& accessor_type() const override {{ return accessor_type_; }}
    void init(const long, grib_arguments*) override;
    size_t preferred_size(int) override;

private:
    grib_expression* expression_ = nullptr;

public:
    static inline const AccessorType accessor_type_{"pad"};
};

}  // namespace eccodes::accessor
