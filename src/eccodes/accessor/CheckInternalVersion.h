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

#include "Ascii.h"

namespace eccodes::accessor
{

class CheckInternalVersion : public Ascii
{
public:
    CheckInternalVersion() :
        Ascii() { }
    const AccessorType& accessor_type() const override {{ return accessor_type_; }}
    size_t string_length() override;
    int value_count(long*) override;
    void init(const long, grib_arguments*) override;
private:

public:
    static inline const AccessorType accessor_type_{"check_internal_version"};
};

}  // namespace eccodes::accessor
