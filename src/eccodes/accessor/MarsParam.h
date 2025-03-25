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

class MarsParam : public Ascii
{
public:
    MarsParam() :
        Ascii() { }
    const AccessorType& accessor_type() const override {{ return accessor_type_; }}
    int unpack_string(char*, size_t* len) override;
    size_t string_length() override;
    void init(const long, grib_arguments*) override;

private:
    const char* paramId_ = nullptr;
    const char* table_ = nullptr;
    const char* param_ = nullptr;

public:
    static inline const AccessorType accessor_type_{"mars_param"};
};

}  // namespace eccodes::accessor
