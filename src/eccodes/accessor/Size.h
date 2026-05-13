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

#include "Long.h"

namespace eccodes::accessor
{

class Size : public Long
{
public:
    Size() :
        Long() { }
    const AccessorType& accessor_type() const override {{ return accessor_type_; }}
    int unpack_long(long* val, size_t* len) override;
    void init(const long, grib_arguments*) override;

private:
    const char* accessor_ = nullptr;

public:
    static inline const AccessorType accessor_type_{"size"};
};

}  // namespace eccodes::accessor
